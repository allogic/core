#ifndef CORE_MAP_H
#define CORE_MAP_H

#include <stdint.h>

#include "core_config.h"
#include "core_macros.h"

typedef struct _map_pair_t
{
	struct _map_pair_t* next;
	uint8_t* key;
	uint64_t key_size;
	uint8_t* value;
	uint64_t value_size;
} map_pair_t;

typedef struct _map_t
{
	map_pair_t** table;
	uint64_t table_size;
	uint64_t table_count;
	uint64_t pair_count;
} map_t;

typedef void (*map_print_func_t)(void* key, uint64_t key_size, void* value, uint64_t value_size);

extern void* map_pair_key(map_pair_t* pair);
extern uint64_t map_pair_key_size(map_pair_t* pair);
extern void* map_pair_value(map_pair_t* pair);
extern uint64_t map_pair_value_size(map_pair_t* pair);
extern map_pair_t* map_pair_next(map_pair_t* pair);

extern map_t map_alloc(void);
extern map_t map_copy(map_t* ref);
extern uint8_t map_equal(map_t* map, map_t* ref);
extern uint8_t map_insert(map_t* map, void const* key, uint64_t key_size, void const* value, uint64_t value_size);
extern uint8_t map_remove(map_t* map, void const* key, uint64_t key_size, void* value, uint64_t value_size);
extern uint8_t map_contains(map_t* map, void const* key, uint64_t key_size);
extern uint64_t map_table_size(map_t* map);
extern uint64_t map_table_count(map_t* map);
extern map_pair_t* map_table_at(map_t* map, uint64_t index);
extern uint64_t map_count(map_t* map);
extern void* map_at(map_t* map, void const* key, uint64_t key_size);
extern void map_expand(map_t* map);
extern void map_clear(map_t* map);
extern uint64_t map_hash(map_t* map, void const* key, uint64_t key_size, uint64_t modulus);
extern uint8_t map_load_factor(map_t* map);
extern void map_print(map_t* map, map_print_func_t print_func);
extern void map_free(map_t* map);

#endif // CORE_MAP_H
