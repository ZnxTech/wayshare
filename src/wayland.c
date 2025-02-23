#include "wayland.h"

// +----------------------------+
// | wayland output event calls |
// +----------------------------+

static void wl_output_event_geometry(void *data, struct wl_output *wl_output,
									 int32_t x, int32_t y, int32_t width_mm, int32_t height_mm,
									 int32_t subpixel, const char *make, const char *model,
									 int32_t transform)
{
	struct wl_output_data *output_data = (struct wl_output_data *)data;
	output_data->x = x;
	output_data->y = y;
	output_data->transform = transform;
}

static void wl_output_event_mode(void *data, struct wl_output *wl_output,
								 uint32_t flags, int32_t width, int32_t height, int32_t refresh)
{
	struct wl_output_data *output_data = (struct wl_output_data *)data;
	output_data->width = width;
	output_data->height = height;
}

static void wl_output_event_done(void *data, struct wl_output *wl_output)
{

}

static void wl_output_event_scale(void *data, struct wl_output *wl_output, int32_t factor)
{
	struct wl_output_data *output_data = (struct wl_output_data *)data;
	output_data->scale_factor = factor;
}

static void wl_output_event_name(void *data, struct wl_output *wl_output, const char *name)
{

}

static void wl_output_event_description(void *data, struct wl_output *wl_output,
										const char *description)
{

}

static const struct wl_output_listener output_listener = {
	.geometry = wl_output_event_geometry,
	.mode = wl_output_event_mode,
	.done = wl_output_event_done,
	.scale = wl_output_event_scale,
	.name = wl_output_event_name,
	.description = wl_output_event_description
};

// +------------------------+
// | xdg output event calls |
// +------------------------+

static void xdg_output_event_logical_position(void *data, struct zxdg_output_v1 *zxdg_output_v1,
											  int32_t x, int32_t y)
{
	struct wl_output_data *output_data_t = (struct wl_output_data *)data;
	output_data_t->x = x;
	output_data_t->y = y;
}

static void xdg_output_event_logical_size(void *data, struct zxdg_output_v1 *zxdg_output_v1,
										  int32_t width, int32_t height)
{
	struct wl_output_data *output_data_t = (struct wl_output_data *)data;
	output_data_t->width = width;
	output_data_t->height = height;
}

static void xdg_output_event_done(void *data, struct zxdg_output_v1 *zxdg_output_v1)
{

}

static void xdg_output_event_name(void *data, struct zxdg_output_v1 *zxdg_output_v1,
								  const char *name)
{

}

static void xdg_output_event_description(void *data, struct zxdg_output_v1 *zxdg_output_v1,
										 const char *description)
{

}

static const struct zxdg_output_v1_listener xdg_output_listener = {
	.logical_position = xdg_output_event_logical_position,
	.logical_size = xdg_output_event_logical_size,
	.done = xdg_output_event_done,
	.name = xdg_output_event_name,
	.description = xdg_output_event_description
};

// +---------------------------+
// | wlroots frame event calls |
// +---------------------------+

static void wlr_frame_event_buffer(void *data,
								   struct zwlr_screencopy_frame_v1 *zwlr_screencopy_frame_v1,
								   uint32_t format, uint32_t width, uint32_t height,
								   uint32_t stride)
{
	struct wlr_frame_data *frame_data = (struct wlr_frame_data *)data;
	size_t size = height * stride;
	struct wl_shm_pool_data *smh_pool_data;
	wl_shm_pool_create(&smh_pool_data, frame_data->state, size);
	wl_buffer_create(&frame_data->buffer_data, smh_pool_data, width, height, stride, format);
}

static void wlr_frame_event_flags(void *data,
								  struct zwlr_screencopy_frame_v1 *zwlr_screencopy_frame_v1,
								  uint32_t flags)
{

}

static void wlr_frame_event_ready(void *data,
								  struct zwlr_screencopy_frame_v1 *zwlr_screencopy_frame_v1,
								  uint32_t tv_sec_hi, uint32_t tv_sec_lo, uint32_t tv_nsec)
{
	struct wlr_frame_data *frame_data = (struct wlr_frame_data *)data;
	frame_data->ready = true;
	if (frame_data->n_ready != NULL)
		(*frame_data->n_ready)++;
}

static void wlr_frame_event_failed(void *data,
								   struct zwlr_screencopy_frame_v1 *zwlr_screencopy_frame_v1)
{

}

static void wlr_frame_event_damage(void *data,
								   struct zwlr_screencopy_frame_v1 *zwlr_screencopy_frame_v1,
								   uint32_t x, uint32_t y, uint32_t width, uint32_t height)
{

}

static void wlr_frame_event_linux_dmabuf(void *data,
										 struct zwlr_screencopy_frame_v1 *zwlr_screencopy_frame_v1,
										 uint32_t format, uint32_t width, uint32_t height)
{

}

static void wlr_frame_event_buffer_done(void *data,
										struct zwlr_screencopy_frame_v1 *zwlr_screencopy_frame_v1)
{

}

static const struct zwlr_screencopy_frame_v1_listener wlr_frame_listener = {
	.buffer = wlr_frame_event_buffer,
	.flags = wlr_frame_event_flags,
	.ready = wlr_frame_event_ready,
	.failed = wlr_frame_event_failed,
	.damage = wlr_frame_event_damage,
	.linux_dmabuf = wlr_frame_event_linux_dmabuf,
	.buffer_done = wlr_frame_event_buffer_done
};

// +------------------------------+
// | wayland registry event calls |
// +------------------------------+

static void wl_registry_event_global(void *data, struct wl_registry *registry,
									 uint32_t name, const char *interface, uint32_t version)
{
	if (strcmp(interface, zwlr_screencopy_manager_v1_interface.name) == 0) {
		WS_LOGF(WS_SEV_INFO, "wlr_screencopy_manager found.\n");
		struct wl_state *state = (struct wl_state *)data;
		state->wlr_screencopy_manager =
			(struct zwlr_screencopy_manager_v1 *)wl_registry_bind(state->registry, name,
																  &zwlr_screencopy_manager_v1_interface,
																  version);
	}

	if (strcmp(interface, zcosmic_screencopy_manager_v2_interface.name) == 0) {
		WS_LOGF(WS_SEV_INFO, "cosmic_screencopy_manager found.\n");
		struct wl_state *state = (struct wl_state *)data;
		state->cosmic_screencopy_manager =
			(struct zcosmic_screencopy_manager_v2 *)wl_registry_bind(state->registry, name,
																	 &zcosmic_screencopy_manager_v2_interface,
																	 version);
	}

	if (strcmp(interface, wl_output_interface.name) == 0) {
		WS_LOGF(WS_SEV_INFO, "wl_output found.\n");
		struct wl_state *state = (struct wl_state *)data;
		struct wl_output *output =
			(struct wl_output *)wl_registry_bind(state->registry, name, &wl_output_interface,
												 version);
		struct wl_output_data *output_data;
		wl_output_create(&output_data, state, output);
		darray_append(state->outputs, &output_data);
	}

	if (strcmp(interface, wl_shm_interface.name) == 0) {
		WS_LOGF(WS_SEV_INFO, "wl_shm found.\n");
		struct wl_state *state = (struct wl_state *)data;
		state->shm =
			(struct wl_shm *)wl_registry_bind(state->registry, name, &wl_shm_interface, version);
	}

	if (strcmp(interface, zxdg_output_manager_v1_interface.name) == 0) {
		WS_LOGF(WS_SEV_INFO, "xdg_output_manager found.\n");
		struct wl_state *state = (struct wl_state *)data;
		state->output_manager =
			(struct zxdg_output_manager_v1 *)wl_registry_bind(state->registry, name,
															  &zxdg_output_manager_v1_interface,
															  version);
	}
}

static void wl_registry_event_global_remove(void *data, struct wl_registry *registry, uint32_t name)
{

}

static const struct wl_registry_listener registry_listener = {
.global = wl_registry_event_global,.global_remove = wl_registry_event_global_remove};

// +-----------------------------+
// | wayland managment functions |
// +-----------------------------+

ecode_t wl_state_connect(struct wl_state **r_state, const char *name)
{
	struct wl_state *state;
	state = *r_state = calloc(sizeof(struct wl_state), 1);
	state->outputs = darray_init(sizeof(struct wl_output_data *), 1);

	state->display = wl_display_connect(name);
	if (state->display == NULL) {
		WS_LOGF(WS_SEV_ERR, "wayland display not found.\n");
		return WSE_WL_NDISPLAY;
	}

	state->registry = wl_display_get_registry(state->display);
	if (state->registry == NULL) {
		WS_LOGF(WS_SEV_ERR, "wayland registry not found.\n");
		return WSE_WL_NREGISTRY;
	}

	wl_registry_add_listener(state->registry, &registry_listener, state);
	wl_display_roundtrip(state->display);	// registry events

	if (state->outputs->count == 0) {
		WS_LOGF(WS_SEV_ERR, "no wl_outputs were found.\n");
		return WSE_WL_NOUTPUT;
	}
	// check for xdg output manager
	if (state->output_manager == NULL) {
		WS_LOGF(WS_SEV_WARN, "no xdg_output_manager was found.\n");
		return WSE_WL_NXDG_OUTPUT;
	}
	// get xdg output data
	darray_foreach(state->outputs, i_output, output_data_p) {
		struct wl_output_data *output_data = *(struct wl_output_data **)output_data_p;
		struct zxdg_output_v1 *xdg_output =
			zxdg_output_manager_v1_get_xdg_output(state->output_manager, output_data->output);
		zxdg_output_v1_add_listener(xdg_output, &xdg_output_listener, output_data);
		wl_display_roundtrip(state->display);
	}

	return WS_OK;
}

ecode_t wl_state_disconnect(struct wl_state *state)
{
	darray_foreach(state->outputs, i_output_data, output_data_p) {
		struct wl_output_data *output_data = *(struct wl_output_data **)output_data_p;
		if (output_data != NULL)
			wl_output_free(output_data);
	}
	darray_free(state->outputs);
	if (state->registry != NULL)
		wl_registry_destroy(state->registry);

	if (state->display != NULL)
		wl_display_disconnect(state->display);

	return WS_OK;
}

ecode_t wl_output_create(struct wl_output_data **r_output_data, struct wl_state *state,
						 struct wl_output *output)
{
	struct wl_output_data *output_data;
	output_data = *r_output_data = calloc(sizeof(struct wl_output_data), 1);
	output_data->state = state;
	output_data->output = output;
	wl_output_add_listener(output_data->output, &output_listener, output_data);
	wl_display_roundtrip(state->display);
	return WS_OK;
}

ecode_t wl_output_free(struct wl_output_data *output_data)
{
	if (output_data->output != NULL)
		wl_output_destroy(output_data->output);

	free(output_data);
	return WS_OK;
}

ecode_t wl_shm_pool_create(struct wl_shm_pool_data **r_shm_pool_data, struct wl_state *state,
						   size_t size)
{
	struct wl_shm_pool_data *shm_pool_data;
	shm_pool_data = *r_shm_pool_data = calloc(sizeof(struct wl_shm_pool_data), 1);
	int fd;

	ecode_t code = create_shm_file(&fd, size);
	if (!code) {
		WS_LOGF(WS_SEV_WARN, "shm file failed.\n");
		return WSE_WL_FDF;
	}

	void *data = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (data == MAP_FAILED) {
		WS_LOGF(WS_SEV_WARN, "shm mmap failed. %i\n", fd);
		close(fd);
		return WSE_WL_MMAPF;
	}

	shm_pool_data->state = state;
	shm_pool_data->shm_pool = wl_shm_create_pool(state->shm, fd, size);
	shm_pool_data->total_size = size;
	shm_pool_data->used_size = 0;
	shm_pool_data->ref_count = 0;
	shm_pool_data->data = data;
	WS_LOGF(WS_SEV_INFO, "%p\n", data);

	close(fd);
	return WS_OK;
}

ecode_t wl_shm_pool_free(struct wl_shm_pool_data *shm_pool_data)
{
	if (shm_pool_data->shm_pool != NULL)
		wl_shm_pool_destroy(shm_pool_data->shm_pool);

	if (shm_pool_data->data != NULL)
		munmap(shm_pool_data->data, shm_pool_data->total_size);

	free(shm_pool_data);
	return WS_OK;
}

ecode_t wl_buffer_create(struct wl_buffer_data **r_buffer_data,
						 struct wl_shm_pool_data *shm_pool_data, uint32_t width, uint32_t height,
						 uint32_t stride, uint32_t format)
{
	struct wl_buffer_data *buffer_data;
	buffer_data = *r_buffer_data = calloc(sizeof(struct wl_buffer_data), 1);
	size_t size = height * stride;
	if (shm_pool_data->used_size + size > shm_pool_data->total_size)
		return WSE_WL_BUFFERF;

	buffer_data->state = shm_pool_data->state;
	buffer_data->ref_shm_pool_data = shm_pool_data;
	buffer_data->buffer =
		wl_shm_pool_create_buffer(shm_pool_data->shm_pool, shm_pool_data->used_size, width, height,
								  stride, format);

	buffer_data->width = width;
	buffer_data->height = height;
	buffer_data->stride = stride;
	buffer_data->format = format;
	buffer_data->size = size;
	buffer_data->data = shm_pool_data->data + shm_pool_data->used_size;
	shm_pool_data->used_size += size;
	shm_pool_data->ref_count++;
	return WS_OK;
}

ecode_t wl_buffer_free(struct wl_buffer_data *buffer_data)
{
	buffer_data->ref_shm_pool_data->ref_count--;
	if (buffer_data->ref_shm_pool_data->ref_count == 0)
		wl_shm_pool_free(buffer_data->ref_shm_pool_data);

	wl_buffer_destroy(buffer_data->buffer);
	free(buffer_data);
	return WS_OK;
}

ecode_t wlr_frame_create(struct wlr_frame_data **r_frame_data, struct wl_state *state,
						 struct wl_output_data *output_data, uint32_t cursor, uint32_t *n_ready)
{
	struct wlr_frame_data *frame_data;
	frame_data = *r_frame_data = calloc(sizeof(struct wlr_frame_data), 1);
	frame_data->state = state;
	frame_data->frame =
		zwlr_screencopy_manager_v1_capture_output(state->wlr_screencopy_manager,
												  cursor, output_data->output);
	frame_data->transform = output_data->transform;
	frame_data->n_ready = n_ready;
	frame_data->ready = false;
	zwlr_screencopy_frame_v1_add_listener(frame_data->frame, &wlr_frame_listener, frame_data);
	wl_display_roundtrip(state->display);
	return WS_OK;
}

ecode_t wlr_frame_region_create(struct wlr_frame_data **r_frame_data, struct wl_state *state,
								struct wl_output_data *output_data, struct rect region,
								uint32_t cursor, uint32_t *n_ready)
{
	struct wlr_frame_data *frame_data;
	frame_data = *r_frame_data = calloc(sizeof(struct wlr_frame_data), 1);
	frame_data->state = state;
	frame_data->frame =
		zwlr_screencopy_manager_v1_capture_output_region(state->wlr_screencopy_manager,
														 cursor, output_data->output,
														 region.x, region.y,
														 region.width, region.height);
	frame_data->region = region;
	frame_data->transform = output_data->transform;
	frame_data->n_ready = n_ready;
	frame_data->ready = false;
	zwlr_screencopy_frame_v1_add_listener(frame_data->frame, &wlr_frame_listener, frame_data);
	wl_display_roundtrip(state->display);
	return WS_OK;
}

ecode_t wlr_frame_abs_region_create(struct wlr_frame_data **r_frame_data, struct wl_state *state,
									struct wl_output_data *output_data, struct rect region,
									uint32_t cursor, uint32_t *n_ready)
{
	struct wlr_frame_data *frame_data;
	frame_data = *r_frame_data = calloc(sizeof(struct wlr_frame_data), 1);
	frame_data->state = state;
	frame_data->frame =
		zwlr_screencopy_manager_v1_capture_output_region(state->wlr_screencopy_manager,
														 cursor, output_data->output,
														 region.x - output_data->x,
														 region.y - output_data->y,
														 region.width, region.height);
	frame_data->region = region;
	frame_data->transform = output_data->transform;
	frame_data->n_ready = n_ready;
	frame_data->ready = false;
	zwlr_screencopy_frame_v1_add_listener(frame_data->frame, &wlr_frame_listener, frame_data);
	wl_display_roundtrip(state->display);
	return WS_OK;
}

ecode_t wlr_frame_copy(struct wlr_frame_data *frame_data)
{
	zwlr_screencopy_frame_v1_copy(frame_data->frame, frame_data->buffer_data->buffer);
	wl_display_dispatch(frame_data->state->display);
	return WS_OK;
}

ecode_t wlr_frame_get_image(struct image *r_image, struct wlr_frame_data *frame_data)
{
	struct format format;
	struct image image;
	ecode_t code = format_from_wl_format(&format, frame_data->buffer_data->format);
	if (!code)
		return 0;

	image_create_from_buffer(&image, frame_data->region, frame_data->buffer_data->data, format);
	*r_image = image;
	return WS_OK;
}

ecode_t wlr_frame_free(struct wlr_frame_data *frame_data)
{
	if (frame_data->buffer_data != NULL)
		wl_buffer_free(frame_data->buffer_data);

	free(frame_data);
	return WS_OK;
}

ecode_t image_wlr_screencopy(struct image *r_image, struct wl_state *state, struct rect region)
{
	struct darray *frames = darray_init(sizeof(struct wlr_frame_data *), 1);
	uint32_t n_requested = 0, n_ready = 0;
	darray_foreach(state->outputs, i_output_data, output_data_p) {
		struct wl_output_data *output_data = *(struct wl_output_data **)output_data_p;
		struct rect inter = rect_inter(output_data->area, region);
		if (!rect_is_valid(inter))
			continue;

		struct wlr_frame_data *frame_data;
		wlr_frame_abs_region_create(&frame_data, state, output_data, inter, 0, &n_ready);
		wlr_frame_copy(frame_data);
		darray_append(frames, &frame_data);
		n_requested++;
	}

	uint32_t n_dispatched = 0;
	while (n_ready != n_requested && n_dispatched != -1)
		n_dispatched = wl_display_roundtrip(state->display);

	struct image image_base;
	image_create_empty(&image_base, region);
	darray_foreach(frames, i_frame_data, frame_data_p) {
		struct wlr_frame_data *frame_data = *(struct wlr_frame_data **)frame_data_p;
		struct image image_part;
		ecode_t code = wlr_frame_get_image(&image_part, frame_data);
		if (!code)
			continue;		   /* unsupported wl format */

		wlr_frame_free(frame_data);
		image_rotate(&image_part, frame_data->transform & 0x03);
		if (frame_data->transform & 0x04)
			image_vaxis_flip(image_part);

		image_layer_overwrite(image_base, image_part);
		image_delete(image_part);
	}

	*r_image = image_base;
	return WS_OK;
}
