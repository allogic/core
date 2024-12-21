#define TEST_IMPLEMENTATION
#include "test.h"

#define HEAP_IMPLEMENTATION
#include "heap.h"

#define SET_IMPLEMENTATION
#include "set.h"

static void tst_empty(void)
{
	TEST_BEGIN();
	set_t set = set_alloc();
	TEST_INT_EQ(set_table_size(&set), 1024);
	TEST_INT_EQ(set_table_count(&set), 128);
	TEST_INT_EQ(set_count(&set), 0);
	TEST_FLT_LEQ(set_load_factor(&set), SET_LOAD_FACTOR);
	set_free(&set);
	TEST_END();
}
static void tst_insert_remove(void)
{
	TEST_BEGIN();
	set_t set = set_alloc();
	char const* key = "test";
	uint64_t key_size = strlen(key);
	set_insert(&set, key, key_size);
	TEST_INT_EQ(set_table_size(&set), 1024);
	TEST_INT_EQ(set_table_count(&set), 128);
	TEST_INT_EQ(set_count(&set), 1);
	TEST_INT_EQ(set_contains(&set, key, key_size), 1);
	TEST_FLT_LEQ(set_load_factor(&set), SET_LOAD_FACTOR);
	set_remove(&set, key, key_size);
	TEST_INT_EQ(set_table_size(&set), 1024);
	TEST_INT_EQ(set_table_count(&set), 128);
	TEST_INT_EQ(set_count(&set), 0);
	TEST_INT_EQ(set_contains(&set, key, key_size), 0);
	TEST_FLT_LEQ(set_load_factor(&set), SET_LOAD_FACTOR);
	set_remove(&set, key, key_size);
	TEST_INT_EQ(set_table_size(&set), 1024);
	TEST_INT_EQ(set_table_count(&set), 128);
	TEST_INT_EQ(set_count(&set), 0);
	TEST_INT_EQ(set_contains(&set, key, key_size), 0);
	TEST_FLT_LEQ(set_load_factor(&set), SET_LOAD_FACTOR);
	set_insert(&set, key, key_size);
	set_insert(&set, key, key_size);
	set_insert(&set, key, key_size);
	TEST_INT_EQ(set_table_size(&set), 1024);
	TEST_INT_EQ(set_table_count(&set), 128);
	TEST_INT_EQ(set_count(&set), 1);
	TEST_INT_EQ(set_contains(&set, key, key_size), 1);
	TEST_FLT_LEQ(set_load_factor(&set), SET_LOAD_FACTOR);
	set_free(&set);
	TEST_END();
}
static void tst_expand(void)
{
	TEST_BEGIN();
	set_t set = set_alloc();
	uint64_t index = 0;
	uint64_t count = 128;
	while (index < count)
	{
		set_insert(&set, &index, sizeof(uint64_t));
		index++;
	}
	TEST_INT_EQ(set_table_size(&set), 2048);
	TEST_INT_EQ(set_table_count(&set), 256);
	TEST_INT_EQ(set_count(&set), 128);
	TEST_FLT_LEQ(set_load_factor(&set), SET_LOAD_FACTOR);
	set_free(&set);
	TEST_END();
}
static void tst_copy_equal(void)
{
	TEST_BEGIN();
	set_t set_a = set_alloc();
	char const* key0 = "key0";
	char const* key1 = "key1";
	char const* key2 = "key2";
	char const* key3 = "key3";
	uint64_t key_size0 = strlen(key0);
	uint64_t key_size1 = strlen(key1);
	uint64_t key_size2 = strlen(key2);
	uint64_t key_size3 = strlen(key3);
	set_insert(&set_a, key0, key_size0);
	set_insert(&set_a, key1, key_size1);
	set_insert(&set_a, key2, key_size2);
	TEST_INT_EQ(set_table_size(&set_a), 1024);
	TEST_INT_EQ(set_table_count(&set_a), 128);
	TEST_INT_EQ(set_count(&set_a), 3);
	TEST_FLT_LEQ(set_load_factor(&set_a), SET_LOAD_FACTOR);
	set_t set_b = set_copy(&set_a);
	TEST_INT_EQ(set_table_size(&set_a), 1024);
	TEST_INT_EQ(set_table_count(&set_a), 128);
	TEST_INT_EQ(set_count(&set_a), 3);
	TEST_FLT_LEQ(set_load_factor(&set_a), SET_LOAD_FACTOR);
	TEST_INT_EQ(set_equal(&set_a, &set_b), 1);
	set_insert(&set_a, key3, key_size3);
	TEST_INT_EQ(set_count(&set_a), 4);
	TEST_INT_EQ(set_equal(&set_a, &set_b), 0);
	set_insert(&set_b, key3, key_size3);
	TEST_INT_EQ(set_count(&set_b), 4);
	TEST_INT_EQ(set_equal(&set_a, &set_b), 1);
	set_free(&set_b);
	set_free(&set_a);
	TEST_END();
}
static void tst_clear(void)
{
	TEST_BEGIN();
	set_t set = set_alloc();
	uint64_t index = 0;
	uint64_t count = 128;
	while (index < count)
	{
		set_insert(&set, &index, sizeof(uint64_t));
		index++;
	}
	TEST_INT_EQ(set_table_size(&set), 2048);
	TEST_INT_EQ(set_table_count(&set), 256);
	TEST_INT_EQ(set_count(&set), 128);
	TEST_FLT_LEQ(set_load_factor(&set), SET_LOAD_FACTOR);
	set_clear(&set);
	TEST_INT_EQ(set_table_size(&set), 2048);
	TEST_INT_EQ(set_table_count(&set), 256);
	TEST_INT_EQ(set_count(&set), 0);
	TEST_FLT_LEQ(set_load_factor(&set), SET_LOAD_FACTOR);
	set_free(&set);
	TEST_END();
}

int32_t main(void)
{
	test_prologue();
	heap_prologue();
	tst_empty();
	tst_insert_remove();
	tst_expand();
	tst_copy_equal();
	tst_clear();
	heap_epilogue();
	test_epilogue();
	return 0;
}
