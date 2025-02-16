#include "logger.h"

void ws_logf(int severity, const char* format, const char *fname, int fline, ...) {
    va_list args;
    va_start(args, fline);

    switch (severity) {
    case WS_SEV_INFO:
        printf("[\e[1;32mINFO\e[0m]: ");
        break;

    case WS_SEV_WARN:
        printf("[\e[1;33mWARN\e[0m]: ");
        break;

    case WS_SEV_ERR:
        printf("[\e[1;31mERROR\e[0m]: ");
        break;

    default:
        break;
    }
    printf("%s:%i ", fname, fline);
    vprintf(format, args);
    va_end(args);
}