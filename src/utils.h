#ifndef WS_UTILS_H
#define WS_UTILS_H

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>

#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <limits.h>
#include <errno.h>
#include <fcntl.h>
#include <pwd.h>

#include "wayshare.h"

// rectangle coords:
// =================

typedef struct rect_s {
    int32_t x;
    int32_t y;
    int32_t width;
    int32_t height;
} rect_t;

rect_t rect_inter(rect_t rect_1, rect_t rect_2);

rect_t rect_trans(rect_t rect);

bool rect_is_valid(rect_t rect);

// posix/unix/linux:
// =================

ecode_t create_shm_file(int *r_fd, size_t size);

ecode_t get_user_name(const char **r_user_name);

ecode_t get_host_name(char **r_host_name);

ecode_t get_home_path(const char **r_home_dir_path);

// dynamic array:
// ==============

typedef struct darray_s {
    void *data;     // the array pointer.
    size_t size;    // the byte size of each element in the array.
    size_t count;   // the amount of elements that data currently has, stack only.
    size_t reserve; // the amount of elements that data can store.
} darray_t;

darray_t *darray_init(size_t _size, size_t _reserve);

void darray_free(darray_t *_array);

void darray_append(darray_t *_array, const void *_elem);

void darray_append_array(darray_t *_array, const void *_elems, size_t _count);

void *darray_pop(darray_t *_array);

void darray_reserve(darray_t *_array, size_t _reserve);

void *darray_get(darray_t *_array, size_t _pos);

void darray_set(darray_t *_array, size_t _pos, const void *_elem);

#define darray_foreach(_array, _index, _elem)   \
    size_t _index; void *_elem;                 \
    for (_index = 0; (_index < _array->count) ? _elem = darray_get(_array, _index) : 0; _index++)

#define darray_foreachf(_array, _index, _elem)  \
    size_t _index; void *_elem;                 \
    for (_index = 0; (_index < _array->reserve) ? _elem = darray_get(_array, _index) : 0; _index++)

// doubley linked list:
// ====================

typedef struct dllist_s {
    void *data;     // the contained data.
    struct dllist_s *prev; // the prev node.
    struct dllist_s *next; // the next node.
} dllist_t;

dllist_t *dllist_init(void *_elem);

dllist_t *dllist_append(dllist_t *_head, void *_elem);

void *dllist_pop(dllist_t *_head);

void dllist_pop_free(dllist_t *_head);

void *dllist_get(dllist_t *_head, size_t _index);

void dllist_set(dllist_t *_head, size_t _index, void *_elem);

void dllist_set_free(dllist_t *_head, size_t _index, void *_elem);

#define dllist_foreach(_head, _index, _elem)    \
    sllist_t *_head_ref = _head;                \
    size_t _index; void *_elem;                 \
    for (_index = 0; (_head_ref != _head->next) ? _elem = _head->data, _head = _head->next : 0; _index++)

#endif // WS_UTILS_H