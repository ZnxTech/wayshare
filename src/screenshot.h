#ifndef WS_SCREENSHOT_H
#define WS_SCREENSHOT_H

#include <stdint.h>

#include <pixman.h>

#include "wayshare.h"
#include "utils.h"
#include "wayland.h"

/**
 * TODO:
 * - test the cosmic part of the code.
 * - impl support for other DEs like Gnome and KDE.
 * - change the wlr-screencopy to use region instead of output when hyprland is fixed.
 */

struct wlr_frame_data {
	struct screenshot_state *state;
	struct zwlr_screencopy_frame_v1 *wlr_frame;
	struct wl_buffer_data *buffer_data;
	struct rect area;
	/* buffer events data. */
	uint32_t format;
	uint32_t width;
	uint32_t height;
	uint32_t stride;
	uint32_t flags;
	int32_t transform;
	bool ready;
};

ecode_t wlr_frame_create(struct wlr_frame_data **r_frame_data, struct screenshot_state *state,
						 struct wl_output_data *output_data);

ecode_t wlr_frame_create_region(struct wlr_frame_data **r_frame_data,
								struct screenshot_state *state, struct wl_output_data *output_data,
								struct rect region);

ecode_t wlr_frame_copy(struct wlr_frame_data *frame_data);

ecode_t wlr_frame_free(struct wlr_frame_data *frame_data);

struct cosmic_frame_data {
	struct screenshot_state *state;
	struct zcosmic_image_source_v1 *cosmic_image_source;
	struct zcosmic_screencopy_session_v2 *cosmic_session;
	struct zcosmic_screencopy_frame_v2 *cosmic_frame;
	struct wl_buffer_data *buffer_data;
	struct rect area;
	/* buffer events data. */
	uint32_t format;
	uint32_t width;
	uint32_t height;
	uint32_t stride;
	/* secondary buffer rotation? */
	uint32_t buffer_transform;
	int32_t transform;
	bool ready;
};

ecode_t cosmic_frame_create(struct cosmic_frame_data **r_frame_data, struct screenshot_state *state,
							struct wl_output_data *output_data);

ecode_t cosmic_frame_copy(struct cosmic_frame_data *frame_data);

ecode_t cosmic_frame_free(struct cosmic_frame_data *frame_data);

enum screenshot_type {
	SCREENSHOT_TYPE_NONE = 0,
	SCREENSHOT_TYPE_WLR,
	SCREENSHOT_TYPE_COSMIC,
	/* currently theres no screenshoting
	   method for Gnome and KDE via wayland. */
};

struct screenshot_state {
	struct wl_state *wayland;
	enum screenshot_type type;
	struct rect region;
	/* stage managment. */
	size_t shm_pool_size;
	struct wl_shm_pool_data *shm_pool_data;
	uint32_t n_requested;
	uint32_t n_buffered;
	uint32_t n_ready;
	/* screecopy frames. */
	struct darray *wlr_frames; /* darray<struct wlr_frame_data *> */
	struct darray *cosmic_frames;	/* darray<struct cosmic_frame_data *> */
};

ecode_t screenshot_create_output(struct screenshot_state **r_screenshot, struct wl_state *wayland,
								 struct wl_output_data *output_data);

ecode_t screenshot_create_region(struct screenshot_state **r_screenshot, struct wl_state *wayland,
								 struct rect region);

ecode_t screenshot_capture(pixman_image_t ** r_image, struct screenshot_state *screenshot);

ecode_t screenshot_free(struct screenshot_state *screenshot);

#endif						   // WS_SCREENSHOT_H
