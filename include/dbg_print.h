#ifndef DGB_PRINT_H
#define DGB_PRINT_H

#include <stdio.h>
#include <errno.h>
#include <stdbool.h>

#define printerrf(...) fprintf(stderr, __VA_ARGS__);fflush(stderr)

#define is_null(p) _is_null((p), #p)

static inline bool _is_null(const void *p, const char *p_sym) {
    if (!p) {
        printerrf("%s: NULL ptr\n", p_sym);
        return true;
    }
    return false;
}

#define DEBUG

#ifdef DEBUG
#define print_dbg_msg(...) fprintf(stderr, __VA_ARGS__);fflush(stderr)
#else
#define print_dbg_msg(...)
#endif

#define ESC "\x1b"
//;[
#define COLOUR_BLACK   "[30m"
#define COLOUR_RED     "[31m"
#define COLOUR_GREEN   "[32m"
#define COLOUR_YELLOW  "[33m"
#define COLOUR_BLUE    "[34m"
#define COLOUR_MAGENTA "[35m"
#define COLOUR_CYAN    "[36m"
#define COLOUR_WHITE   "[37m"
#define RESET_COLOUR   "[0m"



#endif