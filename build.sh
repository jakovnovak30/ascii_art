#!/bin/sh

set -xe

CC=gcc
CFLAGS="-g -std=c23 -Wall -lm -DDEBUG"
OUTPUT="bin/ascii_converter"

SRCS="src/main.c"
IDIR="include/gif_load"

$CC $CFLAGS -I$IDIR $SRCS -o $OUTPUT
