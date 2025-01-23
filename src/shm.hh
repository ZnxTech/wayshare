#ifndef WS_SHM_H
#define WS_SHM_H

#include <cerrno>
#include <cstdlib>
#include <ctime>
#include <cstring>

#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>

#include "wayshare.hh"
#include "logger.hh"

// +----------------------+
// | public shm functions |
// +----------------------+

ecode_t create_shm_file(int *r_fd, size_t size);

// +-----------------------+
// | private shm functions |
// +-----------------------+

static const char *create_shm_name();

static int create_shm_fd();

static int allocate_shm_fd(int fd, size_t size);

#endif // WS_SHM_H