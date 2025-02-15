#ifndef CORE_DAT_H
#define CORE_DAT_H

#include <stdint.h>

#include "core_config.h"
#include "core_macros.h"
#include "core_vec.h"

typedef struct _page_t
{
	uint64_t* sparse;
	uint64_t sparse_size;
	uint64_t sparse_count;
} page_t;

typedef struct _dat_t
{
	page_t* pages;
	uint64_t page_size;
	uint64_t page_count;
	vec_t values;
	vec_t ids;
} dat_t;

extern dat_t dat_alloc(uint64_t value_size);
extern dat_t dat_copy(dat_t* ref);
extern uint8_t dat_equal(dat_t* ref);
extern void dat_set_dense_index(dat_t* dat, uint64_t id, uint64_t index);
extern uint64_t dat_get_dense_index(dat_t* dat, uint64_t id);
extern void dat_pages_resize(dat_t* dat, uint64_t page_count);
extern void dat_sparse_resize(dat_t* dat, page_t* page, uint64_t sparse_count);
extern void dat_set(dat_t* dat, uint64_t id, void const* value);
extern void* dat_get(dat_t* dat, uint64_t id);
extern void dat_remove(dat_t* dat, uint64_t id);
extern uint8_t dat_contains(dat_t* dat, uint64_t id);
extern uint8_t dat_empty(dat_t* dat);
extern void dat_clear(dat_t* dat);
extern uint64_t dat_count(dat_t* dat);
extern uint64_t dat_id(dat_t* dat, uint64_t index);
extern void* dat_value(dat_t* dat, uint64_t index);
extern void dat_print(dat_t* dat);
extern void dat_free(dat_t* dat);

#endif // CORE_DAT_H
