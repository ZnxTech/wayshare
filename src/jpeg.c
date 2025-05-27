#include "jpeg.h"

#include <jpeglib.h>

ecode_t jpeg_write_from_pixman(uint8_t **r_data, size_t *r_size, pixman_image_t *image,
							   int32_t comp_level)
{
	struct jpeg_compress_struct cinfo;
	struct jpeg_error_mgr jerr;
	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_compress(&cinfo);

	uint8_t *buffer_data = NULL;
	size_t buffer_size = 0;
	jpeg_mem_dest(&cinfo, &buffer_data, &buffer_size);

	cinfo.image_width = pixman_image_get_width(image);
	cinfo.image_height = pixman_image_get_height(image);

	pixman_format_code_t format = pixman_image_get_format(image);
	if (format == PIXMAN_a8r8g8b8) {
		cinfo.in_color_space = JCS_EXT_BGRA;
		cinfo.input_components = 4;
	} else if (format == PIXMAN_x8r8g8b8) {
		cinfo.in_color_space = JCS_EXT_BGRX;
		cinfo.input_components = 4;
	} else if (format == PIXMAN_r8g8b8) {
		cinfo.in_color_space = JCS_EXT_BGR;
		cinfo.input_components = 3;
	}

	/* calculate quality 0..100 from wayshare compression 255..0 */
	int32_t quality = (255 - comp_level) / 2.25f;
	if (quality > 100)
		quality = 100;		   /* safe guard. */

	jpeg_set_defaults(&cinfo);
	jpeg_set_quality(&cinfo, quality, TRUE);

	jpeg_start_compress(&cinfo, TRUE);

	JSAMPROW row_pointer[1];
	uint8_t *image_data = pixman_image_get_data(image);
	size_t image_stride = pixman_image_get_stride(image);
	while (cinfo.next_scanline < cinfo.image_height) {
		row_pointer[0] = image_data + cinfo.next_scanline * image_stride;
		jpeg_write_scanlines(&cinfo, row_pointer, 1);
	}

	jpeg_finish_compress(&cinfo);
	jpeg_destroy_compress(&cinfo);

	*r_data = buffer_data;
	*r_size = buffer_size;
	return WS_OK;
}
