#include "format.h"

#define UINTN_MAX16(n) (~(uint16_t)0 >> (16 - n))
#define UINTN_MAX32(n) (~(uint32_t)0 >> (32 - n))
#define UINTN_MAX64(n) (~(uint64_t)0 >> (64 - n))

struct format format_create(uint8_t depth, enum c_space space, uint8_t order[4], uint8_t depths[4],
							uint32_t fourcc)
{
	struct format format;
	format.pix_depth = depth;
	format.c_space = space;
	format.fourcc = fourcc;

	uint8_t delta = 0, j;
	for (uint8_t i = 0; i < 4; i++) {
		j = order[i];
		format.c_deltas[j] = delta;
		format.c_exist[j] = depths[j];
		delta += depths[j];
	}

	return format;
}

static inline uint16_t pix_get_c(uint64_t pix, uint8_t i_c, struct format format)
{
	return (uint16_t) (pix >> format.c_deltas[i_c]) & UINTN_MAX16(format.c_depths[i_c]);
}

static inline uint16_t pix_redepth_c(uint16_t c, uint8_t i_c, struct format old_format,
									 struct format new_format)
{
	return (uint16_t) ((uint32_t) c * UINTN_MAX16(new_format.c_depths[i_c]))
		/ UINTN_MAX16(old_format.c_depths[i_c]);
}

static inline uint64_t pix_form(uint16_t c_0, uint16_t c_1, uint16_t c_2, uint16_t c_3,
								struct format format)
{
	return ((uint64_t) c_0 << format.c_deltas[0]) + ((uint64_t) c_1 << format.c_deltas[1])
		+ ((uint64_t) c_2 << format.c_deltas[2]) + ((uint64_t) c_3 << format.c_deltas[3]);
}

/* same color space reformating function, just changes per color depths. */
uint64_t pix_reformat_same_space(uint64_t pix, struct format old_format, struct format new_format)
{
	uint16_t c[4];

	for (uint8_t i_c = 0; i_c < 4; i_c++) {
		if (!old_format.c_exist[i_c] || !new_format.c_exist[i_c])
			continue;

		c[i_c] = pix_get_c(pix, i_c, old_format);
		c[i_c] = pix_redepth_c(c[i_c], i_c, old_format, new_format);
	}

	return pix_form(c[0], c[1], c[2], c[3], new_format);
}

ecode_t get_reformat_func(uint64_t(**r_reformat_func) (uint64_t, struct format, struct format),
						  struct format from_format, struct format to_format)
{
	if (from_format.fourcc != DRM_FORMAT_INVALID && from_format.fourcc == to_format.fourcc)
		*r_reformat_func = NULL;	/* same exact format, no need for reformating. */
	else if (from_format.c_space == to_format.c_space)
		*r_reformat_func = pix_reformat_same_space;
	else
		return WSE_IMG_REFORMATF;
	/* TODO: add luma and yuv format conversion support. */
	return WS_OK;
}

ecode_t format_wl_to_ws(struct format *r_ws_format, uint32_t wl_format)
{
	switch (wl_format) {
	case WL_SHM_FORMAT_ABGR16161616:
		*r_ws_format = WS_FORMAT_ABGR16161616;
		break;

	case WL_SHM_FORMAT_ARGB16161616:
		*r_ws_format = WS_FORMAT_ARGB16161616;
		break;

	case WL_SHM_FORMAT_XBGR16161616:
		*r_ws_format = WS_FORMAT_XBGR16161616;
		break;

	case WL_SHM_FORMAT_XRGB16161616:
		*r_ws_format = WS_FORMAT_XRGB16161616;
		break;

	case WL_SHM_FORMAT_ABGR8888:
		*r_ws_format = WS_FORMAT_ABGR8888;
		break;

	case WL_SHM_FORMAT_ARGB8888:
		*r_ws_format = WS_FORMAT_ARGB8888;
		break;

	case WL_SHM_FORMAT_BGRA8888:
		*r_ws_format = WS_FORMAT_BGRA8888;
		break;

	case WL_SHM_FORMAT_RGBA8888:
		*r_ws_format = WS_FORMAT_RGBA8888;
		break;

	case WL_SHM_FORMAT_XBGR8888:
		*r_ws_format = WS_FORMAT_XBGR8888;
		break;

	case WL_SHM_FORMAT_XRGB8888:
		*r_ws_format = WS_FORMAT_XRGB8888;
		break;

	case WL_SHM_FORMAT_BGRX8888:
		*r_ws_format = WS_FORMAT_BGRX8888;
		break;

	case WL_SHM_FORMAT_RGBX8888:
		*r_ws_format = WS_FORMAT_RGBX8888;
		break;

	case WL_SHM_FORMAT_ABGR1555:
		*r_ws_format = WS_FORMAT_ABGR1555;
		break;

	case WL_SHM_FORMAT_ARGB1555:
		*r_ws_format = WS_FORMAT_ARGB1555;
		break;

	case WL_SHM_FORMAT_BGRA5551:
		*r_ws_format = WS_FORMAT_BGRA5551;
		break;

	case WL_SHM_FORMAT_RGBA5551:
		*r_ws_format = WS_FORMAT_RGBA5551;
		break;

	case WL_SHM_FORMAT_XBGR1555:
		*r_ws_format = WS_FORMAT_XBGR1555;
		break;

	case WL_SHM_FORMAT_XRGB1555:
		*r_ws_format = WS_FORMAT_XRGB1555;
		break;

	case WL_SHM_FORMAT_BGRX5551:
		*r_ws_format = WS_FORMAT_BGRX5551;
		break;

	case WL_SHM_FORMAT_RGBX5551:
		*r_ws_format = WS_FORMAT_RGBX5551;
		break;

	case WL_SHM_FORMAT_RGB888:
		*r_ws_format = WS_FORMAT_RGB888;
		break;

	case WL_SHM_FORMAT_BGR888:
		*r_ws_format = WS_FORMAT_BGR888;
		break;

	case WL_SHM_FORMAT_ABGR4444:
		*r_ws_format = WS_FORMAT_ABGR4444;
		break;

	case WL_SHM_FORMAT_ARGB4444:
		*r_ws_format = WS_FORMAT_ARGB4444;
		break;

	case WL_SHM_FORMAT_BGRA4444:
		*r_ws_format = WS_FORMAT_BGRA4444;
		break;

	case WL_SHM_FORMAT_RGBA4444:
		*r_ws_format = WS_FORMAT_RGBA4444;
		break;

	case WL_SHM_FORMAT_XBGR4444:
		*r_ws_format = WS_FORMAT_XBGR4444;
		break;

	case WL_SHM_FORMAT_XRGB4444:
		*r_ws_format = WS_FORMAT_XRGB4444;
		break;

	case WL_SHM_FORMAT_BGRX4444:
		*r_ws_format = WS_FORMAT_BGRX4444;
		break;

	case WL_SHM_FORMAT_RGBX4444:
		*r_ws_format = WS_FORMAT_RGBX4444;
		break;

	case WL_SHM_FORMAT_BGR565:
		*r_ws_format = WS_FORMAT_BGR565;
		break;

	case WL_SHM_FORMAT_RGB565:
		*r_ws_format = WS_FORMAT_RGB565;
		break;

	case WL_SHM_FORMAT_BGR233:
		*r_ws_format = WS_FORMAT_BGR233;
		break;

	case WL_SHM_FORMAT_RGB332:
		*r_ws_format = WS_FORMAT_RGB332;
		break;

	default:
		return WSE_IMG_NSFORMAT;
	}

	return WS_OK;
}

ecode_t format_ws_to_wl(uint32_t *r_wl_format, struct format ws_format)
{
	if (ws_format.fourcc == DRM_FORMAT_ARGB8888)
		*r_wl_format = 0;	   /* ARGB8888 exception. */
	else if (ws_format.fourcc == DRM_FORMAT_XRGB8888)
		*r_wl_format = 1;	   /* XRGB8888 exception. */
	else
		*r_wl_format = ws_format.fourcc;

	return WS_OK;
}
