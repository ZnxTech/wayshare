#include "wayland.hh"

bool wl_connect(wl_state &state, const char *name) {
    state.display = wl_display_connect(name);
    return state.display != NULL;
}

bool wl_connect_to_fd(wl_state &state, int fd) {
    state.display = wl_display_connect_to_fd(fd);
    return state.display != NULL;
}

static void wl_registry_event_global(void *data, wl_registry *registry, uint32_t name, const char *interface, uint32_t version) {
    logf(0, "global event: name.%i v.%i, %s\n", name, version, interface);

    if (strcmp(interface, zwlr_screencopy_manager_v1_interface.name) == 0) {
        logf(0, "wlroots screencopy found.\n");
        wl_state state = *(wl_state*)data;
        state.wlr_screencopy_manager = (zwlr_screencopy_manager_v1*)wl_registry_bind(state.registry, name, &zwlr_screencopy_manager_v1_interface, version);
    }

    if (strcmp(interface, zcosmic_screencopy_manager_v2_interface.name) == 0) {
        logf(0, "cosmic screencopy found.\n");
        wl_state state = *(wl_state*)data;
        state.cosmic_screencopy_manager = (zcosmic_screencopy_manager_v2*)wl_registry_bind(state.registry, name, &zcosmic_screencopy_manager_v2_interface, version);
    }

    if (strcmp(interface, wl_output_interface.name) == 0) {
        logf(0, "wayland output found.\n");
        wl_state state = *(wl_state*)data;
        wl_output *output = (wl_output*)wl_registry_bind(state.registry, name, &wl_output_interface, version);
        state.outputs.push_back(output);
    }
}

static void wl_registry_event_global_remove(void *data, wl_registry *registry, uint32_t name) {
    logf(0, "global_remove event: name.%i\n", name);
}

static const wl_registry_listener registry_listener {
    .global        = wl_registry_event_global,
    .global_remove = wl_registry_event_global_remove
};

bool wl_register_globals(wl_state &state) {
    state.registry = wl_display_get_registry(state.display);
    if (state.registry == NULL) {
        logf(2, "wayland registry not found.\n");
        return false;
    }
    wl_registry_add_listener(state.registry, &registry_listener, (void*)&state);
    int dispatch_count = wl_display_dispatch(state.display);
    return true;
}

void wl_disconnect(wl_state &state) {
    wl_display_disconnect(state.display);
}