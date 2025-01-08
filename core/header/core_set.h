#ifndef CORE_SET_H
#define CORE_SET_H

#include <stdint.h>

#include "core_config.h"
#include "core_macros.h"

typedef struct _set_entry_t
{
	struct _set_entry_t* next;
	uint8_t* key;
	uint64_t key_size;
} set_entry_t;

typedef struct _set_t
{
	set_entry_t** table;
	uint64_t table_size;
	uint64_t table_count;
	uint64_t entry_count;
} set_t;

typedef void (*set_print_func_t)(void* key, uint64_t key_size);

extern void* set_entry_key(set_entry_t* entry);
extern uint64_t set_entry_key_size(set_entry_t* entry);
extern set_entry_t* set_entry_next(set_entry_t* entry);

extern set_t set_alloc(void);
extern set_t set_copy(set_t* ref);
extern uint8_t set_equal(set_t* set, set_t* ref);
extern uint8_t set_insert(set_t* set, void const* key, uint64_t key_size);
extern uint8_t set_remove(set_t* set, void const* key, uint64_t key_size);
extern uint8_t set_contains(set_t* set, void const* key, uint64_t key_size);
extern uint64_t set_table_size(set_t* set);
extern uint64_t set_table_count(set_t* set);
extern set_entry_t* set_table_at(set_t* set, uint64_t index);
extern uint64_t set_count(set_t* set);
extern void set_expand(set_t* set);
extern void set_clear(set_t* set);
extern uint64_t set_hash(set_t* set, void const* key, uint64_t key_size, uint64_t modulus);
extern uint8_t set_load_factor(set_t* set);
extern void set_print(set_t* set, set_print_func_t print_func);
extern void set_free(set_t* set);

#endif // CORE_SET_H
