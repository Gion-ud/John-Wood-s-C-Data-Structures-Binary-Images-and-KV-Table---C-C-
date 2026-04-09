all: bipf/imgpch.h.pch bipf-rd bipf-wr

CC = gcc
INCLUDES = -Iinclude -Iwin_include
LIBPATHS = -Llib
LDLIBS = -lmman -lz
CFLAGS = -O2 -Wextra -Wall -Wpedantic # -Werror
LDFLAGS = -s

bipf/imgpch.h.pch: bipf/imgpch.h
	$(CC) $(INCLUDES) -x c-header $< -o $@

bipf-rd: bipf-rd.c
	$(CC) $(CFLAGS) $(LDFLAGS) $(INCLUDES) $^ -o $@ $(LIBPATHS) $(LDLIBS)

bipf-wr: bipf-wr.c
	$(CC) $(CFLAGS) $(LDFLAGS) $(INCLUDES) $^ -o $@ $(LIBPATHS) $(LDLIBS)