#ifndef SET_H
#define SET_H

#include <stdint.h>
#include <math.h>

#ifndef SET_TABLE_COUNT
	#define SET_TABLE_COUNT (128)
#endif // SET_TABLE_COUNT

#ifndef SET_HASH_VALUE
	#define SET_HASH_VALUE (5381)
#endif // SET_HASH_VALUE

#ifndef SET_LOAD_FACTOR
	#define SET_LOAD_FACTOR (0.75F)
#endif // SET_LOAD_FACTOR

typedef struct _set_pair_t
{
	struct _set_pair_t* next;
	uint8_t* key;
	uint64_t key_size;
} set_pair_t;

typedef struct _set_t
{
	set_pair_t** table;
	uint64_t table_size;
	uint64_t table_count;
	uint64_t pair_count;
} set_t;

extern set_t set_alloc(void);
extern set_t set_copy(set_t* ref);
extern uint8_t set_equal(set_t* set, set_t* ref);
extern uint8_t set_insert(set_t* set, void const* key, uint64_t key_size);
extern uint8_t set_remove(set_t* set, void const* key, uint64_t key_size);
extern uint8_t set_contains(set_t* set, void const* key, uint64_t key_size);
extern uint64_t set_table_size(set_t* set);
extern uint64_t set_table_count(set_t* set);
extern uint64_t set_count(set_t* set);
extern void set_expand(set_t* set);
extern void set_clear(set_t* set);
extern uint64_t set_hash(set_t* set, void const* key, uint64_t key_size, uint64_t modulus);
extern float_t set_load_factor(set_t* set);
extern void set_print(set_t* set);
extern void set_free(set_t* set);

#endif // SET_H
