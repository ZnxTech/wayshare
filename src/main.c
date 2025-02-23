#include "logger.h"
#include "wayland.h"

int main(int argc, char **argv)
{
	// connection init
	struct wl_state *state;
	wl_state_connect(&state, NULL);

	wl_state_disconnect(state);
	return WS_OK;
}
