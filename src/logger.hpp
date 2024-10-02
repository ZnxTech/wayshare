#ifndef LOGGER_H
#define LOGGER_H

#include <cstdio>
#include <cstdarg>

void logf(int severity, const char* format, ...);

#endif