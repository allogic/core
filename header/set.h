#ifndef SET_H
#define SET_H

#ifndef SET_TABLE_COUNT
	#define SET_TABLE_COUNT (128)
#endif // SET_TABLE_COUNT

#ifndef SET_HASH_VALUE
	#define SET_HASH_VALUE (5381)
#endif // SET_HASH_VALUE

#ifndef SET_LOAD_FACTOR
	#define SET_LOAD_FACTOR (0.75F)
#endif // SET_LOAD_FACTOR

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
#include <math.h>
#include <assert.h>

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
extern set_t set_copy(set_t* reference);
extern uint8_t set_insert(set_t* set, void const* key, uint64_t key_size);
extern uint8_t set_remove(set_t* set, void const* key, uint64_t key_size);
extern uint8_t set_contains(set_t* set, void const* key, uint64_t key_size);
extern uint64_t set_count(set_t* set);
extern void set_expand(set_t* set);
extern uint64_t set_hash(void const* key, uint64_t key_size, uint64_t modulus);
extern void set_free(set_t* set);

#ifdef SET_IMPLEMENTATION
	set_t set_alloc(void)
	{
		set_t set;
		memset(&set, 0, sizeof(set_t));
		set.table = (set_pair_t**)HEAP_ALLOC(SET_TABLE_COUNT * sizeof(set_pair_t*));
		set.table_size = SET_TABLE_COUNT * sizeof(set_pair_t*);
		set.table_count = SET_TABLE_COUNT;
		set.pair_count = 0;
		memset(set.table, 0, SET_TABLE_COUNT * sizeof(set_pair_t*));
		return set;
	}
	set_t set_copy(set_t* reference)
	{
		assert(reference->table);
		set_t set;
		memset(&set, 0, sizeof(set_t));
		set.table = (set_pair_t**)HEAP_ALLOC(reference->table_size);
		set.table_size = reference->table_size;
		set.table_count = reference->table_count;
		set.pair_count = reference->pair_count;
		memset(set.table, 0, reference->table_size);
		uint64_t table_index = 0;
		while (table_index < set.table_count)
		{
			set_pair_t* ref_curr = reference->table[table_index];
			set_pair_t* new_curr = set.table[table_index];
			if (ref_curr)
			{
				uint64_t hash = set_hash(ref_curr->key, ref_curr->key_size, set.table_count);
				new_curr = (set_pair_t*)HEAP_ALLOC(sizeof(set_pair_t));
				memset(new_curr, 0, sizeof(set_pair_t));
				new_curr->next = set.table[hash];
				new_curr->key = (uint8_t*)HEAP_ALLOC(ref_curr->key_size);
				new_curr->key_size = ref_curr->key_size;
				memcpy(new_curr->key, ref_curr->key, ref_curr->key_size);
			}
			table_index++;
		}
		return set;
	}
	uint8_t set_insert(set_t* set, void const* key, uint64_t key_size)
	{
		uint8_t key_exists = 0;
		if ((((float_t)(set->pair_count + 1)) / (float_t)set->table_count) > SET_LOAD_FACTOR)
		{
			set_expand(set);
		}
		uint64_t hash = set_hash(key, key_size, set->table_count);
		set_pair_t* curr = set->table[hash];
		while (curr)
		{
			if (memcmp(curr->key, key, MIN(curr->key_size, key_size)) == 0)
			{
				key_exists = 1;
				break;
			}
			curr = curr->next;
		}
		if (key_exists == 0)
		{
			curr = (set_pair_t*)HEAP_ALLOC(sizeof(set_pair_t));
			memset(curr, 0, sizeof(set_pair_t));
			curr->next = set->table[hash];
			curr->key = (uint8_t*)HEAP_ALLOC(key_size);
			curr->key_size = key_size;
			memcpy(curr->key, key, key_size);
			set->table[hash] = curr;
			set->pair_count++;
		}
		return key_exists;
	}
	uint8_t set_remove(set_t* set, void const* key, uint64_t key_size)
	{
		uint64_t hash = set_hash(key, key_size, set->table_count);
		set_pair_t* curr = set->table[hash];
		set_pair_t* prev = 0;
		while (curr)
		{
			if (memcmp(curr->key, key, MIN(curr->key_size, key_size)) == 0)
			{
				if (prev)
				{
					prev->next = curr->next;
				}
				else
				{
					set->table[hash] = curr->next;
				}
				HEAP_FREE(curr->key);
				HEAP_FREE(curr);
				set->pair_count--;
				return 1;
			}
			prev = curr;
			curr = curr->next;
		}
		return 0;
	}
	uint8_t set_contains(set_t* set, void const* key, uint64_t key_size)
	{
		uint64_t hash = set_hash(key, key_size, set->table_count);
		set_pair_t* curr = set->table[hash];
		while (curr)
		{
			if (memcmp(curr->key, key, MIN(curr->key_size, key_size)) == 0)
			{
				return 1;
			}
			curr = curr->next;
		}
		return 0;
	}
	uint64_t set_count(set_t* set)
	{
		return set->table_count;
	}
	void set_expand(set_t* set)
	{
		uint64_t table_index = 0;
		uint64_t new_table_size = set->table_size * 2;
		uint64_t new_table_count = set->table_count * 2;
		set_pair_t** new_table = (set_pair_t**)HEAP_ALLOC(new_table_size);
		memset(new_table, 0, new_table_size);
		while (table_index < set->table_count)
		{
			set_pair_t* curr = set->table[table_index];
			while (curr)
			{
				uint64_t hash = set_hash(curr->key, curr->key_size, new_table_count);
				curr->next = new_table[hash];
				new_table[hash] = curr;
				curr = curr->next;
			}
			table_index++;
		}
		HEAP_FREE(set->table);
		set->table = new_table;
		set->table_size = new_table_size;
		set->table_count = new_table_count;
	}
	uint64_t set_hash(void const* key, uint64_t key_size, uint64_t modulus)
	{
		uint64_t hash = SET_HASH_VALUE;
		uint64_t key_index = 0;
		while (key_index < key_size)
		{
			hash = ((hash << 5) + hash) + *(((uint8_t*)key) + key_index);
			key_index++;
		}
		return hash % modulus;
	}
	void set_free(set_t* set)
	{
		uint64_t table_index = 0;
		while (table_index < set->table_count)
		{
			set_pair_t* curr = set->table[table_index];
			while (curr)
			{
				set_pair_t* tmp = curr;
				curr = curr->next;
				HEAP_FREE(tmp->key);
				HEAP_FREE(tmp);
			}
			table_index++;
		}
		HEAP_FREE(set->table);
		memset(set, 0, sizeof(set_t));
	}
#endif // SET_IMPLEMENTATION

#endif // SET_H
