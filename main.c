#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/mman.h>
#include <assert.h>

#define CLOG_PAGE_VIRTUALLY  false
#define CLOG_PAGE_PHYSICALLY true

typedef struct ClogArena {
  size_t size;
  size_t used;
  void *mem_start;
  void *mem_offset;
} ClogArena;

ClogArena *_clog_arena_init(size_t, bool);
void *_clog_arena_alloc(ClogArena *, size_t);
void  _clog_arena_reset(ClogArena *);
void  _clog_arena_free(ClogArena *);

// 10 kilobytes by default
#define __CLOG_ARENA_BYTES__ (1024 * 10)
ClogArena *__CLOG_ARENA__ = NULL;

#define CLOG_INIT()                                                 \
  do {                                                              \
    __CLOG_ARENA__ =                                                \
      _clog_arena_init(__CLOG_ARENA_BYTES__, CLOG_PAGE_PHYSICALLY); \
  } while(0);

const char *CLOG_REGISTER_TYPE_FMT() {
  return "hello number %d\n";
}

int main(void) {
  CLOG_INIT();
  printf(CLOG_REGISTER_TYPE_FMT(), 10);
  return EXIT_SUCCESS;
}

ClogArena *_clog_arena_init(size_t bytes, bool page_now) {
#define PAGE_SIZE 4096
  ClogArena *arena = mmap(NULL, sizeof(ClogArena),
                            PROT_READ | PROT_WRITE,
                            MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  if (arena == MAP_FAILED) assert(0 && "implement panic");
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
