#include "config.h"

ecode_t get_default_config_path(char **r_config_file_path) {
    char *config_dir_path = getenv("XDG_CONFIG_HOME");
    if (!config_dir_path) {
        const char *config_home_file_path = "/.config/wayshare/config.json";
        const char *home_dir_path;
        ecode_t code = get_home_path(&home_dir_path);
        if (!code)
            return code;

        char *config_file_path =
            (char*)malloc(strlen(home_dir_path) + strlen(config_home_file_path) + 1);

        strcpy(config_file_path, home_dir_path);
        strcat(config_file_path, config_home_file_path);
        *r_config_file_path = config_file_path;
        return WS_OK;
    }
    else {
        const char *config_config_file_path = "/wayshare/config.json";
        char *config_file_path =
            (char*)malloc(strlen(config_dir_path) + strlen(config_config_file_path) + 1);

        strcpy(config_file_path, config_dir_path);
        strcat(config_file_path, config_config_file_path);
        *r_config_file_path = config_file_path;
        return WS_OK;
    }
}

ecode_t create_default_config_file(const char *config_file_path) {
    const char *default_config =
        "{\n"
        "    \"wayshare\": {\n"
        "        \"image_uploader\": \"uploder_name\"\n"
        "    },\n"
        "    \"screenshot\": {\n"
        "        \"file_name\": \"{source}_{unix_time}\",\n"
        "        \"file_local_dir\": \"{home}/Pictures/Screenshots\",\n"
        "        \"file_format\": \"PNG\",\n"
        "\n"
        "        \"compress_toggle\": false,\n"
        "        \"compress_format\": \"JPEG\",\n"
        "        \"compress_min_kib\": 2048,\n"
        "        \"compress_level\": 5\n"
        "    },\n"
        "    \"uploaders\": {\n"
        "        \"uploder_name\": {\n"
        "            \"upload_type\": \"POST\",\n"
        "            \"upload_url\": \"https://api.imageuploder.sh/upload\",\n"
        "            \"upload_querys\": {\n"
        "                \"key\": \"R4nD0Mk3Ys7R1Ng\"\n"
        "            },\n"
        "            \"upload_headers\": {\n"
        "\n"
        "            },\n"
        "            \"file_url\": \"{json:url}\"\n"
        "        }\n"
        "    }\n"
        "}";

    char config_dir_path[strlen(config_file_path) + 1];
    strcpy(config_dir_path, config_file_path);
    *strrchr(config_dir_path, '/') = '\0'; // change last '/' to the null byte '\0'.

    mkdir(config_dir_path, S_IRWXU | S_IRWXG | S_IRWXO);
    int config_fd = open(config_file_path, O_WRONLY | O_CREAT | O_EXCL);
    write(config_fd, (void*)default_config, strlen(default_config));
    close(config_fd);
    chmod(config_file_path, S_IRWXU | S_IRWXG | S_IRWXO);

    return WS_OK;
}

ecode_t prompt_default_config_file(const char *config_file_path) {
    printf(
        "No configuration file found at: %s,\n"
        "do you wish to create a default configuration file at that location? [y/N]: ",
        config_file_path
    );

    char reply;
    scanf("%c", &reply);
    fflush(stdin);
    if (reply == 'y' || reply == 'Y') {
        create_default_config_file(config_file_path);
        return WS_OK;
    }
    else {
        return WSE_CONFIG_NCONFIG_FILE;
    }
}

ecode_t config_json_object_get_from_path(json_object_t *r_config_json, char *config_file_path) {
    bool path_alloced = false;
    if (!config_file_path) {
        path_alloced = true;
        ecode_t code = get_default_config_path(&config_file_path);
        if (!code)
            return code;
    }

    WS_LOGF(WS_SEV_INFO, "attempting to open config file: %s\n", config_file_path);
    struct stat buf;
    if (stat(config_file_path, &buf) == -1) {
        ecode_t code = prompt_default_config_file(config_file_path);
        if (!code)
            return code;
    }

    json_object *config_json = json_object_from_file(config_file_path);
    if (path_alloced)
        free(config_file_path);

    if (!config_json)
        return WSE_CONFIG_JSON_PARSEF;

    *r_config_json = config_json;
    return WS_OK;
}

ecode_t config_json_object_get_uploader_json_object(json_object_t *r_uploader_json, json_object_t config_json, const char *uploader_name) {
    json_object_t uploaders = json_object_object_get(config_json, "uploaders");
    if (!uploaders)
        return WSE_CONFIG_NJSON_OBJECT;

    json_object_t uploader = json_object_object_get(uploaders, uploader_name);
    if (!uploader)
        return WSE_CONFIG_NJSON_OBJECT;

    *r_uploader_json = uploader;
    return WS_OK;
}

ecode_t config_json_object_get_image_uploader_json_object(json_object_t *r_uploader_json, json_object_t config_json) {
    json_object_t wayshare = json_object_object_get(config_json, "wayshare");
    if (!wayshare)
        return WSE_CONFIG_NJSON_OBJECT;

    json_object_t image_uploader = json_object_object_get(config_json, "image_uploader");
    if (!image_uploader)
        return WSE_CONFIG_NJSON_OBJECT;

    if (json_object_get_type(image_uploader) != json_type_string)
        return -1;

    const char *image_uplaoder_string = json_object_get_string(image_uploader);
    const uint64_t image_uplaoder_string_len = json_object_get_string_len(image_uploader);

    char image_uplaoder_string_ex[image_uplaoder_string_len];
    strncpy(image_uplaoder_string_ex, image_uplaoder_string, image_uplaoder_string_len);

    json_object_t uploader;
    ecode_t code = config_json_object_get_uploader_json_object(&uploader, config_json, image_uplaoder_string_ex);
    if (!uploader || !code)
        return WSE_CONFIG_NJSON_OBJECT;

    *r_uploader_json = uploader;
    return WS_OK;
}

ecode_t json_object_get_from_json_path(json_object_t *r_json_value, const char *json_path, json_object_t json) {
    json_object_t cur_object = json;
    uint8_t cur_object_type = 0; // 0 - key, 1 - index
    size_t cur_object_start = 0;
    size_t cur_object_len;

    for (size_t i_char = 0; i_char < strlen(json_path) + 1; i_char++) {
        if (json_path[i_char] != '.' && json_path[i_char] != '[' && json_path[i_char] != '\0')
            continue;

        cur_object_len = i_char - cur_object_start - cur_object_type;
        char cur_object_name[cur_object_len + 1];
        strncpy(cur_object_name, (json_path + cur_object_start), cur_object_len);
        cur_object_name[cur_object_len] = '\0';

        cur_object = (cur_object_type)
            ? json_object_object_get(cur_object, cur_object_name)
            : json_object_array_get_idx(cur_object, strtoull(cur_object_name, NULL, 10));

        if (!cur_object)
            return WSE_CONFIG_NJSON_OBJECT;

        if (json_path[i_char] == '\0')
            break;

        cur_object_start = i_char + cur_object_type + 1;
        cur_object_type = (json_path[i_char] == '[');
    }

    *r_json_value = cur_object;
    return WS_OK;
}

ecode_t string_insert_segment(char **r_fin_string, const char *dest_string, size_t position, const char *src_string) {
    const size_t dest_len = strlen(dest_string);
    const size_t src_len = strlen(src_string);
    const size_t fin_len = dest_len + src_len;
    if (dest_len < position)
        return -1;

    char *fin_string = (char*)malloc(fin_len + 1);
    strncpy(fin_string, dest_string, position);
    fin_string[position] = '\0';
    strcat(fin_string, src_string);
    strcat(fin_string, (dest_string + position));

    *r_fin_string = fin_string;
    return WS_OK;
}

ecode_t string_remove_segment(char **r_fin_string, const char *dest_string, size_t position, size_t length) {
    const size_t dest_len = strlen(dest_string);
    const size_t fin_len = dest_len - length;
    if (dest_len < position)
        return -1;

    if (dest_len - position < length)
        return -1;

    char *fin_string = (char*)malloc(fin_len + 1);
    strncpy(fin_string, dest_string, position);
    fin_string[position] = '\0';
    strcat(fin_string, (dest_string + position + length));

    *r_fin_string = fin_string;
    return WS_OK;
}

ecode_t get_config_variable_string(char **r_value, const char *name) {
    if (strcmp(name, "") == 0) {
        char *value_str = (char*)malloc(2);
        strcpy(value_str, "$");

        *r_value = value_str;
        return WS_OK;
    }

    if (strcmp(name, "user_name") == 0) {
        const char *value_str;
        get_user_name(&value_str);

        *r_value = strdup(value_str);
        return WS_OK;
    }

    if (strcmp(name, "home_dir") == 0) {
        const char *value_str;
        get_home_path(&value_str);

        *r_value = strdup(value_str);
        return WS_OK;
    }

    if (strcmp(name, "unix_time") == 0) {
        time_t unix_time = time(NULL);

        size_t value_str_len = snprintf(NULL, 0, "%llu", unix_time);
        char *value_str = (char*)malloc(value_str_len + 1);
        snprintf(value_str, value_str_len + 1, "%llu", unix_time);

        *r_value = value_str;
        return WS_OK;
    }

    if (strcmp(name, "utc_time") == 0) {
        time_t unix_time = time(NULL);
        struct tm *utc_time = gmtime(&unix_time);

        size_t value_str_len = snprintf(NULL, 0, "%.4u_%.2u_%.2u",
            utc_time->tm_year + 1900, utc_time->tm_mon + 1, utc_time->tm_mday);

        char *value_str = (char*)malloc(value_str_len + 1);
        snprintf(value_str, value_str_len + 1, "%.4u_%.2u_%.2u",
            utc_time->tm_year + 1900, utc_time->tm_mon + 1, utc_time->tm_mday);

        *r_value = value_str;
        return WS_OK;
    }

    if (strcmp(name, "local_time") == 0) {
        time_t unix_time = time(NULL);
        struct tm *utc_time = localtime(&unix_time);

        size_t value_str_len = snprintf(NULL, 0, "%.4u_%.2u_%.2u",
            utc_time->tm_year + 1900, utc_time->tm_mon + 1, utc_time->tm_mday);

        char *value_str = (char*)malloc(value_str_len + 1);
        snprintf(value_str, value_str_len + 1, "%.4u_%.2u_%.2u",
            utc_time->tm_year + 1900, utc_time->tm_mon + 1, utc_time->tm_mday);

        *r_value = value_str;
        return WS_OK;
    }

    // default
    char *value_str = (char*)malloc(1);
    strcpy(value_str, "");

    *r_value = value_str;
    return WSE_CONFIG_NVAR_VALUE;
}

ecode_t config_format_string(char **r_formatted_string, const char *string) {
    char *formatted_string = strdup(string);
    bool is_within_var = false;
    int64_t total_var_delta = 0;
    size_t cur_var_start;
    size_t cur_var_end;
    size_t cur_var_len;

    for (size_t i_char = 0; i_char < strlen(string); i_char++) {
        const int64_t i_char_delta = i_char + total_var_delta;

        if (formatted_string[i_char_delta] != '$')
            continue;

        if (!is_within_var) {
            is_within_var = true;
            cur_var_start = i_char_delta;
            continue;
        }

        if (is_within_var) {
            is_within_var = false;
            cur_var_end = i_char_delta;
            cur_var_len = cur_var_end - cur_var_start + 1;

            size_t cur_var_name_len = cur_var_len - 2; // remove the 2 '$'s for the variable name length.
            char cur_var_name[cur_var_name_len + 1];
            strncpy(cur_var_name, (formatted_string + cur_var_start + 1), cur_var_name_len);
            cur_var_name[cur_var_name_len] = '\0';

            char *cur_var_value;
            ecode_t code = get_config_variable_string(&cur_var_value, cur_var_name);
            if (!code) {
                free(formatted_string);
                return code;
            }

            const uint32_t cur_var_value_len = strlen(cur_var_value);
            char *new_formatted_string;
            string_remove_segment(&new_formatted_string, formatted_string, cur_var_start, cur_var_len);
            free(formatted_string);
            formatted_string = new_formatted_string;
            string_insert_segment(&new_formatted_string, formatted_string, cur_var_start, cur_var_value);
            free(formatted_string);
            formatted_string = new_formatted_string;

            free(cur_var_value);
            total_var_delta += cur_var_value_len - cur_var_len;
            continue;
        }
    }

    if (is_within_var) {
        free(formatted_string);
        return WSE_CONFIG_NVAR_CLOSE; // no closing $ for var.
    }

    *r_formatted_string = formatted_string;
    return WS_OK;
}