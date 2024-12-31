#include <wayland-client.h>
#include "logger.hh"
#include "wayland.hh"
#include "shm.hh"

int main(int argc, char *argv[]) {
    // connection init
    wl_state_t state = { 0 };
    wl_state_connect(&state, NULL);

    wl_state_disconnect(state);
    return WS_OK;
}