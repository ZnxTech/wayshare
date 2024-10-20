#!/bin/sh

# wayshare build script

# printf colors
NORM='\033[0m'
HIGH='\033[1;32m'

printf "| --- ${HIGH}creating protocol c files${NORM} --- |\n"

# create screencopy c/hh files from xml protocols
mkdir lib_protocols -p

for file in ./wl_protocols/*; do
    filename="$(basename $file .xml)"
    wayland-scanner private-code \
        ./wl_protocols/$filename.xml \
        ./lib_protocols/$filename.c

    wayland-scanner client-header \
        ./wl_protocols/$filename.xml \
        ./lib_protocols/$filename.hh
done

printf "| --- ${HIGH}compiling debug${NORM} --- |\n"

# compile debug version
gcc ./src/* ./lib_protocols/* \
    -I ./lib_protocols \
    -o ./bin/wayland \
    -x c++ \
    -shared-libgcc \
    -l m \
    -l stdc++ \
    -l wayland-client \
    -D WLR_USE_UNSTABLE \
    -D DEBUG

printf "| --- ${HIGH}compiling release${NORM} --- |\n"

# compile release version
gcc ./src/* ./lib_protocols/* \
    -I ./lib_protocols \
    -o ./bin/wayland_r \
    -x c++ \
    -shared-libgcc \
    -l m \
    -l stdc++ \
    -l wayland-client \
    -D WLR_USE_UNSTABLE

printf "| --- ${HIGH}finished${NORM} --- |\n"