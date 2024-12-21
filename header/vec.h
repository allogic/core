#ifndef VEC_H
#define VEC_H

#ifndef VEC_BUFFER_CAPACITY
	#define VEC_BUFFER_CAPACITY (16)
#endif // VEC_BUFFER_CAPACITY

#ifndef VEC_BUFFER_ALIGNMENT
	#define VEC_BUFFER_ALIGNMENT (16)
#endif // VEC_BUFFER_ALIGNMENT

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

#ifndef ALIGN_DOWN_BY
	#define ALIGN_DOWN_BY(VALUE, ALIGNMENT) (((uint64_t)(VALUE)) & ~(((uint64_t)(ALIGNMENT)) - 1))
#endif // ALIGN_DOWN_BY

#ifndef ALIGN_UP_BY
	#define ALIGN_UP_BY(VALUE, ALIGNMENT) ((((uint64_t)(VALUE)) + (((uint64_t)(ALIGNMENT)) - 1)) & ~(((uint64_t)(ALIGNMENT)) - 1))
#endif // ALIGN_UP_BY

#include <stdio.h>
#include <stdint.h>
#include <string.h>

typedef struct _vec_t
{
	uint8_t* buffer;
	uint8_t* swap_buffer;
	uint64_t value_size;
	uint64_t buffer_capacity;
	uint64_t buffer_size;
	uint64_t buffer_count;
} vec_t;

extern vec_t vec_alloc(uint64_t value_size);
extern vec_t vec_copy(vec_t* ref);
extern uint8_t vec_equal(vec_t* vec, vec_t* ref);
extern void vec_fill(vec_t* vec, void const* value);
extern void vec_push(vec_t* vec, void const* value);
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
extern void vec_print(vec_t* vec);
extern void vec_free(vec_t* vec);

#ifdef VEC_IMPLEMENTATION
	vec_t vec_alloc(uint64_t value_size)
	{
		vec_t vec;
		memset(&vec, 0, sizeof(vec_t));
		vec.buffer = (uint8_t*)HEAP_ALLOC(ALIGN_UP_BY(VEC_BUFFER_CAPACITY, VEC_BUFFER_ALIGNMENT) * value_size);
		vec.swap_buffer = (uint8_t*)HEAP_ALLOC(value_size);
		vec.value_size = value_size;
		vec.buffer_capacity = ALIGN_UP_BY(VEC_BUFFER_CAPACITY, VEC_BUFFER_ALIGNMENT) * value_size;
		vec.buffer_size = 0;
		vec.buffer_count = 0;
		return vec;
	}
	vec_t vec_copy(vec_t* ref)
	{
		vec_t vec;
		memset(&vec, 0, sizeof(vec_t));
		vec.buffer = (uint8_t*)HEAP_ALLOC(ref->buffer_capacity);
		vec.swap_buffer = (uint8_t*)HEAP_ALLOC(ref->value_size);
		vec.value_size = ref->value_size;
		vec.buffer_capacity = ref->buffer_capacity;
		vec.buffer_size = ref->buffer_size;
		vec.buffer_count = ref->buffer_count;
		memcpy(vec.buffer, ref->buffer, ref->buffer_size);
		return vec;
	}
	uint8_t vec_equal(vec_t* vec, vec_t* ref)
	{
		uint8_t not_equal = 0;
		not_equal |= vec->value_size != ref->value_size;
		not_equal |= vec->buffer_size != ref->buffer_size;
		not_equal |= vec->buffer_count != ref->buffer_count;
		not_equal |= memcmp(vec->buffer, ref->buffer, MIN(vec->buffer_size, ref->buffer_size));
		return not_equal == 0;
	}
	void vec_fill(vec_t* vec, void const* value)
	{
		if (value)
		{
			uint64_t buffer_index = 0;
			uint64_t buffer_offset = 0;
			while (buffer_index < vec->buffer_count)
			{
				memcpy(vec->buffer + buffer_offset, value, vec->buffer_size);
				buffer_index++;
				buffer_offset += vec->value_size;
			}
		}
	}
	void vec_push(vec_t* vec, void const* value)
	{
		if (value)
		{
			memcpy(vec->buffer + vec->buffer_size, value, vec->value_size);
		}
		vec->buffer_count++;
		vec->buffer_size += vec->value_size;
		if (vec->buffer_size >= vec->buffer_capacity)
		{
			vec_expand(vec);
		}
	}
	void vec_pop(vec_t* vec, void* value)
	{
		vec->buffer_count--;
		vec->buffer_size -= vec->value_size;
		if (value)
		{
			memcpy(value, vec->buffer + vec->buffer_size, vec->value_size);
		}
	}
	void vec_resize(vec_t* vec, uint64_t count)
	{
		uint64_t buffer_count = count;
		uint64_t buffer_size = count * vec->value_size;
		uint64_t buffer_capacity = 0;
		if (count)
		{
			buffer_capacity = ALIGN_UP_BY(count, VEC_BUFFER_ALIGNMENT) * vec->value_size * 2;
		}
		else
		{
			buffer_capacity = VEC_BUFFER_ALIGNMENT * vec->value_size * 2;
		}
		vec->buffer = (uint8_t*)HEAP_REALLOC(vec->buffer, buffer_capacity);
		vec->buffer_capacity = buffer_capacity;
		vec->buffer_size = buffer_size;
		vec->buffer_count = buffer_count;
	}
	void vec_expand(vec_t* vec)
	{
		uint64_t buffer_capacity = ALIGN_UP_BY(vec->buffer_count, VEC_BUFFER_ALIGNMENT) * vec->value_size * 2;
		vec->buffer = (uint8_t*)HEAP_REALLOC(vec->buffer, buffer_capacity);
		vec->buffer_capacity = buffer_capacity;
	}
	void vec_remove(vec_t* vec, void const* value)
	{
		if (value)
		{
			uint64_t buffer_index = 0;
			uint64_t buffer_offset = 0;
			while (buffer_index < vec->buffer_count)
			{
				if (memcmp(vec->buffer + buffer_offset, value, vec->value_size) == 0)
				{
					break;
				}
				buffer_index++;
				buffer_offset += vec->value_size;
			}
			vec_swap(vec, buffer_index, vec->buffer_count - 1);
			vec->buffer_count--;
			vec->buffer_size -= vec->value_size;
		}
	}
	void vec_swap(vec_t* vec, uint64_t left_index, uint64_t right_index)
	{
		uint8_t* left = vec->buffer + left_index * vec->value_size;
		uint8_t* right = vec->buffer + right_index * vec->value_size;
		memcpy(vec->swap_buffer, left, vec->value_size);
		memcpy(left, right, vec->value_size);
		memcpy(right, vec->swap_buffer, vec->value_size);
	}
	void vec_clear(vec_t* vec)
	{
		vec->buffer_size = 0;
		vec->buffer_count = 0;
	}
	void* vec_back(vec_t* vec)
	{
		return vec->buffer + vec->buffer_size - vec->value_size;
	}
	void* vec_front(vec_t* vec)
	{
		return vec->buffer;
	}
	void* vec_at(vec_t* vec, uint64_t index)
	{
		return vec->buffer + index * vec->value_size;
	}
	void* vec_buffer(vec_t* vec)
	{
		return vec->buffer;
	}
	void vec_set(vec_t* vec, uint64_t index, void const* value)
	{
		if (value)
		{
			memcpy(vec->buffer + index * vec->value_size, value, vec->value_size);
		}
	}
	uint8_t vec_empty(vec_t* vec)
	{
		return vec->buffer_count == 0;
	}
	uint64_t vec_size(vec_t* vec)
	{
		return vec->buffer_size;
	}
	uint64_t vec_count(vec_t* vec)
	{
		return vec->buffer_count;
	}
	uint64_t vec_capacity(vec_t* vec)
	{
		return vec->buffer_capacity;
	}
	void vec_print(vec_t* vec)
	{
		printf("vector:\n");
		printf("\tvalue_size: %zu\n", vec->value_size);
		printf("\tbuffer_capacity: %zu\n", vec->buffer_capacity);
		printf("\tbuffer_size: %zu\n", vec->buffer_size);
		printf("\tbuffer_count: %zu\n", vec->buffer_count);
		printf("\tbuffer:\n");
		uint64_t buffer_index = 0;
		while (buffer_index < vec->buffer_count)
		{
			printf("\t\t%zu -> %x\n", buffer_index, *(uint8_t*)vec_at(vec, buffer_index));
			buffer_index++;
		}
	}
	void vec_free(vec_t* vec)
	{
		HEAP_FREE(vec->buffer);
		HEAP_FREE(vec->swap_buffer);
		memset(vec, 0, sizeof(vec_t));
	}
#endif // VEC_IMPLEMENTATION

#endif // VEC_H
