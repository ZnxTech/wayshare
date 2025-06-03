#ifndef WS_UTILS_H
#define WS_UTILS_H

#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <pwd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "wayshare.h"

/**
 * unique varnames for macros, god do i hate this.
 */
#define _concat(a, b) a ## b
#define _append_var(var, s) _concat(var, s)
#define _unique_var(var) _append_var(var, __LINE__)

/**
 * math util:
 * basic mathematical abstractions.
 */

/**
 * <struct rect>
 * a basic rectangle struct.
 * NOTE:
 * this struct and its subsequent functions represent,
 * and only function, under non-negative width and height values.
 * rect_is_valid() should be used to assess if the rectangle is valid
 * for proper use.
 */
struct rect {
	int32_t x;
	int32_t y;
	int32_t width;
	int32_t height;
};

struct rect rect_transpose(struct rect rect);

struct rect rect_intersect(struct rect rect_1, struct rect rect_2);

struct rect rect_contain(struct rect rect_1, struct rect rect_2);

bool rect_is_intersecting(struct rect rect_1, struct rect rect_2);

/* true if {rect_2} is contained in {rect_1}. */
bool rect_is_contained(struct rect rect_1, struct rect rect_2);

struct rect rect_validate(struct rect rect);

bool rect_is_valid(struct rect rect);

/**
 * <struct anchored_rect>
 * an anchored rectangle that is represented
 * as two points instead of a point and a size.
 */
struct anchored_rect {
	int32_t anchor_x;
	int32_t anchor_y;
	int32_t vector_x;
	int32_t vector_y;
};

/* returns a normal rectangle <struct rect> from a <struct anchored_rect> */
struct rect rect_unanchor(struct anchored_rect arect);

/**
 * unix utils:
 * abstractions for unix/posix/linux interactions.
 */

/* creates a shm (shared memory) file of size {size} and sets pointed r_fd if successfull. */
ecode_t create_shm_file(int *r_fd, size_t size);

/* gets the name of the current user running the program, string must not be freed. */
ecode_t get_user_name(const char **r_user_name);

/* gets the home directory of the current user running the program, string must not be freed. */
ecode_t get_home_path(const char **r_home_dir_path);

/**
 * <struct darray>
 * a basic dynamic array struct.
 */
struct darray {
	void *data;		// the array pointer.
	size_t size;	// the byte size of each element in the array.
	size_t count;	// the amount of elements that data currently has, stack only.
	size_t reserve; // the amount of elements that data can store.
};

struct darray *darray_init(size_t _size, size_t _reserve);

void darray_free(struct darray *_array);

void darray_append(struct darray *_array, const void *_elem);

void darray_append_array(struct darray *_array, const void *_elems, size_t _count);

void *darray_pop(struct darray *_array);

void darray_reserve(struct darray *_array, size_t _reserve);

void *darray_get(struct darray *_array, size_t _pos);

void darray_set(struct darray *_array, size_t _pos, const void *_elem);

#define darray_foreach(_array, _elem_p)											\
	for (size_t _unique_var(_index) = 0;										\
		(_unique_var(_index) < _array->count)									\
			? (_elem_p = *(void **)darray_get(_array, _unique_var(_index))) : 0;\
		_unique_var(_index)++)

#define darray_foreach_i(_array, _index, _elem_p)					\
	for (_index = 0;												\
		(_index < _array->count)									\
			? (_elem_p = *(void **)darray_get(_array, _index)) : 0;	\
		_index++)

#define darray_foreach_f(_array, _elem_p)										\
    for (size_t _unique_var(_index) = 0;										\
		(_unique_var(_index) < _array->reserve)									\
			? (_elem_p = *(void **)darray_get(_array, _unique_var(_index))) : 0;\
		_unique_var(_index)++)

/**
 * <struct dllist>
 * a basic doubley linked list struct.
 */
struct dllist {
	void *data;			 // the contained data.
	struct dllist *prev; // the prev node.
	struct dllist *next; // the next node.
};

struct dllist *dllist_init(void *_elem);

struct dllist *dllist_append(struct dllist *_head, void *_elem);

void *dllist_pop(struct dllist *_head);

void dllist_pop_free(struct dllist *_head);

void *dllist_get(struct dllist *_head, size_t _index);

void dllist_set(struct dllist *_head, size_t _index, void *_elem);

void dllist_set_free(struct dllist *_head, size_t _index, void *_elem);

#define dllist_foreach(_head, _index, _elem)    \
    sllist_t *_head_ref = _head;                \
    size_t _index; void *_elem;                 \
    for (_index = 0; (_head_ref != _head->next) ? (_elem = _head->data), (_head = _head->next) : 0; _index++)

#endif // WS_UTILS_H
