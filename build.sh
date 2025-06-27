#!/bin/sh

set -xe

CC=gcc
CFLAGS="-g -std=c11 -Wall -pedantic"
OUTPUT="bin/ascii_converter"

SRCS="src/main.c"

$CC $CFLAGS $SRCS -o $OUTPUT
