#include "utils.h"

rect_t rect_inter(rect_t rect_1, rect_t rect_2) {
    rect_t inter;

    int32_t x1 = fmax(rect_1.x, rect_2.x);
    int32_t y1 = fmax(rect_1.y, rect_2.y);
    int32_t x2 = fmin(rect_1.x + rect_1.width,  rect_2.x + rect_2.width);
    int32_t y2 = fmin(rect_1.y + rect_1.height, rect_2.y + rect_2.height);

    inter.x = x1;
    inter.y = y1;
    inter.width  = x2 - x1;
    inter.height = y2 - y1;

    return inter;
}

rect_t rect_trans(rect_t rect) {
    rect_t trans;

    trans.x = rect.x;
    trans.y = rect.y;
    trans.width  = rect.height;
    trans.height = rect.width;

    return trans;
}

bool rect_is_valid(rect_t rect) {
    return (rect.width > 0 && rect.height > 0);
}

// ====================================================

static char *create_shm_name() {
    const char *prefix = "/wayshare-shm-";
    const char *charset = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
    const uint8_t randlen = 8;

    // init name string
    char *name = (char*)malloc(strlen(prefix) + randlen + 1);
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

static int create_shm_fd() {
    int retries = 100;
    do {
        char* name = create_shm_name();
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

static int allocate_shm_fd(int fd, size_t size) {
    // allocate shm space
    int ret;
    do {
        ret = ftruncate(fd, size);
    } while (ret < 0 && errno == EINTR);

    if (ret < 0) {
        // could not allocate smh space
        close(fd);
		return -1;
	}
    return 0;
}

ecode_t create_shm_file(int *r_fd, size_t size) {
    const int fd = create_shm_fd();

    if (fd < 0)
        return WSE_SHM_FDF;

    const int ret = allocate_shm_fd(fd, size);
    if (ret < 0)
        return WSE_SHM_ALLOCF;

    *r_fd = fd;
    return WS_OK;
}

ecode_t get_user_name(const char **r_user_name) {
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

ecode_t get_host_name(char **r_host_name) {
    const size_t host_name_max = 256;
    char host_name[host_name_max];
    gethostname(host_name, host_name_max);
    host_name[host_name_max - 1] = '\0';

    *r_host_name = strdup(host_name);
    return WS_OK;
}

ecode_t get_home_path(const char **r_home_dir_path) {
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

// ====================================================

darray_t *darray_init(size_t _size, size_t _reserve) {
    if (!_size || !_reserve)
        return NULL;

    darray_t *array;
    array = (darray_t*)malloc(sizeof(darray_t));

    array->count = 0;
    array->size = _size;
    array->reserve = _reserve;
    array->data = malloc(_size * _reserve);

    return array;
}

void darray_free(darray_t *_array) {
    free(_array->data);
    free(_array);
}

static void *darray_get_intr(darray_t *_array, size_t _pos) {
    return _array->data + _array->size * _pos;
}

static void darray_set_intr(darray_t *_array, size_t _pos, const void *_elems, size_t _count) {
    void *elem = _array->data + _array->size * _pos;
    memcpy(elem, _elems, _array->size * _count);
}

void darray_append(darray_t *_array, const void *_elem) {
    if (_array->count == _array->reserve)
        darray_reserve(_array, _array->reserve * 2);

    darray_set_intr(_array, _array->count, _elem, 1);
    _array->count++;
}

void darray_append_array(darray_t *_array, const void *_elems, size_t _count) {
    if (_array->count + _count >= _array->reserve)
        darray_reserve(_array, (_count > _array->reserve) ? _count * 2 : _array->reserve * 2);

    darray_set_intr(_array, _array->count, _elems, _count);
    _array->count += _count;
}

void *darray_pop(darray_t *_array) {
    if (!_array->count)
        return NULL;
    _array->count--;
    return darray_get_intr(_array, _array->count);
}

void darray_reserve(darray_t *_array, size_t _reserve) {
    _array->data = realloc(_array->data, _array->size * _reserve);
    _array->reserve = _reserve;

    if (_array->count > _reserve)
        _array->count = _reserve;
}

void *darray_get(darray_t *_array, size_t _pos) {
    if (_array->reserve <= _pos)
        return NULL;

    return darray_get_intr(_array, _pos);
}

void darray_set(darray_t *_array, size_t _pos, const void *_elem) {
    if (_array->reserve <= _pos)
        return;

    darray_set_intr(_array, _pos, _elem, 1);
}

// ====================================================

dllist_t *dllist_init(void *_elem) {
    dllist_t *head;

    head = (dllist_t*)malloc(sizeof(dllist_t));
    head->data = _elem;
    head->next = head;
    head->prev = head;

    return head;
}

dllist_t *dllist_append(dllist_t *_head, void *_elem) {
    if (!_head)
        return dllist_init(_elem);

    dllist_t *new_tail; dllist_t *cur_tail;
    new_tail = (dllist_t*)malloc(sizeof(dllist_t));
    cur_tail = _head->prev;

    new_tail->data = _elem;
    new_tail->next = _head;
    new_tail->prev = cur_tail;

    cur_tail->next = new_tail;
    _head->prev = new_tail;
    return _head;
}

void *dllist_pop(dllist_t *_head) {
    dllist_t *cur_tail = _head->prev;
    dllist_t *new_tail = _head->prev->prev;

    new_tail->next = _head;
    _head->prev = new_tail;

    void *elem = cur_tail->data;
    free(cur_tail);
    return elem;
}

void dllist_pop_free(dllist_t *_head) {
    void *elem;
    elem = dllist_pop(_head);
    free(elem);
}

void *dllist_get(dllist_t *_head, size_t _index) {
    for (size_t i = 0; i < _index; i++)
        _head = _head->next;
    return _head->data;
}

void dllist_set(dllist_t *_head, size_t _index, void *_elem) {
    for (size_t i = 0; i < _index; i++)
        _head = _head->next;
    _head->data = _elem;
}

void dllist_set_free(dllist_t *_head, size_t _index, void *_elem) {
    for (size_t i = 0; i < _index; i++)
        _head = _head->next;
    free(_head->data);
    _head->data = _elem;
}