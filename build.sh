#!/usr/bin/env bash

set -ex

FLAGS="-Wall -Wextra -Wpedantic"
INCLUDES="-I./raylib-5.5_linux_amd64/include"
STATIC="./raylib-5.5_linux_amd64/lib/libraylib.a"
LIBS="-lm"

CMD="gcc ${FLAGS} ${INCLUDES} -o main main.c ${STATIC} ${LIBS}"

eval "bear -- ${CMD}"
eval "${CMD}"
