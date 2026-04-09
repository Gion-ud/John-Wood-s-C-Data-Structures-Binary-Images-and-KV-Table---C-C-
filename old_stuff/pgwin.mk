all: page.o main

CC = gcc
INCLUDES = -Iinclude -Iwin_include
LIBPATHS = -Llib
LDLIBS = -lmman -lz
CFLAGS = -O2 -Wextra -Wall -Wpedantic # -Werror
LDFLAGS = -s

page.o: page.c
	$(CC) -c $(CFLAGS) $(INCLUDES) $< -o $@

main: page.o main.c
	$(CC) $(CFLAGS) $(LDFLAGS) $(INCLUDES) $^ -o $@ $(LIBPATHS) $(LDLIBS)