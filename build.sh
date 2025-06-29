#!/bin/sh

set -xe

CC=gcc
CFLAGS="-g -std=c23 -Wall -lm -DDEBUG -DGIF_LOOP"
OUTPUT="bin/ascii_converter"

SRCS="src/main.c src/common.c src/gif.c"
IFLAGS="-Iinclude"

mkdir bin -p
$CC $CFLAGS $IFLAGS $SRCS -o $OUTPUT
