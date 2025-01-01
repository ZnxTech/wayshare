#include "image.hh"

ws_code_t format_from_wl_format(format_t *r_format, int32_t wl_format) {

    // NOTE:
    // =====
    // since wayland formats are represented in little endian
    // their naming is mirrored compared to their wayshare equivalents.

    switch (wl_format) {
    case WL_SHM_FORMAT_ABGR16161616:        // 64 bit:  // A-16161616
        *r_format = WS_FORMAT_R16G16B16A16; // =======
        break;

    case WL_SHM_FORMAT_ARGB16161616:
        *r_format = WS_FORMAT_B16G16R16A16;
        break;

    case WL_SHM_FORMAT_XBGR16161616:                    // X-16161616
        *r_format = WS_FORMAT_R16G16B16X16;
        break;

    case WL_SHM_FORMAT_XRGB16161616:
        *r_format = WS_FORMAT_B16G16R16X16;
        break;

    case WL_SHM_FORMAT_ABGR8888:            // 32 bit:  // A-8888
        *r_format = WS_FORMAT_R8G8B8A8;     // =======
        break;

    case WL_SHM_FORMAT_ARGB8888:
        *r_format = WS_FORMAT_B8G8R8A8;
        break;

    case WL_SHM_FORMAT_BGRA8888:
        *r_format = WS_FORMAT_A8R8G8B8;
        break;

    case WL_SHM_FORMAT_RGBA8888:
        *r_format = WS_FORMAT_A8B8G8R8;
        break;

    case WL_SHM_FORMAT_XBGR8888:                        // X-8888
        *r_format = WS_FORMAT_R8G8B8X8;
        break;

    case WL_SHM_FORMAT_XRGB8888:
        *r_format = WS_FORMAT_B8G8R8X8;
        break;

    case WL_SHM_FORMAT_BGRX8888:
        *r_format = WS_FORMAT_X8R8G8B8;
        break;

    case WL_SHM_FORMAT_RGBX8888:
        *r_format = WS_FORMAT_X8B8G8R8;
        break;

    case WL_SHM_FORMAT_ABGR1555:                        // A-5551
        *r_format = WS_FORMAT_R5G5B5A1;
        break;

    case WL_SHM_FORMAT_ARGB1555:
        *r_format = WS_FORMAT_B5G5R5A1;
        break;

    case WL_SHM_FORMAT_BGRA5551:
        *r_format = WS_FORMAT_A1R5G5B5;
        break;

    case WL_SHM_FORMAT_RGBA5551:
        *r_format = WS_FORMAT_A1B5G5R5;
        break;

    case WL_SHM_FORMAT_XBGR1555:                        // X-5551
        *r_format = WS_FORMAT_R5G5B5X1;
        break;

    case WL_SHM_FORMAT_XRGB1555:
        *r_format = WS_FORMAT_B5G5R5X1;
        break;

    case WL_SHM_FORMAT_BGRX5551:
        *r_format = WS_FORMAT_X1R5G5B5;
        break;

    case WL_SHM_FORMAT_RGBX5551:
        *r_format = WS_FORMAT_X1B5G5R5;
        break;

    case WL_SHM_FORMAT_RGB888:              // 24 bit:  // _-888
        *r_format = WS_FORMAT_B8G8R8;       // =======
        break;

    case WL_SHM_FORMAT_BGR888:
        *r_format = WS_FORMAT_R8G8B8;
        break;

    case WL_SHM_FORMAT_ABGR4444:            // 16 bit:  // A-4444
        *r_format = WS_FORMAT_R4G4B4A4;     // =======
        break;

    case WL_SHM_FORMAT_ARGB4444:
        *r_format = WS_FORMAT_B4G4R4A4;
        break;

    case WL_SHM_FORMAT_BGRA4444:
        *r_format = WS_FORMAT_A4R4G4B4;
        break;

    case WL_SHM_FORMAT_RGBA4444:
        *r_format = WS_FORMAT_A4B4G4R4;
        break;

    case WL_SHM_FORMAT_XBGR4444:                        // X-4444
        *r_format = WS_FORMAT_R4G4B4X4;
        break;

    case WL_SHM_FORMAT_XRGB4444:
        *r_format = WS_FORMAT_B4G4R4X4;
        break;

    case WL_SHM_FORMAT_BGRX4444:
        *r_format = WS_FORMAT_X4R4G4B4;
        break;

    case WL_SHM_FORMAT_RGBX4444:
        *r_format = WS_FORMAT_X4B4G4R4;
        break;

    case WL_SHM_FORMAT_BGR565:                          // _-565
        *r_format = WS_FORMAT_R5G6B5;
        break;

    case WL_SHM_FORMAT_RGB565:
        *r_format = WS_FORMAT_B5G6R5;
        break;

    case WL_SHM_FORMAT_BGR233:              // 8 bit:   // _-332
        *r_format = WS_FORMAT_R3G3B2;       // ======
        break;

    case WL_SHM_FORMAT_RGB332:
        *r_format = WS_FORMAT_B2G3R3;
        break;

    default:
        WS_LOGF(WS_SEV_WARN, "wayland format %i not currently supported.\n", wl_format);
        return WSE_IMG_NSFORMAT;
    }

    return WS_OK;
}

ws_code_t image_create_empty(image_t *r_image, rect_t area) {
    image_t image = { };
    image.area = area;

    image.data = (color32_t*)calloc(image.width * image.height, sizeof(color32_t));

    *r_image = image;
    return WS_OK;
}

ws_code_t image_create_from_buffer(image_t *r_image, rect_t area, uint8_t *buffer, format_t format) {

    image_t image = { };
    image_create_empty(&image, area);

    const uint8_t format_byte_size = (format.pixel_bit_size / 8);

    const uint8_t min_subpixel = format.subpixel_offset;
    const uint8_t max_subpixel = min_subpixel + format.subpixel_count;

    uint8_t total_subpixel_offset = 0;
    for (uint8_t i_subpixel = 0; i_subpixel < format.subpixel_offset; i_subpixel++)
        total_subpixel_offset += format.color_bit_depths[i_subpixel];

    for (uint64_t i_pixel = 0; i_pixel < image.width * image.height; i_pixel++) {
        color32_t *cur_image_pixel = image.data + i_pixel;
        u_int8_t *cur_buffer_pixel = buffer + i_pixel * format_byte_size;

        // copy pixel buffer to value buffer.
        uint64_t cur_buffer_pixel_value = 0;
        memcpy(&cur_buffer_pixel_value, cur_buffer_pixel, format_byte_size);

        // skip offset subpixels.
        cur_buffer_pixel_value >>= total_subpixel_offset;

        for (uint8_t i_subpixel = min_subpixel; i_subpixel < max_subpixel; i_subpixel++) {
            uint8_t cur_subpixel_on_image_pos = WS_GET_COLOR_ORDER(format.color_order, i_subpixel);

            // calculate the amount of overflowing bit above 8, a byte.
            uint8_t cur_bit_depth_overflow =
                format.color_bit_depths[i_subpixel] > 8 ? format.color_bit_depths[i_subpixel] - 8 : 0;

            // new bit depth size for this color, with a max of 8.
            uint8_t cur_bit_depth_new = format.color_bit_depths[i_subpixel] - cur_bit_depth_overflow;

            // biggest number representable with {cur_bit_depth_new} bits.
            uint8_t cur_bit_depth_max = (0xff >> (8 - cur_bit_depth_new));

            // mask out the current color value.
            uint16_t cur_subpixel_color =
                (cur_buffer_pixel_value >> cur_bit_depth_overflow) & cur_bit_depth_max;

            // normalize/scale the color from its bit size to the standard 8 bit.
            cur_subpixel_color = (cur_subpixel_color * UINT8_MAX) / cur_bit_depth_max;

            // cur_subpixel_color =
            //     cur_subpixel_color * (UINT8_MAX / cur_bit_depth_max)
            //   +(cur_subpixel_color * (UINT8_MAX % cur_bit_depth_max)) / cur_bit_depth_max;

            // set final color to image.
            cur_image_pixel->subpixel[cur_subpixel_on_image_pos] = (uint8_t)cur_subpixel_color;

            // shift to next subpixel.
            cur_buffer_pixel_value >>= format.color_bit_depths[i_subpixel];
        }

        // make pixel opaque if buffer has no alpha values.
        if (!(format.color_type & WS_COLOR_TYPE_MASK_ALPHA))
            cur_image_pixel->alpha = UINT8_MAX;
    }

    *r_image = image;
    return WS_OK;
}

// middleman function for free(), used for destructor like purposes if needed.
ws_code_t image_delete(image_t image) {
    free(image.data);
    return WS_OK;
}

ws_code_t image_clone(image_t *r_image, image_t src_image) {
    image_t new_image = { };

    image_create_empty(&new_image, src_image.area);
    memcpy(new_image.data, src_image.data, src_image.width * src_image.height * sizeof(color32_t));

    *r_image = new_image;
    return WS_OK;
}

ws_code_t buffer_create_from_image(uint8_t *r_buffer, uint8_t color_type, image_t image) {

    const uint8_t buffer_subpixel_count = color_type + 1;
    uint8_t *buffer = (uint8_t*)calloc(buffer_subpixel_count * image.width * image.height, sizeof(uint8_t));

    for (uint64_t i_pixel = 0; i_pixel < image.width * image.height; i_pixel++) {
        color32_t *cur_image_pixel = image.data + i_pixel;
        uint8_t *cur_buffer_pixel  = buffer     + i_pixel * buffer_subpixel_count;

        // {image_pixel_delta} [0,2] delta offset in-case the buffer type is grayscale, if so skip 2 subpixels and-
        // set alpha to the alpha subpixel and blue to the gray subpixel which ill be overwriten after.
        uint8_t image_pixel_delta = !(color_type & WS_COLOR_TYPE_MASK_COLOR) * 2;
        memcpy(cur_buffer_pixel, (uint8_t*)cur_image_pixel + image_pixel_delta, buffer_subpixel_count);
        // grayscale by taking the avg of the 3 colors.
        if (!(color_type & WS_COLOR_TYPE_MASK_COLOR))
            *cur_buffer_pixel =
                cur_image_pixel->red / 3 + cur_image_pixel->green / 3 + cur_image_pixel->blue / 3
              +(cur_image_pixel->red % 3 + cur_image_pixel->green % 3 + cur_image_pixel->blue % 3) / 3;
    }

    r_buffer = buffer;
    return WS_OK;
}

ws_code_t image_layer_overwrite(image_t dest_image, image_t src_image) {

    const rect_t intr = rect_intersect(dest_image.area, src_image.area);
    if (!rect_is_valid(intr))
        return WS_OK; // images dont intersect, nothing to do.

    const int64_t dest_0 = (intr.y - dest_image.y) * dest_image.width + intr.x - dest_image.x;
    const int64_t src_0  = (intr.y - src_image.y)  * src_image.width  + intr.x - src_image.x;

    for (uint32_t dy = 0; dy < intr.height; dy++) {
        // "x"_color refers to the color pointer at the start of each horizontal line in the intr
        // from the perspective of the "x"'s image.
        color32_t *dest_color = dest_image.data + dest_0 + dy * dest_image.width;
        color32_t *src_color  = src_image.data  + src_0  + dy * src_image.width;
        memcpy(dest_color, src_color, intr.width * sizeof(color32_t));
    }

    return WS_OK;
}

ws_code_t image_layer_overlay(image_t dest_image, image_t src_image) {
    const rect_t intr = rect_intersect(dest_image.area, src_image.area);
    if (!rect_is_valid(intr))
        return WS_OK; // images dont intersect, nothing to do.

    const int64_t dest_0 = (intr.y - dest_image.y) * dest_image.width + intr.x - dest_image.x;
    const int64_t src_0  = (intr.y - src_image.y)  * src_image.width  + intr.x - src_image.x;

    for (uint32_t dy = 0; dy < intr.height; dy++) {
        for (uint32_t dx = 0; dx < intr.width; dx++) {
            // "x"_color refers to the color32 pointer at the point (dx,dy) in the intr
            // from the perspective of the "x"'s image.
            color32_t *dest_color = dest_image.data + dest_0 + dx + dy * dest_image.width;
            color32_t *src_color  = src_image.data  + src_0  + dx + dy * src_image.width;

            // alpha compositing:
            // 0 <- A over B, where a - alpha [0,1] & C - color [0,1]
            // a_0 = a_A + a_B*(1-a_A)
            // C_0 = (C_A*a_A + C_B*a_B*(1-a_A)) / a_0

            const uint8_t dest_alpha_part =
                (uint16_t)dest_color->alpha * (UINT8_MAX - src_color->alpha) / UINT8_MAX;

            const uint16_t new_alpha = src_color->alpha + dest_alpha_part;

            dest_color->red =
                ((uint16_t)src_color->red  * src_color->alpha
               + (uint16_t)dest_color->red * dest_alpha_part)
               / new_alpha;

            dest_color->green =
                ((uint16_t)src_color->green  * src_color->alpha
               + (uint16_t)dest_color->green * dest_alpha_part)
               / new_alpha;

            dest_color->blue =
                ((uint16_t)src_color->blue  * src_color->alpha
               + (uint16_t)dest_color->blue * dest_alpha_part)
               / new_alpha;

            dest_color->alpha = new_alpha;
        }
    }

    return WS_OK;
}

ws_code_t image_vaxis_flip(image_t image) {
    color32_t temp_pixel;

    for (uint32_t x = 0; x < image.width / 2; x++) {
        for (uint32_t y = 0; y < image.height; y++) {
            temp_pixel = image.data[x + y * image.width];
            image.data[x + y * image.width] = image.data[image.width - x - 1 + y * image.width];
            image.data[image.width - x - 1 + y * image.width] = temp_pixel;
        }
    }

    return WS_OK;
}

ws_code_t image_haxis_flip(image_t image) {
    color32_t temp_pixel;

    for (uint32_t x = 0; x < image.width; x++) {
        for (uint32_t y = 0; y < image.height / 2; y++) {
            temp_pixel = image.data[x + y * image.width];
            image.data[x + y * image.width] = image.data[x + (image.height - y - 1) * image.width];
            image.data[x + (image.height - y - 1) * image.width] = temp_pixel;
        }
    }

    return WS_OK;
}

// O(n^2) memory algorithms, might improve them later.

ws_code_t image_transpose(image_t *image) {
    color32_t *trans_buffer = (color32_t*)calloc(image->width * image->height, sizeof(color32_t));

    for (uint32_t x = 0; x < image->width; x++) {
        for (uint32_t y = 0; y < image->height; y++) {
            trans_buffer[y + x * image->height] = image->data[x + y * image->width];
        }
    }

    free(image->data);
    image->data = trans_buffer;
    image->area = rect_transpose(image->area);

    return WS_OK;
}

ws_code_t image_rotate(image_t *image, uint8_t rotation) {
    rotation = rotation % 4;

    switch(rotation) {
    case 1:
        image_transpose(image);
        image_vaxis_flip(*image);
        break;

    case 2:
        image_vaxis_flip(*image);
        image_haxis_flip(*image);
        break;

    case 3:
        image_transpose(image);
        image_haxis_flip(*image);
        break;

    default:
        break;
    }

    return WS_OK;
}