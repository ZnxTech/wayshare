#include "png.h"

#include "utils.h"

static void png_event_write(png_struct *handle, png_byte *data, size_t size)
{
	struct png_image *png;
	png = png_get_io_ptr(handle);
	darray_append_array(png->data, data, size / sizeof(png_byte));
}

static void png_event_flush(png_struct *png)
{
	/* left empty. */
}

struct png_image *png_create_from_pixman(pixman_image_t *image, uint32_t compression)
{
	int image_width = pixman_image_get_width(image);
	int image_height = pixman_image_get_height(image);
	int image_stride = pixman_image_get_stride(image);
	uint8_t *data = (uint8_t *) pixman_image_get_data(image);

	struct png_image *png;
	png = calloc(1, sizeof(struct png_image));
	png->data = darray_init(sizeof(png_byte), 1);

	png->handle = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (!png->handle)
		return NULL;

	png->info = png_create_info_struct(png->handle);
	if (!png->info)
		return NULL;

	png_set_write_fn(png->handle, png, png_event_write, png_event_flush);

	png_set_IHDR(png->handle, png->info, (uint32_t) image_width, (uint32_t) image_height,
				 8, PNG_COLOR_TYPE_RGBA, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE,
				 PNG_FILTER_TYPE_BASE);

	png_write_info(png->handle, png->info);

	/* compression and filters. */
	png_set_compression_level(png->handle, PNG_Z_DEFAULT_COMPRESSION);
	png_set_filter(png->handle, PNG_FILTER_TYPE_BASE, PNG_ALL_FILTERS);

	for (int y = 0; y < image_height; y++) {
		uint8_t *row_data = (data + y * image_stride);
		png_write_row(png->handle, row_data);
	}

	png_write_end(png->handle, NULL);

	/* cleanup. */
	if (png->info)
		png_destroy_info_struct(png->handle, &png->info);

	if (png->handle)
		png_destroy_write_struct(&png->handle, NULL);

	return png;
}
