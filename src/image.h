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

enum image_transform {
	TRANSFORM_NONE = 0,
	TRANSFORM_90DEG = 1,
	TRANSFORM_180DEG = 2,
	TRANSFORM_MIRRORED = 4
};

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
	uint32_t transform;
	struct format format;
	void *data;
};

ecode_t image_create(struct image **r_image, struct rect area, struct format format);

ecode_t image_create_empty(struct image **r_image, struct rect area, struct format format);

ecode_t image_create_buffer(struct image **r_image, struct rect area, struct format format,
							void *buffer);

ecode_t image_free(struct image *image);

ecode_t image_clone(struct image **r_image, struct image *src_image);

ecode_t image_set_transform(struct image *image, uint32_t transform);

ecode_t image_reformat(struct image **r_image, struct image *src_image, struct format format);

ecode_t image_reformat_transform(struct image **r_image, struct image *src_image,
								 struct format format);

ecode_t image_transform(struct image **r_image, struct image *src_image);

ecode_t image_layer(struct image *dst_image, struct image *src_image);

#endif						   // WS_IMAGE_H
