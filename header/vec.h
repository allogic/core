#ifndef VEC_H
#define VEC_H

#ifndef VEC_BUFFER_COUNT
	#define VEC_BUFFER_COUNT (16)
#endif // VEC_BUFFER_COUNT

#ifdef _HEAP_TRACE
	#include "heap.h"
	#define HEAP_ALLOC(SIZE) heap_alloc(SIZE)
	#define HEAP_REALLOC(BLOCK, SIZE) heap_realloc(BLOCK, SIZE)
	#define HEAP_FREE(BLOCK) heap_free(BLOCK)
#else
	#include <stdlib.h>
	#define HEAP_ALLOC(SIZE) malloc(SIZE)
	#define HEAP_REALLOC(BLOCK, SIZE) realloc(BLOCK, SIZE)
	#define HEAP_FREE(BLOCK) free(BLOCK)
#endif // _HEAP_TRACE

#ifndef MAX
	#define MAX(A, B) (((A) > (B)) ? (A) : (B))
#endif // MAX

#ifndef MIN
	#define MIN(A, B) (((A) < (B)) ? (A) : (B))
#endif // MIN

#include <stdint.h>
#include <string.h>
#include <assert.h>

typedef struct _vec_t
{
	uint8_t* buffer;
	uint8_t* swap_buffer;
	uint64_t value_size;
	uint64_t buffer_size;
	uint64_t buffer_count;
	uint64_t buffer_index;
	uint64_t buffer_offset;
} vec_t;

extern vec_t vec_alloc(uint64_t value_size);
extern vec_t vec_copy(vec_t* reference);
extern void vec_push(vec_t* vec, void const* value);
extern void vec_pop(vec_t* vec, void* value);
extern void vec_resize(vec_t* vec, uint64_t count);
extern void vec_swap(vec_t* vec, uint64_t left_index, uint64_t right_index);
extern void vec_clear(vec_t* vec);
extern void* vec_back(vec_t* vec);
extern void* vec_front(vec_t* vec);
extern void* vec_at(vec_t* vec, uint64_t index);
extern void const* vec_buffer(vec_t* vec);
extern void vec_set(vec_t* vec, uint64_t index, void const* value);
extern uint8_t vec_empty(vec_t* vec);
extern uint64_t vec_count(vec_t* vec);
extern uint64_t vec_size(vec_t* vec);
extern void vec_expand(vec_t* vec);
extern void vec_free(vec_t* vec);

#ifdef VEC_IMPLEMENTATION
	vec_t vec_alloc(uint64_t value_size)
	{
		vec_t vec;
		memset(&vec, 0, sizeof(vec_t));
		vec.buffer = (uint8_t*)HEAP_ALLOC(value_size * VEC_BUFFER_COUNT);
		vec.swap_buffer = (uint8_t*)HEAP_ALLOC(value_size);
		vec.value_size = value_size;
		vec.buffer_size = value_size * VEC_BUFFER_COUNT;
		vec.buffer_count = VEC_BUFFER_COUNT;
		vec.buffer_index = 0;
		vec.buffer_offset = 0;
		return vec;
	}
	vec_t vec_copy(vec_t* reference)
	{
		assert(reference->buffer);
		vec_t vec;
		memset(&vec, 0, sizeof(vec_t));
		vec.buffer = (uint8_t*)HEAP_ALLOC(reference->buffer_size);
		vec.swap_buffer = (uint8_t*)HEAP_ALLOC(reference->value_size);
		vec.value_size = reference->value_size;
		vec.buffer_size = reference->buffer_size;
		vec.buffer_count = reference->buffer_count;
		vec.buffer_index = reference->buffer_index;
		vec.buffer_offset = reference->buffer_offset;
		memcpy(vec.buffer, reference->buffer, reference->buffer_size);
		return vec;
	}
	void vec_push(vec_t* vec, void const* value)
	{
		if (value)
		{
			memcpy(vec->buffer + vec->buffer_offset, value, vec->value_size);
		}
		vec->buffer_index++;
		vec->buffer_offset += vec->value_size;
		if (vec->buffer_index >= vec->buffer_count)
		{
			vec_expand(vec);
		}
	}
	void vec_pop(vec_t* vec, void* value)
	{
		vec->buffer_index--;
		vec->buffer_offset -= vec->value_size;
		if (value)
		{
			memcpy(value, vec->buffer + vec->buffer_offset, vec->value_size);
		}
	}
	void vec_resize(vec_t* vec, uint64_t count)
	{
		if (count > vec->buffer_count)
		{
			vec->buffer = (uint8_t*)HEAP_REALLOC(vec->buffer, count * vec->value_size);
			vec->buffer_size = count * vec->value_size;
			vec->buffer_count = count;
			vec->buffer_index = count;
		}
		else if (count < vec->buffer_count)
		{
			vec->buffer = (uint8_t*)HEAP_REALLOC(vec->buffer, count * vec->value_size);
			vec->buffer_size = count * vec->value_size;
			vec->buffer_count = count;
			vec->buffer_index = MIN(vec->buffer_index, count);
			vec->buffer_offset = MIN(vec->buffer_index, count) * vec->value_size;
		}
	}
	void vec_swap(vec_t* vec, uint64_t left_index, uint64_t right_index)
	{
		assert(left_index < vec->buffer_index);
		assert(right_index < vec->buffer_index);
		uint8_t* left = vec->buffer + left_index * vec->value_size;
		uint8_t* right = vec->buffer + right_index * vec->value_size;
		memcpy(vec->swap_buffer, left, vec->value_size);
		memcpy(left, right, vec->value_size);
		memcpy(right, vec->swap_buffer, vec->value_size);
	}
	void vec_clear(vec_t* vec)
	{
		vec->buffer_index = 0;
		vec->buffer_offset = 0;
	}
	void* vec_back(vec_t* vec)
	{
		return vec->buffer + vec->buffer_offset - vec->value_size;
	}
	void* vec_front(vec_t* vec)
	{
		return vec->buffer;
	}
	void* vec_at(vec_t* vec, uint64_t index)
	{
		assert(index < vec->buffer_count);
		return vec->buffer + index * vec->value_size;
	}
	void const* vec_buffer(vec_t* vec)
	{
		return (void const*)vec->buffer;
	}
	void vec_set(vec_t* vec, uint64_t index, void const* value)
	{
		assert(index < vec->buffer_count);
		if (value)
		{
			memcpy(vec->buffer + index * vec->value_size, value, vec->value_size);
		}
	}
	uint8_t vec_empty(vec_t* vec)
	{
		return vec->buffer_index == 0;
	}
	uint64_t vec_count(vec_t* vec)
	{
		return vec->buffer_index;
	}
	uint64_t vec_size(vec_t* vec)
	{
		return vec->buffer_offset;
	}
	void vec_expand(vec_t* vec)
	{
		uint64_t new_buffer_size = vec->buffer_size * 2;
		uint64_t new_buffer_count = vec->buffer_count * 2;
		vec->buffer = (uint8_t*)HEAP_REALLOC(vec->buffer, new_buffer_size);
		vec->buffer_size = new_buffer_size;
		vec->buffer_count = new_buffer_count;
	}
	void vec_free(vec_t* vec)
	{
		HEAP_FREE(vec->buffer);
		HEAP_FREE(vec->swap_buffer);
		memset(vec, 0, sizeof(vec_t));
	}
#endif // VEC_IMPLEMENTATION

#endif // VEC_H
