CC = clang
CFLAGS = -I./lib -Wextra -Wall -pedantic -std=c99 -ggdb -O3

all: bak rename

lib/%.o: %.c %.h
	$(CC) $(CFLAGS) -c $< -o $@

bak: bak.c lib/cstr.o lib/cli.o
	$(CC) $(CFLAGS) $^ -o $@

rename: rename.c lib/cstr.o lib/cli.o lib/str.o lib/sbuf.o lib/err.o
	$(CC) $(CFLAGS) $^ -o $@


install: all
	mkdir -p ~/.local/bin
	cp bak rename ~/.local/bin

clean:
	rm -f bak rename lib/*.o
