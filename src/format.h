#ifndef WS_FORMAT_H
#define WS_FORMAT_H

#include <stdint.h>
#include <stdbool.h>

#include <wayland-client.h>
#include <libdrm/drm_fourcc.h>

#include "wayshare.h"
#include "utils.h"

enum c_space {
	C_SPACE_NONE,			   /* TODO */
	C_SPACE_WHITE,			   /* TODO */
	C_SPACE_RGB,
	C_SPACE_YUV				   /* TODO */
};

struct format {
	uint8_t pix_depth;
	enum c_space c_space;
	bool c_exist[4];
	uint8_t c_deltas[4];
	uint8_t c_depths[4];
	uint32_t fourcc;
};

struct format format_create(uint8_t depth, enum c_space space, uint8_t order[4], uint8_t depths[4],
							uint32_t fourcc);

ecode_t get_reformat_func(uint64_t(**r_reformat_func) (uint64_t, struct format, struct format),
						  struct format from_format, struct format to_format);

ecode_t format_wl_to_ws(struct format *r_ws_format, uint32_t wl_format);
ecode_t format_ws_to_wl(uint32_t * r_wl_format, struct format ws_format);

#define C_ORDER_RGBA (uint8_t[4]) { 3, 2, 1, 0 }
#define C_ORDER_BGRA (uint8_t[4]) { 1, 2, 3, 0 }
#define C_ORDER_ARGB (uint8_t[4]) { 2, 1, 0, 3 }
#define C_ORDER_ABGR (uint8_t[4]) { 0, 1, 2, 3 }

#define WS_FORMAT_RGBA16161616  format_create(64, C_SPACE_RGB, C_ORDER_RGBA, (uint8_t[4]) { 16, 16, 16, 16 }, DRM_FORMAT_INVALID)
#define WS_FORMAT_BGRA16161616  format_create(64, C_SPACE_RGB, C_ORDER_BGRA, (uint8_t[4]) { 16, 16, 16, 16 }, DRM_FORMAT_INVALID)
#define WS_FORMAT_ARGB16161616  format_create(64, C_SPACE_RGB, C_ORDER_ARGB, (uint8_t[4]) { 16, 16, 16, 16 }, DRM_FORMAT_ARGB16161616)
#define WS_FORMAT_ABGR16161616  format_create(64, C_SPACE_RGB, C_ORDER_ABGR, (uint8_t[4]) { 16, 16, 16, 16 }, DRM_FORMAT_ABGR16161616)
#define WS_FORMAT_RGBX16161616  format_create(64, C_SPACE_RGB, C_ORDER_RGBA, (uint8_t[4]) { 16, 16, 16, 0 }, DRM_FORMAT_INVALID)
#define WS_FORMAT_BGRX16161616  format_create(64, C_SPACE_RGB, C_ORDER_BGRA, (uint8_t[4]) { 16, 16, 16, 0 }, DRM_FORMAT_INVALID)
#define WS_FORMAT_XRGB16161616  format_create(64, C_SPACE_RGB, C_ORDER_ARGB, (uint8_t[4]) { 16, 16, 16, 0 }, DRM_FORMAT_XRGB16161616)
#define WS_FORMAT_XBGR16161616  format_create(64, C_SPACE_RGB, C_ORDER_ABGR, (uint8_t[4]) { 16, 16, 16, 0 }, DRM_FORMAT_XBGR16161616)

#define WS_FORMAT_RGB161616     format_create(48, C_SPACE_RGB, C_ORDER_RGBA, (uint8_t[4]) { 16, 16, 16, 0 }, DRM_FORMAT_INVALID)
#define WS_FORMAT_BGR161616     format_create(48, C_SPACE_RGB, C_ORDER_BGRA, (uint8_t[4]) { 16, 16, 16, 0 }, DRM_FORMAT_INVALID)

#define WS_FORMAT_RGBA8888      format_create(32, C_SPACE_RGB, C_ORDER_RGBA, (uint8_t[4]) { 8, 8, 8, 8 }, DRM_FORMAT_RGBA8888)
#define WS_FORMAT_BGRA8888      format_create(32, C_SPACE_RGB, C_ORDER_BGRA, (uint8_t[4]) { 8, 8, 8, 8 }, DRM_FORMAT_BGRA8888)
#define WS_FORMAT_ARGB8888      format_create(32, C_SPACE_RGB, C_ORDER_ARGB, (uint8_t[4]) { 8, 8, 8, 8 }, DRM_FORMAT_ARGB8888)
#define WS_FORMAT_ABGR8888      format_create(32, C_SPACE_RGB, C_ORDER_ABGR, (uint8_t[4]) { 8, 8, 8, 8 }, DRM_FORMAT_ARGB8888)
#define WS_FORMAT_RGBX8888      format_create(32, C_SPACE_RGB, C_ORDER_RGBA, (uint8_t[4]) { 8, 8, 8, 0 }, DRM_FORMAT_RGBX8888)
#define WS_FORMAT_BGRX8888      format_create(32, C_SPACE_RGB, C_ORDER_BGRA, (uint8_t[4]) { 8, 8, 8, 0 }, DRM_FORMAT_BGRX8888)
#define WS_FORMAT_XRGB8888      format_create(32, C_SPACE_RGB, C_ORDER_ARGB, (uint8_t[4]) { 8, 8, 8, 0 }, DRM_FORMAT_XRGB8888)
#define WS_FORMAT_XBGR8888      format_create(32, C_SPACE_RGB, C_ORDER_ABGR, (uint8_t[4]) { 8, 8, 8, 0 }, DRM_FORMAT_XRGB8888)

#define WS_FORMAT_RGBA1010102   format_create(32, C_SPACE_RGB, C_ORDER_RGBA, (uint8_t[4]) { 10, 10, 10, 2 }, DRM_FORMAT_RGBA1010102)
#define WS_FORMAT_BGRA1010102   format_create(32, C_SPACE_RGB, C_ORDER_BGRA, (uint8_t[4]) { 10, 10, 10, 2 }, DRM_FORMAT_BGRA1010102)
#define WS_FORMAT_ARGB2101010   format_create(32, C_SPACE_RGB, C_ORDER_ARGB, (uint8_t[4]) { 10, 10, 10, 2 }, DRM_FORMAT_ARGB2101010)
#define WS_FORMAT_ABGR2101010   format_create(32, C_SPACE_RGB, C_ORDER_ABGR, (uint8_t[4]) { 10, 10, 10, 2 }, DRM_FORMAT_ARGB2101010)
#define WS_FORMAT_RGBX1010102   format_create(32, C_SPACE_RGB, C_ORDER_RGBA, (uint8_t[4]) { 10, 10, 10, 0 }, DRM_FORMAT_RGBX1010102)
#define WS_FORMAT_BGRX1010102   format_create(32, C_SPACE_RGB, C_ORDER_BGRA, (uint8_t[4]) { 10, 10, 10, 0 }, DRM_FORMAT_BGRX1010102)
#define WS_FORMAT_XRGB2101010   format_create(32, C_SPACE_RGB, C_ORDER_ARGB, (uint8_t[4]) { 10, 10, 10, 0 }, DRM_FORMAT_XRGB2101010)
#define WS_FORMAT_XBGR2101010   format_create(32, C_SPACE_RGB, C_ORDER_ABGR, (uint8_t[4]) { 10, 10, 10, 0 }, DRM_FORMAT_XRGB2101010)

#define WS_FORMAT_RGB888        format_create(24, C_SPACE_RGB, C_ORDER_RGBA, (uint8_t[4]) { 8, 8, 8, 0 }, DRM_FORMAT_RGB888)
#define WS_FORMAT_BGR888        format_create(24, C_SPACE_RGB, C_ORDER_BGRA, (uint8_t[4]) { 8, 8, 8, 0 }, DRM_FORMAT_BGR888)

#define WS_FORMAT_RGBA4444      format_create(16, C_SPACE_RGB, C_ORDER_RGBA, (uint8_t[4]) { 4, 4, 4, 4 }, DRM_FORMAT_RGBA4444)
#define WS_FORMAT_BGRA4444      format_create(16, C_SPACE_RGB, C_ORDER_BGRA, (uint8_t[4]) { 4, 4, 4, 4 }, DRM_FORMAT_BGRA4444)
#define WS_FORMAT_ARGB4444      format_create(16, C_SPACE_RGB, C_ORDER_ARGB, (uint8_t[4]) { 4, 4, 4, 4 }, DRM_FORMAT_ARGB4444)
#define WS_FORMAT_ABGR4444      format_create(16, C_SPACE_RGB, C_ORDER_ABGR, (uint8_t[4]) { 4, 4, 4, 4 }, DRM_FORMAT_ARGB4444)
#define WS_FORMAT_RGBX4444      format_create(16, C_SPACE_RGB, C_ORDER_RGBA, (uint8_t[4]) { 4, 4, 4, 0 }, DRM_FORMAT_RGBX4444)
#define WS_FORMAT_BGRX4444      format_create(16, C_SPACE_RGB, C_ORDER_BGRA, (uint8_t[4]) { 4, 4, 4, 0 }, DRM_FORMAT_BGRX4444)
#define WS_FORMAT_XRGB4444      format_create(16, C_SPACE_RGB, C_ORDER_ARGB, (uint8_t[4]) { 4, 4, 4, 0 }, DRM_FORMAT_XRGB4444)
#define WS_FORMAT_XBGR4444      format_create(16, C_SPACE_RGB, C_ORDER_ABGR, (uint8_t[4]) { 4, 4, 4, 0 }, DRM_FORMAT_XRGB4444)

#define WS_FORMAT_RGBA5551      format_create(16, C_SPACE_RGB, C_ORDER_RGBA, (uint8_t[4]) { 5, 5, 5, 1 }, DRM_FORMAT_RGBA5551)
#define WS_FORMAT_BGRA5551      format_create(16, C_SPACE_RGB, C_ORDER_BGRA, (uint8_t[4]) { 5, 5, 5, 1 }, DRM_FORMAT_BGRA5551)
#define WS_FORMAT_ARGB1555      format_create(16, C_SPACE_RGB, C_ORDER_ARGB, (uint8_t[4]) { 5, 5, 5, 1 }, DRM_FORMAT_ARGB1555)
#define WS_FORMAT_ABGR1555      format_create(16, C_SPACE_RGB, C_ORDER_ABGR, (uint8_t[4]) { 5, 5, 5, 1 }, DRM_FORMAT_ARGB1555)
#define WS_FORMAT_RGBX5551      format_create(16, C_SPACE_RGB, C_ORDER_RGBA, (uint8_t[4]) { 5, 5, 5, 0 }, DRM_FORMAT_RGBX5551)
#define WS_FORMAT_BGRX5551      format_create(16, C_SPACE_RGB, C_ORDER_BGRA, (uint8_t[4]) { 5, 5, 5, 0 }, DRM_FORMAT_BGRX5551)
#define WS_FORMAT_XRGB1555      format_create(16, C_SPACE_RGB, C_ORDER_ARGB, (uint8_t[4]) { 5, 5, 5, 0 }, DRM_FORMAT_XRGB1555)
#define WS_FORMAT_XBGR1555      format_create(16, C_SPACE_RGB, C_ORDER_ABGR, (uint8_t[4]) { 5, 5, 5, 0 }, DRM_FORMAT_XRGB1555)

#define WS_FORMAT_RGB565        format_create(16, C_SPACE_RGB, C_ORDER_RGBA, (uint8_t[4]) { 5, 6, 5, 0 }, DRM_FORMAT_RGB565)
#define WS_FORMAT_BGR565        format_create(16, C_SPACE_RGB, C_ORDER_BGRA, (uint8_t[4]) { 5, 6, 5, 0 }, DRM_FORMAT_BGR565)
#define WS_FORMAT_RGB332        format_create(8,  C_SPACE_RGB, C_ORDER_RGBA, (uint8_t[4]) { 3, 3, 2, 0 }, DRM_FORMAT_RGB332)
#define WS_FORMAT_BGR233        format_create(8,  C_SPACE_RGB, C_ORDER_BGRA, (uint8_t[4]) { 3, 3, 2, 0 }, DRM_FORMAT_BGR233)

#endif
