#ifndef CLOGGEN_H_
#define CLOGGEN_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <sys/mman.h>
#include <stdbool.h>

// HACK: macros to get around annoying format string warnings
// We need to get rid of this, perhaps a flag in a ClogString which
// indicates whether or not the string is a format string.
#define CLOG_DISABLE_FORMAT_SECURITY                      \
  _Pragma("GCC diagnostic push")                          \
  _Pragma("GCC diagnostic ignored \"-Wformat-security\"")
#define CLOG_ENABLE_FORMAT_SECURITY  _Pragma("GCC diagnostic pop")

#define CLOG_PAGE_VIRTUALLY  false
#define CLOG_PAGE_PHYSICALLY true
#define __CLOG_DEFAULT_ARENA_BYTES__ (1024 * 10) /* 10 kB default */

typedef struct {
  size_t size;
  size_t used;
  void *mem_start;
  void *mem_offset;
} ClogArena;

typedef struct { const char *str; size_t len; } ClogString;

#define CLOG_INIT()                                                         \
  do {                                                                      \
    __CLOG_ARENA__ =                                                        \
      _clog_arena_init(__CLOG_DEFAULT_ARENA_BYTES__, CLOG_PAGE_PHYSICALLY); \
  } while(0);
#define CLOG_ALLOC(SZ) _clog_arena_alloc(__CLOG_ARENA__, SZ)
#define CLOG_RESET()   _clog_arena_reset(__CLOG_ARENA__)
#define CLOG_CLOSE()   _clog_arena_free(__CLOG_ARENA__)

#define CLOG_STRING_FPRINT(clog_string, ...)          \
  do {                                                \
    CLOG_DISABLE_FORMAT_SECURITY;                     \
    fprintf(stdout, clog_string.str, ##__VA_ARGS__);  \
    CLOG_ENABLE_FORMAT_SECURITY;                      \
  } while (0);

ClogString clog_string_make(const char *str);
void clog_string_write(ClogString clog_string, FILE *fd);

ClogArena *_clog_arena_init(size_t, bool);
void *_clog_arena_alloc(ClogArena *, size_t);
void  _clog_arena_reset(ClogArena *);
void  _clog_arena_free(ClogArena *);

extern ClogArena *__CLOG_ARENA__;

#endif // CLOGGEN_H_
