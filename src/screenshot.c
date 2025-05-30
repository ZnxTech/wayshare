#include "screenshot.h"

static void null_func()
{
	/* left empty. */
}

static pixman_format_code_t format_wl_to_pixman(enum wl_shm_format format)
{
	switch (format) {
	case WL_SHM_FORMAT_RGB332:
		return PIXMAN_r3g3b2;

	case WL_SHM_FORMAT_BGR233:
		return PIXMAN_b2g3r3;

	case WL_SHM_FORMAT_ARGB4444:
		return PIXMAN_a4r4g4b4;

	case WL_SHM_FORMAT_XRGB4444:
		return PIXMAN_x4r4g4b4;

	case WL_SHM_FORMAT_ABGR4444:
		return PIXMAN_a4b4g4r4;

	case WL_SHM_FORMAT_XBGR4444:
		return PIXMAN_x4b4g4r4;

	case WL_SHM_FORMAT_ARGB1555:
		return PIXMAN_a1r5g5b5;

	case WL_SHM_FORMAT_XRGB1555:
		return PIXMAN_x1r5g5b5;

	case WL_SHM_FORMAT_ABGR1555:
		return PIXMAN_a1b5g5r5;

	case WL_SHM_FORMAT_XBGR1555:
		return PIXMAN_x1b5g5r5;

	case WL_SHM_FORMAT_RGB565:
		return PIXMAN_r5g6b5;

	case WL_SHM_FORMAT_BGR565:
		return PIXMAN_b5g6r5;

	case WL_SHM_FORMAT_RGB888:
		return PIXMAN_r8g8b8;

	case WL_SHM_FORMAT_BGR888:
		return PIXMAN_b8g8r8;

	case WL_SHM_FORMAT_ARGB8888:
		return PIXMAN_a8r8g8b8;

	case WL_SHM_FORMAT_XRGB8888:
		return PIXMAN_x8r8g8b8;

	case WL_SHM_FORMAT_ABGR8888:
		return PIXMAN_a8b8g8r8;

	case WL_SHM_FORMAT_XBGR8888:
		return PIXMAN_x8b8g8r8;

	case WL_SHM_FORMAT_BGRA8888:
		return PIXMAN_b8g8r8a8;

	case WL_SHM_FORMAT_BGRX8888:
		return PIXMAN_b8g8r8x8;

	case WL_SHM_FORMAT_RGBA8888:
		return PIXMAN_r8g8b8a8;

	case WL_SHM_FORMAT_RGBX8888:
		return PIXMAN_r8g8b8x8;

	case WL_SHM_FORMAT_ARGB2101010:
		return PIXMAN_a2r10g10b10;

	case WL_SHM_FORMAT_ABGR2101010:
		return PIXMAN_a2b10g10r10;

	case WL_SHM_FORMAT_XRGB2101010:
		return PIXMAN_x2r10g10b10;

	case WL_SHM_FORMAT_XBGR2101010:
		return PIXMAN_x2b10g10r10;

	default:
		return 0;
	}
}

uint32_t format_get_depth(enum wl_shm_format format)
{
	switch (format) {
	case WL_SHM_FORMAT_RGB332:
	case WL_SHM_FORMAT_BGR233:
		return 8;

	case WL_SHM_FORMAT_ARGB4444:
	case WL_SHM_FORMAT_XRGB4444:
	case WL_SHM_FORMAT_ABGR4444:
	case WL_SHM_FORMAT_XBGR4444:
	case WL_SHM_FORMAT_ARGB1555:
	case WL_SHM_FORMAT_XRGB1555:
	case WL_SHM_FORMAT_ABGR1555:
	case WL_SHM_FORMAT_XBGR1555:
	case WL_SHM_FORMAT_RGB565:
	case WL_SHM_FORMAT_BGR565:
		return 16;

	case WL_SHM_FORMAT_RGB888:
	case WL_SHM_FORMAT_BGR888:
		return 24;

	case WL_SHM_FORMAT_ARGB8888:
	case WL_SHM_FORMAT_XRGB8888:
	case WL_SHM_FORMAT_ABGR8888:
	case WL_SHM_FORMAT_XBGR8888:
	case WL_SHM_FORMAT_BGRA8888:
	case WL_SHM_FORMAT_BGRX8888:
	case WL_SHM_FORMAT_RGBA8888:
	case WL_SHM_FORMAT_RGBX8888:
	case WL_SHM_FORMAT_ARGB2101010:
	case WL_SHM_FORMAT_ABGR2101010:
	case WL_SHM_FORMAT_XRGB2101010:
	case WL_SHM_FORMAT_XBGR2101010:
		return 32;

	default:
		return 0;
	}
}

static void wlr_frame_event_buffer(void *data,
								   struct zwlr_screencopy_frame_v1 *zwlr_screencopy_frame_v1,
								   uint32_t format, uint32_t width, uint32_t height,
								   uint32_t stride)
{
	struct wlr_frame_data *frame_data = data;
	frame_data->format = format;
	frame_data->width = width;
	frame_data->height = height;
	frame_data->stride = stride;
}

static void wlr_frame_event_buffer_done(void *data,
										struct zwlr_screencopy_frame_v1 *zwlr_screencopy_frame_v1)
{
	struct wlr_frame_data *frame_data = data;
	frame_data->state->shm_pool_size += frame_data->stride * frame_data->height;
	frame_data->state->n_buffered++;
}

static void wlr_frame_event_flags(void *data,
								  struct zwlr_screencopy_frame_v1 *zwlr_screencopy_frame_v1,
								  uint32_t flags)
{
	struct wlr_frame_data *frame_data = data;
	frame_data->flags = flags;
}

static void wlr_frame_event_ready(void *data,
								  struct zwlr_screencopy_frame_v1 *zwlr_screencopy_frame_v1,
								  uint32_t tv_sec_hi, uint32_t tv_sec_lo, uint32_t tv_nsec)
{
	struct wlr_frame_data *frame_data = data;
	frame_data->ready = true;
	frame_data->buffer_data->busy = false;
	frame_data->state->n_ready++;
}

static void wlr_frame_event_failed(void *data,
								   struct zwlr_screencopy_frame_v1 *zwlr_screencopy_frame_v1)
{
	struct wlr_frame_data *frame_data = data;
	frame_data->ready = false;
}

const static struct zwlr_screencopy_frame_v1_listener wlr_frame_listener = {
	.buffer = wlr_frame_event_buffer,
	.buffer_done = wlr_frame_event_buffer_done,
	.flags = wlr_frame_event_flags,
	.ready = wlr_frame_event_ready,
	.failed = wlr_frame_event_failed,
	/* unused: */
	.damage = null_func,
	.linux_dmabuf = null_func};

ecode_t wlr_frame_create(struct wlr_frame_data **r_frame_data, struct screenshot_state *state,
						 struct wl_output_data *output_data)
{
	struct wlr_frame_data *frame_data;
	frame_data = calloc(1, sizeof(struct wlr_frame_data));
	frame_data->state = state;
	frame_data->area = output_data->area;
	frame_data->transform = output_data->transform;
	frame_data->wlr_frame = zwlr_screencopy_manager_v1_capture_output(
		state->wayland->wlr_screencopy_manager, 0, output_data->output);

	zwlr_screencopy_frame_v1_add_listener(frame_data->wlr_frame, &wlr_frame_listener, frame_data);
	*r_frame_data = frame_data;
	return WS_OK;
}

ecode_t wlr_frame_create_region(struct wlr_frame_data **r_frame_data,
								struct screenshot_state *state, struct wl_output_data *output_data,
								struct rect area)
{
	struct wlr_frame_data *frame_data;
	frame_data = calloc(1, sizeof(struct wlr_frame_data));
	frame_data->state = state;
	frame_data->area = area;
	frame_data->transform = output_data->transform;
	frame_data->wlr_frame = zwlr_screencopy_manager_v1_capture_output_region(
		state->wayland->wlr_screencopy_manager, 0, output_data->output, area.x - output_data->x,
		area.y - output_data->y, area.width, area.height);

	zwlr_screencopy_frame_v1_add_listener(frame_data->wlr_frame, &wlr_frame_listener, frame_data);
	*r_frame_data = frame_data;
	return WS_OK;
}

ecode_t wlr_frame_copy(struct wlr_frame_data *frame_data)
{
	zwlr_screencopy_frame_v1_copy(frame_data->wlr_frame, frame_data->buffer_data->buffer);
	frame_data->buffer_data->busy = true;
	return WS_OK;
}

ecode_t wlr_frame_free(struct wlr_frame_data *frame_data)
{
	if (frame_data->buffer_data != NULL)
		wl_buffer_free(frame_data->buffer_data);

	zwlr_screencopy_frame_v1_destroy(frame_data->wlr_frame);
	free(frame_data);
	return WS_OK;
}

static void cosmic_screencopy_session_event_buffer_size(
	void *data, struct zcosmic_screencopy_session_v2 *zcosmic_screencopy_session_v2, uint32_t width,
	uint32_t height)
{
	struct cosmic_frame_data *frame_data = data;
	frame_data->width = width;
	frame_data->height = height;
}

static void cosmic_screencopy_session_event_shm_format(
	void *data, struct zcosmic_screencopy_session_v2 *zcosmic_screencopy_session_v2,
	uint32_t format)
{
	struct cosmic_frame_data *frame_data = data;
	frame_data->format = format;
}

static void cosmic_screencopy_session_event_done(
	void *data, struct zcosmic_screencopy_session_v2 *zcosmic_screencopy_session_v2)
{
	struct cosmic_frame_data *frame_data = data;
	frame_data->stride = frame_data->width * format_get_depth(frame_data->format);
	frame_data->state->shm_pool_size += frame_data->stride * frame_data->height;
	frame_data->state->n_buffered++;
}

static void cosmic_screencopy_session_event_stopped(
	void *data, struct zcosmic_screencopy_session_v2 *zcosmic_screencopy_session_v2)
{
	struct cosmic_frame_data *frame_data = data;
}

const static struct zcosmic_screencopy_session_v2_listener cosmic_screencopy_session_listener = {
	.buffer_size = cosmic_screencopy_session_event_buffer_size,
	.shm_format = cosmic_screencopy_session_event_shm_format,
	.done = cosmic_screencopy_session_event_done,
	.stopped = cosmic_screencopy_session_event_stopped,
	/* unused. */
	.dmabuf_device = null_func,
	.dmabuf_format = null_func,
};

static void cosmic_screencopy_frame_event_transform(
	void *data, struct zcosmic_screencopy_frame_v2 *zcosmic_screencopy_frame_v2, uint32_t transform)
{
	struct cosmic_frame_data *frame_data = data;
	frame_data->buffer_transform = transform;
}

static void
cosmic_screencopy_frame_event_ready(void *data,
									struct zcosmic_screencopy_frame_v2 *zcosmic_screencopy_frame_v2)
{
	struct cosmic_frame_data *frame_data = data;
	frame_data->ready = true;
	frame_data->buffer_data->busy = false;
	frame_data->state->n_ready++;
}

static void cosmic_screencopy_frame_event_failed(
	void *data, struct zcosmic_screencopy_frame_v2 *zcosmic_screencopy_frame_v2, uint32_t reason)
{
	struct cosmic_frame_data *frame_data = data;
}

const static struct zcosmic_screencopy_frame_v2_listener cosmic_screencopy_frame_listener = {
	.transform = cosmic_screencopy_frame_event_transform,
	.ready = cosmic_screencopy_frame_event_ready,
	.failed = cosmic_screencopy_frame_event_failed,
	/* unused: */
	.damage = null_func,
	.presentation_time = null_func,
};

ecode_t cosmic_frame_create(struct cosmic_frame_data **r_frame_data, struct screenshot_state *state,
							struct wl_output_data *output_data)
{
	struct cosmic_frame_data *frame_data;
	frame_data = calloc(1, sizeof(struct cosmic_frame_data));
	frame_data->state = state;
	frame_data->area = output_data->area;
	frame_data->transform = output_data->transform;

	/* clang-format off */ /* yeah this suck... */
	frame_data->cosmic_image_source = zcosmic_output_image_source_manager_v1_create_source(
		state->wayland->cosmic_output_image_source_manager, output_data->output);

	frame_data->cosmic_session = zcosmic_screencopy_manager_v2_create_session(
		state->wayland->cosmic_screencopy_manager, frame_data->cosmic_image_source, 0);

	zcosmic_screencopy_session_v2_add_listener(frame_data->cosmic_session,
		&cosmic_screencopy_session_listener, frame_data);

	frame_data->cosmic_frame = zcosmic_screencopy_session_v2_create_frame(frame_data->cosmic_session);

	zcosmic_screencopy_frame_v2_add_listener(frame_data->cosmic_frame,
		&cosmic_screencopy_frame_listener, frame_data);
	/* clang-format on */

	*r_frame_data = frame_data;
	return WS_OK;
}

ecode_t cosmic_frame_copy(struct cosmic_frame_data *frame_data)
{
	zcosmic_screencopy_frame_v2_capture(frame_data->cosmic_frame);
	frame_data->buffer_data->busy = true;
	return WS_OK;
}

ecode_t cosmic_frame_free(struct cosmic_frame_data *frame_data)
{
	if (frame_data->buffer_data != NULL)
		wl_buffer_free(frame_data->buffer_data);

	zcosmic_screencopy_frame_v2_destroy(frame_data->cosmic_frame);
	zcosmic_screencopy_session_v2_destroy(frame_data->cosmic_session);
	zcosmic_image_source_v1_destroy(frame_data->cosmic_image_source);
	free(frame_data);
	return WS_OK;
}

static enum screenshot_type get_screenshot_type(struct wl_state *wayland)
{
	if (wayland->wlr_screencopy_manager != NULL)
		return SCREENSHOT_TYPE_WLR;
	else if (wayland->cosmic_screencopy_manager != NULL)
		return SCREENSHOT_TYPE_COSMIC;
	else
		return SCREENSHOT_TYPE_NONE;
}

static void screenshot_init_buffers(struct screenshot_state *screenshot)
{
	/* create pool and assign buffers to frames. */
	struct wl_shm_pool_data *shm_pool_data;
	wl_shm_pool_create(&shm_pool_data, screenshot->wayland, screenshot->shm_pool_size);
	screenshot->shm_pool_data = shm_pool_data;

	switch (screenshot->type) {
	case SCREENSHOT_TYPE_WLR:
		struct wlr_frame_data *wlr_frame_data;
		darray_foreach (screenshot->wlr_frames, wlr_frame_data) {
			struct wl_buffer_data *buffer_data;
			wl_buffer_create(&buffer_data, screenshot->shm_pool_data, wlr_frame_data->width,
							 wlr_frame_data->height, wlr_frame_data->stride,
							 wlr_frame_data->format);
			wlr_frame_data->buffer_data = buffer_data;
		}
		break;

	case SCREENSHOT_TYPE_COSMIC:
		struct cosmic_frame_data *cosmic_frame_data;
		darray_foreach (screenshot->cosmic_frames, cosmic_frame_data) {
			struct wl_buffer_data *buffer_data;
			wl_buffer_create(&buffer_data, screenshot->shm_pool_data, cosmic_frame_data->width,
							 cosmic_frame_data->height, cosmic_frame_data->stride,
							 cosmic_frame_data->format);
			cosmic_frame_data->buffer_data = buffer_data;
		}
		break;

	default:
		return;
	}
}

static void screenshot_init_output(struct screenshot_state *screenshot,
								   struct wl_output_data *output_data)
{
	switch (screenshot->type) {
	case SCREENSHOT_TYPE_WLR:
		struct wlr_frame_data *wlr_frame_data;
		wlr_frame_create(&wlr_frame_data, screenshot, output_data);
		darray_append(screenshot->wlr_frames, &wlr_frame_data);
		break;

	case SCREENSHOT_TYPE_COSMIC:
		struct cosmic_frame_data *cosmic_frame_data;
		cosmic_frame_create(&cosmic_frame_data, screenshot, output_data);
		darray_append(screenshot->cosmic_frames, &cosmic_frame_data);
		break;

	default:
		return;
	}

	screenshot->n_requested++;
	/* await all frame's buffer data to finish. */
	while (screenshot->n_buffered != screenshot->n_requested
		   && wl_display_dispatch(screenshot->wayland->display) != -1) {
		/* left empty. */
	}
}

static void screenshot_init_region(struct screenshot_state *screenshot, struct rect region)
{
	struct wl_output_data *output_data;
	darray_foreach (screenshot->wayland->outputs, output_data) {
		struct rect intersect = rect_intersect(output_data->area, region);
		if (!rect_is_valid(intersect))
			continue;

		switch (screenshot->type) {
		case SCREENSHOT_TYPE_WLR:
			struct wlr_frame_data *wlr_frame_data;
			/* the capture_output_region request is currently broken on Hyprland.
			   avoid for now, re-impl when fixed. */
			wlr_frame_create(&wlr_frame_data, screenshot, output_data);
			darray_append(screenshot->wlr_frames, &wlr_frame_data);
			break;

		case SCREENSHOT_TYPE_COSMIC:
			struct cosmic_frame_data *cosmic_frame_data;
			/* cosmic doesn't have a region selection request for outputs. */
			cosmic_frame_create(&cosmic_frame_data, screenshot, output_data);
			darray_append(screenshot->cosmic_frames, &cosmic_frame_data);
			break;

		default:
			continue;
		}

		screenshot->n_requested++;
	}

	/* await all frame's buffer data to finish. */
	while (screenshot->n_buffered != screenshot->n_requested
		   && wl_display_dispatch(screenshot->wayland->display) != -1) {
		/* left empty. */
	}
}

ecode_t screenshot_create_output(struct screenshot_state **r_screenshot, struct wl_state *wayland,
								 struct wl_output_data *output_data)
{
	struct screenshot_state *screenshot;
	screenshot = calloc(1, sizeof(struct screenshot_state));
	screenshot->wayland = wayland;
	screenshot->region = output_data->area;
	screenshot->type = get_screenshot_type(wayland);
	screenshot->wlr_frames = darray_init(sizeof(struct wlr_frame_data *), 1);
	screenshot->cosmic_frames = darray_init(sizeof(struct cosmic_frame_data *), 1);

	screenshot_init_output(screenshot, output_data);
	screenshot_init_buffers(screenshot);

	*r_screenshot = screenshot;
	return WS_OK;
}

ecode_t screenshot_create_region(struct screenshot_state **r_screenshot, struct wl_state *wayland,
								 struct rect region)
{
	struct screenshot_state *screenshot;
	screenshot = calloc(1, sizeof(struct screenshot_state));
	screenshot->wayland = wayland;
	screenshot->region = region;
	screenshot->type = get_screenshot_type(wayland);
	screenshot->wlr_frames = darray_init(sizeof(struct wlr_frame_data *), 1);
	screenshot->cosmic_frames = darray_init(sizeof(struct cosmic_frame_data *), 1);

	screenshot_init_region(screenshot, region);
	screenshot_init_buffers(screenshot);

	*r_screenshot = screenshot;
	return WS_OK;
}

static void screenshot_copy(struct screenshot_state *screenshot)
{
	switch (screenshot->type) {
	case SCREENSHOT_TYPE_WLR:
		struct wlr_frame_data *wlr_frame_data;
		darray_foreach (screenshot->wlr_frames, wlr_frame_data)
			wlr_frame_copy(wlr_frame_data);
		break;

	case SCREENSHOT_TYPE_COSMIC:
		struct cosmic_frame_data *cosmic_frame_data;
		darray_foreach (screenshot->cosmic_frames, cosmic_frame_data)
			cosmic_frame_copy(cosmic_frame_data);
		break;

	default:
		return;
	}

	while (screenshot->n_ready != screenshot->n_requested
		   && wl_display_dispatch(screenshot->wayland->display) != -1) {
		/* left empty. */
	}

	/* reset {n_ready} for the next copy. */
	screenshot->n_ready = 0;
}

static inline double get_rotation_cos(int32_t transform)
{
	return transform & WL_OUTPUT_TRANSFORM_90 ? 0 : (transform & WL_OUTPUT_TRANSFORM_180 ? -1 : 1);
}

static inline double get_rotation_sin(int32_t transform)
{
	return transform & WL_OUTPUT_TRANSFORM_90 ? (transform & WL_OUTPUT_TRANSFORM_180 ? -1 : 1) : 0;
}

static void composite_buffer_onto_base(pixman_image_t *base_image,
									   struct wl_buffer_data *buffer_data,
									   struct rect composite_area, int32_t wl_transform)
{
	pixman_image_t *part_image = NULL;

	void *data = buffer_data->data;
	uint32_t width = buffer_data->width;
	uint32_t height = buffer_data->height;
	uint32_t stride = buffer_data->stride;
	uint32_t wl_format = buffer_data->format;

	/* buffer to logical transfom data. */
	double cos_rotation = get_rotation_cos(wl_transform);
	double sin_rotation = get_rotation_sin(wl_transform);
	/* {sin_rotation} is 0 when frame has a 90deg transfom. */
	double x_scale = (double)composite_area.width / (sin_rotation ? height : width);
	double y_scale = (double)composite_area.height / (sin_rotation ? width : height);
	bool x_invert = wl_transform & WL_OUTPUT_TRANSFORM_FLIPPED;
	bool y_invert = wl_transform & (1 << 4); /* custom y_invert flag for wlr. */

	pixman_format_code_t pixman_format;
	pixman_format = format_wl_to_pixman(wl_format);
	if (!pixman_format)
		return;

	part_image = pixman_image_create_bits(pixman_format, width, height, data, stride);

	/* part to base image transform. */
	struct pixman_f_transform f_transform;
	pixman_f_transform_init_identity(&f_transform);

	// /* center the image on the origin. */
	pixman_f_transform_translate(&f_transform, NULL, -(double)width / 2, -(double)height / 2);

	pixman_f_transform_scale(&f_transform, NULL, 1, y_invert ? -1 : 1);
	pixman_f_transform_rotate(&f_transform, NULL, cos_rotation, sin_rotation);
	pixman_f_transform_scale(&f_transform, NULL, x_invert ? -1 : 1, 1);
	pixman_f_transform_scale(&f_transform, NULL, x_scale, y_scale);

	// /* re-move the image to its original place. */
	pixman_f_transform_translate(&f_transform, NULL, (double)composite_area.width / 2,
								 (double)composite_area.height / 2);

	struct pixman_f_transform inv_f_transform;
	pixman_f_transform_invert(&inv_f_transform, &f_transform);

	struct pixman_transform transform;
	pixman_transform_from_pixman_f_transform(&transform, &inv_f_transform);
	pixman_image_set_transform(part_image, &transform);

	pixman_image_composite32(PIXMAN_OP_SRC, part_image, NULL, base_image, 0, 0, 0, 0,
							 composite_area.x, composite_area.y, composite_area.width,
							 composite_area.height);

	pixman_image_unref(part_image);
}

static void screenshot_composite(pixman_image_t **r_image, struct screenshot_state *screenshot)
{
	struct rect base_area = screenshot->region;
	pixman_image_t *base_image = NULL;
	base_image =
		pixman_image_create_bits(PIXMAN_a8b8g8r8, base_area.width, base_area.height, NULL, 0);

	switch (screenshot->type) {
	case SCREENSHOT_TYPE_WLR:
		struct wlr_frame_data *wlr_frame_data;
		darray_foreach (screenshot->wlr_frames, wlr_frame_data) {
			struct rect composite_area = wlr_frame_data->area;
			composite_area.x -= base_area.x;
			composite_area.y -= base_area.y;

			/* add y_invert flag to 4th bit. */
			int32_t transform = wlr_frame_data->transform;
			transform += (wlr_frame_data->flags & ZWLR_SCREENCOPY_FRAME_V1_FLAGS_Y_INVERT) << 4;

			composite_buffer_onto_base(base_image, wlr_frame_data->buffer_data, composite_area,
									   transform);
		}
		break;

	case SCREENSHOT_TYPE_COSMIC:
		struct cosmic_frame_data *cosmic_frame_data;
		darray_foreach (screenshot->cosmic_frames, cosmic_frame_data) {
			struct rect composite_area = cosmic_frame_data->area;
			composite_area.x -= base_area.x;
			composite_area.y -= base_area.y;

			int32_t transform = cosmic_frame_data->transform;
			/* TODO: test transform property. */

			composite_buffer_onto_base(base_image, cosmic_frame_data->buffer_data, composite_area,
									   transform);
		}
		break;

	default:
		break;
	}

	*r_image = base_image;
}

ecode_t screenshot_capture(pixman_image_t **r_image, struct screenshot_state *screenshot)
{
	pixman_image_t *image;
	screenshot_copy(screenshot);
	screenshot_composite(&image, screenshot);

	*r_image = image;
	return WS_OK;
}

ecode_t screenshot_free(struct screenshot_state *screenshot)
{
	struct wlr_frame_data *wlr_frame_data;
	darray_foreach (screenshot->wlr_frames, wlr_frame_data)
		wlr_frame_free(wlr_frame_data);

	struct cosmic_frame_data *cosmic_frame_data;
	darray_foreach (screenshot->cosmic_frames, cosmic_frame_data)
		cosmic_frame_free(cosmic_frame_data);

	darray_free(screenshot->wlr_frames);
	darray_free(screenshot->cosmic_frames);
	free(screenshot);
	return WS_OK;
}
