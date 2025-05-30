#ifndef WS_CONFIG_H
#define WS_CONFIG_H

#include <json-c/json.h>

#include "wayshare.h"

enum uploader_type {
	UPLOADER_TYPE_OTHER = 0,
	UPLOADER_TYPE_TEXT,
	UPLOADER_TYPE_IMAGE,
	UPLOADER_TYPE_AUDIO,
	UPLOADER_TYPE_VIDEO
};

ecode_t get_config_json(struct json_object **r_config_json, const char *custom_path);

ecode_t get_uploader_json_name(struct json_object **r_uploader_json,
							   struct json_object *config_json, const char *name);

ecode_t get_uploader_json_type(struct json_object **r_uploader_json,
							   struct json_object *config_json, enum uploader_type type);

ecode_t json_get_from_path(struct json_object **r_json, struct json_object *src_json,
						   const char *path);

ecode_t format_variable_string(char **r_str, const char *vstr, struct json_object *response_json);

#endif // WS_CONFIG_H
