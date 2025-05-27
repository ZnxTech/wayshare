#ifndef WS_JPEG_H
#define WS_JPEG_H

#include <pixman.h>

#include "wayshare.h"
#include "utils.h"

ecode_t jpeg_write_from_pixman(uint8_t ** r_data, size_t *r_size, pixman_image_t * image,
							   int32_t comp_level);

#endif						   // WS_JPEG_H
