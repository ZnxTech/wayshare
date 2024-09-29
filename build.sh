#! /bin/sh

#wayshare build script
echo "[Build] Started"
g++ ./src/* -o ./bin/wayshare -l wayland-client
echo "[Build] Finished"