#include "image.h"

ecode_t image_create(struct image **r_image, struct rect area, struct format format)
{
	struct image *image;
	image = calloc(1, sizeof(struct image));
	image->area = area;
	image->format = format;
	image->data = malloc(area.width * area.height * (format.pix_depth / 8));
	*r_image = image;
	return WS_OK;
}

ecode_t image_create_empty(struct image **r_image, struct rect area, struct format format)
{
	struct image *image;
	image = calloc(1, sizeof(struct image));
	image->area = area;
	image->format = format;
	image->data = calloc(area.width * area.height, (format.pix_depth / 8));
	*r_image = image;
	return WS_OK;
}

ecode_t image_create_buffer(struct image **r_image, struct rect area, struct format format,
							void *buffer)
{
	struct image *image;
	image_create(&image, area, format);
	memcpy(image->data, buffer, area.width * area.height * (format.pix_depth / 8));
	*r_image = image;
	return WS_OK;
}

ecode_t image_free(struct image *image)
{
	free(image->data);
	free(image);
	return WS_OK;
}

ecode_t image_clone(struct image **r_image, struct image *src_image)
{
	struct image *image;
	image_create_buffer(&image, src_image->area, src_image->format, src_image->data);
	image->transform = src_image->transform;
	*r_image = image;
	return WS_OK;
}

ecode_t image_set_transform(struct image *image, uint32_t transform)
{
	uint32_t rotation = (image->transform + transform) % 4;
	/* replace the 2 first bits with the new rotation values. */
	image->transform = (image->transform & ~(uint32_t) 0b11) | rotation;
	image->transform ^= transform & TRANSFORM_MIRRORED;
	return WS_OK;
}

struct rect image_get_logical_area(struct image *image)
{
	if (image->transform & TRANSFORM_90DEG)
		return rect_transpose(image->area);
	else
		return image->area;
}

static void coords_buffer_to_logical(uint32_t *r_logical_x, uint32_t *r_logical_y,
									 struct image *image, uint32_t buffer_x, uint32_t buffer_y)
{
	/* first invert then swap coords. */
	bool inv_x = false, inv_y = false, swap = false;

	/* 90deg CCW transform. */
	inv_x ^= image->transform & TRANSFORM_90DEG;
	swap ^= image->transform & TRANSFORM_90DEG;

	/* 180deg CCW transform. */
	inv_x ^= image->transform & TRANSFORM_180DEG;
	inv_y ^= image->transform & TRANSFORM_180DEG;

	/* mirror transform. */
	inv_x ^= image->transform & TRANSFORM_MIRRORED;

	uint32_t x = (inv_x) ? (image->width - buffer_x - 1) : buffer_x;
	uint32_t y = (inv_y) ? (image->height - buffer_y - 1) : buffer_y;

	*r_logical_x = (swap) ? y : x;
	*r_logical_y = (swap) ? x : y;
}

static void coords_logical_to_buffer(uint32_t *r_buffer_x, uint32_t *r_buffer_y,
									 struct image *image, uint32_t logical_x, uint32_t logical_y)
{
	/* first invert then swap coords. */
	bool inv_x = false, inv_y = false, swap = false;

	/* 90deg CW transform. */
	inv_y ^= image->transform & TRANSFORM_90DEG;
	swap ^= image->transform & TRANSFORM_90DEG;

	/* 180deg CW transform. */
	inv_x ^= image->transform & TRANSFORM_180DEG;
	inv_y ^= image->transform & TRANSFORM_180DEG;

	/* mirror transform. */
	inv_x ^= image->transform & TRANSFORM_MIRRORED;

	uint32_t x = (inv_x) ? (image->width - logical_x - 1) : logical_x;
	uint32_t y = (inv_y) ? (image->height - logical_y - 1) : logical_y;

	*r_buffer_x = (swap) ? y : x;
	*r_buffer_y = (swap) ? x : y;
}

static inline void *get_pix_pointer(struct image *image, uint32_t x, uint32_t y)
{
	return image->data + (y * image->width + x) * (image->format.pix_depth / 8);
}

/* creates a new image from the {src_image} with format {format}. */
ecode_t image_reformat(struct image **r_image, struct image *src_image, struct format format)
{
	uint64_t(*reformat_func) (uint64_t, struct format, struct format) = NULL;
	ecode_t code = get_reformat_func(&reformat_func, src_image->format, format);
	if (code)
		return WSE_IMG_REFORMATF;

	struct image *image;
	image_create(&image, src_image->area, format);
	image_set_transform(image, src_image->transform);

	const size_t new_pix_size = image->format.pix_depth / 8;
	for (size_t i_pix = 0; i_pix < image->width * image->height; i_pix++) {
		/* extract pixel and reformat. */
		uint64_t pix = *(uint64_t *) get_pix_pointer(src_image, i_pix, 0);
		if (reformat_func != NULL)
			pix = reformat_func(pix, src_image->format, format);
		memcpy(get_pix_pointer(image, i_pix, 0), &pix, new_pix_size);
	}

	*r_image = image;
	return WS_OK;
}

/* creates a new image from the {src_image} with formath {format}, transformed. */
ecode_t image_reformat_transform(struct image **r_image, struct image *src_image,
								 struct format format)
{
	uint64_t(*reformat_func) (uint64_t, struct format, struct format) = NULL;
	ecode_t code = get_reformat_func(&reformat_func, src_image->format, format);
	if (code)
		return WSE_IMG_REFORMATF;

	struct image *image;
	image_create(&image, image_get_logical_area(src_image), format);

	const size_t new_pix_size = image->format.pix_depth / 8;
	for (uint32_t y = 0; y < image->height; y++) {
		for (uint32_t x = 0; x < image->width; x++) {
			uint32_t buffer_x, buffer_y;
			coords_logical_to_buffer(&buffer_x, &buffer_y, src_image, x, y);

			uint64_t pix = *(uint64_t *) get_pix_pointer(src_image, buffer_x, buffer_y);
			if (reformat_func != NULL)
				pix = reformat_func(pix, src_image->format, image->format);
			memcpy(get_pix_pointer(image, x, y), &pix, new_pix_size);
		}
	}

	*r_image = image;
	return WS_OK;
}

/* creates a new image from {src_image} transformed. */
ecode_t image_transform(struct image **r_image, struct image *src_image)
{
	struct image *image;
	image_create(&image, image_get_logical_area(src_image), src_image->format);

	const size_t new_pix_size = image->format.pix_depth / 8;
	for (uint32_t y = 0; y < image->height; y++) {
		for (uint32_t x = 0; x < image->width; x++) {
			uint32_t buffer_x, buffer_y;
			coords_logical_to_buffer(&buffer_x, &buffer_y, src_image, x, y);

			uint64_t pix = *(uint64_t *) get_pix_pointer(src_image, buffer_x, buffer_y);
			memcpy(get_pix_pointer(image, x, y), &pix, new_pix_size);
		}
	}

	return WS_OK;
}

/**
 * layers an image {src_image} on top of the {dst_image}, copying the reletive to their coords.
 * {src_image} pixels will have their format converted if they dont match {cst_image}'s format.
 */
ecode_t image_layer(struct image *dst_image, struct image *src_image)
{
	struct rect src_area = image_get_logical_area(src_image);
	struct rect dst_area = image_get_logical_area(dst_image);
	struct rect intersect = rect_intersect(src_area, dst_area);
	if (!rect_is_valid(intersect))
		return WS_OK;		   /* images dont intersect, nothing to do. */

	/* calculate the intersection's starting coords delta in src and dst. */
	const uint32_t src_delta_x = intersect.x - src_area.x;
	const uint32_t src_delta_y = intersect.y - src_area.y;

	const uint32_t dst_delta_x = intersect.x - dst_area.x;
	const uint32_t dst_delta_y = intersect.y - dst_area.y;

	uint64_t(*reformat_func) (uint64_t, struct format, struct format) = NULL;
	ecode_t code = get_reformat_func(&reformat_func, src_image->format, dst_image->format);
	if (code)
		return WSE_IMG_REFORMATF;

	const size_t dst_pix_size = dst_image->format.pix_depth / 8;
	for (uint32_t y = 0; y < intersect.height; y++) {
		for (uint32_t x = 0; x < intersect.width; x++) {
			uint32_t src_x, src_y, dst_x, dst_y;
			coords_logical_to_buffer(&src_x, &src_y, src_image, x + src_delta_x, y + src_delta_y);
			coords_logical_to_buffer(&dst_x, &dst_y, dst_image, x + dst_delta_x, y + dst_delta_y);

			uint64_t pix = *(uint64_t *) get_pix_pointer(src_image, src_x, src_y);
			if (reformat_func != NULL)
				pix = reformat_func(pix, src_image->format, dst_image->format);
			memcpy(get_pix_pointer(dst_image, dst_x, dst_y), &pix, dst_pix_size);
		}
	}

	return WS_OK;
}
