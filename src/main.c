#include "logger.h"
#include "wayland.h"
#include "selector.h"

int main(int argc, char **argv)
{
	// connection init
	struct wl_state *wayland;
	struct selector_state *selector;
	wl_state_connect(&wayland, NULL);
	selector_create(&selector, wayland);

	struct rect selection;
	selector_apply(&selection, selector);

	selector_free(selector);
	wl_state_disconnect(wayland);
	return 0;
}
