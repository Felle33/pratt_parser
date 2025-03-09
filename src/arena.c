#include <stdlib.h>
#include <assert.h>
#include "arena.h"

#define REGION_INIT(reg)						\
  do {									\
    (reg)->memory = (uintptr_t) malloc(REGION_INIT_CAP);		\
    assert((reg)->memory != 0 && "Not enough memory for reg!");	\
    (reg)->capacity = REGION_INIT_CAP;					\
    (reg)->used = 0;							\
    (reg)->next = NULL;							\
  } while(0)

void arena_init(Arena *arena) {
  assert(arena != NULL && "Arena is null!");

  Region *start_region = malloc(sizeof(Region));
  REGION_INIT(start_region);
  arena->begin = start_region;
  arena->end = start_region;
}

// Remember that the pointer stored in addr is in bytes
// so a +1 is 1 byte after
static uintptr_t align_forward(uintptr_t addr) {
  uintptr_t modulo;
  // TODO: insert the conditional compilation for different architectures
  // suppose we are in a 64 bit architecture, we would like to align at a multiple of 64 bits
  // so at a multiple of 8 bytes, since addr is already in bytes, we want that the last
  // 3 digits are 0
  modulo = addr & 0x7;

  return modulo == 0 ? addr : addr + 0x8 - modulo;
}

/*
  I will try to allocate the memory in the last block.
  Why? Since I cannot free single objects, I cannot free the regions, but I
  can only continue to allocate and the first region always free is the last one,
  otherwise if the capacity is not sufficient, I will allocate a new region
*/
void* arena_alloc(Arena *arena, size_t size) {
  assert(arena != NULL && "Arena is null!");
  assert(arena->end != NULL && "Ending region is null");

  //TODO: this is not always true, because if i reset the whole arena
  // I should start from the beginning
  Region* tar_reg = arena->begin;
  while(tar_reg != NULL && tar_reg->used + size > tar_reg->capacity) {
    tar_reg = tar_reg->next;
  }
  
  if(tar_reg == NULL) {
    Region* new_region = malloc(sizeof(Region));
    arena->end->next = new_region;
    arena->end = new_region;
    REGION_INIT(new_region);
    tar_reg = new_region;
  }
    
  uintptr_t start_addr = tar_reg->memory + tar_reg->used;
  assert((start_addr & 0x7) == 0 && "Starting address of region is not aligned!");

  uintptr_t next_aligned_addr = align_forward(start_addr + size);

  tar_reg->used = next_aligned_addr - tar_reg->memory;
  assert(tar_reg->used <= tar_reg->capacity && "Not enough capacity in the region, some bug appeared!");
  return (void*)start_addr;
}

void arena_reset(Arena *arena) {
  Region *cur_region = arena->begin;
  while(cur_region != NULL) {
    cur_region->used = 0;
    cur_region = cur_region->next;
  }
}

void arena_free(Arena *arena) {
  Region *cur_region = arena->begin;
  Region *next_region = cur_region->next;
  while(cur_region != NULL) {
    free((void*)cur_region->memory);
    free(cur_region);
    cur_region = next_region;
    if(next_region != NULL) {
      next_region = next_region->next;
    }
  }
}
