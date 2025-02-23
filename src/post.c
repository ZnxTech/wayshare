#include "post.h"

static size_t response_write_callback(char *data, size_t data_size, size_t data_count, void *darray)
{
	const char null_byte = '\0';
	darray_pop((struct darray *)darray);
	darray_append_array((struct darray *)darray, data, data_size * data_count);
	darray_append((struct darray *)darray, &null_byte);
}

static ecode_t get_headers_slist(struct curl_slist **r_slist, json_object_t headers_json)
{
	struct curl_slist *slist = NULL;
	json_object_object_foreach(headers_json, key_str, val_json) {
		if (!json_object_is_type(val_json, json_type_string))
			continue;

		const char *val_str = json_object_get_string(val_json);
		size_t header_len = snprintf(NULL, 0, "%s: %s", key_str, val_str);
		char header_str[header_len + 1];
		snprintf(header_str, header_len + 1, "%s: %s", key_str, val_str);

		slist = curl_slist_append(slist, header_str);
	}

	*r_slist = slist;
	return WS_OK;
}

static ecode_t set_mime_forms(curl_mime *mime, json_object_t forms_json)
{
	json_object_object_foreach(forms_json, key_str, val_json) {
		if (!json_object_is_type(val_json, json_type_string))
			continue;

		const char *val_str = json_object_get_string(val_json);
		curl_mimepart *mime_part = curl_mime_addpart(mime);
		curl_mime_name(mime_part, key_str);
		curl_mime_data(mime_part, val_str, CURL_ZERO_TERMINATED);
		curl_mime_type(mime_part, "application/form-data");
	}

	return WS_OK;
}

static ecode_t append_curl_url_querys(CURLU *url, json_object_t querys_json)
{
	json_object_object_foreach(querys_json, key_str, val_json) {
		if (!json_object_is_type(val_json, json_type_string))
			continue;

		const char *val_str = json_object_get_string(val_json);
		size_t query_len = snprintf(NULL, 0, "%s=%s", key_str, val_str);
		char query_str[query_len + 1];
		snprintf(query_str, query_len + 1, "%s=%s", key_str, val_str);
		curl_url_set(url, CURLUPART_QUERY, query_str, CURLU_APPENDQUERY);
	};

	return WS_OK;
}

static ecode_t get_curl_url(CURLU **r_url, json_object_t url_json, json_object_t querys_json)
{
	const char *uploader_url_str = json_object_get_string(url_json);
	CURLU *url = curl_url();
	curl_url_set(url, CURLUPART_URL, uploader_url_str, 0);

	if (json_object_is_type(querys_json, json_type_object))
		append_curl_url_querys(url, querys_json);

	*r_url = url;
	return WS_OK;
}

ecode_t request_generic_file(json_object_t *r_response_json, json_object_t uploader_json,
							 void *file_data, size_t file_size, const char *file_name)
{
	json_object_t upload_method_json = json_object_object_get(uploader_json, "upload_method");

	if (!json_object_is_type(upload_method_json, json_type_string))
		return WSE_POST_NSETTING;

	const char *upload_method_str = json_object_get_string(upload_method_json);

	if (strcmp(upload_method_str, "GET") == 0) {
		ecode_t code;
		code = request_get(r_response_json, uploader_json);
		return code;
	}

	if (strcmp(upload_method_str, "POST") == 0) {
		ecode_t code;
		code = request_post_file(r_response_json, uploader_json, file_data, file_size, file_name);
		return code;
	}

	if (strcmp(upload_method_str, "PUT") == 0) {
		ecode_t code;
		code = request_post_file(r_response_json, uploader_json, file_data, file_size, file_name);
		return code;
	}

	return WS_OK;
}

ecode_t request_get(json_object_t *r_response_json, json_object_t uploader_json)
{
	CURL *curl = curl_easy_init();
	if (!curl)
		return WSE_POST_CURLF;

	curl_mime *mime = curl_mime_init(curl);
	if (!mime)
		return WSE_POST_CURLF;

	// set upload headers
	json_object_t headers_json = json_object_object_get(uploader_json, "upload_querys");
	if (json_object_is_type(headers_json, json_type_object)) {
		struct curl_slist *header_slist;
		get_headers_slist(&header_slist, headers_json);
		if (!header_slist)
			curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header_slist);
	}
	// upload url and querys
	json_object_t url_json = json_object_object_get(uploader_json, "upload_url");
	json_object_t querys_json = json_object_object_get(uploader_json, "upload_querys");
	if (!json_object_is_type(url_json, json_type_string)) {
		curl_easy_cleanup(curl);
		return WSE_POST_NSETTING;
	}

	CURLU *url;
	get_curl_url(&url, url_json, querys_json);
	curl_easy_setopt(curl, CURLOPT_CURLU, url);

	// response write handling
	struct darray *response_data = darray_init(1, 16);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, response_write_callback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, response_data);

	// perform request
	curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
#ifdef DEBUG
	WS_LOGF(WS_SEV_INFO, "libcurl request verbose:\n");
	curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
#endif
	CURLcode code = curl_easy_perform(curl);

	curl_easy_cleanup(curl);
	if (!code)
		return WSE_POST_REQUESTF;

	// json parsing
	json_object_t response_json;
	response_json = json_tokener_parse((const char *)response_data->data);
	darray_free(response_data);
	if (!response_json)
		return WSE_POST_RESPONSEF;

	*r_response_json = response_json;
	return WS_OK;
}

ecode_t request_post_file(json_object_t *r_response_json, json_object_t uploader_json,
						  void *file_data, size_t file_size, const char *file_name)
{
	CURL *curl = curl_easy_init();
	if (!curl)
		return WSE_POST_CURLF;

	curl_mime *mime = curl_mime_init(curl);
	if (!mime)
		return WSE_POST_CURLF;

	// set file form
	json_object_t file_form_json = json_object_object_get(uploader_json, "upload_file_form");
	if (!file_form_json && json_object_get_type(file_form_json) != json_type_string) {
		curl_easy_cleanup(curl);
		return WSE_POST_NSETTING;
	}

	curl_mimepart *mime_part = curl_mime_addpart(mime);
	curl_mime_name(mime_part, json_object_get_string(file_form_json));
	curl_mime_data(mime_part, (char *)file_data, file_size);
	curl_mime_type(mime_part, "application/form-data");
	curl_mime_filename(mime_part, file_name);

	// set upload headers
	json_object_t headers_json = json_object_object_get(uploader_json, "upload_querys");
	if (json_object_is_type(headers_json, json_type_object)) {
		struct curl_slist *header_slist;
		get_headers_slist(&header_slist, headers_json);
		if (!header_slist)
			curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header_slist);
	}
	// set upload forms
	json_object_t forms_json = json_object_object_get(uploader_json, "upload_forms");
	if (json_object_is_type(forms_json, json_type_object))
		set_mime_forms(mime, forms_json);

	// upload url and querys
	json_object_t url_json = json_object_object_get(uploader_json, "upload_url");
	json_object_t querys_json = json_object_object_get(uploader_json, "upload_querys");
	if (!json_object_is_type(url_json, json_type_string)) {
		curl_easy_cleanup(curl);
		return WSE_POST_NSETTING;
	}

	CURLU *url;
	get_curl_url(&url, url_json, querys_json);
	curl_easy_setopt(curl, CURLOPT_CURLU, url);

	// response write handling
	struct darray *response_data = darray_init(1, 16);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, response_write_callback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, response_data);

	// perform request
	curl_easy_setopt(curl, CURLOPT_MIMEPOST, mime);
#ifdef DEBUG
	WS_LOGF(WS_SEV_INFO, "libcurl request verbose:\n");
	curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
#endif
	CURLcode code = curl_easy_perform(curl);

	curl_easy_cleanup(curl);
	if (!code)
		return WSE_POST_REQUESTF;

	// json parsing
	json_object_t response_json;
	response_json = json_tokener_parse((const char *)response_data->data);
	darray_free(response_data);
	if (!response_json)
		return WSE_POST_RESPONSEF;

	*r_response_json = response_json;
	return WS_OK;
}

ecode_t request_put_file(json_object_t *r_response_json, json_object_t uploader_json,
						 void *file_data, size_t file_size, const char *file_name)
{
	CURL *curl = curl_easy_init();
	if (!curl)
		return WSE_POST_CURLF;

	curl_mime *mime = curl_mime_init(curl);
	if (!mime)
		return WSE_POST_CURLF;

	// set file form
	json_object_t file_form_json = json_object_object_get(uploader_json, "upload_file_form");
	if (!file_form_json && json_object_get_type(file_form_json) != json_type_string) {
		curl_easy_cleanup(curl);
		return WSE_POST_NSETTING;
	}

	curl_mimepart *mime_part = curl_mime_addpart(mime);
	curl_mime_name(mime_part, json_object_get_string(file_form_json));
	curl_mime_data(mime_part, (char *)file_data, file_size);
	curl_mime_type(mime_part, "application/form-data");
	curl_mime_filename(mime_part, file_name);

	// set upload headers
	json_object_t headers_json = json_object_object_get(uploader_json, "upload_querys");
	if (json_object_is_type(headers_json, json_type_object)) {
		struct curl_slist *header_slist;
		get_headers_slist(&header_slist, headers_json);
		if (!header_slist)
			curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header_slist);
	}
	// set upload forms
	json_object_t forms_json = json_object_object_get(uploader_json, "upload_forms");
	if (json_object_is_type(forms_json, json_type_object))
		set_mime_forms(mime, forms_json);

	// upload url and querys
	json_object_t url_json = json_object_object_get(uploader_json, "upload_url");
	json_object_t querys_json = json_object_object_get(uploader_json, "upload_querys");
	if (!json_object_is_type(url_json, json_type_string)) {
		curl_easy_cleanup(curl);
		return WSE_POST_NSETTING;
	}

	CURLU *url;
	get_curl_url(&url, url_json, querys_json);
	curl_easy_setopt(curl, CURLOPT_CURLU, url);

	// response write handling
	struct darray *response_data = darray_init(1, 16);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, response_write_callback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, response_data);

	// perform request
	curl_easy_setopt(curl, CURLOPT_MIMEPOST, mime);
	curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
#ifdef DEBUG
	WS_LOGF(WS_SEV_INFO, "libcurl request verbose:\n");
	curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
#endif
	CURLcode code = curl_easy_perform(curl);

	curl_easy_cleanup(curl);
	if (!code)
		return WSE_POST_REQUESTF;

	// json parsing
	json_object_t response_json;
	response_json = json_tokener_parse((const char *)response_data->data);
	darray_free(response_data);
	if (!response_json)
		return WSE_POST_RESPONSEF;

	*r_response_json = response_json;
	return WS_OK;
}
