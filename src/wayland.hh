#ifndef WAYLAND_H
#define WAYLAND_H

#include <wayland-client.h>
#include <cstring>
#include <vector>
#include "wlr-screencopy-unstable-v1.hh"
#include "cosmic-screencopy-unstable-v2.hh"
#include "logger.hh"

struct wl_output_data {
    // wayland output object
    wl_output *output = nullptr;

    // output data
    int32_t x;
    int32_t y;
    int32_t width;
    int32_t height;
    int32_t transform;
    int32_t scale_factor;

    // for debugging
    const char *name;
    const char *model;
};

struct wl_state {
    // base wayland connection objects
    wl_display *display   = nullptr;
    wl_registry *registry = nullptr;

    // wayland screencopy objects
    zwlr_screencopy_manager_v1 *wlr_screencopy_manager       = nullptr;
    zcosmic_screencopy_manager_v2 *cosmic_screencopy_manager = nullptr;
    bool wlr_found = false;
    bool cosmic_found = false;
    std::vector<wl_output_data> outputs;
};

bool wl_connect(wl_state &state, const char *name);

bool wl_connect_to_fd(wl_state &state, int fd);

void wl_disconnect(wl_state &state);

#endif