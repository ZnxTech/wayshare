#include "selector.h"

static void null_func()
{
	/* left empty. */
}

/* forward declaration for static and/or private functions. */
static void selector_start_selection(struct selector_state *state, int32_t init_x, int32_t init_y);

static void selector_cancel_selection(struct selector_state *state);

static void selector_end_selection(struct selector_state *state);

static void selector_quit_selection(struct selector_state *state);

static void selector_handle_anchormode(struct selector_state *state, uint32_t key_state);
/* --- */

static void layer_surface_event_configure(void *data,
										  struct zwlr_layer_surface_v1 *zwlr_layer_surface_v1,
										  uint32_t serial, uint32_t width, uint32_t height)
{
	struct selector_layer *layer = data;
	/* ignore size hint, just send ack config request. */
	zwlr_layer_surface_v1_ack_configure(layer->layer_surface, serial);
	layer->configured = true;
}

static const struct zwlr_layer_surface_v1_listener layer_surface_listener = {
	.configure = layer_surface_event_configure
};

static void selector_init_layer_surface(struct selector_layer *layer)
{
	struct wl_state *wayland = layer->state->wayland;
	layer->layer_surface =
		zwlr_layer_shell_v1_get_layer_surface(wayland->wlr_layer_shell,
											  layer->surface, layer->output_data->output,
											  ZWLR_LAYER_SHELL_V1_LAYER_OVERLAY, "selector");
	zwlr_layer_surface_v1_add_listener(layer->layer_surface, &layer_surface_listener, layer);
	zwlr_layer_surface_v1_set_anchor(layer->layer_surface, SELECTOR_ANCHORS);
	zwlr_layer_surface_v1_set_keyboard_interactivity(layer->layer_surface, 1);
	zwlr_layer_surface_v1_set_exclusive_zone(layer->layer_surface, -1);
	wl_surface_commit(layer->surface);
}

static void selector_init_buffers(struct selector_layer *layer)
{
	struct rect output_area = layer->output_data->area;
	const size_t buffer_size = output_area.width * output_area.height;

	struct wl_shm_pool_data *shm_pool_data;
	wl_shm_pool_create(&shm_pool_data, layer->state->wayland, 2 * 4 * buffer_size);
	/* create both buffers, multiply by 4 for format size. format: 0xRRGGBBAA */
	wl_buffer_create(&layer->buffers[0], shm_pool_data, output_area.width, output_area.height,
					 4 * output_area.width, WL_SHM_FORMAT_ARGB8888);
	wl_buffer_create(&layer->buffers[1], shm_pool_data, output_area.width, output_area.height,
					 4 * output_area.width, WL_SHM_FORMAT_ARGB8888);
}

ecode_t selector_layer_create(struct selector_layer **r_layer, struct selector_state *state,
							  struct wl_output_data *output_data)
{
	struct selector_layer *layer;
	layer = calloc(sizeof(struct selector_layer), 1);
	layer->state = state;
	layer->output_data = output_data;
	layer->surface = wl_compositor_create_surface(state->wayland->compositor);
	selector_init_layer_surface(layer);	/* creates and configs {layer::layer_surface}. */
	selector_init_buffers(layer);

	*r_layer = layer;
	return WS_OK;
}

ecode_t selector_layer_free(struct selector_layer *layer)
{
	zwlr_layer_surface_v1_destroy(layer->layer_surface);
	wl_surface_destroy(layer->surface);
	wl_buffer_free(layer->buffers[0]);
	wl_buffer_free(layer->buffers[1]);
	free(layer);
	return WS_OK;
}

static void buffer_render_fill(uint32_t *buffer_data, struct rect buffer_area, uint32_t color)
{
	const uint64_t buffer_size = buffer_area.width * buffer_area.height;
	for (uint64_t i = 0; i < buffer_size; i++)
		buffer_data[i] = color;
}

static void buffer_render_rect_fill(uint32_t *buffer_data, struct rect buffer_area,
									struct rect draw_area, uint32_t color)
{
	uint32_t y_0 = draw_area.y - buffer_area.y;
	uint32_t x_0 = draw_area.x - buffer_area.x;

	for (uint32_t y = y_0; y < draw_area.height + y_0; y++) {
		for (uint32_t x = x_0; x < draw_area.width + x_0; x++) {
			buffer_data[buffer_area.width * y + x] = color;
		}
	}
}

static void layer_render_background(struct selector_layer *layer)
{
	uint32_t *buffer_data = layer->current_buffer->data;
	struct rect buffer_area = layer->output_data->area;
	buffer_render_fill(buffer_data, buffer_area, 0x20202020);
}

static void layer_render_selection(struct selector_layer *layer)
{
	if (!layer->state->has_selection)
		return;

	uint32_t *buffer_data = layer->current_buffer->data;
	struct rect buffer_area = layer->output_data->area;
	struct rect selection_area = rect_unanchor(layer->state->selection);
	struct rect selection_inter = rect_intersect(buffer_area, selection_area);
	if (!rect_is_valid(selection_inter))
		return;

	buffer_render_rect_fill(buffer_data, buffer_area, selection_inter, 0x00000000);
}

static struct wl_buffer_data *buffer_get_next(struct selector_layer *layer)
{
	for (size_t i = 0; i < 2; i++) {
		if (!layer->buffers[i]->busy)
			return layer->buffers[i];
	}
	return NULL;
}

static void layer_send_frame(struct selector_layer *layer)
{
	if (!layer->configured)
		return;

	layer->current_buffer = buffer_get_next(layer);
	if (layer->current_buffer == NULL)
		return;

	layer->current_buffer->busy = true;
	layer_render_background(layer);
	layer_render_selection(layer);

	wl_surface_attach(layer->surface, layer->current_buffer->buffer, 0, 0);
	wl_surface_damage(layer->surface, 0, 0, layer->output_data->width, layer->output_data->height);
	wl_surface_commit(layer->surface);
}

static void wl_callback_event_done(void *data, struct wl_callback *wl_callback,
								   uint32_t callback_data)
{
	struct selector_layer *layer = data;
	layer_send_frame(layer);
	wl_callback_destroy(wl_callback);
}

static const struct wl_callback_listener callback_listener = {
	.done = wl_callback_event_done
};

static void layer_request_frame(struct selector_layer *layer)
{
	struct wl_callback *callback = wl_surface_frame(layer->surface);
	wl_callback_add_listener(callback, &callback_listener, layer);
	wl_surface_commit(layer->surface);
}

static struct selector_layer *layer_from_surface(struct selector_state *state,
												 struct wl_surface *surface)
{
	struct selector_layer *layer;
	darray_foreach(state->layers, layer) {
		if (layer->surface == surface)
			return layer;
	}
	return NULL;
}

void selector_request_frame(struct selector_state *state)
{
	struct rect selection_area = rect_unanchor(state->selection);
	struct selector_layer *layer;
	darray_foreach(state->layers, layer) {
		struct rect layer_area = layer->output_data->area;
		if (rect_is_intersecting(layer_area, selection_area))
			layer_request_frame(layer);
	}
}

static void wl_pointer_event_enter(void *data, struct wl_pointer *wl_pointer, uint32_t serial,
								   struct wl_surface *surface, wl_fixed_t surface_x,
								   wl_fixed_t surface_y)
{
	struct selector_pointer *pointer = data;
	struct selector_layer *layer = layer_from_surface(pointer->state, surface);
	pointer->current_layer = (layer != NULL) ? layer : NULL;

	struct wl_state *wayland = pointer->state->wayland;
	if (wayland->cursor_shape_manager != NULL) {
		struct wp_cursor_shape_device_v1 *device;
		device = wp_cursor_shape_manager_v1_get_pointer(wayland->cursor_shape_manager,
														pointer->pointer);
		wp_cursor_shape_device_v1_set_shape(device, serial,
											WP_CURSOR_SHAPE_DEVICE_V1_SHAPE_CROSSHAIR);
		wp_cursor_shape_device_v1_destroy(device);
	} else {
		/* TODO: pointer surface/shape. */
	}

	selector_request_frame(pointer->state);
}

static void wl_pointer_event_leave(void *data, struct wl_pointer *wl_pointer, uint32_t serial,
								   struct wl_surface *surface)
{
	struct selector_pointer *pointer = data;
	pointer->current_layer = NULL;
}

static void wl_pointer_event_motion(void *data, struct wl_pointer *wl_pointer, uint32_t time,
									wl_fixed_t surface_x, wl_fixed_t surface_y)
{
	struct selector_pointer *pointer = data;
	if (pointer->current_layer == NULL)
		return;

	struct rect output_area = pointer->current_layer->output_data->area;
	int32_t absolute_x = wl_fixed_to_int(surface_x) + output_area.x;
	int32_t absolute_y = wl_fixed_to_int(surface_y) + output_area.y;
	pointer->init_x = absolute_x;
	pointer->init_y = absolute_y;

	struct selector_state *state = pointer->state;
	switch (state->mode) {
	case SELECTOR_SELECTMODE_SELECT:
		state->selection.vector_x = absolute_x;
		state->selection.vector_y = absolute_y;
		selector_request_frame(state);
		break;

	case SELECTOR_SELECTMODE_ANCHOR:
		state->selection.anchor_x += absolute_x - state->selection.vector_x;
		state->selection.anchor_y += absolute_y - state->selection.vector_y;
		state->selection.vector_x += absolute_x - state->selection.vector_x;
		state->selection.vector_y += absolute_y - state->selection.vector_y;
		selector_request_frame(state);
		break;
	}
}

static void wl_pointer_event_button(void *data, struct wl_pointer *wl_pointer, uint32_t serial,
									uint32_t time, uint32_t button, uint32_t button_state)
{
	struct selector_pointer *pointer = data;

	switch (button) {
	case BTN_LEFT:
		if (button_state == WL_POINTER_BUTTON_STATE_PRESSED)
			selector_start_selection(pointer->state, pointer->init_x, pointer->init_y);

		if (button_state == WL_POINTER_BUTTON_STATE_RELEASED)
			selector_end_selection(pointer->state);
		break;

	case BTN_RIGHT:
		if (button_state == WL_POINTER_BUTTON_STATE_PRESSED) {
			selector_cancel_selection(pointer->state);
			selector_request_frame(pointer->state);
		}
		break;
	}
}

static const struct wl_pointer_listener pointer_listener = {
	.enter = wl_pointer_event_enter,
	.leave = wl_pointer_event_leave,
	.motion = wl_pointer_event_motion,
	.button = wl_pointer_event_button,
	/* unused: */
	.axis = null_func,
	.frame = null_func,
	.axis_source = null_func,
	.axis_stop = null_func,
	.axis_discrete = null_func,
	.axis_value120 = null_func,
	.axis_relative_direction = null_func
};

ecode_t selector_pointer_create(struct selector_pointer **r_pointer,
								struct selector_state *state, struct wl_seat_data *seat_data)
{
	struct selector_pointer *pointer;
	pointer = calloc(sizeof(struct selector_pointer), 1);
	pointer->state = state;
	pointer->pointer = wl_seat_get_pointer(seat_data->seat);
	wl_pointer_add_listener(pointer->pointer, &pointer_listener, pointer);

	*r_pointer = pointer;
	return WS_OK;
}

ecode_t selector_pointer_free(struct selector_pointer *pointer)
{
	wl_pointer_destroy(pointer->pointer);
	free(pointer);
	return WS_OK;
}

static void wl_keyboard_event_keymap(void *data, struct wl_keyboard *wl_keyboard, uint32_t format,
									 int32_t fd, uint32_t size)
{
	struct selector_keyboard *keyboard = data;
	keyboard->format = format;
	switch (format) {
	case WL_KEYBOARD_KEYMAP_FORMAT_NO_KEYMAP:
		break;

	case WL_KEYBOARD_KEYMAP_FORMAT_XKB_V1:
		void *buffer = mmap(NULL, size, PROT_READ, MAP_PRIVATE, fd, 0);
		if (buffer == MAP_FAILED)
			return;

		keyboard->xkb_context = xkb_context_new(XKB_CONTEXT_NO_FLAGS);
		keyboard->xkb_keymap = xkb_keymap_new_from_string(keyboard->xkb_context, buffer,
														  XKB_KEYMAP_FORMAT_TEXT_V1,
														  XKB_KEYMAP_COMPILE_NO_FLAGS);
		keyboard->xkb_state = xkb_state_new(keyboard->xkb_keymap);
		break;
	}
}

static void handle_xkb_key(struct selector_keyboard *keyboard, xkb_keysym_t keysym,
						   uint32_t key_state)
{
	struct selector_state *state = keyboard->state;
	switch (keysym) {
	case XKB_KEY_Shift_L:
	case XKB_KEY_Shift_R:
		selector_handle_anchormode(state, key_state);
		break;

	case XKB_KEY_Escape:
		selector_quit_selection(state);
		break;
	}
}

static void wl_keyboard_event_key(void *data, struct wl_keyboard *wl_keyboard, uint32_t serial,
								  uint32_t time, uint32_t key, uint32_t key_state)
{
	struct selector_keyboard *keyboard = data;
	switch (keyboard->format) {
	case WL_KEYBOARD_KEYMAP_FORMAT_NO_KEYMAP:
		break;

	case WL_KEYBOARD_KEYMAP_FORMAT_XKB_V1:
		xkb_keysym_t keysym = xkb_state_key_get_one_sym(keyboard->xkb_state, key + 8);
		handle_xkb_key(keyboard, keysym, key_state);
		break;
	}
}

static const struct wl_keyboard_listener keyboard_listener = {
	.keymap = wl_keyboard_event_keymap,
	.key = wl_keyboard_event_key,
	/* unused: */
	.enter = null_func,
	.leave = null_func,
	.modifiers = null_func,
	.repeat_info = null_func
};

ecode_t selector_keyboard_create(struct selector_keyboard **r_keyboard,
								 struct selector_state *state, struct wl_seat_data *seat_data)
{
	struct selector_keyboard *keyboard;
	keyboard = calloc(sizeof(struct selector_keyboard), 1);
	keyboard->state = state;
	keyboard->keyboard = wl_seat_get_keyboard(seat_data->seat);
	wl_keyboard_add_listener(keyboard->keyboard, &keyboard_listener, keyboard);

	*r_keyboard = keyboard;
	return WS_OK;
}

ecode_t selector_keyboard_free(struct selector_keyboard *keyboard)
{
	switch (keyboard->format) {
	case WL_KEYBOARD_KEYMAP_FORMAT_NO_KEYMAP:
		break;

	case WL_KEYBOARD_KEYMAP_FORMAT_XKB_V1:
		xkb_state_unref(keyboard->xkb_state);
		xkb_keymap_unref(keyboard->xkb_keymap);
		xkb_context_unref(keyboard->xkb_context);
		break;
	}

	wl_keyboard_destroy(keyboard->keyboard);
	free(keyboard);
	return WS_OK;
}

static void selector_start_selection(struct selector_state *state, int32_t init_x, int32_t init_y)
{
	state->selection.anchor_x = init_x;
	state->selection.anchor_y = init_y;
	state->mode = SELECTOR_SELECTMODE_SELECT;
	state->has_selection = true;
}

static void selector_cancel_selection(struct selector_state *state)
{
	state->mode = SELECTOR_SELECTMODE_NONE;
	state->has_selection = false;
}

static void selector_end_selection(struct selector_state *state)
{
	if (!state->has_selection)
		return;				   /* selector has no selection area, can not end. */

	state->mode = SELECTOR_SELECTMODE_NONE;
	state->running = false;
}

static void selector_quit_selection(struct selector_state *state)
{
	state->mode = SELECTOR_SELECTMODE_NONE;
	state->has_selection = false;
	state->running = false;
}

static void selector_handle_anchormode(struct selector_state *state, uint32_t key_state)
{
	/* counter of how many key are held down. */
	static int32_t key_count = 0;

	/* ignore repeat key. */
	if (key_state > 1)
		return;

	if (key_state > 0)
		key_count++;
	else
		key_count--;

	/* ensure no release only keys. */
	if (key_count < 0)
		key_count = 0;

	if (key_count != 0 && state->mode == SELECTOR_SELECTMODE_SELECT)
		state->mode = SELECTOR_SELECTMODE_ANCHOR;

	if (key_count == 0 && state->mode == SELECTOR_SELECTMODE_ANCHOR)
		state->mode = SELECTOR_SELECTMODE_SELECT;
}

static void selector_init_inputs(struct selector_state *state)
{
	struct wl_seat_data *seat_data;
	darray_foreach(state->wayland->seats, seat_data) {
		if (seat_data->capabilities & WL_SEAT_CAPABILITY_POINTER) {
			struct selector_pointer *pointer;
			selector_pointer_create(&pointer, state, seat_data);
			darray_append(state->pointers, &pointer);
		}

		if (seat_data->capabilities & WL_SEAT_CAPABILITY_KEYBOARD) {
			struct selector_keyboard *keyboard;
			selector_keyboard_create(&keyboard, state, seat_data);
			darray_append(state->keyboards, &keyboard);
		}
	}

	/* get seat callbacks, mainly a <struct wl_keyboard *>'s keymap. */
	wl_display_roundtrip(state->wayland->display);
}

ecode_t selector_init_layers(struct selector_state *state)
{
	if (state->wayland->wlr_layer_shell == NULL)
		return WSE_SELECTOR_NWLR_LAYER;

	struct wl_output_data *output_data;
	darray_foreach(state->wayland->outputs, output_data) {
		struct selector_layer *layer;
		selector_layer_create(&layer, state, output_data);
		darray_append(state->layers, &layer);
	}

	/* init layers and get configuration events. */
	wl_display_roundtrip(state->wayland->display);
	return WS_OK;
}

ecode_t selector_create(struct selector_state **r_selector, struct wl_state *wayland)
{
	struct selector_state *selector;
	selector = calloc(sizeof(struct selector_state), 1);
	selector->layers = darray_init(sizeof(struct selector_layer *), 1);
	selector->pointers = darray_init(sizeof(struct selector_pointer *), 1);
	selector->keyboards = darray_init(sizeof(struct selector_keyboard *), 1);
	selector->wayland = wayland;

	/* init wayland input devices. */
	selector_init_inputs(selector);
	/* init selector wlr layers. */
	ecode_t code = selector_init_layers(selector);
	if (!code)
		return code;

	*r_selector = selector;
	return WS_OK;
}

ecode_t selector_apply(struct rect *r_selection, struct selector_state *selector)
{
	struct selector_layer *layer;
	darray_foreach(selector->layers, layer)
		layer_send_frame(layer);

	selector->running = true;
	while (selector->running && wl_display_dispatch(selector->wayland->display) != -1) {
		/* left empty. */
	}

	if (!selector->has_selection)
		return WSE_SELECTOR_NSELECTION;

	WS_LOGF(WS_SEV_INFO, "selection: %i,%i %ix%i\n",
			selector->selection.anchor_x,
			selector->selection.anchor_y,
			abs(selector->selection.anchor_x - selector->selection.vector_x),
			abs(selector->selection.anchor_y - selector->selection.vector_y));

	*r_selection = rect_unanchor(selector->selection);
	return WS_OK;
}

ecode_t selector_free(struct selector_state *selector)
{
	struct selector_layer *layer;
	darray_foreach(selector->layers, layer)
		selector_layer_free(layer);
	darray_free(selector->layers);

	struct selector_pointer *pointer;
	darray_foreach(selector->pointers, pointer)
		selector_pointer_free(pointer);
	darray_free(selector->pointers);

	struct selector_keyboard *keyboard;
	darray_foreach(selector->keyboards, keyboard)
		selector_keyboard_free(keyboard);
	darray_free(selector->keyboards);

	free(selector);
	return WS_OK;
}
