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

typedef struct wl_state_s {
    struct wl_display *display;
    struct wl_registry *registry;
    struct wl_shm *shm;
    struct zxdg_output_manager_v1 *output_manager;
    struct zwlr_screencopy_manager_v1 *wlr_screencopy_manager;
    struct zcosmic_screencopy_manager_v2 *cosmic_screencopy_manager;
    darray_t *outputs;
} wl_state_t;

typedef struct wl_output_data_s {
    wl_state_t *state;
    struct wl_output *output;
    union {
        struct {
            int32_t x;
            int32_t y;
            int32_t width;
            int32_t height;
        };
        rect_t area;
    };
    int32_t transform;
    int32_t scale_factor;
} wl_output_data_t;

typedef struct wl_shm_pool_data_s {
    wl_state_t *state;
    struct wl_shm_pool *shm_pool;
    size_t total_size;
    size_t used_size;
    uint32_t ref_count;
    void *data;
} wl_shm_pool_data_t;

typedef struct wl_buffer_data_s {
    wl_state_t *state;
    wl_shm_pool_data_t *ref_shm_pool_data;
    struct wl_buffer *buffer;
    uint32_t width;
    uint32_t height;
    uint32_t stride;
    uint32_t format;
    size_t size;
    void *data;
} wl_buffer_data_t;

typedef struct wlr_frame_data_s {
    wl_state_t *state;
    struct zwlr_screencopy_frame_v1 *frame;
    wl_buffer_data_t *buffer_data;
    rect_t region;
    int32_t transform;
    uint32_t *n_ready;
    bool ready;
} wlr_frame_data_t;

// +-----------------------------+
// | wayland managment functions |
// +-----------------------------+

ecode_t wl_state_connect(wl_state_t **r_state, const char *name);

ecode_t wl_state_disconnect(wl_state_t *state);

ecode_t wl_output_create(wl_output_data_t **r_output_data, wl_state_t *state, struct wl_output *output);

ecode_t wl_output_free(wl_output_data_t *output_data);

ecode_t wl_shm_pool_create(wl_shm_pool_data_t **r_shm_pool_data, wl_state_t *state, size_t size);

ecode_t wl_shm_pool_free(wl_shm_pool_data_t *shm_pool_data);

ecode_t wl_buffer_create(wl_buffer_data_t **r_buffer_data, wl_shm_pool_data_t *shm_pool_data,
        uint32_t width, uint32_t height, uint32_t stride, uint32_t format);

ecode_t wl_buffer_free(wl_buffer_data_t *buffer_data);

ecode_t wlr_frame_create(wlr_frame_data_t **r_frame_data, wl_state_t *state,
        wl_output_data_t *output_data, uint32_t cursor, uint32_t *n_ready);

ecode_t wlr_frame_region_create(wlr_frame_data_t **r_frame_data, wl_state_t *state,
        wl_output_data_t *output_data, rect_t region, uint32_t cursor, uint32_t *n_ready);

ecode_t wlr_frame_abs_region_create(wlr_frame_data_t **r_frame_data, wl_state_t *state,
        wl_output_data_t *output_data, rect_t region, uint32_t cursor, uint32_t *n_ready);

ecode_t wlr_frame_get_image(image_t *r_image, wlr_frame_data_t *frame_data);

ecode_t wlr_frame_free(wlr_frame_data_t *frame_data);

// +----------------------+
// | screencopy functions |
// +----------------------+

ecode_t image_wlr_screencopy(image_t *r_image, wl_state_t *state, rect_t region);

#endif // WS_WAYLAND_H