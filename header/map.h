#ifndef MAP_H
#define MAP_H

#ifndef MAP_TABLE_COUNT
	#define MAP_TABLE_COUNT (128)
#endif // MAP_TABLE_COUNT

#ifndef MAP_HASH_VALUE
	#define MAP_HASH_VALUE (5381)
#endif // MAP_HASH_VALUE

#ifndef MAP_LOAD_FACTOR
	#define MAP_LOAD_FACTOR (0.75F)
#endif // MAP_LOAD_FACTOR

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

typedef struct _map_pair_t
{
	struct _map_pair_t* next;
	uint8_t* key;
	uint64_t key_size;
	uint8_t* value;
} map_pair_t;

typedef struct _map_t
{
	map_pair_t** table;
	uint64_t table_size;
	uint64_t table_count;
	uint64_t pair_count;
	uint64_t value_size;
} map_t;

extern map_t map_alloc(uint64_t value_size);
extern map_t map_copy(map_t* reference);
extern uint8_t map_insert(map_t* map, void const* key, uint64_t key_size, void const* value);
extern uint8_t map_remove(map_t* map, void const* key, uint64_t key_size, void* value);
extern uint8_t map_contains(map_t* map, void const* key, uint64_t key_size);
extern uint64_t map_count(map_t* map);
extern void* map_at(map_t* map, void const* key, uint64_t key_size);
extern void map_expand(map_t* map);
extern uint64_t map_hash(void const* key, uint64_t key_size, uint64_t modulus);
extern void map_free(map_t* map);

#ifdef MAP_IMPLEMENTATION
	map_t map_alloc(uint64_t value_size)
	{
		map_t map;
		memset(&map, 0, sizeof(map_t));
		map.table = (map_pair_t**)HEAP_ALLOC(MAP_TABLE_COUNT * sizeof(map_pair_t*));
		map.table_size = MAP_TABLE_COUNT * sizeof(map_pair_t*);
		map.table_count = MAP_TABLE_COUNT;
		map.pair_count = 0;
		map.value_size = value_size;
		memset(map.table, 0, MAP_TABLE_COUNT * sizeof(map_pair_t*));
		return map;
	}
	map_t map_copy(map_t* reference)
	{
		assert(reference->table);
		map_t map;
		memset(&map, 0, sizeof(map_t));
		map.table = (map_pair_t**)HEAP_ALLOC(reference->table_size);
		map.table_size = reference->table_size;
		map.table_count = reference->table_count;
		map.pair_count = reference->pair_count;
		map.value_size = reference->value_size;
		memset(map.table, 0, reference->table_size);
		uint64_t table_index = 0;
		while (table_index < map.table_count)
		{
			map_pair_t* ref_curr = reference->table[table_index];
			map_pair_t* new_curr = map.table[table_index];
			if (ref_curr)
			{
				uint64_t hash = map_hash(ref_curr->key, ref_curr->key_size, map.table_count);
				new_curr = (map_pair_t*)HEAP_ALLOC(sizeof(map_pair_t));
				memset(new_curr, 0, sizeof(map_pair_t));
				new_curr->next = map.table[hash];
				new_curr->key = (uint8_t*)HEAP_ALLOC(ref_curr->key_size);
				new_curr->value = (uint8_t*)HEAP_ALLOC(map.value_size);
				new_curr->key_size = ref_curr->key_size;
				memcpy(new_curr->key, ref_curr->key, ref_curr->key_size);
				memcpy(new_curr->value, ref_curr->value, map.value_size);
			}
			table_index++;
		}
		return map;
	}
	uint8_t map_insert(map_t* map, void const* key, uint64_t key_size, void const* value)
	{
		uint8_t key_exists = 0;
		if ((((float_t)(map->pair_count + 1)) / (float_t)map->table_count) > MAP_LOAD_FACTOR)
		{
			map_expand(map);
		}
		uint64_t hash = map_hash(key, key_size, map->table_count);
		map_pair_t* curr = map->table[hash];
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
			curr = (map_pair_t*)HEAP_ALLOC(sizeof(map_pair_t));
			memset(curr, 0, sizeof(map_pair_t));
			curr->next = map->table[hash];
			curr->key = (uint8_t*)HEAP_ALLOC(key_size);
			curr->value = (uint8_t*)HEAP_ALLOC(map->value_size);
			curr->key_size = key_size;
			memcpy(curr->key, key, key_size);
			memcpy(curr->value, value, map->value_size);
			map->table[hash] = curr;
			map->pair_count++;
		}
		return key_exists;
	}
	uint8_t map_remove(map_t* map, void const* key, uint64_t key_size, void* value)
	{
		uint64_t hash = map_hash(key, key_size, map->table_count);
		map_pair_t* curr = map->table[hash];
		map_pair_t* prev = 0;
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
					map->table[hash] = curr->next;
				}
				memcpy(value, curr->value, map->value_size);
				HEAP_FREE(curr->key);
				HEAP_FREE(curr->value);
				HEAP_FREE(curr);
				map->pair_count--;
				return 1;
			}
			prev = curr;
			curr = curr->next;
		}
		return 0;
	}
	uint8_t map_contains(map_t* map, void const* key, uint64_t key_size)
	{
		uint64_t hash = map_hash(key, key_size, map->table_count);
		map_pair_t* curr = map->table[hash];
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
	uint64_t map_count(map_t* map)
	{
		return map->table_count;
	}
	void* map_at(map_t* map, void const* key, uint64_t key_size)
	{
		uint64_t hash = map_hash(key, key_size, map->table_count);
		map_pair_t* curr = map->table[hash];
		while (curr)
		{
			if (memcmp(curr->key, key, MIN(curr->key_size, key_size)) == 0)
			{
				return curr->value;
			}
			curr = curr->next;
		}
		return 0;
	}
	void map_expand(map_t* map)
	{
		uint64_t table_index = 0;
		uint64_t new_table_size = map->table_size * 2;
		uint64_t new_table_count = map->table_count * 2;
		map_pair_t** new_table = (map_pair_t**)HEAP_ALLOC(new_table_size);
		memset(new_table, 0, new_table_size);
		while (table_index < map->table_count)
		{
			map_pair_t* curr = map->table[table_index];
			while (curr)
			{
				uint64_t hash = map_hash(curr->key, curr->key_size, new_table_count);
				curr->next = new_table[hash];
				new_table[hash] = curr;
				curr = curr->next;
			}
			table_index++;
		}
		HEAP_FREE(map->table);
		map->table = new_table;
		map->table_size = new_table_size;
		map->table_count = new_table_count;
	}
	uint64_t map_hash(void const* key, uint64_t key_size, uint64_t modulus)
	{
		uint64_t hash = MAP_HASH_VALUE;
		uint64_t key_index = 0;
		while (key_index < key_size)
		{
			hash = ((hash << 5) + hash) + *(((uint8_t*)key) + key_index);
			key_index++;
		}
		return hash % modulus;
	}
	void map_free(map_t* map)
	{
		uint64_t table_index = 0;
		while (table_index < map->table_count)
		{
			map_pair_t* curr = map->table[table_index];
			while (curr)
			{
				map_pair_t* tmp = curr;
				curr = curr->next;
				HEAP_FREE(tmp->key);
				HEAP_FREE(tmp->value);
				HEAP_FREE(tmp);
			}
			table_index++;
		}
		HEAP_FREE(map->table);
		memset(map, 0, sizeof(map_t));
	}
#endif // MAP_IMPLEMENTATION

#endif // MAP_H
