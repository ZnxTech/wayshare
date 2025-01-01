#include "rect.hh"

rect_t rect_intersect(rect_t r1, rect_t r2) {
    rect_t inter = { };

    int x1 = fmax(r1.x, r2.x);
    int y1 = fmax(r1.y, r2.y);
    int x2 = fmin(r1.x + r1.width, r2.x + r2.width);
    int y2 = fmin(r1.y + r1.height, r2.y + r2.height);

    inter.x = x1;
    inter.y = y1;
    inter.width  = x2 - x1;
    inter.height = y2 - y1;

    return inter;
}

rect_t rect_transpose(rect_t r) {
    rect_t trans = { };

    trans.x = r.x;
    trans.y = r.y;
    trans.height = r.width;
    trans.width  = r.height;

    return trans;
}

bool rect_is_valid(rect_t r) {
    return (r.width > 0 && r.height > 0);
}
