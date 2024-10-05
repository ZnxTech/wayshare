#include <wayland-client.h>
#include "logger.hh"
#include "wayland.hh"

int main(int argc, char *argv[]) {

    // connection init
    struct wl_state state = {};
    if (!wl_connect(state, NULL)) {
        return 1;
    }

    // dispatch loop
    while (wl_display_dispatch(state.display) != -1) {

    }

    // fin
    wl_disconnect(state);
    return 0;
}