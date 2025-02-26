CC = clang
CFLAGS = -I./lib -Wextra -Wall -pedantic -std=c99 -ggdb -O3

all: rename

lib/%.o: %.c %.h
	$(CC) $(CFLAGS) -c $< -o $@

rename: rename.c lib/cstr.o lib/cli.o
	$(CC) $(CFLAGS) $^ -o $@

install: all
	mkdir -p ~/.local/bin
	cp rename ~/.local/bin

clean:
	rm -f rename lib/*.o
