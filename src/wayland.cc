#include "wayland.hh"

static void wl_output_event_geometry(void *data, wl_output *wl_output, int32_t x, int32_t y, int32_t width_mm, int32_t height_mm, int32_t subpixel, const char *make, const char *model, int32_t transform) {
    logf(0, "wl_output geometry event: make:\"%s\", model:\"%s\"\n", make, model);
    static int output_count = 0;
    wl_state &state = *(wl_state*)data;
    state.outputs[output_count].x = x;
    state.outputs[output_count].y = y;
    state.outputs[output_count].transform = transform;
    state.outputs[output_count].model = model;
    output_count++;
}

static void wl_output_event_mode(void *data, wl_output *wl_output, uint32_t flags, int32_t width, int32_t height, int32_t refresh) {
    logf(0, "wl_output mode event: w:%ipx, h:%ipx, rr:%ihz\n", width, height, refresh / 1000 );
    static int output_count = 0;
    wl_state &state = *(wl_state*)data;
    state.outputs[output_count].width = width;
    state.outputs[output_count].height = height;
    output_count++;
}

static void wl_output_event_done(void *data, wl_output *wl_output) {
    logf(0, "wl_output done event:\n");
}

static void wl_output_event_scale(void *data, wl_output *wl_output, int32_t factor) {
    logf(0, "wl_output scale event: factor:%i\n", factor);
    static int output_count = 0;
    wl_state &state = *(wl_state*)data;
    state.outputs[output_count].scale_factor = factor;
    output_count++;
}

static void wl_output_event_name(void *data, wl_output *wl_output, const char *name) {
    logf(0, "wl_output name event: name:%s\n", name);
    static int output_count = 0;
    wl_state &state = *(wl_state*)data;
    state.outputs[output_count].name = name;
    output_count++;
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

static void wl_registry_event_global(void *data, wl_registry *registry, uint32_t name, const char *interface, uint32_t version) {
    logf(0, "wl_registry global event: name:%i, v:%i, interface:%s\n", name, version, interface);

    if (strcmp(interface, zwlr_screencopy_manager_v1_interface.name) == 0) {
        logf(0, "wlroots screencopy found.\n");
        wl_state &state = *(wl_state*)data;
        state.wlr_screencopy_manager = (zwlr_screencopy_manager_v1*)wl_registry_bind(state.registry, name, &zwlr_screencopy_manager_v1_interface, version);
        state.wlr_found = true;
    }

    if (strcmp(interface, zcosmic_screencopy_manager_v2_interface.name) == 0) {
        logf(0, "cosmic screencopy found.\n");
        wl_state &state = *(wl_state*)data;
        state.cosmic_screencopy_manager = (zcosmic_screencopy_manager_v2*)wl_registry_bind(state.registry, name, &zcosmic_screencopy_manager_v2_interface, version);
        state.cosmic_found = true;
    }

    if (strcmp(interface, wl_output_interface.name) == 0) {
        logf(0, "wayland output found.\n");
        wl_state &state = *(wl_state*)data;
        wl_output *output = (wl_output*)wl_registry_bind(state.registry, name, &wl_output_interface, version);
        wl_output_add_listener(output, &output_listener, &state);
        wl_output_data output_data = { .output = output };
        state.outputs.push_back(output_data);
    }
}

static void wl_registry_event_global_remove(void *data, wl_registry *registry, uint32_t name) {
    logf(0, "wl_registry global_remove event: name:%i\n", name);
}

static const wl_registry_listener registry_listener {
    .global        = wl_registry_event_global,
    .global_remove = wl_registry_event_global_remove
};

bool wl_connect(wl_state &state, const char *name) {
    state.display = wl_display_connect(name);
    if (state.display == NULL) {
        logf(2, "wayland display not found.\n");
        return false;
    }
    logf(0, "wayland display found.\n");

    state.registry = wl_display_get_registry(state.display);
    if (state.registry == NULL) {
        logf(2, "wayland registry not found.\n");
        return false;
    }
    logf(0, "wayland registry found.\n");

    wl_registry_add_listener(state.registry, &registry_listener, &state);
    int dispatch_count = wl_display_dispatch(state.display);
    return dispatch_count != -1;
}

bool wl_connect_to_fd(wl_state &state, int fd) {
    state.display = wl_display_connect_to_fd(fd);
        if (state.display == NULL) {
        logf(2, "wayland display not found.\n");
        return false;
    }
    logf(0, "wayland display found.\n");

    state.registry = wl_display_get_registry(state.display);
    if (state.registry == NULL) {
        logf(2, "wayland registry not found.\n");
        return false;
    }
    logf(0, "wayland registry found.\n");

    wl_registry_add_listener(state.registry, &registry_listener, &state);
    int dispatch_count = wl_display_dispatch(state.display);
    return dispatch_count != -1;
}

void wl_disconnect(wl_state &state) {
    wl_display_disconnect(state.display);
}