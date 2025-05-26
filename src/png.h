#ifndef WS_PNG_H
#define WS_PNG_H

#include <pixman.h>
#include <png.h>

struct png_image {
	png_struct *handle;
	png_info *info;
	struct darray *data;
};

struct png_image *png_create_from_pixman(pixman_image_t * image, uint32_t compression);

#endif						   // WS_PNG_H
