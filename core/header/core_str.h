#ifndef CORE_STR_H
#define CORE_STR_H

#include <stdint.h>

#include "core_config.h"
#include "core_macros.h"

typedef struct _str_t
{
	char* buffer;
	uint64_t buffer_capacity;
	uint64_t buffer_size;
} str_t;

extern str_t str_alloc(void);
extern str_t str_from(char const* value, uint64_t size);
extern str_t str_copy(str_t* ref);
extern void str_fill(str_t* str, char value);
extern uint8_t str_equal(str_t* str, str_t* ref);
extern void str_append(str_t* str, char const* value, uint64_t size);
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

#endif // CORE_STR_H
