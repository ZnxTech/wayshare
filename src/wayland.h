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

typedef struct wl_buffer_data_s {
    // wayland objects
    struct zwlr_screencopy_frame_v1 *frame;
    struct wl_buffer *buffer;

    // buffer data
    int32_t format;
    int32_t stride;
    int32_t size;
    int32_t transform;
    union {
        struct {
            int32_t x;
            int32_t y;
            int32_t width;
            int32_t height;
        };
        rect_t area;
    };
    void *data;
    int *n_ready;
} wl_buffer_data_t;

typedef struct wl_output_data_s {
    // wayland output object
    struct wl_output *output;

    // output data
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

typedef struct wl_state_s {
    // base wayland connection objects
    struct wl_display *display;
    struct wl_registry *registry;
    struct wl_shm *shm;
    struct zxdg_output_manager_v1 *output_manager;

    // wayland screencopy objects
    struct zwlr_screencopy_manager_v1 *wlr_screencopy_manager;
    struct zcosmic_screencopy_manager_v2 *cosmic_screencopy_manager;
    darray_t *outputs;
} wl_state_t;

// +-----------------------------+
// | wayland managment functions |
// +-----------------------------+

ecode_t wl_state_connect(wl_state_t *r_state, const char *name);

ecode_t wl_state_disconnect(wl_state_t state);

ecode_t wl_buffer_create(wl_state_t state, wl_buffer_data_t *buffer_data);

ecode_t wl_buffer_delete(wl_buffer_data_t buffer_data);

// +----------------------+
// | screencopy functions |
// +----------------------+

ecode_t image_wlr_screencopy(image_t *r_image, wl_state_t state, rect_t area, int32_t cursor);

#endif