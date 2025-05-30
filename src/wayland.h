#ifndef WS_WAYLAND_H
#define WS_WAYLAND_H

#include <stdint.h>
#include <string.h>

#include "cosmic-image-source-unstable-v1.h"
#include "cosmic-screencopy-unstable-v2.h"
#include "cursor-shape-v1.h"
#include "wlr-layer-shell-unstable-v1.h"
#include "wlr-screencopy-unstable-v1.h"
#include "xdg-output-unstable-v1.h"
#include <wayland-client.h>

#include "logger.h"
#include "utils.h"
#include "wayshare.h"

/**
 * <struct wl_state>
 * container struct for wayland global object along with the display and the registry.
 * the global objects that the struct contains may be none existant, invalid objects will be-
 * {NULL}, or in the case of multi-object darrays, they will have a {darray::count} of {0}.
 */
struct wl_state {
	struct wl_display *display;
	struct wl_registry *registry;
	struct wl_compositor *compositor;
	struct darray *outputs;
	struct zxdg_output_manager_v1 *xdg_output_manager;
	struct wp_cursor_shape_manager_v1 *cursor_shape_manager;
	struct darray *seats;
	struct wl_shm *shm;
	struct zwlr_layer_shell_v1 *wlr_layer_shell;
	struct zwlr_screencopy_manager_v1 *wlr_screencopy_manager;
	struct zcosmic_output_image_source_manager_v1 *cosmic_output_image_source_manager;
	struct zcosmic_screencopy_manager_v2 *cosmic_screencopy_manager;
};

ecode_t wl_state_connect(struct wl_state **r_state, const char *name);

ecode_t wl_state_disconnect(struct wl_state *state);

/**
 * <struct wl_output_data>
 * container struct for <struct wl_outout *> containing the data retrived from its creation events.
 * if the compositor does not support <struct zxdg_output_manager_v1 *> the logistical size with be
 * estimated using its {scale_factor} and {transform}.
 */
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

ecode_t wl_output_create(struct wl_output_data **r_output_data, struct wl_state *state,
						 struct wl_output *output);

ecode_t wl_output_free(struct wl_output_data *output_data);

/**
 * <struct wl_seat_data>
 * container struct for <struct wl_seat *> mainly to store its capability.
 */
struct wl_seat_data {
	struct wl_state *state;
	struct wl_seat *seat;
	uint32_t capabilities;
};

ecode_t wl_seat_create(struct wl_seat_data **r_seat_data, struct wl_state *state,
					   struct wl_seat *seat);

ecode_t wl_seat_free(struct wl_seat_data *seat_data);

/**
 * <struct wl_shm_pool_data>
 * container and manager struct for <struct wl_shm_pool *> to store the mmap'ed shm file
 * and manage its buffer childern in a form of a refrence counter {ref_count}, the pool will
 * automatically be cleared and freed once all buffer children are freed as-well.
 */
struct wl_shm_pool_data {
	struct wl_state *state;
	struct wl_shm_pool *shm_pool;
	size_t total_size;
	size_t used_size;
	uint32_t ref_count;
	void *data;
};

ecode_t wl_shm_pool_create(struct wl_shm_pool_data **r_shm_pool_data, struct wl_state *state,
						   size_t size);

ecode_t wl_shm_pool_free(struct wl_shm_pool_data *shm_pool_data);

/**
 * <struct wl_buffer_data>
 * container struct for <struct wl_buffer *> as-well as storing it's size, format, data pointer
 * and parent shm pool variables that were given when the buffer was created client-side or
 * given from the server-side.
 * additionally, when freeing a buffer, if the parent shm pool has no other buffers created
 * from it still in memory it will free itself as mentioned in it's description.
 */
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
	bool busy;
};

ecode_t wl_buffer_create(struct wl_buffer_data **r_buffer_data,
						 struct wl_shm_pool_data *shm_pool_data, uint32_t width, uint32_t height,
						 uint32_t stride, uint32_t format);

ecode_t wl_buffer_free(struct wl_buffer_data *buffer_data);

#endif // WS_WAYLAND_H
