#!/bin/sh

CFLAGS='-I./lib -Wextra -Wall -pedantic -std=c99 -ggdb -O3' 

CC -o rename $CFLAGS rename.c ./lib/cstr.c ./lib/cli.c
