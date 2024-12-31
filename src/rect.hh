#ifndef WS_RECT_H
#define WS_RECT_H

#include <cstdint>
#include <cmath>

struct rect {
    int32_t x = 0;
    int32_t y = 0;
    int32_t width = 0;
    int32_t height = 0;
};

rect rect_get_intersection(rect b1, rect b2);

bool rect_is_valid(rect b);

#endif