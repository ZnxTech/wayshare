#include "shm.hh"

const char *NAME_TEMP  = "/wayshare-shm-";
const char *CHARSET    = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
const int   RAND_COUNT = 8;

posix_shm_file create_shm_file(size_t size) {
    const char *name = create_shm_name();
    int fd = create_shm_fd(name);
    posix_shm_file shm = {
        .name = name,
        .fd = fd,
        .size = size
    };

    if (fd < 0) {
        shm_unlink(name);
        delete name;
        return shm;
    }

    int ret = allocate_shm_fd(fd, size);
    if (fd < 0) {
        shm_unlink(name);
        delete name;
        shm.fd = -1;
        return shm;
    }

    return shm;
}

int delete_shm_file(posix_shm_file shm) {
    int ret = shm_unlink(shm.name);
    delete[] shm.name;
    return ret;
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

static int create_shm_fd(const char *name) {
    int retries = 100;
    while (retries > 0) {
        // try creating shm file
        int fd = shm_open(name, O_RDWR | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR);
        if (fd >= 0) {
            // shm successfully created, return fd
            logf(0, "created shm file: %i\n", fd);
            return fd;
        }
        // shm failed, try again
        delete[] name;
        retries--;
    }
    // shm failed, return error
    return -1;
}

static int allocate_shm_fd(int fd, size_t size) {
    // allocate shm space
    int ret;
    int retries = 100;
    while (ret < 0 && retries > 0) {
        ret = ftruncate(fd, size);
        retries--;
    }
    if (ret < 0) {
        // could not allocate smh space
        logf(2, "could not allocate shm space.");
        return -1;
    }
    return 0;
}
