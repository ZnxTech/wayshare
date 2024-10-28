#ifndef SHM_H
#define SHM_H

#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include "logger.hh"

static int create_shm_fd();

int allocate_shm_fd(size_t size);

#endif