#ifndef WS_WAYLAND_H
#define WS_WAYLAND_H

#include <wayland-client.h>
#include <cstring>
#include <cstdint>
#include <vector>

#include "wlr-screencopy-unstable-v1.hh"
#include "cosmic-screencopy-unstable-v2.hh"
#include "xdg-output-unstable-v1.hh"

#include "wayshare.hh"
#include "shm.hh"
#include "rect.hh"
#include "image.hh"
#include "logger.hh"

// +-----------------+
// | wayland structs |
// +-----------------+

struct wl_buffer_data_t;
struct wl_output_data_t;
struct wl_state_t;

struct wl_buffer_data_t {
    // wayland objects
    zwlr_screencopy_frame_v1 *frame = nullptr;
    wl_buffer *buffer = nullptr;

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
    void *data = nullptr;
    int *n_ready = nullptr;
};

struct wl_output_data_t {
    // wayland output object
    wl_output *output = nullptr;

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
};

struct wl_state_t {
    // base wayland connection objects
    wl_display *display   = nullptr;
    wl_registry *registry = nullptr;
    wl_shm *shm = nullptr;
    zxdg_output_manager_v1 *output_manager = nullptr;

    // wayland screencopy objects
    zwlr_screencopy_manager_v1 *wlr_screencopy_manager       = nullptr;
    zcosmic_screencopy_manager_v2 *cosmic_screencopy_manager = nullptr;
    std::vector<wl_output_data_t> outputs;
};

// +-----------------------------+
// | wayland managment functions |
// +-----------------------------+

ecode_t wl_state_connect(wl_state_t *r_state, const char *name);

ecode_t wl_state_disconnect(wl_state_t state);

ecode_t wl_buffer_create(wl_state_t state, wl_buffer_data_t buffer_data);

ecode_t wl_buffer_delete(wl_buffer_data_t buffer_data);

// +----------------------+
// | screencopy functions |
// +----------------------+

ecode_t image_wlr_screencopy(image_t *r_image, wl_state_t state, rect_t area, int32_t cursor);

#endif