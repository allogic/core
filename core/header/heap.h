#ifndef HEAP_H
#define HEAP_H

#include <stdint.h>

extern void heap_prologue(void);
extern void* heap_alloc(uint64_t size);
extern void* heap_realloc(void* block, uint64_t size);
extern void heap_free(void* block);
extern void heap_epilogue(void);

extern uint64_t g_heap_allocated_bytes;

#endif // HEAP_H
