#include "config.h"

#include <stdio.h>
#include <string.h>
#include <time.h>

#include <errno.h>
#include <sys/stat.h>
#include <unistd.h>

#include "utils.h"

static ecode_t get_config_path(char **r_config_path)
{
	const char *home_suffix = "/.config/wayhsare/config.json";
	const char *xdg_config_suffix = "/wayhsare/config.json";
	char *path = NULL;

	const char *xdg_config = getenv("XDG_CONFIG_HOME");
	if (!xdg_config) { /* no $XDG_CONFIG_HOME envar, try "$HOME/.config". */
		const char *home;
		ecode_t code;
		code = get_home_path(&home);
		if (!code)
			return WSE_CONFIG_NHOME;

		size_t strlen;
		strlen = snprintf(NULL, 0, "%s%s", home, home_suffix);
		path = malloc(strlen + 1);
		snprintf(path, strlen + 1, "%s%s", home, home_suffix);
	} else {
		size_t strlen;
		strlen = snprintf(NULL, 0, "%s%s", xdg_config, xdg_config_suffix);
		path = malloc(strlen + 1);
		snprintf(path, strlen + 1, "%s%s", xdg_config, xdg_config_suffix);
	}

	*r_config_path = path;
	return WS_OK;
}

ecode_t get_config_json(struct json_object **r_config_json, const char *custom_path)
{
	char *config_path = NULL;
	get_config_path(&config_path);
	struct json_object *config_json;

	/* check custom --config path. */
	if ((config_json = json_object_from_file(custom_path)) != NULL)
		*r_config_json = config_json;

	/* check local user .config dir. */
	else if ((config_json = json_object_from_file(config_path)) != NULL)
		*r_config_json = config_json;

	/* check global system etc dir. */
	else if ((config_json = json_object_from_file("/etc/wayshare/config.json")) != NULL)
		*r_config_json = config_json;

	if (config_path)
		free(config_path);

	if (!config_json)
		return WSE_CONFIG_CONFIG_FILEF;
	else
		return WS_OK;
}

ecode_t get_uploader_json_name(struct json_object **r_uploader_json,
							   struct json_object *config_json, const char *name)
{
	struct json_object *uploaders_json;
	uploaders_json = json_object_object_get(config_json, "uploaders");
	if (!uploaders_json || !json_object_is_type(uploaders_json, json_type_object))
		return WSE_CONFIG_NJSON_OBJECT;

	struct json_object *uploader_json;
	uploader_json = json_object_object_get(config_json, name);
	if (!uploader_json || !json_object_is_type(uploader_json, json_type_object))
		return WSE_CONFIG_NJSON_OBJECT;

	*r_uploader_json = uploader_json;
	return WS_OK;
}

const static char *get_type_key(enum uploader_type type)
{
	switch (type) {
	case UPLOADER_TYPE_OTHER:
		return "other_uploader";

	case UPLOADER_TYPE_TEXT:
		return "text_uploader";

	case UPLOADER_TYPE_IMAGE:
		return "image_uploader";

	case UPLOADER_TYPE_AUDIO:
		return "audio_uploader";

	case UPLOADER_TYPE_VIDEO:
		return "video_uploader";

	default:
		return "other_uploader";
	}
}

ecode_t get_uploader_json_type(struct json_object **r_uploader_json,
							   struct json_object *config_json, enum uploader_type type)
{
	struct json_object *wayshare_json;
	wayshare_json = json_object_object_get(config_json, "wayshare");
	if (!wayshare_json || !json_object_is_type(wayshare_json, json_type_object))
		return WSE_CONFIG_NJSON_OBJECT;

	struct json_object *uploader_name_json;
	uploader_name_json = json_object_object_get(config_json, get_type_key(type));
	if (!uploader_name_json || !json_object_is_type(uploader_name_json, json_type_string))
		return WSE_CONFIG_NJSON_OBJECT;

	const char *uploader_name;
	uploader_name = json_object_get_string(uploader_name_json);
	struct json_object *uploader_json;
	ecode_t code;
	if ((code = get_uploader_json_name(&uploader_json, config_json, uploader_name)))
		return code;

	*r_uploader_json = uploader_json;
	return WS_OK;
}

static inline bool is_prefix_invalid(const char *prefix, size_t spn)
{
	/* clang-format off */ /* weird formatting. */
	return spn < 1 || spn > 3
		|| (spn == 2) ? strncmp(prefix, ".[", 2) && strncmp(prefix, "[\"", 2) : 0
		|| (spn == 3) ? strncmp(prefix, ".[\"", 3) : 0;
	/* clang-format on */
}

static inline bool is_suffix_invalid(const char *suffix, size_t spn, char type)
{
	/* clang-format off */ /* weird formatting. */
	return spn > 2 
		|| (type == '[') ? spn != 1 || strncmp(suffix, "]", 1) : 0 
		|| (type == '"') ? spn != 2 || strncmp(suffix, "\"]", 2) : 0;
	/* clang-format on */
}

static ecode_t handle_token(struct json_object **trv_json, char *token, const char type)
{
	if (*trv_json == NULL)
		return WSE_CONFIG_NJSON_OBJECT; /* json error. */

	if (type == '[') {
		long index;
		index = strtol(token, NULL, 10);
		if (errno == ERANGE)
			return WSE_CONFIG_JSON_PATHF; /* out of range error. */

		if (!json_object_is_type(*trv_json, json_type_array))
			return WSE_CONFIG_JSON_PATHF; /* json error. */

		unsigned long arrlen;
		arrlen = json_object_array_length(*trv_json);

		/* modulate to allow negative indexs in the syntax. */
		index = ((index % arrlen) + arrlen) % arrlen;
		*trv_json = json_object_array_get_idx(*trv_json, index);
	} else {
		if (!json_object_is_type(*trv_json, json_type_object))
			return WSE_CONFIG_JSON_PATHF; /* json error. */

		*trv_json = json_object_object_get(*trv_json, token);
	}

	return WS_OK;
}

ecode_t json_get_from_path(struct json_object **r_json, struct json_object *src_json,
						   const char *path)
{
	char pathbuf[strlen(path)];
	strcpy(pathbuf, path);

	struct json_object *trv_json = src_json;
	char *c_token = NULL;
	char *n_token = pathbuf;
	char *p_ptr;
	char p_char;

	while (n_token != NULL) {
		c_token = n_token;
		size_t c_prespn = strspn(c_token, ".[\"");
		if (is_prefix_invalid(c_token, c_prespn))
			return WSE_CONFIG_JSON_PATHF; /* syntax error. */

		c_token += c_prespn;
		char c_type = c_token[-1];

		/* check for closing quatation mark/bracket. */
		if (c_type == '.')
			n_token = strpbrk(c_token, ".[");
		else if (c_type == '[')
			n_token = strchr(c_token, ']');
		else if (c_type == '"')
			n_token = strchr(c_token, '"');
		else
			return WSE_CONFIG_JSON_PATHF; /* syntax error. */

		/* check for a valid suffix. */
		size_t c_sufspn;
		if (c_type != '.') {
			if (n_token == NULL)
				return WSE_CONFIG_JSON_PATHF; /* syntax error. */

			c_sufspn = strspn(n_token, "\"]");
			if (is_suffix_invalid(n_token, c_sufspn, c_type))
				return WSE_CONFIG_JSON_PATHF; /* syntax error. */
		}

		if (n_token != NULL) {
			/* terminate the token and save the prev state. */
			p_ptr = n_token;
			p_char = n_token[0];
			n_token[0] = '\0';
		}

		ecode_t code;
		if ((code = handle_token(&trv_json, c_token, c_type)))
			return code;

		/* return to prev state. */
		if (n_token != NULL)
			p_ptr[0] = p_char;

		/* move from current suffix to next prefix if a suffix exists. */
		if (c_type != '.')
			n_token += c_sufspn;
	}

	*r_json = trv_json;
	return WS_OK;
}

static char *get_date_str()
{
	time_t unix_time;
	time(&unix_time);

	struct tm *tm_time;
	tm_time = localtime(&unix_time);

	int len = snprintf(NULL, 0, "%i_%i_%i", tm_time->tm_year + 1900, tm_time->tm_mon + 1, tm_time->tm_mday);
	char *str = malloc(len + 1);
	snprintf(str, len + 1, "%i_%i_%i", tm_time->tm_year + 1900, tm_time->tm_mon + 1, tm_time->tm_mday);
	return str;
}

static char *get_time_str()
{
	time_t unix_time;
	time(&unix_time);

	struct tm *tm_time;
	tm_time = localtime(&unix_time);

	int len = snprintf(NULL, 0, "%i_%i_%i", tm_time->tm_hour, tm_time->tm_min, tm_time->tm_sec);
	char *str = malloc(len + 1);
	snprintf(str, len + 1, "%i_%i_%i", tm_time->tm_hour, tm_time->tm_min, tm_time->tm_sec);
	return str;
}

static char *get_epoch_str()
{
	time_t unix_time;
	time(&unix_time);

	int len = snprintf(NULL, 0, "%lli", unix_time);
	char *str = malloc(len + 1);
	snprintf(str, len + 1, "%lli", unix_time);
	return str;
}

static ecode_t get_value_str(const char **r_value, const char *key, struct json_object *response_json)
{
	if (strcmp(key, "") == 0) {
		*r_value = strdup("$");
	} else if (strcmp(key, "epoch") == 0) {
		*r_value = get_epoch_str();
	} else if (strcmp(key, "time") == 0) {
		*r_value = get_time_str();
	} else if (strcmp(key, "date") == 0) {
		*r_value = get_date_str();
	} else if (strcmp(key, "home") == 0) {
		const char *home_path;
		get_home_path(&home_path);
		*r_value = strdup(home_path);
	} else if (strncmp(key, "json:", 5) == 0 && response_json != NULL) {
		struct json_object *var_json;
		json_get_from_path(&var_json, response_json, key + 5);
		*r_value = strdup(json_object_get_string(var_json));
	} else
		return WSE_CONFIG_NVAR_VALUE;

	return WS_OK;
}

ecode_t format_vstr(char **r_str, const char *vstr, struct json_object *response_json)
{
	/* parse string. */
	char vstrbuf[strlen(vstr) + 1];
	strcpy(vstrbuf, vstr);

	/* write string. */
	int32_t t_delta = 0;
	char *strbuf = malloc(strlen(vstr) + 1);
	strcpy(strbuf, vstr);

	char *c_token = NULL;
	char *n_token = strchr(vstrbuf, '$');

	while (n_token != NULL) {
		c_token = n_token;
		c_token++;
		n_token = strchr(c_token, '$');
		if (n_token == NULL)
			return WSE_CONFIG_NVAR_CLOSE; /* syntax error. */

		n_token[0] = '\0';
		n_token++;
		n_token = strchr(n_token, '$');

		const char *value = NULL;
		if (get_value_str(&value, c_token, response_json))
			return WSE_CONFIG_NVAR_VALUE; /* invalid variable key. */

		size_t keylen = strlen(c_token);
		size_t vallen = strlen(value);
		int32_t delta = vallen - keylen - 2; /* -2 for the '$'s. */
		t_delta += delta;

		/* realloc to new +delta size. */
		if (delta > 0)
			strbuf = realloc(strbuf, strlen(strbuf) + delta + 1);

		/* get pointer to the current variable in the write string. */
		char *c_strbuf = strbuf + t_delta + (c_token - vstrbuf);

		/* move string segment from after the variable to after the future value. */
		memmove(c_strbuf + vallen, c_strbuf + keylen + 2, strlen(c_strbuf + keylen + 2) + 1);
		memmove(c_strbuf, value, strlen(value));

		/* realloc to new -delta size. */
		if (delta < 0)
			strbuf = realloc(strbuf, strlen(strbuf) + delta + 1);

		if (value)
			free(value);
	}

	*r_str = strbuf;
	return WS_OK;
}
