#ifndef SHM_H
#define SHM_H

#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include "logger.hh"

// +-----------------+
// | shm file struct |
// +-----------------+

struct posix_shm_file {
    const char* name;
    int fd;
    size_t size;
};

// +----------------------+
// | public shm functions |
// +----------------------+

posix_shm_file create_shm_file(size_t size);

int delete_shm_file(posix_shm_file shm);

// +-----------------------+
// | private shm functions |
// +-----------------------+

static const char *create_shm_name();

static int create_shm_fd();

static int allocate_shm_fd(size_t size);

#endif