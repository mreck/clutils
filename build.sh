#!/bin/sh

CFLAGS='-I./lib -Wextra -Wall -pedantic -std=c99 -ggdb -O3'

clang -o rename $CFLAGS rename.c ./lib/cstr.c ./lib/cli.c
