#include <wayland-client.h>
#include "logger.hh"
#include "wayland.hh"

int main(int argc, char *argv[]) {

    // connection init
    struct wl_state state = {};
    if (!wl_connect(state, NULL)) {
        logf(2, "wayland display not found.\n");
        return 1;
    }
    logf(0, "wayland display found.\n");

    // registry init
    if (!wl_register_globals(state)) {
        logf(2, "screencopy protocol not found.\n");
        return 1;
    }
    logf(0, "screencopy protocol found.\n");

    while (wl_display_dispatch(state.display) != -1) {

    }

    // fin
    wl_disconnect(state);
    return 0;
}