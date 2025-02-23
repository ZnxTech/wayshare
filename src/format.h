#ifndef WS_FORMAT_H
#define WS_FORMAT_H

#include <stdint.h>

struct format {
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

#define WS_FORMAT_R16G16B16A16  (struct format) { 64, WS_COLOR_TYPE_COLOR_ALPHA, WS_COLOR_ORDER_RGBA, { 16, 16, 16, 16 }, 0, 4 }
#define WS_FORMAT_B16G16R16A16  (struct format) { 64, WS_COLOR_TYPE_COLOR_ALPHA, WS_COLOR_ORDER_BGRA, { 16, 16, 16, 16 }, 0, 4 }
#define WS_FORMAT_R16G16B16X16  (struct format) { 64, WS_COLOR_TYPE_COLOR,       WS_COLOR_ORDER_RGBA, { 16, 16, 16, 16 }, 0, 4 }
#define WS_FORMAT_B16G16R16X16  (struct format) { 64, WS_COLOR_TYPE_COLOR,       WS_COLOR_ORDER_BGRA, { 16, 16, 16, 16 }, 0, 4 }

#define WS_FORMAT_R8G8B8A8      (struct format) { 32, WS_COLOR_TYPE_COLOR_ALPHA, WS_COLOR_ORDER_RGBA, { 8, 8, 8, 8 },     0, 4 }
#define WS_FORMAT_B8G8R8A8      (struct format) { 32, WS_COLOR_TYPE_COLOR_ALPHA, WS_COLOR_ORDER_BGRA, { 8, 8, 8, 8 },     0, 4 }
#define WS_FORMAT_A8R8G8B8      (struct format) { 32, WS_COLOR_TYPE_COLOR_ALPHA, WS_COLOR_ORDER_ARGB, { 8, 8, 8, 8 },     0, 4 }
#define WS_FORMAT_A8B8G8R8      (struct format) { 32, WS_COLOR_TYPE_COLOR_ALPHA, WS_COLOR_ORDER_ABGR, { 8, 8, 8, 8 },     0, 4 }
#define WS_FORMAT_R8G8B8X8      (struct format) { 32, WS_COLOR_TYPE_COLOR,       WS_COLOR_ORDER_RGBA, { 8, 8, 8, 8 },     0, 3 }
#define WS_FORMAT_B8G8R8X8      (struct format) { 32, WS_COLOR_TYPE_COLOR,       WS_COLOR_ORDER_BGRA, { 8, 8, 8, 8 },     0, 3 }
#define WS_FORMAT_X8R8G8B8      (struct format) { 32, WS_COLOR_TYPE_COLOR,       WS_COLOR_ORDER_ARGB, { 8, 8, 8, 8 },     1, 3 }
#define WS_FORMAT_X8B8G8R8      (struct format) { 32, WS_COLOR_TYPE_COLOR,       WS_COLOR_ORDER_ABGR, { 8, 8, 8, 8 },     1, 3 }

#define WS_FORMAT_R10G10B10A2   (struct format) { 32, WS_COLOR_TYPE_COLOR_ALPHA, WS_COLOR_ORDER_RGBA, { 10, 10, 10, 2 },  0, 4 }
#define WS_FORMAT_B10G10R10A2   (struct format) { 32, WS_COLOR_TYPE_COLOR_ALPHA, WS_COLOR_ORDER_BGRA, { 10, 10, 10, 2 },  0, 4 }
#define WS_FORMAT_A2R10G10B10   (struct format) { 32, WS_COLOR_TYPE_COLOR_ALPHA, WS_COLOR_ORDER_ARGB, { 2, 10, 10, 10 },  0, 4 }
#define WS_FORMAT_A2B10G10R10   (struct format) { 32, WS_COLOR_TYPE_COLOR_ALPHA, WS_COLOR_ORDER_ABGR, { 2, 10, 10, 10 },  0, 4 }
#define WS_FORMAT_R10G10B10X2   (struct format) { 32, WS_COLOR_TYPE_COLOR,       WS_COLOR_ORDER_RGBA, { 10, 10, 10, 2 },  0, 3 }
#define WS_FORMAT_B10G10R10X2   (struct format) { 32, WS_COLOR_TYPE_COLOR,       WS_COLOR_ORDER_BGRA, { 10, 10, 10, 2 },  0, 3 }
#define WS_FORMAT_X2R10G10B10   (struct format) { 32, WS_COLOR_TYPE_COLOR,       WS_COLOR_ORDER_ARGB, { 2, 10, 10, 10 },  1, 3 }
#define WS_FORMAT_X2B10G10R10   (struct format) { 32, WS_COLOR_TYPE_COLOR,       WS_COLOR_ORDER_ABGR, { 2, 10, 10, 10 },  1, 3 }

#define WS_FORMAT_R8G8B8        (struct format) { 24, WS_COLOR_TYPE_COLOR,       WS_COLOR_ORDER_RGBA, { 8, 8, 8, 0 },     0, 3 }
#define WS_FORMAT_B8G8R8        (struct format) { 24, WS_COLOR_TYPE_COLOR,       WS_COLOR_ORDER_BGRA, { 8, 8, 8, 0 },     0, 3 }

#define WS_FORMAT_R4G4B4A4      (struct format) { 16, WS_COLOR_TYPE_COLOR_ALPHA, WS_COLOR_ORDER_RGBA, { 4, 4, 4, 4 },     0, 4 }
#define WS_FORMAT_B4G4R4A4      (struct format) { 16, WS_COLOR_TYPE_COLOR_ALPHA, WS_COLOR_ORDER_BGRA, { 4, 4, 4, 4 },     0, 4 }
#define WS_FORMAT_A4R4G4B4      (struct format) { 16, WS_COLOR_TYPE_COLOR_ALPHA, WS_COLOR_ORDER_ARGB, { 4, 4, 4, 4 },     0, 4 }
#define WS_FORMAT_A4B4G4R4      (struct format) { 16, WS_COLOR_TYPE_COLOR_ALPHA, WS_COLOR_ORDER_ABGR, { 4, 4, 4, 4 },     0, 4 }
#define WS_FORMAT_R4G4B4X4      (struct format) { 16, WS_COLOR_TYPE_COLOR,       WS_COLOR_ORDER_RGBA, { 4, 4, 4, 4 },     0, 3 }
#define WS_FORMAT_B4G4R4X4      (struct format) { 16, WS_COLOR_TYPE_COLOR,       WS_COLOR_ORDER_BGRA, { 4, 4, 4, 4 },     0, 3 }
#define WS_FORMAT_X4R4G4B4      (struct format) { 16, WS_COLOR_TYPE_COLOR,       WS_COLOR_ORDER_ARGB, { 4, 4, 4, 4 },     1, 3 }
#define WS_FORMAT_X4B4G4R4      (struct format) { 16, WS_COLOR_TYPE_COLOR,       WS_COLOR_ORDER_ABGR, { 4, 4, 4, 4 },     1, 3 }

#define WS_FORMAT_R5G5B5A1      (struct format) { 16, WS_COLOR_TYPE_COLOR_ALPHA, WS_COLOR_ORDER_RGBA, { 5, 5, 5, 1 },     0, 4 }
#define WS_FORMAT_B5G5R5A1      (struct format) { 16, WS_COLOR_TYPE_COLOR_ALPHA, WS_COLOR_ORDER_BGRA, { 5, 5, 5, 1 },     0, 4 }
#define WS_FORMAT_A1R5G5B5      (struct format) { 16, WS_COLOR_TYPE_COLOR_ALPHA, WS_COLOR_ORDER_ARGB, { 1, 5, 5, 5 },     0, 4 }
#define WS_FORMAT_A1B5G5R5      (struct format) { 16, WS_COLOR_TYPE_COLOR_ALPHA, WS_COLOR_ORDER_ABGR, { 1, 5, 5, 5 },     0, 4 }
#define WS_FORMAT_R5G5B5X1      (struct format) { 16, WS_COLOR_TYPE_COLOR,       WS_COLOR_ORDER_RGBA, { 5, 5, 5, 1 },     0, 3 }
#define WS_FORMAT_B5G5R5X1      (struct format) { 16, WS_COLOR_TYPE_COLOR,       WS_COLOR_ORDER_BGRA, { 5, 5, 5, 1 },     0, 3 }
#define WS_FORMAT_X1R5G5B5      (struct format) { 16, WS_COLOR_TYPE_COLOR,       WS_COLOR_ORDER_ARGB, { 1, 5, 5, 5 },     1, 3 }
#define WS_FORMAT_X1B5G5R5      (struct format) { 16, WS_COLOR_TYPE_COLOR,       WS_COLOR_ORDER_ABGR, { 1, 5, 5, 5 },     1, 3 }

#define WS_FORMAT_R5G6B5        (struct format) { 16, WS_COLOR_TYPE_COLOR,       WS_COLOR_ORDER_RGBA, { 5, 6, 5, 0 },     0, 3 }
#define WS_FORMAT_B5G6R5        (struct format) { 16, WS_COLOR_TYPE_COLOR,       WS_COLOR_ORDER_BGRA, { 5, 6, 5, 0 },     0, 3 }
#define WS_FORMAT_R3G3B2        (struct format) { 8,  WS_COLOR_TYPE_COLOR,       WS_COLOR_ORDER_RGBA, { 3, 3, 2, 0 },     0, 3 }
#define WS_FORMAT_B2G3R3        (struct format) { 8,  WS_COLOR_TYPE_COLOR,       WS_COLOR_ORDER_BGRA, { 2, 3, 3, 0 },     0, 3 }

#endif
