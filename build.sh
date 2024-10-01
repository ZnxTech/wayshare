#! /bin/sh

#wayshare build script
echo "[Build] Started"
g++ ./src/* -o ./bin/wayshare   -l wayland-client -D __debug__
g++ ./src/* -o ./bin/wayshare_r -l wayland-client
echo "[Build] Finished"