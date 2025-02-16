#ifndef WS_POST_H
#define WS_POST_H

#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#include <curl/curl.h>
#include <json-c/json.h>

#include "utils.h"
#include "config.h"

// libcurl... why...
typedef CURL                curl_t;
typedef CURLcode            curl_code_t;
typedef CURLoption          curl_option_t;
typedef curl_mime           curl_mime_t;
typedef curl_mimepart       curl_mimepart_t;
typedef struct curl_slist   curl_slist_t;
typedef CURLU               curl_url_t;

ecode_t request_generic_file(json_object_t *r_response_json, json_object_t uploader_json,
        void *file_data, size_t file_size, const char *file_name);

ecode_t request_get(json_object_t *r_response_json, json_object_t uploader_json);

ecode_t request_post_file(json_object_t *r_response_json, json_object_t uploader_json,
        void *file_data, size_t file_size, const char *file_name);

ecode_t request_put_file(json_object_t *r_response_json, json_object_t uploader_json,
        void *file_data, size_t file_size, const char *file_name);

#endif // WS_POST_H