#ifndef WS_IMAGE_H
#define WS_IMAGE_H

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <wayland-client.h>

#include "utils.h"
#include "wayshare.h"
#include "logger.h"
#include "format.h"

// +--------------+
// | color struct |
// +--------------+

struct color32 {
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

struct image {
	union {
		struct {
			int32_t x;
			int32_t y;
			int32_t width;
			int32_t height;
		};
		struct rect area;
	};
	struct color32 *data;
};

// +---------------------------+
// | image managment functions |
// +---------------------------+

ecode_t format_from_wl_format(struct format *r_format, int32_t wl_format);

ecode_t image_create_empty(struct image *r_image, struct rect area);

ecode_t image_create_from_buffer(struct image *r_image, struct rect area, uint8_t * buffer,
								 struct format format);

ecode_t image_clone(struct image *r_image, struct image src_image);

// middleman function for free(), used for destructor like purposes.
ecode_t image_delete(struct image image);

// +----------------------------+
// | image processing functions |
// +----------------------------+

ecode_t buffer_create_from_image(uint8_t * r_buffer, uint8_t color_type, struct image image);

// faster overlay, overwrites and ignores alphas, use for stiching and opaque images.
ecode_t image_layer_overwrite(struct image dest_image, struct image src_image);

// slower overlay but doesnt ignore alphas, use for layering.
ecode_t image_layer_overlay(struct image dest_image, struct image src_image);

ecode_t image_vaxis_flip(struct image image);

ecode_t image_haxis_flip(struct image image);

ecode_t image_transpose(struct image *image);

// rotate the image by 90 * rotation counter-clockwise.
ecode_t image_rotate(struct image *image, uint8_t rotation);

#endif						   // WS_IMAGE_H
