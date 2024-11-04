#ifndef RECT_H
#define RECT_H

#include <math.h>

struct rect {
    int x = 0;
    int y = 0;
    int width = 0;
    int height = 0;
};

rect rect_get_intersection(rect b1, rect b2);

bool rect_is_valid(rect b);

#endif