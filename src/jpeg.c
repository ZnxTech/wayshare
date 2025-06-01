#include "jpeg.h"

#include <jpeglib.h>

/* checks if the image has transparancy or not. */
static bool check_pixman_transparency(void *data, int width, int height, int stride)
{
	for (int y = 0; y < height; y++) {
		uint32_t *row = (data + y * stride);
		for (int x = 0; x < width; x++) {
			if (row[x] >> 24 != 0xff)
				return true;
		}
	}
	/* no transparancy found, all pixels are opaque. */
	return false;
}

ecode_t jpeg_write_from_pixman(uint8_t **r_data, size_t *r_size, pixman_image_t *image,
							   int32_t comp_level)
{
	int image_height = pixman_image_get_height(image);
	int image_width = pixman_image_get_width(image);
	int image_stride = pixman_image_get_stride(image);
	void *data = pixman_image_get_data(image);

	struct jpeg_compress_struct cinfo;
	struct jpeg_error_mgr jerr;
	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_compress(&cinfo);

	uint8_t *buffer_data = NULL;
	size_t buffer_size = 0;
	jpeg_mem_dest(&cinfo, &buffer_data, &buffer_size);

	cinfo.image_width = image_width;
	cinfo.image_height = image_height;

	if (check_pixman_transparency(data, image_width, image_height, image_stride)) {
		cinfo.in_color_space = JCS_EXT_RGBA;
		cinfo.input_components = 4;
	} else {
		/* thankfully provides an RGBX space,
		   no need for packing. unlike libpng... */
		cinfo.in_color_space = JCS_EXT_RGBX;
		cinfo.input_components = 4;
	}

	/* calculate quality 0..100 from wayshare compression 255..0 */
	int32_t quality = (255 - comp_level) / 2.55f;
	if (quality > 100)
		quality = 100; /* safe guard. */

	jpeg_set_defaults(&cinfo);
	jpeg_set_quality(&cinfo, quality, TRUE);

	jpeg_start_compress(&cinfo, TRUE);

	JSAMPROW row_pointer[1];
	while (cinfo.next_scanline < cinfo.image_height) {
		row_pointer[0] = data + cinfo.next_scanline * image_stride;
		jpeg_write_scanlines(&cinfo, row_pointer, 1);
	}

	jpeg_finish_compress(&cinfo);
	jpeg_destroy_compress(&cinfo);

	*r_data = buffer_data;
	*r_size = buffer_size;
	return WS_OK;
}
