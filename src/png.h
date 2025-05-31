#ifndef WS_PNG_H
#define WS_PNG_H

#include <pixman.h>

#include "utils.h"
#include "wayshare.h"

ecode_t png_write_from_pixman(void **r_data, size_t *r_size, pixman_image_t *image, int32_t comp_level);

#endif // WS_PNG_H
