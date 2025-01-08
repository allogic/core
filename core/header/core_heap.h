#ifndef CORE_HEAP_H
#define CORE_HEAP_H

#include <stdint.h>

#include "core_config.h"
#include "core_macros.h"

extern void heap_prologue(void);
extern void* heap_alloc(uint64_t size);
extern void* heap_realloc(void* block, uint64_t size);
extern void heap_free(void* block);
extern void heap_epilogue(void);

extern uint64_t g_heap_allocated_bytes;

#endif // CORE_HEAP_H
