#ifndef WS_POST_H
#define WS_POST_H

#include <fcntl.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

#include <curl/curl.h>
#include <json-c/json.h>

#include "config.h"
#include "utils.h"

ecode_t request_generic_file(json_object **r_response_json, json_object *uploader_json,
							 void *file_data, size_t file_size, const char *file_name);

ecode_t request_get(json_object **r_response_json, json_object *uploader_json);

ecode_t request_post_file(json_object **r_response_json, json_object *uploader_json,
						  void *file_data, size_t file_size, const char *file_name);

ecode_t request_put_file(json_object **r_response_json, json_object *uploader_json, void *file_data,
						 size_t file_size, const char *file_name);

#endif // WS_POST_H
