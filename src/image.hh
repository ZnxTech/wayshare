#ifndef WS_IMAGE_H
#define WS_IMAGE_H

#include <stdlib.h>
#include <cstdint>
#include <cstring>
#include <vector>
#include <wayland-client.h>

#include "wayshare.hh"
#include "rect.hh"
#include "logger.hh"

// +---------------+
// | pixel formats |
// +---------------+

struct format_t {
    uint8_t pixel_bit_size;
    uint8_t color_type;
    uint8_t color_order;
    uint8_t color_bit_depths[4];
    uint8_t subpixel_offset;
    uint8_t subpixel_count;
};

// color types:
// ============
#define WS_COLOR_TYPE_MASK_ALPHA 0x01
#define WS_COLOR_TYPE_MASK_COLOR 0x02

#define WS_COLOR_TYPE_GRAYSCALE       ( 0x00 )
#define WS_COLOR_TYPE_GRAYSCALE_ALPHA ( 0x00 | WS_COLOR_TYPE_MASK_ALPHA )
#define WS_COLOR_TYPE_COLOR           ( 0x00 | WS_COLOR_TYPE_MASK_COLOR )
#define WS_COLOR_TYPE_COLOR_ALPHA     ( 0x00 | WS_COLOR_TYPE_MASK_COLOR | WS_COLOR_TYPE_MASK_ALPHA )

// color orders:
// =============
#define WS_CREATE_COLOR_ORDER(red, green, blue, alpha) \
    ( 0x00 << (2 * red) | 0x01 << (2 * green) | 0x02 << (2 * blue) | 0x03 << (2 * alpha) )

#define WS_GET_COLOR_ORDER(order, n) \
    ( (order >> (2 * n)) & 0x03 )

#define WS_COLOR_ORDER_RGBA WS_CREATE_COLOR_ORDER(0, 1, 2, 3)
#define WS_COLOR_ORDER_BGRA WS_CREATE_COLOR_ORDER(2, 1, 0, 3)
#define WS_COLOR_ORDER_ARGB WS_CREATE_COLOR_ORDER(3, 0, 1, 2)
#define WS_COLOR_ORDER_ABGR WS_CREATE_COLOR_ORDER(3, 2, 1, 0)

// known/frequent formats:
// =======================

// 64 bit:
// =======
#define WS_FORMAT_R16G16B16A16 { 64, WS_COLOR_TYPE_COLOR_ALPHA, WS_COLOR_ORDER_RGBA, { 16, 16, 16, 16 }, 0, 4 }
#define WS_FORMAT_B16G16R16A16 { 64, WS_COLOR_TYPE_COLOR_ALPHA, WS_COLOR_ORDER_BGRA, { 16, 16, 16, 16 }, 0, 4 }

#define WS_FORMAT_R16G16B16X16 { 64, WS_COLOR_TYPE_COLOR,       WS_COLOR_ORDER_RGBA, { 16, 16, 16, 16 }, 0, 4 }
#define WS_FORMAT_B16G16R16X16 { 64, WS_COLOR_TYPE_COLOR,       WS_COLOR_ORDER_BGRA, { 16, 16, 16, 16 }, 0, 4 }

// 32 bit:
// =======
#define WS_FORMAT_R8G8B8A8 { 32, WS_COLOR_TYPE_COLOR_ALPHA, WS_COLOR_ORDER_RGBA, { 8, 8, 8, 8 }, 0, 4 }
#define WS_FORMAT_B8G8R8A8 { 32, WS_COLOR_TYPE_COLOR_ALPHA, WS_COLOR_ORDER_BGRA, { 8, 8, 8, 8 }, 0, 4 }
#define WS_FORMAT_A8R8G8B8 { 32, WS_COLOR_TYPE_COLOR_ALPHA, WS_COLOR_ORDER_ARGB, { 8, 8, 8, 8 }, 0, 4 }
#define WS_FORMAT_A8B8G8R8 { 32, WS_COLOR_TYPE_COLOR_ALPHA, WS_COLOR_ORDER_ABGR, { 8, 8, 8, 8 }, 0, 4 }

#define WS_FORMAT_R8G8B8X8 { 32, WS_COLOR_TYPE_COLOR,       WS_COLOR_ORDER_RGBA, { 8, 8, 8, 8 }, 0, 3 }
#define WS_FORMAT_B8G8R8X8 { 32, WS_COLOR_TYPE_COLOR,       WS_COLOR_ORDER_BGRA, { 8, 8, 8, 8 }, 0, 3 }
#define WS_FORMAT_X8R8G8B8 { 32, WS_COLOR_TYPE_COLOR,       WS_COLOR_ORDER_ARGB, { 8, 8, 8, 8 }, 1, 3 }
#define WS_FORMAT_X8B8G8R8 { 32, WS_COLOR_TYPE_COLOR,       WS_COLOR_ORDER_ABGR, { 8, 8, 8, 8 }, 1, 3 }

#define WS_FORMAT_R10G10B10A2 { 32, WS_COLOR_TYPE_COLOR_ALPHA, WS_COLOR_ORDER_RGBA, { 10, 10, 10, 2 }, 0, 4 }
#define WS_FORMAT_B10G10R10A2 { 32, WS_COLOR_TYPE_COLOR_ALPHA, WS_COLOR_ORDER_BGRA, { 10, 10, 10, 2 }, 0, 4 }
#define WS_FORMAT_A2R10G10B10 { 32, WS_COLOR_TYPE_COLOR_ALPHA, WS_COLOR_ORDER_ARGB, { 2, 10, 10, 10 }, 0, 4 }
#define WS_FORMAT_A2B10G10R10 { 32, WS_COLOR_TYPE_COLOR_ALPHA, WS_COLOR_ORDER_ABGR, { 2, 10, 10, 10 }, 0, 4 }

#define WS_FORMAT_R10G10B10X2 { 32, WS_COLOR_TYPE_COLOR,       WS_COLOR_ORDER_RGBA, { 10, 10, 10, 2 }, 0, 3 }
#define WS_FORMAT_B10G10R10X2 { 32, WS_COLOR_TYPE_COLOR,       WS_COLOR_ORDER_BGRA, { 10, 10, 10, 2 }, 0, 3 }
#define WS_FORMAT_X2R10G10B10 { 32, WS_COLOR_TYPE_COLOR,       WS_COLOR_ORDER_ARGB, { 2, 10, 10, 10 }, 1, 3 }
#define WS_FORMAT_X2B10G10R10 { 32, WS_COLOR_TYPE_COLOR,       WS_COLOR_ORDER_ABGR, { 2, 10, 10, 10 }, 1, 3 }

// 24 bit:
// =======
#define WS_FORMAT_R8G8B8 { 24, WS_COLOR_TYPE_COLOR, WS_COLOR_ORDER_RGBA, { 8, 8, 8, 0}, 0, 3 }
#define WS_FORMAT_B8G8R8 { 24, WS_COLOR_TYPE_COLOR, WS_COLOR_ORDER_BGRA, { 8, 8, 8, 0}, 0, 3 }

// 16 bit:
// =======
#define WS_FORMAT_R4G4B4A4 { 16, WS_COLOR_TYPE_COLOR_ALPHA, WS_COLOR_ORDER_RGBA, { 4, 4, 4, 4 }, 0, 4 }
#define WS_FORMAT_B4G4R4A4 { 16, WS_COLOR_TYPE_COLOR_ALPHA, WS_COLOR_ORDER_BGRA, { 4, 4, 4, 4 }, 0, 4 }
#define WS_FORMAT_A4R4G4B4 { 16, WS_COLOR_TYPE_COLOR_ALPHA, WS_COLOR_ORDER_ARGB, { 4, 4, 4, 4 }, 0, 4 }
#define WS_FORMAT_A4B4G4R4 { 16, WS_COLOR_TYPE_COLOR_ALPHA, WS_COLOR_ORDER_ABGR, { 4, 4, 4, 4 }, 0, 4 }

#define WS_FORMAT_R4G4B4X4 { 16, WS_COLOR_TYPE_COLOR,       WS_COLOR_ORDER_RGBA, { 4, 4, 4, 4 }, 0, 3 }
#define WS_FORMAT_B4G4R4X4 { 16, WS_COLOR_TYPE_COLOR,       WS_COLOR_ORDER_BGRA, { 4, 4, 4, 4 }, 0, 3 }
#define WS_FORMAT_X4R4G4B4 { 16, WS_COLOR_TYPE_COLOR,       WS_COLOR_ORDER_ARGB, { 4, 4, 4, 4 }, 1, 3 }
#define WS_FORMAT_X4B4G4R4 { 16, WS_COLOR_TYPE_COLOR,       WS_COLOR_ORDER_ABGR, { 4, 4, 4, 4 }, 1, 3 }

#define WS_FORMAT_R5G5B5A1 { 16, WS_COLOR_TYPE_COLOR_ALPHA, WS_COLOR_ORDER_RGBA, { 5, 5, 5, 1 }, 0, 4 }
#define WS_FORMAT_B5G5R5A1 { 16, WS_COLOR_TYPE_COLOR_ALPHA, WS_COLOR_ORDER_BGRA, { 5, 5, 5, 1 }, 0, 4 }
#define WS_FORMAT_A1R5G5B5 { 16, WS_COLOR_TYPE_COLOR_ALPHA, WS_COLOR_ORDER_ARGB, { 1, 5, 5, 5 }, 0, 4 }
#define WS_FORMAT_A1B5G5R5 { 16, WS_COLOR_TYPE_COLOR_ALPHA, WS_COLOR_ORDER_ABGR, { 1, 5, 5, 5 }, 0, 4 }

#define WS_FORMAT_R5G5B5X1 { 16, WS_COLOR_TYPE_COLOR,       WS_COLOR_ORDER_RGBA, { 5, 5, 5, 1 }, 0, 3 }
#define WS_FORMAT_B5G5R5X1 { 16, WS_COLOR_TYPE_COLOR,       WS_COLOR_ORDER_BGRA, { 5, 5, 5, 1 }, 0, 3 }
#define WS_FORMAT_X1R5G5B5 { 16, WS_COLOR_TYPE_COLOR,       WS_COLOR_ORDER_ARGB, { 1, 5, 5, 5 }, 1, 3 }
#define WS_FORMAT_X1B5G5R5 { 16, WS_COLOR_TYPE_COLOR,       WS_COLOR_ORDER_ABGR, { 1, 5, 5, 5 }, 1, 3 }

#define WS_FORMAT_R5G6B5 { 16, WS_COLOR_TYPE_COLOR, WS_COLOR_ORDER_RGBA, { 5, 6, 5, 0}, 0, 3 }
#define WS_FORMAT_B5G6R5 { 16, WS_COLOR_TYPE_COLOR, WS_COLOR_ORDER_BGRA, { 5, 6, 5, 0}, 0, 3 }

// 8 bit:
// ======
#define WS_FORMAT_R3G3B2 { 8, WS_COLOR_TYPE_COLOR, WS_COLOR_ORDER_RGBA, { 3, 3, 2, 0}, 0, 3 }
#define WS_FORMAT_B2G3R3 { 8, WS_COLOR_TYPE_COLOR, WS_COLOR_ORDER_BGRA, { 2, 3, 3, 0}, 0, 3 }

// +--------------+
// | color struct |
// +--------------+

struct color32_t;
struct image_t;

struct color32_t {
    union {
        struct {
            uint8_t red;
            uint8_t green;
            uint8_t blue;
            uint8_t alpha;
        };
        uint8_t subpixel[4];
    };
};

// +--------------+
// | image struct |
// +--------------+

struct image_t {
    union {
        struct {
            int32_t x;
            int32_t y;
            int32_t width;
            int32_t height;
        };
        rect_t area;
    };
    color32_t *data;
};

// +---------------------------+
// | image managment functions |
// +---------------------------+

ws_code_t format_from_wl_format(format_t *r_format, int32_t wl_format);

ws_code_t image_create_empty(image_t *r_image, rect_t area);

ws_code_t image_create_from_buffer(image_t *r_image, rect_t area, uint8_t *buffer, format_t format);

ws_code_t image_clone(image_t *r_image, image_t src_image);

// middleman function for free(), used for destructor like purposes.
ws_code_t image_delete(image_t image);

// +----------------------------+
// | image processing functions |
// +----------------------------+

ws_code_t buffer_create_from_image(uint8_t *r_buffer, uint8_t color_type, image_t image);

// faster overlay, overwrites and ignores alphas, use for stiching and opaque images.
ws_code_t image_layer_overwrite(image_t dest_image, image_t src_image);

// slower overlay but doesnt ignore alphas, use for layering.
ws_code_t image_layer_overlay(image_t dest_image, image_t src_image);

ws_code_t image_vaxis_flip(image_t image);

ws_code_t image_haxis_flip(image_t image);

ws_code_t image_transpose(image_t *image);

// rotate the image by 90 * rotation counter-clockwise.
ws_code_t image_rotate(image_t *image, uint8_t rotation);

#endif // WS_IMAGE_H