#ifndef WS_SELECTOR_H
#define WS_SELECTOR_H

#include <stdbool.h>

#include <xkbcommon/xkbcommon.h>
#include <wayland-cursor.h>
#include <linux/input-event-codes.h>

#include "wayshare.h"
#include "wayland.h"
#include "logger.h"
#include "utils.h"

/**
 * TODO:
 * - impl selector window snapping, {SELECTOR_SELECTMODE_SNAP}.
 * - impl custom selector colors and styles.
 * - impl custom input from config file.
 * - improve selection buffer rendering performance.
 */

#define SELECTOR_ANCHORS ZWLR_LAYER_SURFACE_V1_ANCHOR_TOP       \
                       | ZWLR_LAYER_SURFACE_V1_ANCHOR_BOTTOM    \
                       | ZWLR_LAYER_SURFACE_V1_ANCHOR_LEFT      \
                       | ZWLR_LAYER_SURFACE_V1_ANCHOR_RIGHT

struct selector_layer {
	struct selector_state *state;
	struct zwlr_layer_surface_v1 *layer_surface;
	struct wl_output_data *output_data;
	struct wl_surface *surface;
	bool configured;
	bool init;
	struct wl_buffer_data *buffers[2];	/* double buffered per wayland spec. */
	struct wl_buffer_data *current_buffer;
};

ecode_t selector_layer_create(struct selector_layer **r_layer, struct selector_state *state,
							  struct wl_output_data *output_data);

ecode_t selector_layer_free(struct selector_layer *layer);

struct selector_pointer {
	struct selector_state *state;
	struct wl_pointer *pointer;
	struct selector_layer *current_layer;
	struct wl_surface *surface;
	int32_t init_x, init_y;
};

ecode_t selector_pointer_create(struct selector_pointer **r_pointer,
								struct selector_state *state, struct wl_seat_data *seat_data);

ecode_t selector_pointer_free(struct selector_pointer *pointer);

struct selector_keyboard {
	struct selector_state *state;
	struct wl_keyboard *keyboard;
	uint32_t format;
	/* xkb variables */
	struct xkb_context *xkb_context;
	struct xkb_keymap *xkb_keymap;
	struct xkb_state *xkb_state;
};

ecode_t selector_keyboard_create(struct selector_keyboard **r_keyboard,
								 struct selector_state *state, struct wl_seat_data *seat_data);

ecode_t selector_keyboard_free(struct selector_keyboard *keyboard);

struct selector_theme {
	/* colors in format [WL_SHM_FORMAT_ABGR8888], or 0xAARRGGBB */
	uint32_t background_color;
	uint32_t border_color;
	uint32_t border_size;
	uint32_t selection_color;
};

enum selector_mode {
	SELECTOR_SELECTMODE_NONE = 0,
	SELECTOR_SELECTMODE_SELECT,
	SELECTOR_SELECTMODE_ANCHOR,
	/* TODO: ext toplevel window snapping using wm's ctl, e.g. "hyprctl clients -j".
	   there is currently no wayland prot that provides this feature, sad. */
	SELECTOR_SELECTMODE_SNAP
};

struct selector_state {
	struct wl_state *wayland;
	struct darray *layers;	   /* darray<struct selector_layer *> */
	struct darray *keyboards;  /* darray<struct selector_keyboard *> */
	struct darray *pointers;   /* darray<struct selector_pointer *> */
	struct selector_theme theme;
	struct anchored_rect selection;
	enum selector_mode mode;
	bool has_selection;
	bool running;
};

ecode_t selector_create(struct selector_state **r_selector, struct wl_state *wayland);

ecode_t selector_apply(struct rect *r_selection, struct selector_state *selector);

ecode_t selector_free(struct selector_state *selector);

void selector_request_frame(struct selector_state *state);

#endif						   // WS_SELECTOR_H
