#include "cloggen.h"

ClogArena *__CLOG_ARENA__ = NULL;

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
