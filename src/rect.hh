#ifndef WS_RECT_H
#define WS_RECT_H

#include <cstdint>
#include <cmath>

struct rect_t {
    int32_t x = 0;
    int32_t y = 0;
    int32_t width = 0;
    int32_t height = 0;
};

rect_t rect_intersect(rect_t r1, rect_t r2);

rect_t rect_transpose(rect_t r);

bool rect_is_valid(rect_t r);

#endif