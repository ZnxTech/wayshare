#include "wayland.h"

// +----------------------------+
// | wayland output event calls |
// +----------------------------+

static void wl_output_event_geometry(void *data, struct wl_output *wl_output,
        int32_t x, int32_t y, int32_t width_mm, int32_t height_mm,
        int32_t subpixel, const char *make, const char *model, int32_t transform) {
    wl_output_data_t *output_data = (wl_output_data_t*)data;
    output_data->x = x;
    output_data->y = y;
    output_data->transform = transform;
}

static void wl_output_event_mode(void *data, struct wl_output *wl_output,
        uint32_t flags, int32_t width, int32_t height, int32_t refresh) {
    wl_output_data_t *output_data = (wl_output_data_t*)data;
    output_data->width = width;
    output_data->height = height;
}

static void wl_output_event_done(void *data, struct wl_output *wl_output) {
}

static void wl_output_event_scale(void *data, struct wl_output *wl_output, int32_t factor) {
    wl_output_data_t *output_data = (wl_output_data_t*)data;
    output_data->scale_factor = factor;
}

static void wl_output_event_name(void *data, struct wl_output *wl_output, const char *name) {
}

static void wl_output_event_description(void *data, struct wl_output *wl_output, const char *description) {
}

static const struct wl_output_listener output_listener = {
    .geometry    = wl_output_event_geometry,
    .mode        = wl_output_event_mode,
    .done        = wl_output_event_done,
    .scale       = wl_output_event_scale,
    .name        = wl_output_event_name,
    .description = wl_output_event_description
};

// +------------------------+
// | xdg output event calls |
// +------------------------+

static void xdg_output_event_logical_position(void *data, struct zxdg_output_v1 *zxdg_output_v1, int32_t x, int32_t y) {
    wl_output_data_t *output_data_t = (wl_output_data_t*)data;
    output_data_t->x = x;
    output_data_t->y = y;
}

static void xdg_output_event_logical_size(void *data, struct zxdg_output_v1 *zxdg_output_v1, int32_t width, int32_t height) {
    wl_output_data_t *output_data_t = (wl_output_data_t*)data;
    output_data_t->width = width;
    output_data_t->height = height;
}

static void xdg_output_event_done(void *data, struct zxdg_output_v1 *zxdg_output_v1) {
}

static void xdg_output_event_name(void *data, struct zxdg_output_v1 *zxdg_output_v1, const char *name) {
}

static void xdg_output_event_description(void *data, struct zxdg_output_v1 *zxdg_output_v1, const char *description) {
}

static const struct zxdg_output_v1_listener xdg_output_listener = {
    .logical_position = xdg_output_event_logical_position,
    .logical_size     = xdg_output_event_logical_size,
    .done             = xdg_output_event_done,
    .name             = xdg_output_event_name,
    .description      = xdg_output_event_description
};

// +---------------------------+
// | wlroots frame event calls |
// +---------------------------+

static void wlr_frame_event_buffer(void *data, struct zwlr_screencopy_frame_v1 *zwlr_screencopy_frame_v1,
        uint32_t format, uint32_t width, uint32_t height, uint32_t stride) {
    wl_buffer_data_t *buffer_data = (wl_buffer_data_t*)data;
    buffer_data->width = width;
    buffer_data->height = height;
    buffer_data->format = format;
    buffer_data->stride = stride;
    buffer_data->size = height * stride;
}

static void wlr_frame_event_flags(void *data, struct zwlr_screencopy_frame_v1 *zwlr_screencopy_frame_v1, uint32_t flags) {

}

static void wlr_frame_event_ready(void *data, struct zwlr_screencopy_frame_v1 *zwlr_screencopy_frame_v1,
        uint32_t tv_sec_hi, uint32_t tv_sec_lo, uint32_t tv_nsec) {
    wl_buffer_data_t *buffer_data = (wl_buffer_data_t*)data;
    (*buffer_data->n_ready)++;
}

static void wlr_frame_event_failed(void *data, struct zwlr_screencopy_frame_v1 *zwlr_screencopy_frame_v1) {
}

static void wlr_frame_event_damage(void *data, struct zwlr_screencopy_frame_v1 *zwlr_screencopy_frame_v1,
        uint32_t x, uint32_t y, uint32_t width, uint32_t height) {
}

static void wlr_frame_event_linux_dmabuf(void *data, struct zwlr_screencopy_frame_v1 *zwlr_screencopy_frame_v1,
        uint32_t format, uint32_t width, uint32_t height) {
}

static void wlr_frame_event_buffer_done(void *data, struct zwlr_screencopy_frame_v1 *zwlr_screencopy_frame_v1) {
}

static const struct zwlr_screencopy_frame_v1_listener wlr_frame_listener = {
    .buffer       = wlr_frame_event_buffer,
    .flags        = wlr_frame_event_flags,
    .ready        = wlr_frame_event_ready,
    .failed       = wlr_frame_event_failed,
    .damage       = wlr_frame_event_damage,
    .linux_dmabuf = wlr_frame_event_linux_dmabuf,
    .buffer_done  = wlr_frame_event_buffer_done
};

// +------------------------------+
// | wayland registry event calls |
// +------------------------------+

static void wl_registry_event_global(void *data, struct wl_registry *registry,
        uint32_t name, const char *interface, uint32_t version) {

    if (strcmp(interface, zwlr_screencopy_manager_v1_interface.name) == 0) {
        WS_LOGF(WS_SEV_INFO, "wlr_screencopy_manager found.\n");
        wl_state_t *state = (wl_state_t*)data;
        state->wlr_screencopy_manager = (struct zwlr_screencopy_manager_v1*)wl_registry_bind(
            state->registry, name, &zwlr_screencopy_manager_v1_interface, version);
    }

    if (strcmp(interface, zcosmic_screencopy_manager_v2_interface.name) == 0) {
        WS_LOGF(WS_SEV_INFO, "cosmic_screencopy_manager found.\n");
        wl_state_t *state = (wl_state_t*)data;
        state->cosmic_screencopy_manager = (struct zcosmic_screencopy_manager_v2*)wl_registry_bind(
            state->registry, name, &zcosmic_screencopy_manager_v2_interface, version);
    }

    if (strcmp(interface, wl_output_interface.name) == 0) {
        WS_LOGF(WS_SEV_INFO, "wl_output found.\n");
        wl_state_t *state = (wl_state_t*)data;
        wl_output_data_t output_data = { 0 };
        output_data.output = (struct wl_output*)wl_registry_bind(state->registry, name, &wl_output_interface, version);
        wl_output_add_listener(output_data.output, &output_listener, &output_data);
        wl_display_roundtrip(state->display); // handle wl_output events
        darray_append(state->outputs, &output_data);
    }

    if (strcmp(interface, wl_shm_interface.name) == 0) {
        WS_LOGF(WS_SEV_INFO, "wl_shm found.\n");
        wl_state_t *state = (wl_state_t*)data;
        state->shm = (struct wl_shm*)wl_registry_bind(state->registry, name, &wl_shm_interface, version);
        wl_display_roundtrip(state->display);
    }

    if (strcmp(interface, zxdg_output_manager_v1_interface.name) == 0) {
        WS_LOGF(WS_SEV_INFO, "xdg_output_manager found.\n");
        wl_state_t *state = (wl_state_t*)data;
        state->output_manager = (struct zxdg_output_manager_v1*)wl_registry_bind(
            state->registry, name, &zxdg_output_manager_v1_interface, version);
    }
}

static void wl_registry_event_global_remove(void *data, struct wl_registry *registry, uint32_t name) {
}

static const struct wl_registry_listener registry_listener = {
    .global        = wl_registry_event_global,
    .global_remove = wl_registry_event_global_remove
};

// +-----------------------------+
// | wayland managment functions |
// +-----------------------------+

ecode_t wl_state_connect(wl_state_t *r_state, const char *name) {
    r_state->outputs = darray_init(sizeof(wl_output_data_t), 1);

    r_state->display = wl_display_connect(name);
    if (r_state->display == NULL) {
        WS_LOGF(WS_SEV_ERR, "wayland display not found.\n");
        return WSE_WL_NDISPLAY;
    }

    r_state->registry = wl_display_get_registry(r_state->display);
    if (r_state->registry == NULL) {
        WS_LOGF(WS_SEV_ERR, "wayland registry not found.\n");
        return WSE_WL_NREGISTRY;
    }

    wl_registry_add_listener(r_state->registry, &registry_listener, r_state);
    wl_display_roundtrip(r_state->display); // registry events

    if (r_state->outputs->count == 0) {
        WS_LOGF(WS_SEV_ERR, "no wl_outputs were found.\n");
        return WSE_WL_NOUTPUT;
    }

    // check for xdg output manager
    if (!r_state->output_manager) {
        WS_LOGF(WS_SEV_WARN, "no xdg_output_manager was found.\n");
        return WSE_WL_NXDG_OUTPUT;
    }

    // get xdg output data
    darray_foreach(r_state->outputs, i_output, output_data_ptr) {
        wl_output_data_t *output_data = (wl_output_data_t*)output_data_ptr;
        struct zxdg_output_v1 *xdg_output =
            zxdg_output_manager_v1_get_xdg_output(r_state->output_manager, output_data->output);
        zxdg_output_v1_add_listener(xdg_output, &xdg_output_listener, output_data);
        wl_display_roundtrip(r_state->display);
    }

    return WS_OK;
}

ecode_t wl_state_disconnect(wl_state_t state) {
    if (state.registry)
        wl_registry_destroy(state.registry);
    if (state.display)
        wl_display_disconnect(state.display);
    return WS_OK;
}

ecode_t wl_buffer_create(wl_state_t state, wl_buffer_data_t *buffer_data) {
    WS_LOGF(WS_SEV_WARN, "fd size: %u\n", buffer_data->size);

    int fd;
    ecode_t code = create_shm_file(&fd, buffer_data->size);
    if (!code) {
        WS_LOGF(WS_SEV_WARN, "shm file failed.\n");
        return WSE_WL_FDF;
    }

    void *data = mmap(NULL, buffer_data->size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (data == MAP_FAILED) {
        WS_LOGF(WS_SEV_WARN, "shm mmap failed. %i\n", fd);
        close(fd);
        return WSE_WL_MMAPF;
    }

    struct wl_shm_pool *shm_pool = wl_shm_create_pool(state.shm, fd, buffer_data->size);
    struct wl_buffer *buffer = wl_shm_pool_create_buffer(shm_pool, 0, 
        buffer_data->width, buffer_data->height,
        buffer_data->stride , buffer_data->format);
    wl_shm_pool_destroy(shm_pool);
    buffer_data->data   = data;
    buffer_data->buffer = buffer;
    close(fd);
    return WS_OK;
}

ecode_t wl_buffer_delete(wl_buffer_data_t buffer_data) {
    if (buffer_data.frame)
        zwlr_screencopy_frame_v1_destroy(buffer_data.frame);
    if (buffer_data.buffer)
        wl_buffer_destroy(buffer_data.buffer);
    if (buffer_data.data)
        munmap(buffer_data.data, buffer_data.size);
    return WS_OK;
}

ecode_t image_wlr_screencopy(image_t *r_image, wl_state_t state, rect_t area, int32_t cursor) {
    WS_LOGF(WS_SEV_INFO, "requesting wlr screencopy image.\n");
    int n_requested = 0;
    int n_ready = 0;
    darray_t *buffers = darray_init(sizeof(wl_buffer_data_t), 1);
    darray_foreach(state.outputs, i_output, output_data_ptr) {
        wl_output_data_t *output_data = (wl_output_data_t*)output_data_ptr;
        wl_buffer_data_t buffer_data;
        buffer_data.n_ready = &n_ready;
        buffer_data.transform = output_data->transform;
        buffer_data.area = rect_inter(area, output_data->area);

        if (!rect_is_valid(buffer_data.area))
            continue;

        // create frame
        buffer_data.frame = zwlr_screencopy_manager_v1_capture_output_region(state.wlr_screencopy_manager,
            cursor, output_data->output,
            buffer_data.x - output_data->x, buffer_data.y - output_data->y,
            buffer_data.width, buffer_data.height);
        zwlr_screencopy_frame_v1_add_listener(buffer_data.frame, &wlr_frame_listener, &buffer_data);
        wl_display_dispatch(state.display);

        // allocate shm buffer
        const ecode_t code = wl_buffer_create(state, &buffer_data);
        if (!code)
            return WSE_WL_BUFFERF;

        // copy frame
        zwlr_screencopy_frame_v1_copy(buffer_data.frame, buffer_data.buffer);
        wl_display_dispatch(state.display);
        darray_append(buffers, &buffer_data);
        n_requested++;
    }

    WS_LOGF(WS_SEV_INFO, "requested %i screencopy slices.\n", n_requested);
    int n_dispatched = 0;
    while (n_ready != n_requested && n_dispatched != -1) {
        n_dispatched = wl_display_roundtrip(state.display);
    }

    image_t image_main = { };
    image_create_empty(&image_main, area);

    darray_foreach(buffers, i_buffer, buffer_data_ptr) {
        wl_buffer_data_t *buffer_data = (wl_buffer_data_t*)buffer_data_ptr;
        image_t image_part = { };
        format_t image_format = { };

        ecode_t code = format_from_wl_format(&image_format, buffer_data->format);
        if (!code)
            continue;

        image_create_from_buffer(&image_part, buffer_data->area, (uint8_t*)buffer_data->data, image_format);
        image_rotate(&image_part, buffer_data->transform & 0x03);
        if (buffer_data->transform & 0x04)
            image_vaxis_flip(image_part);

        image_layer_overlay(image_main, image_part);
        image_delete(image_part);
        wl_buffer_delete(*buffer_data);
    }

    WS_LOGF(WS_SEV_INFO, "screencopy image complete.\n");
    *r_image = image_main;
    return WS_OK;
}