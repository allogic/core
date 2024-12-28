#include <stdio.h>
#include <string.h>

#include "set.h"

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

#ifndef SET_MAX
	#define SET_MAX(A, B) (((A) > (B)) ? (A) : (B))
#endif // SET_MAX

#ifndef SET_MIN
	#define SET_MIN(A, B) (((A) < (B)) ? (A) : (B))
#endif // SET_MIN

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
set_t set_copy(set_t* ref)
{
	set_t set;
	memset(&set, 0, sizeof(set_t));
	set.table = (set_pair_t**)HEAP_ALLOC(ref->table_size);
	set.table_size = ref->table_size;
	set.table_count = ref->table_count;
	set.pair_count = ref->pair_count;
	memset(set.table, 0, ref->table_size);
	uint64_t table_index = 0;
	while (table_index < set.table_count)
	{
		set_pair_t* ref_curr = ref->table[table_index];
		set_pair_t* curr = set.table[table_index];
		if (ref_curr)
		{
			uint64_t hash = set_hash(&set, ref_curr->key, ref_curr->key_size, set.table_count);
			curr = (set_pair_t*)HEAP_ALLOC(sizeof(set_pair_t));
			memset(curr, 0, sizeof(set_pair_t));
			curr->next = set.table[hash];
			curr->key = (uint8_t*)HEAP_ALLOC(ref_curr->key_size);
			curr->key_size = ref_curr->key_size;
			memcpy(curr->key, ref_curr->key, ref_curr->key_size);
			set.table[hash] = curr;
		}
		table_index++;
	}
	return set;
}
uint8_t set_equal(set_t* set, set_t* ref)
{
	uint8_t not_equal = 0;
	not_equal |= set->pair_count != ref->pair_count;
	if (set->pair_count == ref->pair_count)
	{
		uint64_t table_index = 0;
		while (table_index < set->table_count)
		{
			set_pair_t* ref_curr = ref->table[table_index];
			set_pair_t* curr = set->table[table_index];
			not_equal |= (ref_curr == 0) != (curr == 0);
			while (curr && ref_curr)
			{
				not_equal |= curr->key_size != ref_curr->key_size;
				if (not_equal == 0)
				{
					not_equal |= memcmp(curr->key, ref_curr->key, SET_MIN(curr->key_size, ref_curr->key_size));
				}
				ref_curr = ref_curr->next;
				curr = curr->next;
			}
			table_index++;
		}
	}
	return not_equal == 0;
}
uint8_t set_insert(set_t* set, void const* key, uint64_t key_size)
{
	uint8_t key_exists = 0;
	float_t load_factor = set_load_factor(set);
	if (load_factor > SET_LOAD_FACTOR)
	{
		set_expand(set);
	}
	uint64_t hash = set_hash(set, key, key_size, set->table_count);
	set_pair_t* curr = set->table[hash];
	while (curr)
	{
		if (memcmp(curr->key, key, SET_MIN(curr->key_size, key_size)) == 0)
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
	uint64_t hash = set_hash(set, key, key_size, set->table_count);
	set_pair_t* curr = set->table[hash];
	set_pair_t* prev = 0;
	while (curr)
	{
		if (memcmp(curr->key, key, SET_MIN(curr->key_size, key_size)) == 0)
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
	uint64_t hash = set_hash(set, key, key_size, set->table_count);
	set_pair_t* curr = set->table[hash];
	while (curr)
	{
		if (memcmp(curr->key, key, SET_MIN(curr->key_size, key_size)) == 0)
		{
			return 1;
		}
		curr = curr->next;
	}
	return 0;
}
uint64_t set_table_size(set_t* set)
{
	return set->table_size;
}
uint64_t set_table_count(set_t* set)
{
	return set->table_count;
}
uint64_t set_count(set_t* set)
{
	return set->pair_count;
}
void set_expand(set_t* set)
{
	uint64_t table_index = 0;
	uint64_t table_size = set->table_size * 2;
	uint64_t table_count = set->table_count * 2;
	set_pair_t** table = (set_pair_t**)HEAP_ALLOC(table_size);
	memset(table, 0, table_size);
	while (table_index < set->table_count)
	{
		set_pair_t* curr = set->table[table_index];
		while (curr)
		{
			uint64_t hash = set_hash(set, curr->key, curr->key_size, table_count);
			curr->next = table[hash];
			table[hash] = curr;
			curr = curr->next;
		}
		table_index++;
	}
	HEAP_FREE(set->table);
	set->table = table;
	set->table_size = table_size;
	set->table_count = table_count;
}
void set_clear(set_t* set)
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
	memset(set->table, 0, set->table_size);
	set->pair_count = 0;
}
uint64_t set_hash(set_t* set, void const* key, uint64_t key_size, uint64_t modulus)
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
float_t set_load_factor(set_t* set)
{
	return ((float_t)(set->pair_count + 1)) / (float_t)set->table_count;
}
void set_print(set_t* set)
{
	printf("set:\n");
	printf("\ttable_size: %zu\n", set->table_size);
	printf("\ttable_count: %zu\n", set->table_count);
	printf("\tpair_count: %zu\n", set->pair_count);
	printf("\ttable:\n");
	uint64_t table_index = 0;
	while (table_index < set->table_count)
	{
		set_pair_t* curr = set->table[table_index];
		while (curr)
		{
			printf("\t\t%s\n", (char const*)curr->key);
			curr = curr->next;
		}
		table_index++;
	}
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
