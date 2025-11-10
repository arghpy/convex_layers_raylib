#!/usr/bin/env bash

set -ex

CFLAGS="-Wall -Wextra -Wpedantic"
INCLUDES="-I./thirdparty/raylib-5.5_linux_amd64/include -I./thirdparty/utils/c/ -Wl,-rpath=./thirdparty/utils/c/"
LIBS="-lm -L./thirdparty/raylib-5.5_linux_amd64/lib -lraylib -Wl,-rpath=./thirdparty/raylib-5.5_linux_amd64/lib"

CMD="gcc ${CFLAGS} ${INCLUDES} -o convex_hull convex_hull.c ${LIBS}"

eval "bear -- ${CMD}"
