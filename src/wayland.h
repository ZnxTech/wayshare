#ifndef WS_WAYLAND_H
#define WS_WAYLAND_H

#include <string.h>
#include <stdint.h>

#include <wayland-client.h>
#include "wlr-screencopy-unstable-v1.h"
#include "cosmic-screencopy-unstable-v2.h"
#include "xdg-output-unstable-v1.h"

#include "utils.h"
#include "wayshare.h"
#include "image.h"
#include "logger.h"

// +-----------------+
// | wayland structs |
// +-----------------+

struct wl_state {
	struct wl_display *display;
	struct wl_registry *registry;
	struct wl_shm *shm;
	struct zxdg_output_manager_v1 *output_manager;
	struct zwlr_screencopy_manager_v1 *wlr_screencopy_manager;
	struct zcosmic_screencopy_manager_v2 *cosmic_screencopy_manager;
	struct darray *outputs;
};

struct wl_output_data {
	struct wl_state *state;
	struct wl_output *output;
	union {
		struct {
			int32_t x;
			int32_t y;
			int32_t width;
			int32_t height;
		};
		struct rect area;
	};
	int32_t transform;
	int32_t scale_factor;
};

struct wl_shm_pool_data {
	struct wl_state *state;
	struct wl_shm_pool *shm_pool;
	size_t total_size;
	size_t used_size;
	uint32_t ref_count;
	void *data;
};

struct wl_buffer_data {
	struct wl_state *state;
	struct wl_shm_pool_data *ref_shm_pool_data;
	struct wl_buffer *buffer;
	uint32_t width;
	uint32_t height;
	uint32_t stride;
	uint32_t format;
	size_t size;
	void *data;
};

struct wlr_frame_data {
	struct wl_state *state;
	struct zwlr_screencopy_frame_v1 *frame;
	struct wl_buffer_data *buffer_data;
	struct rect region;
	int32_t transform;
	uint32_t *n_ready;
	bool ready;
};

// +-----------------------------+
// | wayland managment functions |
// +-----------------------------+

ecode_t wl_state_connect(struct wl_state **r_state, const char *name);

ecode_t wl_state_disconnect(struct wl_state *state);

ecode_t wl_output_create(struct wl_output_data **r_output_data, struct wl_state *state,
						 struct wl_output *output);

ecode_t wl_output_free(struct wl_output_data *output_data);

ecode_t wl_shm_pool_create(struct wl_shm_pool_data **r_shm_pool_data, struct wl_state *state,
						   size_t size);

ecode_t wl_shm_pool_free(struct wl_shm_pool_data *shm_pool_data);

ecode_t wl_buffer_create(struct wl_buffer_data **r_buffer_data,
						 struct wl_shm_pool_data *shm_pool_data, uint32_t width, uint32_t height,
						 uint32_t stride, uint32_t format);

ecode_t wl_buffer_free(struct wl_buffer_data *buffer_data);

ecode_t wlr_frame_create(struct wlr_frame_data **r_frame_data, struct wl_state *state,
						 struct wl_output_data *output_data, uint32_t cursor, uint32_t * n_ready);

ecode_t wlr_frame_region_create(struct wlr_frame_data **r_frame_data, struct wl_state *state,
								struct wl_output_data *output_data, struct rect region,
								uint32_t cursor, uint32_t * n_ready);

ecode_t wlr_frame_abs_region_create(struct wlr_frame_data **r_frame_data, struct wl_state *state,
									struct wl_output_data *output_data, struct rect region,
									uint32_t cursor, uint32_t * n_ready);

ecode_t wlr_frame_get_image(struct image *r_image, struct wlr_frame_data *frame_data);

ecode_t wlr_frame_free(struct wlr_frame_data *frame_data);

// +----------------------+
// | screencopy functions |
// +----------------------+

ecode_t image_wlr_screencopy(struct image *r_image, struct wl_state *state, struct rect region);

#endif						   // WS_WAYLAND_H
