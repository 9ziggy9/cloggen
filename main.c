#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/mman.h>
#include <assert.h>
#include <string.h>

#define CLOG_PAGE_VIRTUALLY  false
#define CLOG_PAGE_PHYSICALLY true

typedef struct {
  size_t size;
  size_t used;
  void *mem_start;
  void *mem_offset;
} ClogArena;

ClogArena *_clog_arena_init(size_t, bool);
void *_clog_arena_alloc(ClogArena *, size_t);
void  _clog_arena_reset(ClogArena *);
void  _clog_arena_free(ClogArena *);

ClogArena *__CLOG_ARENA__ = NULL;

#define __CLOG_DEFAULT_ARENA_BYTES__ (1024 * 10) /* 10 kB default */

#define CLOG_INIT()                                                         \
  do {                                                                      \
    __CLOG_ARENA__ =                                                        \
      _clog_arena_init(__CLOG_DEFAULT_ARENA_BYTES__, CLOG_PAGE_PHYSICALLY); \
  } while(0);

#define CLOG_ALLOC(SZ) _clog_arena_alloc(__CLOG_ARENA__, SZ)
#define CLOG_RESET()   _clog_arena_reset(__CLOG_ARENA__)
#define CLOG_CLOSE()   _clog_arena_free(__CLOG_ARENA__)

typedef struct { const char *restrict str; size_t len; } ClogString;

ClogString clog_string_make(const char *str) {
  size_t len = strlen(str);
  char *buffer = CLOG_ALLOC(len + 1);
  if (buffer == NULL) assert(0 && "implement panic <-- clog_string_make");
  memcpy(buffer, str, len + 1);
  return (ClogString){ buffer, len };
}

void clog_string_write(ClogString clog_string, FILE *fd) {
  fwrite(clog_string.str, sizeof(char), clog_string.len, fd);
}

// HACK: macros to get around annoying format string warnings
// We need to get rid of this, perhaps a flag in a ClogString which
// indicates whether or not the string is a format string.
#define CLOG_DISABLE_FORMAT_SECURITY                      \
  _Pragma("GCC diagnostic push")                          \
  _Pragma("GCC diagnostic ignored \"-Wformat-security\"")
#define CLOG_ENABLE_FORMAT_SECURITY  _Pragma("GCC diagnostic pop")

#define CLOG_STRING_FPRINT(clog_string, ...)          \
  do {                                                \
    CLOG_DISABLE_FORMAT_SECURITY;                     \
    fprintf(stdout, clog_string.str, ##__VA_ARGS__);  \
    CLOG_ENABLE_FORMAT_SECURITY;                      \
  } while (0);

int main(void) {
  CLOG_INIT();
  ClogString str = clog_string_make("Hello, world!\n");
  clog_string_write(str, stdout);
  CLOG_STRING_FPRINT(str);
  return EXIT_SUCCESS;
}

ClogArena *_clog_arena_init(size_t bytes, bool page_now) {
#define PAGE_SIZE 4096
  ClogArena *arena = mmap(NULL, sizeof(ClogArena),
                            PROT_READ | PROT_WRITE,
                            MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  if (arena == MAP_FAILED || arena == NULL) assert(0 && "implement panic");
  arena->size = bytes;
  arena->used = 0;
  arena->mem_start = mmap(NULL, bytes,
                          PROT_READ | PROT_WRITE,
                          MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  if (arena->mem_start == NULL) assert(0 && "implement panic");
  arena->mem_offset = arena->mem_start;
  if (page_now) {
    for (size_t i = 0; i < bytes; i += PAGE_SIZE) {
      ((char *)arena->mem_start)[i] = 0;
    }
  }
  return arena;
#undef PAGE_SIZE
}

void *_clog_arena_alloc(ClogArena *arena, size_t size) {
  // Round up the allocation size to the nearest multiple of 8 for alignment.
  size = (size + 7) & ~(size_t)7;

  // Check if there's enough space in the arena otherwise return NULL pointer.
  if (arena->used + size > arena->size) return NULL;

  // Allocate memory from the arena
  void *ptr = arena->mem_offset;
  arena->mem_offset = (char *)arena->mem_offset + size;
  arena->used += size;

  // Return the allocated memory
  return ptr;
}

void _clog_arena_reset(ClogArena *arena) {
  if (arena != NULL) {
    arena->used = 0;
    arena->mem_offset = arena->mem_start;
  }
}

void _clog_arena_free(ClogArena *arena) {
  if (arena != NULL) {
    // Free the memory allocated for the arena metadata
    if (munmap(arena, sizeof(ClogArena)) == -1) {
      assert(0 && "Failed to munmap arena metadata");
    }
    // Free the memory allocated for the arena memory block
    if (munmap(arena->mem_start, arena->size) == -1) {
      assert(0 && "Failed to munmap arena memory block");
    }
  }
}
