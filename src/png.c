#include "png.h"

#include <png.h>

/* checks if the image has transparancy or not. */
static bool check_pixman_transparency(const void *data, int width, int height, int stride)
{
	for (int y = 0; y < height; y++) {
		const uint32_t *row = (data + y * stride);
		for (int x = 0; x < width; x++) {
			if (row[x] >> 24 != 0xff)
				return true;
		}
	}
	/* no transparancy found, all pixels are opaque. */
	return false;
}

static void pack_png_row(void *png_row, const void *pixman_row, int width, bool transparent)
{
	/* png and pixman rows are identical,
	   memcpy and skip packing process. */
	if (transparent) {
		/* RGBA 4 byte pixel size */
		memcpy(png_row, pixman_row, width * 4);
		return;
	}

	for (int x = 0; x < width; x++) {
		/* fetch pixel colors */
		uint8_t r = ((uint32_t *)pixman_row)[x] >> 0 & 0xff;
		uint8_t g = ((uint32_t *)pixman_row)[x] >> 8 & 0xff;
		uint8_t b = ((uint32_t *)pixman_row)[x] >> 16 & 0xff;
		uint8_t a = ((uint32_t *)pixman_row)[x] >> 24 & 0xff;

		/* revert from premult alpha colors,
		   dont if alpha is 0 or 255. */
		if (a != 0x00 && a != 0xff) {
			r = ((uint16_t)r * 0xff) / a;
			g = ((uint16_t)g * 0xff) / a;
			b = ((uint16_t)b * 0xff) / a;
		}

		/* pack into {png_row}. */
		*(uint8_t *)png_row++ = r;
		*(uint8_t *)png_row++ = g;
		*(uint8_t *)png_row++ = b;
	}
}

static void png_event_write(png_struct *handle, png_byte *data, size_t size)
{
	struct darray *write_buffer;
	write_buffer = png_get_io_ptr(handle);
	darray_append_array(write_buffer->data, data, size / sizeof(png_byte));
}

static void png_event_flush(png_struct *png)
{
	/* left empty. */
}

ecode_t png_write_from_pixman(struct darray **r_buffer, pixman_image_t *image, int32_t comp_level)
{
	int image_width = pixman_image_get_width(image);
	int image_height = pixman_image_get_height(image);
	int image_stride = pixman_image_get_stride(image);
	void *data = (void *)pixman_image_get_data(image);

	bool transparent;
	transparent = check_pixman_transparency(data, image_width, image_height, image_height);

	png_struct *png_handle = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (!png_handle)
		return WSE_PNG_STRUCT_INITF;

	png_info *png_info = png_create_info_struct(png_handle);
	if (!png_info) {
		/* cleanup handle if defined. */
		if (png_handle)
			png_destroy_write_struct(&png_handle, NULL);
		return WSE_PNG_INFO_INITF;
	}

	/* init write buffer after errors. */
	struct darray *buffer;
	buffer = darray_init(sizeof(uint8_t), 1);

	png_set_write_fn(png_handle, buffer, png_event_write, png_event_flush);

	png_set_IHDR(png_handle, png_info, (uint32_t)image_width, (uint32_t)image_height, 8,
				 (transparent) ? PNG_COLOR_TYPE_RGBA : PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
				 PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

	png_write_info(png_handle, png_info);

	/* calculate compression 0..9 from wayshare compression 0..255 */
	int32_t comp = comp_level / 28.33f;
	if (comp > 9)
		comp = 9; /* safe guard. */

	png_set_compression_level(png_handle, comp);
	if (comp_level == 0)
		/* no need for filters if png has no compression. */
		png_set_filter(png_handle, PNG_FILTER_TYPE_BASE, PNG_NO_FILTERS);
	else
		png_set_filter(png_handle, PNG_FILTER_TYPE_BASE, PNG_ALL_FILTERS);

	uint8_t png_row[image_width * ((transparent) ? 4 : 3)];
	for (int y = 0; y < image_height; y++) {
		void *pixman_row = (data + y * image_stride);
		pack_png_row(png_row, pixman_row, image_width, transparent);
		png_write_row(png_handle, png_row);
	}

	png_write_end(png_handle, NULL);

	/* cleanup. */
	if (png_info)
		png_destroy_info_struct(png_handle, &png_info);

	if (png_handle)
		png_destroy_write_struct(&png_handle, NULL);

	*r_buffer = buffer;
	return WS_OK;
}
