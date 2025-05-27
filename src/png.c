#include "png.h"

#include <png.h>

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
	uint8_t *data = (uint8_t *) pixman_image_get_data(image);

	png_struct *png_handle = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (!png_handle)
		return WSE_PNG_STRUCT_INITF;

	png_info *png_info = png_create_info_struct(png_handle);
	if (!png_info)
		return WSE_PNG_INFO_INITF;

	/* init write buffer after errors. */
	struct darray *buffer;
	buffer = darray_init(sizeof(uint8_t), 1);

	png_set_write_fn(png_handle, buffer, png_event_write, png_event_flush);

	png_set_IHDR(png_handle, png_info, (uint32_t) image_width, (uint32_t) image_height,
				 8, PNG_COLOR_TYPE_RGBA, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE,
				 PNG_FILTER_TYPE_BASE);

	png_write_info(png_handle, png_info);

	/* calculate compression 0..9 from wayshare compression 0..255 */
	int32_t comp = comp_level / 28.33f;
	if (comp > 9)
		comp = 9;			   /* safe guard. */

	png_set_compression_level(png_handle, comp);
	if (comp_level == 0)
		/* no need for filters if png has no compression. */
		png_set_filter(png_handle, PNG_FILTER_TYPE_BASE, PNG_NO_FILTERS);
	else
		png_set_filter(png_handle, PNG_FILTER_TYPE_BASE, PNG_ALL_FILTERS);

	for (int y = 0; y < image_height; y++) {
		uint8_t *row_data = (data + y * image_stride);
		png_write_row(png_handle, row_data);
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
