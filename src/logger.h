#ifndef WS_LOGGER_H
#define WS_LOGGER_H

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdint.h>

#define WS_SEV_INFO 0
#define WS_SEV_WARN 1
#define WS_SEV_ERR  2

#define __FILENAME__ (strrchr("/" __FILE__, '/') + 1)

void ws_logf(int severity, const char* format, const char *fname, int fline, ...);

#ifdef DEBUG
# define WS_LOGF(severity, format, ...) ws_logf(severity, format, __FILENAME__, __LINE__ __VA_OPT__(,)__VA_ARGS__)
#else
# define WS_LOGF(severity, format, ...)
#endif // DEBUG

#endif // WS_LOGGER_H