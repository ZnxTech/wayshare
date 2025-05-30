#include "wayland.h"

static void null_func()
{
	/* left empty. */
}

static void wl_output_event_geometry(void *data, struct wl_output *wl_output, int32_t x, int32_t y,
									 int32_t width_mm, int32_t height_mm, int32_t subpixel,
									 const char *make, const char *model, int32_t transform)
{
	struct wl_output_data *output_data = data;
	output_data->x = x;
	output_data->y = y;
	output_data->transform = transform;
}

static void wl_output_event_mode(void *data, struct wl_output *wl_output, uint32_t flags,
								 int32_t width, int32_t height, int32_t refresh)
{
	struct wl_output_data *output_data = data;
	output_data->width = width;
	output_data->height = height;
}

static void wl_output_event_scale(void *data, struct wl_output *wl_output, int32_t factor)
{
	struct wl_output_data *output_data = data;
	output_data->scale_factor = factor;
}

const static struct wl_output_listener output_listener = {
	.geometry = wl_output_event_geometry,
	.mode = wl_output_event_mode,
	.scale = wl_output_event_scale,
	/* unused: */
	.done = null_func,
	.name = null_func,
	.description = null_func,
};

static void wl_buffer_event_release(void *data, struct wl_buffer *wl_buffer)
{
	struct wl_buffer_data *buffer_data = data;
	buffer_data->busy = false;
}

const static struct wl_buffer_listener buffer_listener = {
	.release = wl_buffer_event_release,
};

static void wl_seat_event_capabilities(void *data, struct wl_seat *wl_seat, uint32_t capabilities)
{
	struct wl_seat_data *seat_data = data;
	seat_data->capabilities = capabilities;
}

const static struct wl_seat_listener seat_listener = {
	.capabilities = wl_seat_event_capabilities,
	/* unused: */
	.name = null_func,
};

static void xdg_output_event_logical_position(void *data, struct zxdg_output_v1 *zxdg_output_v1,
											  int32_t x, int32_t y)
{
	struct wl_output_data *output_data = data;
	output_data->x = x;
	output_data->y = y;
}

static void xdg_output_event_logical_size(void *data, struct zxdg_output_v1 *zxdg_output_v1,
										  int32_t width, int32_t height)
{
	struct wl_output_data *output_data = data;
	output_data->width = width;
	output_data->height = height;
}

const static struct zxdg_output_v1_listener xdg_output_listener = {
	.logical_position = xdg_output_event_logical_position,
	.logical_size = xdg_output_event_logical_size,
	/* unused: */
	.done = null_func,
	.name = null_func,
	.description = null_func,
};

static void wl_registry_event_global(void *data, struct wl_registry *registry, uint32_t name,
									 const char *interface, uint32_t version)
{
	if (strcmp(interface, wl_compositor_interface.name) == 0) {
		WS_LOGF(WS_SEV_INFO, "wl_compositor found.\n");
		struct wl_state *state = data;
		state->compositor =
			wl_registry_bind(state->registry, name, &wl_compositor_interface, version);
	}

	if (strcmp(interface, wl_output_interface.name) == 0) {
		WS_LOGF(WS_SEV_INFO, "wl_output found.\n");
		struct wl_state *state = data;
		struct wl_output *output =
			wl_registry_bind(state->registry, name, &wl_output_interface, version);

		struct wl_output_data *output_data;
		wl_output_create(&output_data, state, output);
		darray_append(state->outputs, &output_data);
	}

	if (strcmp(interface, zxdg_output_manager_v1_interface.name) == 0) {
		WS_LOGF(WS_SEV_INFO, "xdg_output_manager found.\n");
		struct wl_state *state = data;
		state->xdg_output_manager =
			wl_registry_bind(state->registry, name, &zxdg_output_manager_v1_interface, version);
	}

	if (strcmp(interface, wp_cursor_shape_manager_v1_interface.name) == 0) {
		WS_LOGF(WS_SEV_INFO, "cursor_shape_manager found.\n");
		struct wl_state *state = data;
		state->cursor_shape_manager =
			wl_registry_bind(state->registry, name, &wp_cursor_shape_manager_v1_interface, version);
	}

	if (strcmp(interface, wl_seat_interface.name) == 0) {
		WS_LOGF(WS_SEV_INFO, "wl_seat found.\n");
		struct wl_state *state = data;
		struct wl_seat *seat = wl_registry_bind(state->registry, name, &wl_seat_interface, version);

		struct wl_seat_data *seat_data;
		wl_seat_create(&seat_data, state, seat);
		darray_append(state->seats, &seat_data);
	}

	if (strcmp(interface, wl_shm_interface.name) == 0) {
		WS_LOGF(WS_SEV_INFO, "wl_shm found.\n");
		struct wl_state *state = data;
		state->shm = wl_registry_bind(state->registry, name, &wl_shm_interface, version);
	}

	if (strcmp(interface, zwlr_layer_shell_v1_interface.name) == 0) {
		WS_LOGF(WS_SEV_INFO, "wlr_layer_shell found.\n");
		struct wl_state *state = data;
		state->wlr_layer_shell =
			wl_registry_bind(state->registry, name, &zwlr_layer_shell_v1_interface, version);
	}

	if (strcmp(interface, zwlr_screencopy_manager_v1_interface.name) == 0) {
		WS_LOGF(WS_SEV_INFO, "wlr_screencopy_manager found.\n");
		struct wl_state *state = data;
		state->wlr_screencopy_manager =
			wl_registry_bind(state->registry, name, &zwlr_screencopy_manager_v1_interface, version);
	}

	if (strcmp(interface, zcosmic_output_image_source_manager_v1_interface.name) == 0) {
		WS_LOGF(WS_SEV_INFO, "cosmic_output_image_source_manager found.\n");
		struct wl_state *state = data;
		state->cosmic_screencopy_manager = wl_registry_bind(
			state->registry, name, &zcosmic_output_image_source_manager_v1_interface, version);
	}

	if (strcmp(interface, zcosmic_screencopy_manager_v2_interface.name) == 0) {
		WS_LOGF(WS_SEV_INFO, "cosmic_screencopy_manager found.\n");
		struct wl_state *state = data;
		state->cosmic_screencopy_manager = wl_registry_bind(
			state->registry, name, &zcosmic_screencopy_manager_v2_interface, version);
	}
}

const static struct wl_registry_listener registry_listener = {
	.global = wl_registry_event_global,
	/* unused: */
	.global_remove = null_func,
};

static ecode_t wl_output_get_logistical(struct wl_state *state, struct wl_output_data *output_data)
{
	if (state->xdg_output_manager != NULL) {
		struct zxdg_output_v1 *xdg_output =
			zxdg_output_manager_v1_get_xdg_output(state->xdg_output_manager, output_data->output);
		zxdg_output_v1_add_listener(xdg_output, &xdg_output_listener, output_data);
		return WS_OK;
	} else {
		/* guess logictical area using {scale_factor} and {transform}. */
		if (output_data->transform & 1)
			output_data->area = rect_transpose(output_data->area);

		output_data->width /= output_data->scale_factor;
		output_data->height /= output_data->scale_factor;
		return WSE_WL_NXDG_OUTPUT;
	}
}

ecode_t wl_state_connect(struct wl_state **r_state, const char *name)
{
	struct wl_state *state;
	state = calloc(sizeof(struct wl_state), 1);
	state->outputs = darray_init(sizeof(struct wl_output_data *), 1);
	state->seats = darray_init(sizeof(struct wl_seat_data *), 1);

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
	wl_display_roundtrip(state->display); /* listen to registry events. */

	if (state->outputs->count == 0) {
		WS_LOGF(WS_SEV_ERR, "no wl_outputs were found.\n");
		return WSE_WL_NOUTPUT;
	}

	/* get logictical output values. */
	struct wl_output_data *output_data;
	darray_foreach (state->outputs, output_data)
		wl_output_get_logistical(state, output_data);

	wl_display_roundtrip(state->display); /* listen to xdg output events. */

	*r_state = state;
	return WS_OK;
}

ecode_t wl_state_disconnect(struct wl_state *state)
{
	struct wl_output_data *output_data;
	darray_foreach (state->outputs, output_data)
		if (output_data != NULL)
			wl_output_free(output_data);
	darray_free(state->outputs);

	struct wl_seat_data *seat_data;
	darray_foreach (state->seats, seat_data)
		if (seat_data != NULL)
			wl_seat_free(seat_data);
	darray_free(state->seats);

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
	output_data = calloc(sizeof(struct wl_output_data), 1);
	output_data->state = state;
	output_data->output = output;
	wl_output_add_listener(output_data->output, &output_listener, output_data);
	wl_display_roundtrip(state->display);
	*r_output_data = output_data;
	return WS_OK;
}

ecode_t wl_output_free(struct wl_output_data *output_data)
{
	if (output_data->output != NULL)
		wl_output_destroy(output_data->output);

	free(output_data);
	return WS_OK;
}

ecode_t wl_seat_create(struct wl_seat_data **r_seat_data, struct wl_state *state,
					   struct wl_seat *seat)
{
	struct wl_seat_data *seat_data;
	seat_data = calloc(sizeof(struct wl_seat_data), 1);
	seat_data->state = state;
	seat_data->seat = seat;
	wl_seat_add_listener(seat_data->seat, &seat_listener, seat_data);
	wl_display_roundtrip(state->display);

	*r_seat_data = seat_data;
	return WS_OK;
}

ecode_t wl_seat_free(struct wl_seat_data *seat_data)
{
	if (seat_data->seat != NULL)
		wl_seat_destroy(seat_data->seat);

	free(seat_data);
	return WS_OK;
}

ecode_t wl_shm_pool_create(struct wl_shm_pool_data **r_shm_pool_data, struct wl_state *state,
						   size_t size)
{
	struct wl_shm_pool_data *shm_pool_data;
	shm_pool_data = calloc(sizeof(struct wl_shm_pool_data), 1);
	int fd;

	ecode_t code = create_shm_file(&fd, size);
	if (code) {
		WS_LOGF(WS_SEV_WARN, "shm file failed.\n");
		return WSE_WL_FDF;
	}

	WS_LOGF(WS_SEV_WARN, "mmaping fd %i of size %u, struct: %p.\n", fd, size, shm_pool_data);
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
	close(fd);

	*r_shm_pool_data = shm_pool_data;
	return WS_OK;
}

ecode_t wl_shm_pool_free(struct wl_shm_pool_data *shm_pool_data)
{
	WS_LOGF(WS_SEV_WARN, "freeing struct: %p.\n", shm_pool_data);
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
	buffer_data = calloc(sizeof(struct wl_buffer_data), 1);
	size_t size = height * stride;
	if (shm_pool_data->used_size + size > shm_pool_data->total_size)
		return WSE_WL_BUFFERF;

	buffer_data->state = shm_pool_data->state;
	buffer_data->ref_shm_pool_data = shm_pool_data;
	buffer_data->buffer = wl_shm_pool_create_buffer(
		shm_pool_data->shm_pool, shm_pool_data->used_size, width, height, stride, format);
	buffer_data->width = width;
	buffer_data->height = height;
	buffer_data->stride = stride;
	buffer_data->format = format;
	buffer_data->size = size;
	buffer_data->data = shm_pool_data->data + shm_pool_data->used_size;
	shm_pool_data->used_size += size;
	shm_pool_data->ref_count++;
	wl_buffer_add_listener(buffer_data->buffer, &buffer_listener, buffer_data);
	*r_buffer_data = buffer_data;
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
