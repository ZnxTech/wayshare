#ifndef BOX_H
#define BOX_H

#include <math.h>

struct box {
    int x = 0;
    int y = 0;
    int width = 0;
    int height = 0;
};

box box_get_intersection(box b1, box b2);

bool box_is_valid(box b);

#endif