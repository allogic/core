#ifndef STR_H
#define STR_H

#ifndef STR_BUFFER_CAPACITY
	#define STR_BUFFER_CAPACITY (16)
#endif // STR_BUFFER_CAPACITY

#ifndef STR_BUFFER_ALIGNMENT
	#define STR_BUFFER_ALIGNMENT (16)
#endif // STR_BUFFER_ALIGNMENT

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

typedef struct _str_t
{
	char* buffer;
	uint64_t buffer_capacity;
	uint64_t buffer_size;
} str_t;

extern str_t str_alloc(void);
extern str_t str_from(char const* value);
extern str_t str_copy(str_t* ref);
extern void str_fill(str_t* str, char value);
extern uint8_t str_equal(str_t* str, str_t* ref);
extern void str_append(str_t* str, char const* value);
extern void str_resize(str_t* str, uint64_t size);
extern void str_expand(str_t* str);
extern void str_clear(str_t* str);
extern char str_at(str_t* str, uint64_t index);
extern char* str_buffer(str_t* str);
extern void str_set(str_t* str, char const* value);
extern uint8_t str_empty(str_t* str);
extern uint64_t str_size(str_t* str);
extern uint64_t str_capacity(str_t* str);
extern void str_print(str_t* str);
extern void str_free(str_t* str);

#ifdef STR_IMPLEMENTATION
	str_t str_alloc(void)
	{
		str_t str;
		memset(&str, 0, sizeof(str_t));
		str.buffer = (char*)HEAP_ALLOC(ALIGN_UP_BY(STR_BUFFER_CAPACITY, STR_BUFFER_ALIGNMENT));
		str.buffer_capacity = ALIGN_UP_BY(STR_BUFFER_CAPACITY, STR_BUFFER_ALIGNMENT);
		str.buffer_size = 0;
		str.buffer[0] = 0;
		return str;
	}
	str_t str_from(char const* value)
	{
		str_t str = str_alloc();
		if (value)
		{
			str_append(&str, value);
		}
		return str;
	}
	str_t str_copy(str_t* ref)
	{
		str_t str;
		memset(&str, 0, sizeof(str_t));
		str.buffer = (char*)HEAP_ALLOC(ref->buffer_capacity);
		str.buffer_capacity = ref->buffer_capacity;
		str.buffer_size = ref->buffer_size;
		memcpy(str.buffer, ref->buffer, ref->buffer_size);
		return str;
	}
	uint8_t str_equal(str_t* str, str_t* ref)
	{
		uint8_t not_equal = 0;
		not_equal |= str->buffer_size != ref->buffer_size;
		not_equal |= memcmp(str->buffer, ref->buffer, MIN(str->buffer_size, str->buffer_size));
		return not_equal == 0;
	}
	void str_fill(str_t* str, char value)
	{
		if (value)
		{
			memset(str->buffer, value, str->buffer_size);
			str->buffer[str->buffer_size] = 0;
		}
	}
	void str_append(str_t* str, char const* value)
	{
		if (value)
		{
			uint64_t value_size = strlen(value);
			str->buffer_size += value_size;
			while (str->buffer_size >= str->buffer_capacity)
			{
				str_expand(str);
			}
			memcpy(str->buffer + str->buffer_size - value_size, value, value_size);
			str->buffer[str->buffer_size] = 0;
		}
	}
	void str_resize(str_t* str, uint64_t size)
	{
		uint64_t buffer_size = size;
		uint64_t buffer_capacity = 0;
		if (size)
		{
			buffer_capacity = ALIGN_UP_BY(size + 1, STR_BUFFER_ALIGNMENT);
		}
		else
		{
			buffer_capacity = ALIGN_UP_BY(STR_BUFFER_CAPACITY, STR_BUFFER_ALIGNMENT);
		}
		str->buffer = (char*)HEAP_REALLOC(str->buffer, buffer_capacity);
		str->buffer_capacity = buffer_capacity;
		str->buffer_size = buffer_size;
		str->buffer[str->buffer_size] = 0;
	}
	void str_expand(str_t* str)
	{
		uint64_t buffer_capacity = ALIGN_UP_BY(str->buffer_size + 1, STR_BUFFER_ALIGNMENT);
		str->buffer = (char*)HEAP_REALLOC(str->buffer, buffer_capacity);
		str->buffer_capacity = buffer_capacity;
	}
	void str_clear(str_t* str)
	{
		str->buffer_size = 0;
		str->buffer[str->buffer_size] = 0;
	}
	char str_at(str_t* str, uint64_t index)
	{
		return str->buffer[index];
	}
	char* str_buffer(str_t* str)
	{
		return str->buffer;
	}
	void str_set(str_t* str, char const* value)
	{
		if (value)
		{
			uint64_t value_size = strlen(value);
			str->buffer_size = value_size;
			while (str->buffer_size >= str->buffer_capacity)
			{
				str_expand(str);
			}
			memcpy(str->buffer + str->buffer_size - value_size, value, value_size);
			str->buffer[str->buffer_size] = 0;
		}
	}
	uint8_t str_empty(str_t* str)
	{
		return str->buffer_size == 0;
	}
	uint64_t str_size(str_t* str)
	{
		return str->buffer_size;
	}
	uint64_t str_capacity(str_t* str)
	{
		return str->buffer_capacity;
	}
	void str_print(str_t* str)
	{
		printf("string:\n");
		printf("\tbuffer_capacity: %zu\n", str->buffer_capacity);
		printf("\tbuffer_size: %zu\n", str->buffer_size);
		printf("\tbuffer: %s\n", str->buffer);
	}
	void str_free(str_t* str)
	{
		HEAP_FREE(str->buffer);
		memset(str, 0, sizeof(str_t));
	}
#endif // STR_IMPLEMENTATION

#endif // STR_H
