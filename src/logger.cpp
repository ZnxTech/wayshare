#include "logger.hpp"

void logf(int severity, char* format, ...) {
    va_list args;
    va_start(args, format);

    switch (severity) {
    case 0:
        printf("[\e[1;32mINFO\e[0m]: ");
        vprintf(format, args);
        break;

    case 1:
        printf("[\e[1;33mWARN\e[0m]: ");
        vprintf(format, args);
        break;

    case 2:
        printf("[\e[1;31mERROR\e[0m]: ");
        vprintf(format, args);
        break;

    default:
        break;
    }

    va_end(args);
}