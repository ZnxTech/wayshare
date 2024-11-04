#include "wayland.hh"

static void wl_output_event_geometry(void *data, wl_output *wl_output,
        int32_t x, int32_t y, int32_t width_mm, int32_t height_mm,
        int32_t subpixel, const char *make, const char *model, int32_t transform) {
    logf(0, "wl_output geometry event: make:\"%s\", model:\"%s\"\n", make, model);
    wl_output_data *output_data = (wl_output_data*)data;
    output_data->area.x = x;
    output_data->area.y = y;
    output_data->transform = transform;
    output_data->model = model;
}

static void wl_output_event_mode(void *data, wl_output *wl_output,
        uint32_t flags, int32_t width, int32_t height, int32_t refresh) {
    logf(0, "wl_output mode event: w:%ipx, h:%ipx, rr:%ihz\n", width, height, refresh / 1000 );
    wl_output_data *output_data = (wl_output_data*)data;
    output_data->area.width = width;
    output_data->area.height = height;
}

static void wl_output_event_done(void *data, wl_output *wl_output) {
    logf(0, "wl_output done event:\n");
}

static void wl_output_event_scale(void *data, wl_output *wl_output, int32_t factor) {
    logf(0, "wl_output scale event: factor:%i\n", factor);
    wl_output_data *output_data = (wl_output_data*)data;
    output_data->scale_factor = factor;
}

static void wl_output_event_name(void *data, wl_output *wl_output, const char *name) {
    logf(0, "wl_output name event: name:%s\n", name);
    wl_output_data *output_data = (wl_output_data*)data;
    output_data->name = name;
}

static void wl_output_event_description(void *data, wl_output *wl_output, const char *description) {
    logf(0, "wl_output description event: description:%s\n", description);
}

static const wl_output_listener output_listener {
    .geometry    = wl_output_event_geometry,
    .mode        = wl_output_event_mode,
    .done        = wl_output_event_done,
    .scale       = wl_output_event_scale,
    .name        = wl_output_event_name,
    .description = wl_output_event_description
};

static void wl_registry_event_global(void *data, wl_registry *registry,
        uint32_t name, const char *interface, uint32_t version) {
    logf(0, "wl_registry global event: name:%i, v:%i, interface:%s\n", name, version, interface);

    if (strcmp(interface, zwlr_screencopy_manager_v1_interface.name) == 0) {
        logf(0, "wlroots screencopy found.\n");
        wl_state *state = (wl_state*)data;
        state->wlr_screencopy_manager = (zwlr_screencopy_manager_v1*)wl_registry_bind(
            state->registry, name, &zwlr_screencopy_manager_v1_interface, version);
    }

    if (strcmp(interface, zcosmic_screencopy_manager_v2_interface.name) == 0) {
        logf(0, "cosmic screencopy found.\n");
        wl_state *state = (wl_state*)data;
        state->cosmic_screencopy_manager = (zcosmic_screencopy_manager_v2*)wl_registry_bind(
            state->registry, name, &zcosmic_screencopy_manager_v2_interface, version);
    }

    if (strcmp(interface, wl_output_interface.name) == 0) {
        logf(0, "wayland output found.\n");
        wl_state *state = (wl_state*)data;
        wl_output *output = (wl_output*)wl_registry_bind(state->registry, name, &wl_output_interface, version);
        wl_output_data *output_data = new wl_output_data;
        output_data->output = output;
        wl_output_add_listener(output, &output_listener, output_data);
        state->outputs.push_back(output_data);
    }

    if (strcmp(interface, wl_shm_interface.name) == 0) {
        logf(0, "wayland shm found.\n");
        wl_state *state = (wl_state*)data;
        state->shm = (wl_shm*)wl_registry_bind(state->registry, name, &wl_shm_interface, version);
    }

    if (strcmp(interface, zxdg_output_manager_v1_interface.name) == 0) {
        logf(0, "xdg output_manager found.\n");
        wl_state *state = (wl_state*)data;
        state->output_manager = (zxdg_output_manager_v1*)wl_registry_bind(
            state->registry, name, &zxdg_output_manager_v1_interface, version);
    }
}

static void wl_registry_event_global_remove(void *data, wl_registry *registry, uint32_t name) {
    logf(0, "wl_registry global_remove event: name:%i\n", name);
}

static const wl_registry_listener registry_listener {
    .global        = wl_registry_event_global,
    .global_remove = wl_registry_event_global_remove
};

wl_state *wl_connect(const char *name) {
    wl_state *state = new wl_state;
    state->display = wl_display_connect(name);
    if (state->display == NULL) {
        logf(2, "wayland display not found.\n");
        return NULL;
    }
    logf(0, "wayland display found.\n");

    state->registry = wl_display_get_registry(state->display);
    if (state->registry == NULL) {
        logf(2, "wayland registry not found.\n");
        return NULL;
    }
    logf(0, "wayland registry found.\n");

    wl_registry_add_listener(state->registry, &registry_listener, state);
    // registry events
    wl_display_roundtrip(state->display);
    // output events
    wl_display_roundtrip(state->display);

    // check for xdg output manager
    if (state->output_manager == nullptr) {
        logf(1, "no xdg_output_manager was found.\n");
        return state;
    }

    // get xdg output data
    for (wl_output_data *output_data : state->outputs) {
        zxdg_output_v1 *xdg_output = zxdg_output_manager_v1_get_xdg_output(state->output_manager, output_data->output);
        zxdg_output_v1_add_listener(xdg_output, &xdg_output_listener, output_data);
        wl_display_roundtrip(state->display);
    }
    return state;
}

void wl_disconnect(wl_state *state) {
    wl_display_disconnect(state->display);
    delete state;
}

static void xdg_output_event_logical_position(void *data, zxdg_output_v1 *zxdg_output_v1, int32_t x, int32_t y) {
    logf(0, "xdg_output logical_position event: x:%i y:%i\n", x, y);
    wl_output_data *output_data = (wl_output_data*)data;
    output_data->area.x = x;
    output_data->area.y = y;
}

static void xdg_output_event_logical_size(void *data, zxdg_output_v1 *zxdg_output_v1, int32_t width, int32_t height) {
    logf(0, "xdg_output logical_size event:\n");
    wl_output_data *output_data = (wl_output_data*)data;
    output_data->area.width = width;
    output_data->area.height = height;
}

static void xdg_output_event_done(void *data, zxdg_output_v1 *zxdg_output_v1) {
    logf(0, "xdg_output done event:\n");

}

static void xdg_output_event_name(void *data, zxdg_output_v1 *zxdg_output_v1, const char *name) {
    logf(0, "xdg_output name event:\n");

}

static void xdg_output_event_description(void *data, zxdg_output_v1 *zxdg_output_v1, const char *description) {
    logf(0, "xdg_output description event:\n");
}

static const zxdg_output_v1_listener xdg_output_listener = {
    .logical_position = xdg_output_event_logical_position,
    .logical_size     = xdg_output_event_logical_size,
    .done             = xdg_output_event_done,
    .name             = xdg_output_event_name,
    .description      = xdg_output_event_description
};

static void wl_request_wlr_screencopy(wl_state *state, wl_output_data *output_data, int32_t cursor) {

}

static void wl_request_cosmic_screencopy(wl_state &state) {

}