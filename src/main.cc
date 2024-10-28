#include <wayland-client.h>
#include "logger.hh"
#include "wayland.hh"
#include "shm.hh"

int main(int argc, char *argv[]) {
    // connection init
    struct wl_state *state = wl_connect(NULL);
    if (state == NULL) {
        return -1;
    }

    // fin
    wl_disconnect(state);
    return 0;
}