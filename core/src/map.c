#include <stdio.h>
#include <string.h>
#include <math.h>

#include "map.h"

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

#ifndef MAP_MAX
	#define MAP_MAX(A, B) (((A) > (B)) ? (A) : (B))
#endif // MAP_MAX

#ifndef MAP_MIN
	#define MAP_MIN(A, B) (((A) < (B)) ? (A) : (B))
#endif // MAP_MIN

map_t map_alloc(void)
{
	map_t map;
	memset(&map, 0, sizeof(map_t));
	map.table = (map_pair_t**)HEAP_ALLOC(MAP_TABLE_COUNT * sizeof(map_pair_t*));
	map.table_size = MAP_TABLE_COUNT * sizeof(map_pair_t*);
	map.table_count = MAP_TABLE_COUNT;
	map.pair_count = 0;
	memset(map.table, 0, MAP_TABLE_COUNT * sizeof(map_pair_t*));
	return map;
}
map_t map_copy(map_t* ref)
{
	map_t map;
	memset(&map, 0, sizeof(map_t));
	map.table = (map_pair_t**)HEAP_ALLOC(ref->table_size);
	map.table_size = ref->table_size;
	map.table_count = ref->table_count;
	map.pair_count = ref->pair_count;
	memset(map.table, 0, ref->table_size);
	uint64_t table_index = 0;
	while (table_index < map.table_count)
	{
		map_pair_t* ref_curr = ref->table[table_index];
		map_pair_t* curr = map.table[table_index];
		if (ref_curr)
		{
			uint64_t hash = map_hash(&map, ref_curr->key, ref_curr->key_size, map.table_count);
			curr = (map_pair_t*)HEAP_ALLOC(sizeof(map_pair_t));
			memset(curr, 0, sizeof(map_pair_t));
			curr->next = map.table[hash];
			curr->key = (uint8_t*)HEAP_ALLOC(ref_curr->key_size);
			curr->value = (uint8_t*)HEAP_ALLOC(ref_curr->value_size);
			curr->key_size = ref_curr->key_size;
			curr->value_size = ref_curr->value_size;
			memcpy(curr->key, ref_curr->key, ref_curr->key_size);
			memcpy(curr->value, ref_curr->value, ref_curr->value_size);
			map.table[hash] = curr;
		}
		table_index++;
	}
	return map;
}
uint8_t map_equal(map_t* map, map_t* ref)
{
	uint8_t not_equal = 0;
	not_equal |= map->pair_count != ref->pair_count;
	if (map->pair_count == ref->pair_count)
	{
		uint64_t table_index = 0;
		while (table_index < map->table_count)
		{
			map_pair_t* ref_curr = ref->table[table_index];
			map_pair_t* curr = map->table[table_index];
			not_equal |= (ref_curr == 0) != (curr == 0);
			while (curr && ref_curr)
			{
				not_equal |= curr->key_size != ref_curr->key_size;
				not_equal |= curr->value_size != ref_curr->value_size;
				if (not_equal == 0)
				{
					not_equal |= memcmp(curr->key, ref_curr->key, MAP_MIN(curr->key_size, ref_curr->key_size));
					not_equal |= memcmp(curr->value, ref_curr->value, MAP_MIN(curr->value_size, ref_curr->value_size));
				}
				ref_curr = ref_curr->next;
				curr = curr->next;
			}
			table_index++;
		}
	}
	return not_equal == 0;
}
uint8_t map_insert(map_t* map, void const* key, uint64_t key_size, void const* value, uint64_t value_size)
{
	uint8_t key_exists = 0;
	float_t load_factor = map_load_factor(map);
	if (load_factor > MAP_LOAD_FACTOR)
	{
		map_expand(map);
	}
	uint64_t hash = map_hash(map, key, key_size, map->table_count);
	map_pair_t* curr = map->table[hash];
	while (curr)
	{
		if (memcmp(curr->key, key, MAP_MIN(curr->key_size, key_size)) == 0)
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
		if (value)
		{
			curr->value = (uint8_t*)HEAP_ALLOC(value_size);
		}
		curr->key_size = key_size;
		memcpy(curr->key, key, key_size);
		if (value)
		{
			memcpy(curr->value, value, value_size);
		}
		curr->value_size = value_size;
		map->table[hash] = curr;
		map->pair_count++;
	}
	return key_exists;
}
uint8_t map_remove(map_t* map, void const* key, uint64_t key_size, void* value, uint64_t value_size)
{
	uint64_t hash = map_hash(map, key, key_size, map->table_count);
	map_pair_t* curr = map->table[hash];
	map_pair_t* prev = 0;
	while (curr)
	{
		if (memcmp(curr->key, key, MAP_MIN(curr->key_size, key_size)) == 0)
		{
			if (prev)
			{
				prev->next = curr->next;
			}
			else
			{
				map->table[hash] = curr->next;
			}
			if (value)
			{
				memcpy(value, curr->value, MAP_MIN(value_size, curr->value_size));
			}
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
	uint64_t hash = map_hash(map, key, key_size, map->table_count);
	map_pair_t* curr = map->table[hash];
	while (curr)
	{
		if (memcmp(curr->key, key, MAP_MIN(curr->key_size, key_size)) == 0)
		{
			return 1;
		}
		curr = curr->next;
	}
	return 0;
}
uint64_t map_table_size(map_t* map)
{
	return map->table_size;
}
uint64_t map_table_count(map_t* map)
{
	return map->table_count;
}
uint64_t map_count(map_t* map)
{
	return map->pair_count;
}
void* map_at(map_t* map, void const* key, uint64_t key_size)
{
	uint64_t hash = map_hash(map, key, key_size, map->table_count);
	map_pair_t* curr = map->table[hash];
	while (curr)
	{
		if (memcmp(curr->key, key, MAP_MIN(curr->key_size, key_size)) == 0)
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
	uint64_t table_size = map->table_size * 2;
	uint64_t table_count = map->table_count * 2;
	map_pair_t** table = (map_pair_t**)HEAP_ALLOC(table_size);
	memset(table, 0, table_size);
	while (table_index < map->table_count)
	{
		map_pair_t* curr = map->table[table_index];
		while (curr)
		{
			uint64_t hash = map_hash(map, curr->key, curr->key_size, table_count);
			curr->next = table[hash];
			table[hash] = curr;
			curr = curr->next;
		}
		table_index++;
	}
	HEAP_FREE(map->table);
	map->table = table;
	map->table_size = table_size;
	map->table_count = table_count;
}
void map_clear(map_t* map)
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
	memset(map->table, 0, map->table_size);
	map->pair_count = 0;
}
uint64_t map_hash(map_t* map, void const* key, uint64_t key_size, uint64_t modulus)
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
float_t map_load_factor(map_t* map)
{
	return ((float_t)(map->pair_count + 1)) / (float_t)map->table_count;
}
void map_print(map_t* map)
{
	printf("map:\n");
	printf("\ttable_size: %zu\n", map->table_size);
	printf("\ttable_count: %zu\n", map->table_count);
	printf("\tpair_count: %zu\n", map->pair_count);
	printf("\ttable:\n");
	uint64_t table_index = 0;
	while (table_index < map->table_count)
	{
		map_pair_t* curr = map->table[table_index];
		while (curr)
		{
			printf("\t\t%s -> %s\n", (char const*)curr->key, (char const*)curr->value);
			curr = curr->next;
		}
		table_index++;
	}
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
