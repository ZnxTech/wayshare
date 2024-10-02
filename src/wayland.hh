#ifndef WAYLAND_H
#define WAYLAND_H

#include <wayland-client.h>
#include <cstring>
#include "wlr-screencopy-unstable-v1.hh"
#include "logger.hh"

struct wl_state {
    // base wayland connection objects
    wl_display* display   = nullptr;
    wl_registry* registry = nullptr;

    // wayland screencopy objects
    zwlr_screencopy_manager_v1* screencopy_manager = nullptr;
    int output_count = 0; 
    wl_output** outputs = nullptr;
};

bool wl_connect(wl_state& state, const char* name);

bool wl_connect_to_fd(wl_state& state, int fd);

bool wl_register_globals(wl_state& state);

void wl_disconnect(wl_state& state);

#endif