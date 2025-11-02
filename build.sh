#!/usr/bin/env bash

set -ex

CFLAGS="-Wall -Wextra -Wpedantic"
INCLUDES="-I./thirdparty/raylib-5.5_linux_amd64/include"
STATIC="./thirdparty/raylib-5.5_linux_amd64/lib/libraylib.a"
LIBS="-lm"

CMD="gcc ${CFLAGS} ${INCLUDES} -o convex_hull convex_hull.c ${STATIC} ${LIBS}"

eval "bear -- ${CMD}"
eval "${CMD}"
