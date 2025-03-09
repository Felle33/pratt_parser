#ifndef ARENA_ALLOC
#define ARENA_ALLOC

#include <stdint.h>
#include <stddef.h>

#ifndef REGION_INIT_CAP
#define REGION_INIT_CAP (8 * 1024)
#endif

typedef struct Region{
  uintptr_t memory;
  size_t capacity;
  size_t used;
  struct Region *next;
} Region;

typedef struct {
  Region *begin;
  Region *end;
} Arena;

void arena_init(Arena *arena);
void* arena_alloc(Arena *arena, size_t size);
void arena_reset(Arena *arena);
void arena_free(Arena *arena);

#endif // ARENA_ALLOC
