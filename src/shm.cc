#include "shm.hh"

static int create_shm_fd() {
    int retries = 100;
    while (retries > 0) {
        // init name string
        int n = 8;
        const char *name_temp = "/wayshare-shm-";
        char name[strlen(name_temp) + n + 1];
        strcpy(name, name_temp);

        // set seed to ns time
        timespec ts;
        timespec_get(&ts, TIME_UTC);
        srand(ts.tv_nsec);

        // create random string
        for (int i = 0; i < n; i++) {
            (name+strlen(name_temp))[i] = 'A' + (rand() % 26);
        }
        name[strlen(name_temp) + n] = '\0';

        logf(0, "creating shm file: %s\n", name);

        // try creating shm file
        int fd = shm_open(name, O_RDWR | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR);
        if (fd >= 0) {
            // shm successfully created, return fd
            logf(0, "created shm file: %i\n", fd);
            return fd;
        }
        logf(1, "shm file creation failed, trying %i more times.\n", retries);
        retries--;
    }
    // shm failed
    return -1;
}

int allocate_shm_fd(size_t size) {
    // create shm
    int fd = create_shm_fd();
    if (fd < 0) {
        // invalid shm fd
        logf(2, "could not create shm file.");
        return -1;
    }

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
    return fd;
}
