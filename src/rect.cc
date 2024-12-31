#include "rect.hh"

rect_t rect_get_intersection(rect_t r1, rect_t r2) {
    int x1 = fmax(r1.x, r2.x);
    int y1 = fmax(r1.y, r2.y);
    int x2 = fmin(r1.x + r1.width, r2.x + r2.width);
    int y2 = fmin(r1.y + r1.height, r2.y + r2.height);

    return {
        .x = x1,
        .y = y1,
        .width  = x2 - x1,
        .height = y2 - y1
    };
}

bool rect_is_valid(rect_t r) {
    return (r.width > 0 && r.height > 0);
}
