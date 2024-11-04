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

static void wl_output_event_geometry(void *data, wl_output *wl_output,
        int32_t x, int32_t y, int32_t width_mm, int32_t height_mm,
        int32_t subpixel, const char *make, const char *model, int32_t transform);

static void wl_output_event_mode(void *data, wl_output *wl_output,
        uint32_t flags, int32_t width, int32_t height, int32_t refresh);

static void wl_output_event_done(void *data, wl_output *wl_output);

static void wl_output_event_scale(void *data, wl_output *wl_output, int32_t factor);

static void wl_output_event_name(void *data, wl_output *wl_output, const char *name);

static void wl_output_event_description(void *data, wl_output *wl_output, const char *description);

static void wl_registry_event_global(void *data, wl_registry *registry,
        uint32_t name, const char *, uint32_t version);

static void wl_registry_event_global_remove(void *data, wl_registry *registry, uint32_t name);

#endif