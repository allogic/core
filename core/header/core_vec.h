#ifndef CORE_VEC_H
#define CORE_VEC_H

#include <stdint.h>

#include "core_config.h"
#include "core_macros.h"

typedef struct _vec_t
{
	uint8_t* buffer;
	uint8_t* swap_buffer;
	uint64_t value_size;
	uint64_t buffer_capacity;
	uint64_t buffer_size;
	uint64_t buffer_count;
} vec_t;

typedef void (*vec_print_func_t)(uint64_t index, void* value);

extern vec_t vec_alloc(uint64_t value_size);
extern vec_t vec_copy(vec_t* ref);
extern uint8_t vec_equal(vec_t* vec, vec_t* ref);
extern void vec_fill(vec_t* vec, void const* value);
extern void vec_push(vec_t* vec, void const* value);
extern void vec_push_range(vec_t* vec, vec_t* range);
extern void vec_pop(vec_t* vec, void* value);
extern void vec_resize(vec_t* vec, uint64_t count);
extern void vec_expand(vec_t* vec);
extern void vec_remove(vec_t* vec, void const* value);
extern void vec_swap(vec_t* vec, uint64_t left_index, uint64_t right_index);
extern void vec_clear(vec_t* vec);
extern void* vec_back(vec_t* vec);
extern void* vec_front(vec_t* vec);
extern void* vec_at(vec_t* vec, uint64_t index);
extern void* vec_buffer(vec_t* vec);
extern void vec_set(vec_t* vec, uint64_t index, void const* value);
extern uint8_t vec_empty(vec_t* vec);
extern uint64_t vec_size(vec_t* vec);
extern uint64_t vec_count(vec_t* vec);
extern uint64_t vec_capacity(vec_t* vec);
extern void vec_print(vec_t* vec, vec_print_func_t print_func);
extern void vec_free(vec_t* vec);

#endif // CORE_VEC_H
