#include "test.h"
#include "heap.h"
#include "map.h"

#include "test_map.h"

void test_map_empty(void)
{
	TEST_BEGIN();
	map_t map = map_alloc();
	TEST_INT_EQ(map_table_size(&map), 1024);
	TEST_INT_EQ(map_table_count(&map), 128);
	TEST_INT_EQ(map_count(&map), 0);
	TEST_FLT_LEQ(map_load_factor(&map), MAP_LOAD_FACTOR);
	map_free(&map);
	TEST_END();
}
void test_map_insert_remove(void)
{
	TEST_BEGIN();
	map_t map = map_alloc();
	char const* key = "test";
	char value[4] = "ABC";
	uint64_t key_size = strlen(key);
	map_insert(&map, key, key_size, value, 4);
	TEST_INT_EQ(map_table_size(&map), 1024);
	TEST_INT_EQ(map_table_count(&map), 128);
	TEST_INT_EQ(map_count(&map), 1);
	TEST_INT_EQ(map_contains(&map, key, key_size), 1);
	TEST_FLT_LEQ(map_load_factor(&map), MAP_LOAD_FACTOR);
	memset(value, 0, 4);
	map_remove(&map, key, key_size, value, 4);
	TEST_STR_EQ(value, "ABC");
	TEST_INT_EQ(map_table_size(&map), 1024);
	TEST_INT_EQ(map_table_count(&map), 128);
	TEST_INT_EQ(map_count(&map), 0);
	TEST_INT_EQ(map_contains(&map, key, key_size), 0);
	TEST_FLT_LEQ(map_load_factor(&map), MAP_LOAD_FACTOR);
	memset(value, 0, 4);
	map_remove(&map, key, key_size, value, 4);
	TEST_STR_EQ(value, "");
	TEST_INT_EQ(map_table_size(&map), 1024);
	TEST_INT_EQ(map_table_count(&map), 128);
	TEST_INT_EQ(map_count(&map), 0);
	TEST_INT_EQ(map_contains(&map, key, key_size), 0);
	TEST_FLT_LEQ(map_load_factor(&map), MAP_LOAD_FACTOR);
	memcpy(value, "CBA", 3);
	map_insert(&map, key, key_size, value, 4);
	map_insert(&map, key, key_size, value, 4);
	map_insert(&map, key, key_size, value, 4);
	TEST_INT_EQ(map_table_size(&map), 1024);
	TEST_INT_EQ(map_table_count(&map), 128);
	TEST_INT_EQ(map_count(&map), 1);
	TEST_INT_EQ(map_contains(&map, key, key_size), 1);
	TEST_FLT_LEQ(map_load_factor(&map), MAP_LOAD_FACTOR);
	TEST_STR_EQ(map_at(&map, key, key_size), "CBA");
	map_free(&map);
	TEST_END();
}
void test_map_expand(void)
{
	TEST_BEGIN();
	map_t map = map_alloc();
	uint64_t value = 42;
	uint64_t index = 0;
	uint64_t count = 128;
	while (index < count)
	{
		map_insert(&map, &index, sizeof(uint64_t), &value, sizeof(uint64_t));
		index++;
	}
	TEST_INT_EQ(map_table_size(&map), 2048);
	TEST_INT_EQ(map_table_count(&map), 256);
	TEST_INT_EQ(map_count(&map), 128);
	TEST_FLT_LEQ(map_load_factor(&map), MAP_LOAD_FACTOR);
	map_free(&map);
	TEST_END();
}
void test_map_copy_equal(void)
{
	TEST_BEGIN();
	map_t map_a = map_alloc();
	char const* key0 = "key0";
	char const* key1 = "key1";
	char const* key2 = "key2";
	char const* key3 = "key3";
	char value0[] = "ABCD";
	char value1[] = "BCDA";
	char value2[] = "CDAB";
	char value3[] = "DABC";
	uint64_t key_size0 = strlen(key0);
	uint64_t key_size1 = strlen(key1);
	uint64_t key_size2 = strlen(key2);
	uint64_t key_size3 = strlen(key3);
	uint64_t value_size0 = strlen(value0);
	uint64_t value_size1 = strlen(value1);
	uint64_t value_size2 = strlen(value2);
	uint64_t value_size3 = strlen(value3);
	map_insert(&map_a, key0, key_size0, value0, value_size0);
	map_insert(&map_a, key1, key_size1, value1, value_size1);
	map_insert(&map_a, key2, key_size2, value2, value_size2);
	TEST_INT_EQ(map_table_size(&map_a), 1024);
	TEST_INT_EQ(map_table_count(&map_a), 128);
	TEST_INT_EQ(map_count(&map_a), 3);
	TEST_FLT_LEQ(map_load_factor(&map_a), MAP_LOAD_FACTOR);
	map_t map_b = map_copy(&map_a);
	TEST_INT_EQ(map_table_size(&map_a), 1024);
	TEST_INT_EQ(map_table_count(&map_a), 128);
	TEST_INT_EQ(map_count(&map_a), 3);
	TEST_FLT_LEQ(map_load_factor(&map_a), MAP_LOAD_FACTOR);
	TEST_INT_EQ(map_equal(&map_a, &map_b), 1);
	map_insert(&map_a, key3, key_size3, value3, value_size3);
	TEST_INT_EQ(map_count(&map_a), 4);
	TEST_INT_EQ(map_equal(&map_a, &map_b), 0);
	map_insert(&map_b, key3, key_size3, value3, value_size3);
	TEST_INT_EQ(map_count(&map_b), 4);
	TEST_INT_EQ(map_equal(&map_a, &map_b), 1);
	map_free(&map_b);
	map_free(&map_a);
	TEST_END();
}
void test_map_clear(void)
{
	TEST_BEGIN();
	map_t map = map_alloc();
	uint64_t value = 42;
	uint64_t index = 0;
	uint64_t count = 128;
	while (index < count)
	{
		map_insert(&map, &index, sizeof(uint64_t), &value, sizeof(uint64_t));
		index++;
	}
	TEST_INT_EQ(map_table_size(&map), 2048);
	TEST_INT_EQ(map_table_count(&map), 256);
	TEST_INT_EQ(map_count(&map), 128);
	TEST_FLT_LEQ(map_load_factor(&map), MAP_LOAD_FACTOR);
	map_clear(&map);
	TEST_INT_EQ(map_table_size(&map), 2048);
	TEST_INT_EQ(map_table_count(&map), 256);
	TEST_INT_EQ(map_count(&map), 0);
	TEST_FLT_LEQ(map_load_factor(&map), MAP_LOAD_FACTOR);
	map_free(&map);
	TEST_END();
}
void test_map(void)
{
	heap_prologue();
	test_prologue();
	test_map_empty();
	test_map_insert_remove();
	test_map_expand();
	test_map_copy_equal();
	test_map_clear();
	test_epilogue("map");
	heap_epilogue();
}
