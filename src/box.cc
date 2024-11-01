#include "box.hh"

box box_get_intersection(box b1, box b2) {
    int x1 = fmax(b1.x, b2.x);
    int y1 = fmax(b1.y, b2.y);
    int x2 = fmin(b1.x + b1.width, b2.x + b2.width);
    int y2 = fmin(b1.y + b1.height, b2.y + b2.height);

    return {
        .x = x1,
        .y = y1,
        .width  = x2 - x1,
        .height = y2 - y1
    };
}

bool box_is_valid(box b) {
    return (b.width > 0 && b.height > 0);
}
