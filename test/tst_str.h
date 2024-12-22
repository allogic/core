#ifndef TST_STR_H
#define TST_STR_H

#include "test.h"
#include "heap.h"
#include "str.h"

static void tst_str_empty(void)
{
	TEST_BEGIN();
	str_t str = str_alloc();
	TEST_STR_EQ(str_buffer(&str), "");
	TEST_INT_EQ(str_size(&str), 0);
	TEST_INT_EQ(str_capacity(&str), 16);
	str_free(&str);
	TEST_END();
}
static void tst_str_append(void)
{
	TEST_BEGIN();
	str_t str = str_from("A");
	TEST_STR_EQ(str_buffer(&str), "A");
	TEST_INT_EQ(str_size(&str), 1);
	TEST_INT_EQ(str_capacity(&str), 16);
	str_append(&str, "B");
	TEST_STR_EQ(str_buffer(&str), "AB");
	TEST_INT_EQ(str_size(&str), 2);
	TEST_INT_EQ(str_capacity(&str), 16);
	str_append(&str, "C");
	TEST_STR_EQ(str_buffer(&str), "ABC");
	TEST_INT_EQ(str_size(&str), 3);
	TEST_INT_EQ(str_capacity(&str), 16);
	str_free(&str);
	TEST_END();
}
static void tst_str_expand(void)
{
	TEST_BEGIN();
	str_t str = str_from("ABCDEFGHIJKLMNO");
	TEST_STR_EQ(str_buffer(&str), "ABCDEFGHIJKLMNO");
	TEST_INT_EQ(str_size(&str), 15);
	TEST_INT_EQ(str_capacity(&str), 16);
	str_append(&str, "P");
	TEST_STR_EQ(str_buffer(&str), "ABCDEFGHIJKLMNOP");
	TEST_INT_EQ(str_size(&str), 16);
	TEST_INT_EQ(str_capacity(&str), 32);
	str_free(&str);
	TEST_END();
}
static void tst_str_resize(void)
{
	TEST_BEGIN();
	str_t str = str_alloc();
	TEST_STR_EQ(str_buffer(&str), "");
	TEST_INT_EQ(str_size(&str), 0);
	TEST_INT_EQ(str_capacity(&str), 16);
	str_resize(&str, 15);
	TEST_STR_EQ(str_buffer(&str), "");
	TEST_INT_EQ(str_size(&str), 15);
	TEST_INT_EQ(str_capacity(&str), 16);
	str_resize(&str, 16);
	TEST_STR_EQ(str_buffer(&str), "");
	TEST_INT_EQ(str_size(&str), 16);
	TEST_INT_EQ(str_capacity(&str), 32);
	str_fill(&str, 'A');
	TEST_STR_EQ(str_buffer(&str), "AAAAAAAAAAAAAAAA");
	TEST_INT_EQ(str_size(&str), 16);
	TEST_INT_EQ(str_capacity(&str), 32);
	str_resize(&str, 15);
	TEST_STR_EQ(str_buffer(&str), "AAAAAAAAAAAAAAA");
	TEST_INT_EQ(str_size(&str), 15);
	TEST_INT_EQ(str_capacity(&str), 16);
	str_free(&str);
	TEST_END();
}
static void tst_str_copy_equal(void)
{
	TEST_BEGIN();
	str_t str_a = str_from("ABC");
	str_t str_b = str_from("ABC");
	TEST_INT_EQ(str_equal(&str_a, &str_b), 1);
	str_set(&str_a, "AAABBBAAABBBAAABBBAAABBB");
	TEST_INT_EQ(str_equal(&str_a, &str_b), 0);
	str_set(&str_b, "AAABBBAAABBBAAABBBAAABBB");
	TEST_INT_EQ(str_equal(&str_a, &str_b), 1);
	str_set(&str_a, "XXX");
	TEST_INT_EQ(str_equal(&str_a, &str_b), 0);
	str_set(&str_b, "XXX");
	TEST_INT_EQ(str_equal(&str_a, &str_b), 1);
	str_set(&str_a, "");
	str_set(&str_b, "");
	TEST_INT_EQ(str_equal(&str_a, &str_b), 1);
	str_free(&str_a);
	str_free(&str_b);
	TEST_END();
}
static void tst_str_clear(void)
{
	TEST_BEGIN();
	str_t str = str_from("ABC");
	TEST_STR_EQ(str_buffer(&str), "ABC");
	str_clear(&str);
	TEST_STR_EQ(str_buffer(&str), "");
	TEST_INT_EQ(str_size(&str), 0);
	TEST_INT_EQ(str_capacity(&str), 16);
	str_free(&str);
	TEST_END();
}
static void tst_str(void)
{
	test_prologue();
	heap_prologue();
	tst_str_empty();
	tst_str_append();
	tst_str_expand();
	tst_str_resize();
	tst_str_copy_equal();
	tst_str_clear();
	heap_epilogue();
	test_epilogue("str");
}

#endif // TST_STR_H
