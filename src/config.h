#ifndef WS_CONFIG_H
#define WS_CONFIG_H

#include <stdint.h>
#include <string.h>
#include <time.h>

#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <pwd.h>
#include <json-c/json.h>

#include "utils.h"
#include "wayshare.h"
#include "logger.h"

typedef json_object* json_object_t;

// +-------------------------+
// | file & dir manipulation |
// +-------------------------+

ecode_t get_default_config_path(char **r_config_file_path);

ecode_t create_default_config_file(const char *config_file_path);

ecode_t prompt_default_config_file(const char *config_file_path);

// +---------------+
// | json handling |
// +---------------+

ecode_t config_json_object_get_from_path(json_object_t *r_config_json, char *config_file_path);

ecode_t config_json_object_get_uploader_json_object(json_object_t *r_uploader_json, json_object_t config_json, const char *uploader_name);

ecode_t config_json_object_get_image_uploader_json_object(json_object_t *r_uploader_json, json_object_t config_json);

ecode_t json_object_get_from_json_path(json_object_t *r_json_value, const char *json_path, json_object_t json);

// +-------------------+
// | string formatting |
// +-------------------+

ecode_t string_insert_segment(char **r_fin_string, const char *dest_string, size_t position, const char *src_string);

ecode_t string_remove_segment(char **r_fin_string, const char *dest_string, size_t position, size_t length);

ecode_t get_config_variable_string(char **r_value, const char *name);

ecode_t get_config_formatted_string(char **r_formatted_string, const char *string);

#endif // WS_CONFIG_H
