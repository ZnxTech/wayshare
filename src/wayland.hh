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
    rect area;
    int32_t transform;
    int32_t scale_factor;

    // for debugging
    const char *name  = nullptr;
    const char *model = nullptr;
};

struct wl_state {
    // base wayland connection objects
    wl_display *display   = nullptr;
    wl_registry *registry = nullptr;
    wl_shm *shm = nullptr;
    zxdg_output_manager_v1 *output_manager = nullptr;

    // wayland screencopy objects
    zwlr_screencopy_manager_v1 *wlr_screencopy_manager       = nullptr;
    zcosmic_screencopy_manager_v2 *cosmic_screencopy_manager = nullptr;
    std::vector<wl_output_data*> outputs;
};

bool wl_connect(wl_state *state, const char *name);

bool wl_connect_to_fd(wl_state *state, int fd);

void wl_disconnect(wl_state *state);








#endif