#!/bin/sh

# wayshare build script

NORM='\033[0m'
HIGH='\033[1;32m'

printf "| --- ${HIGH}creating protocol c files${NORM} --- |\n"

# create wlroots screencopy header from xml protocol
wayland-scanner client-header \
    ./wl_protocols/wlr-screencopy-unstable-v1.xml \
    ./src/wlr-screencopy-unstable-v1.h

wayland-scanner private-code \
    ./wl_protocols/wlr-screencopy-unstable-v1.xml \
    ./src/wlr-screencopy-unstable-v1.c

printf "| --- ${HIGH}compiling debug${NORM} --- |\n"

# compile debug version
gcc ./src/* \
    -o ./bin/wayshare \
    -l wayland-client \
    -D WLR_USE_UNSTABLE \
    -D DEBUG

printf "| --- ${HIGH}compiling release${NORM} --- |\n"

# compile release version
gcc ./src/* \
    -o ./bin/wayshare_r \
    -l wayland-client \
    -D WLR_USE_UNSTABLE

printf "| --- ${HIGH}finished${NORM} --- |\n"