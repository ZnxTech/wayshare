#include "shm.hh"

const char *NAME_TEMP  = "/wayshare-shm-";
const char *CHARSET    = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
const int   RAND_COUNT = 8;

int create_shm_file(size_t size) {
    int fd = create_shm_fd();

    if (fd < 0) {
        logf(0, "failed to create shm file: %i\n", fd);
        return fd;
    }

    int ret = allocate_shm_fd(fd, size);
    if (fd < 0) {
        logf(0, "failed to allocate shm file: %i\n", fd);
        return -1;
    }

    logf(0, "created shm file: %i of size %i bytes.\n", fd, size);
    return fd;
}

static const char *create_shm_name() {
    // init name string
    char *name = new char[strlen(NAME_TEMP) + RAND_COUNT + 1];
    strcpy(name, NAME_TEMP);

    // set seed to ns time
    timespec ts;
    timespec_get(&ts, TIME_UTC);
    srand(ts.tv_nsec);

    // create random string
    for (int i = 0; i < RAND_COUNT; i++) {
        (name + strlen(NAME_TEMP))[i] = CHARSET[rand() % strlen(CHARSET)];
    }
    name[strlen(NAME_TEMP) + RAND_COUNT] = '\0';
    return name;
}

static int create_shm_fd() {
    int retries = 100;
    do {
        const char* name = create_shm_name();
        // try creating shm file
        int fd = shm_open(name, O_RDWR | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR);
        if (fd >= 0) {
            // shm successfully created, return fd
            shm_unlink(name);
            delete[] name;
            return fd;
        }
        // shm failed, try again
        delete[] name;
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
