#include "utils.h"

struct rect rect_transpose(struct rect rect)
{
	return (struct rect){
		.x = rect.x,
		.y = rect.y,
		.width = rect.height,
		.height = rect.width,
	};
}

struct rect rect_intersect(struct rect rect_1, struct rect rect_2)
{
	int32_t x1 = max(rect_1.x, rect_2.x);
	int32_t y1 = max(rect_1.y, rect_2.y);
	int32_t x2 = min(rect_1.x + rect_1.width, rect_2.x + rect_2.width);
	int32_t y2 = min(rect_1.y + rect_1.height, rect_2.y + rect_2.height);

	return (struct rect){
		.x = x1,
		.y = y1,
		.width = x2 - x1,
		.height = y2 - y1,
	};
}

struct rect rect_contain(struct rect rect_1, struct rect rect_2)
{
	int32_t x1 = min(rect_1.x, rect_2.x);
	int32_t y1 = min(rect_1.y, rect_2.y);
	int32_t x2 = max(rect_1.x + rect_1.width, rect_2.x + rect_2.width);
	int32_t y2 = max(rect_1.y + rect_1.height, rect_2.y + rect_2.height);

	return (struct rect){
		.x = x1,
		.y = y1,
		.width = x2 - x1,
		.height = y2 - y1,
	};
}

bool rect_is_intersecting(struct rect rect_1, struct rect rect_2)
{
	return (rect_1.x < rect_2.x + rect_2.width) && (rect_2.x < rect_1.x + rect_1.width)
		   && (rect_1.y < rect_2.y + rect_2.height) && (rect_2.y < rect_1.y + rect_1.height);
}

bool rect_is_contained(struct rect rect_1, struct rect rect_2)
{
	return (rect_1.x < rect_2.x) && (rect_2.x + rect_2.width < rect_1.x + rect_1.width)
		   && (rect_1.y < rect_2.y) && (rect_2.y + rect_2.height < rect_1.y + rect_1.height);
}

struct rect rect_validate(struct rect rect)
{
	return (struct rect){
		.x = (rect.width) ? rect.x : rect.x + rect.width,
		.y = (rect.height) ? rect.y : rect.y + rect.height,
		.width = abs(rect.width),
		.height = abs(rect.height),
	};
}

bool rect_is_valid(struct rect rect)
{
	return (rect.width > 0 && rect.height > 0);
}

struct rect rect_unanchor(struct anchored_rect arect)
{
	return (struct rect){
		.x = min(arect.anchor_x, arect.vector_x),
		.y = min(arect.anchor_y, arect.vector_y),
		.width = abs(arect.anchor_x - arect.vector_x),
		.height = abs(arect.anchor_y - arect.vector_y),
	};
}

static char *create_shm_name()
{
	const char *prefix = "/wayshare-shm-";
	const char *charset = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
	const uint8_t randlen = 8;

	// init name string
	char *name = (char *)malloc(strlen(prefix) + randlen + 1);
	strcpy(name, prefix);

	// set seed to ns time
	struct timespec ts;
	timespec_get(&ts, TIME_UTC);
	srand(ts.tv_nsec);

	// create random string
	for (int i = 0; i < randlen; i++) {
		(name + strlen(prefix))[i] = charset[rand() % strlen(charset)];
	}
	name[strlen(prefix) + randlen] = '\0';
	return name;
}

static int create_shm_fd()
{
	int retries = 100;
	do {
		char *name = create_shm_name();
		// try creating shm file
		int fd = shm_open(name, O_RDWR | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR);
		if (fd >= 0) {
			// shm successfully created, return fd
			shm_unlink(name);
			free(name);
			return fd;
		}
		// shm failed, try again
		free(name);
		retries--;
	} while (retries > 0 && errno == EEXIST);
	// shm failed, return error
	return -1;
}

static int allocate_shm_fd(int fd, size_t size)
{
	// allocate shm space
	int ret;
	do
		ret = ftruncate(fd, size);
	while (ret < 0 && errno == EINTR);

	if (ret < 0) {
		// could not allocate shm space
		close(fd);
		return -1;
	}
	return 0;
}

ecode_t create_shm_file(int *r_fd, size_t size)
{
	const int fd = create_shm_fd();

	if (fd < 0)
		return WSE_UTIL_FDF;

	const int ret = allocate_shm_fd(fd, size);
	if (ret < 0)
		return WSE_UTIL_ALLOCF;

	*r_fd = fd;
	return WS_OK;
}

ecode_t get_user_name(const char **r_user_name)
{
	const char *user_name = getlogin();
	if (!user_name) {
		struct passwd *pwuid = getpwuid(getuid());
		if (pwuid)
			user_name = pwuid->pw_name;
		else
			return -1;
	}

	*r_user_name = user_name;
	return WS_OK;
}

ecode_t get_home_path(const char **r_home_dir_path)
{
	const char *home_dir_path = getenv("HOME");
	if (!home_dir_path) {
		struct passwd *pwuid = getpwuid(getuid());
		if (pwuid)
			home_dir_path = pwuid->pw_dir;
		else
			return WSE_CONFIG_NHOME;
	}

	*r_home_dir_path = home_dir_path;
	return WS_OK;
}

struct darray *darray_init(size_t _size, size_t _reserve)
{
	if (!_size || !_reserve)
		return NULL;

	struct darray *array;
	array = (struct darray *)malloc(sizeof(struct darray));

	array->count = 0;
	array->size = _size;
	array->reserve = _reserve;
	array->data = malloc(_size * _reserve);

	return array;
}

void darray_free(struct darray *_array)
{
	free(_array->data);
	free(_array);
}

static void *darray_get_intr(struct darray *_array, size_t _pos)
{
	return _array->data + _array->size * _pos;
}

static void darray_set_intr(struct darray *_array, size_t _pos, const void *_elems, size_t _count)
{
	void *elem = _array->data + _array->size * _pos;
	memcpy(elem, _elems, _array->size * _count);
}

void darray_append(struct darray *_array, const void *_elem)
{
	if (_array->count == _array->reserve)
		darray_reserve(_array, _array->reserve * 2);

	darray_set_intr(_array, _array->count, _elem, 1);
	_array->count++;
}

void darray_append_array(struct darray *_array, const void *_elems, size_t _count)
{
	if (_array->count + _count >= _array->reserve)
		darray_reserve(_array, (_count > _array->reserve) ? _count * 2 : _array->reserve * 2);

	darray_set_intr(_array, _array->count, _elems, _count);
	_array->count += _count;
}

void *darray_pop(struct darray *_array)
{
	if (!_array->count)
		return NULL;
	_array->count--;
	return darray_get_intr(_array, _array->count);
}

void darray_reserve(struct darray *_array, size_t _reserve)
{
	_array->data = realloc(_array->data, _array->size * _reserve);
	_array->reserve = _reserve;

	if (_array->count > _reserve)
		_array->count = _reserve;
}

void *darray_get(struct darray *_array, size_t _pos)
{
	if (_array->reserve <= _pos)
		return NULL;

	return darray_get_intr(_array, _pos);
}

void darray_set(struct darray *_array, size_t _pos, const void *_elem)
{
	if (_array->reserve <= _pos)
		return;

	darray_set_intr(_array, _pos, _elem, 1);
}

// ====================================================

struct dllist *dllist_init(void *_elem)
{
	struct dllist *head;

	head = (struct dllist *)malloc(sizeof(struct dllist));
	head->data = _elem;
	head->next = head;
	head->prev = head;

	return head;
}

struct dllist *dllist_append(struct dllist *_head, void *_elem)
{
	if (!_head)
		return dllist_init(_elem);

	struct dllist *new_tail;
	struct dllist *cur_tail;
	new_tail = (struct dllist *)malloc(sizeof(struct dllist));
	cur_tail = _head->prev;

	new_tail->data = _elem;
	new_tail->next = _head;
	new_tail->prev = cur_tail;

	cur_tail->next = new_tail;
	_head->prev = new_tail;
	return _head;
}

void *dllist_pop(struct dllist *_head)
{
	struct dllist *cur_tail = _head->prev;
	struct dllist *new_tail = _head->prev->prev;

	new_tail->next = _head;
	_head->prev = new_tail;

	void *elem = cur_tail->data;
	free(cur_tail);
	return elem;
}

void dllist_pop_free(struct dllist *_head)
{
	void *elem;
	elem = dllist_pop(_head);
	free(elem);
}

void *dllist_get(struct dllist *_head, size_t _index)
{
	for (size_t i = 0; i < _index; i++)
		_head = _head->next;
	return _head->data;
}

void dllist_set(struct dllist *_head, size_t _index, void *_elem)
{
	for (size_t i = 0; i < _index; i++)
		_head = _head->next;
	_head->data = _elem;
}

void dllist_set_free(struct dllist *_head, size_t _index, void *_elem)
{
	for (size_t i = 0; i < _index; i++)
		_head = _head->next;
	free(_head->data);
	_head->data = _elem;
}
