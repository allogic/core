#ifndef STR_H
#define STR_H

#ifndef STR_BUFFER_COUNT
	#define STR_BUFFER_COUNT (16)
#endif // STR_BUFFER_COUNT

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

typedef struct _str_t
{
	char* buffer;
	uint64_t buffer_size;
	uint64_t buffer_length;
} str_t;

extern str_t str_alloc(void);
extern str_t str_from(char const* value);
extern str_t str_copy(str_t* reference);
extern void str_append(str_t* str, char const* value);
extern void str_resize(str_t* str, uint64_t size);
extern void str_clear(str_t* str);
extern char const str_at(str_t* str, uint64_t index);
extern char const* str_buffer(str_t* str);
extern uint8_t str_empty(str_t* str);
extern uint64_t str_length(str_t* str);
extern void str_expand(str_t* str);
extern void str_free(str_t* str);

#ifdef STR_IMPLEMENTATION
	str_t str_alloc(void)
	{
		str_t str;
		memset(&str, 0, sizeof(str_t));
		str.buffer = (char*)HEAP_ALLOC(STR_BUFFER_COUNT);
		str.buffer_size = STR_BUFFER_COUNT;
		str.buffer_length = 0;
		return str;
	}
	str_t str_from(char const* value)
	{
		str_t str = str_alloc();
		str_append(&str, value);
		return str;
	}
	str_t str_copy(str_t* reference)
	{
		str_t str;
		memset(&str, 0, sizeof(str_t));
		str.buffer = (char*)HEAP_ALLOC(reference->buffer_size);
		str.buffer_size = reference->buffer_size;
		str.buffer_length = reference->buffer_length;
		memcpy(str.buffer, reference->buffer, reference->buffer_size);
		return str;
	}
	void str_append(str_t* str, char const* value)
	{
		uint64_t value_length = strlen(value);
		while ((str->buffer_length + value_length) >= str->buffer_size)
		{
			str_expand(str);
		}
		memcpy(str->buffer, value, value_length);
		str->buffer_length += value_length;
	}
	void str_resize(str_t* str, uint64_t size)
	{
		if (size > str->buffer_size)
		{
			str->buffer = (char*)HEAP_REALLOC(str->buffer, size);
			str->buffer_size = size;
		}
		else if (size < str->buffer_size)
		{
			str->buffer = (char*)HEAP_REALLOC(str->buffer, size);
			str->buffer_size = size;
			str->buffer_length = MIN(str->buffer_length, size);
			str->buffer[str->buffer_length - 1] = 0;
		}
	}
	void str_clear(str_t* str)
	{
		str->buffer[0] = 0;
		str->buffer_length = 0;
	}
	char const str_at(str_t* str, uint64_t index)
	{
		return str->buffer[index];
	}
	char const* str_buffer(str_t* str)
	{
		return (char const*)str->buffer;
	}
	uint8_t str_empty(str_t* str)
	{
		return str->buffer_length == 0;
	}
	uint64_t str_length(str_t* str)
	{
		return str->buffer_length;
	}
	void str_expand(str_t* str)
	{
		uint64_t new_buffer_size = str->buffer_size * 2;
		str->buffer = (char*)HEAP_REALLOC(str->buffer, new_buffer_size);
		str->buffer_size = new_buffer_size;
	}
	void str_free(str_t* str)
	{
		HEAP_FREE(str->buffer);
		memset(str, 0, sizeof(str_t));
	}
#endif // STR_IMPLEMENTATION

#endif // STR_H
