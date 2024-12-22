#ifndef TST_VEC_H
#define TST_VEC_H

#include "test.h"
#include "heap.h"
#include "vec.h"

static void tst_vec_empty(void)
{
	TEST_BEGIN();
	vec_t vec = vec_alloc(sizeof(uint64_t));
	TEST_INT_EQ(vec_count(&vec), 0);
	TEST_INT_EQ(vec_size(&vec), 0);
	TEST_INT_EQ(vec_capacity(&vec), 128);
	vec_free(&vec);
	TEST_END();
}
static void tst_vec_push_pop(void)
{
	TEST_BEGIN();
	vec_t vec = vec_alloc(sizeof(uint64_t));
	uint64_t value = 42;
	vec_push(&vec, &value);
	TEST_INT_EQ(vec_count(&vec), 1);
	TEST_INT_EQ(vec_size(&vec), 8);
	TEST_INT_EQ(vec_capacity(&vec), 128);
	vec_pop(&vec, &value);
	TEST_INT_EQ(vec_count(&vec), 0);
	TEST_INT_EQ(vec_size(&vec), 0);
	TEST_INT_EQ(vec_capacity(&vec), 128);
	TEST_INT_EQ(value, 42);
	vec_free(&vec);
	TEST_END();
}
static void tst_vec_expand(void)
{
	TEST_BEGIN();
	vec_t vec = vec_alloc(sizeof(uint64_t));
	uint64_t value = 42;
	uint64_t index = 0;
	uint64_t count = 15;
	while (index < count)
	{
		vec_push(&vec, &value);
		index++;
	}
	TEST_INT_EQ(vec_count(&vec), 15);
	TEST_INT_EQ(vec_size(&vec), 120);
	TEST_INT_EQ(vec_capacity(&vec), 128);
	vec_push(&vec, &value);
	TEST_INT_EQ(vec_count(&vec), 16);
	TEST_INT_EQ(vec_size(&vec), 128);
	TEST_INT_EQ(vec_capacity(&vec), 256);
	vec_free(&vec);
	TEST_END();
}
static void tst_vec_resize(void)
{
	TEST_BEGIN();
	vec_t vec = vec_alloc(sizeof(uint64_t));
	vec_resize(&vec, 16);
	TEST_INT_EQ(vec_count(&vec), 16);
	TEST_INT_EQ(vec_size(&vec), 128);
	TEST_INT_EQ(vec_capacity(&vec), 256);
	vec_resize(&vec, 17);
	TEST_INT_EQ(vec_count(&vec), 17);
	TEST_INT_EQ(vec_size(&vec), 136);
	TEST_INT_EQ(vec_capacity(&vec), 512);
	vec_resize(&vec, 16);
	TEST_INT_EQ(vec_count(&vec), 16);
	TEST_INT_EQ(vec_size(&vec), 128);
	TEST_INT_EQ(vec_capacity(&vec), 256);
	vec_resize(&vec, 0);
	TEST_INT_EQ(vec_count(&vec), 0);
	TEST_INT_EQ(vec_size(&vec), 0);
	TEST_INT_EQ(vec_capacity(&vec), 256);
	vec_free(&vec);
	TEST_END();
}
static void tst_vec_copy_equal(void)
{
	TEST_BEGIN();
	vec_t vec_a = vec_alloc(sizeof(uint64_t));
	vec_t vec_b = vec_copy(&vec_a);
	uint64_t value = 42;
	uint64_t index = 0;
	uint64_t count = 32;
	TEST_INT_EQ(vec_equal(&vec_a, &vec_b), 1);
	vec_resize(&vec_a, 32);
	while (index < count)
	{
		vec_set(&vec_a, index, &value);
		index++;
	}
	TEST_INT_EQ(vec_equal(&vec_a, &vec_b), 0);
	vec_resize(&vec_b, 32);
	index = 0;
	while (index < count)
	{
		vec_set(&vec_b, index, &value);
		index++;
	}
	TEST_INT_EQ(vec_equal(&vec_a, &vec_b), 1);
	value = 1337;
	vec_fill(&vec_a, &value);
	TEST_INT_EQ(vec_equal(&vec_a, &vec_b), 0);
	vec_fill(&vec_b, &value);
	TEST_INT_EQ(vec_equal(&vec_a, &vec_b), 1);
	vec_push(&vec_b, 0);
	TEST_INT_EQ(vec_equal(&vec_a, &vec_b), 0);
	vec_free(&vec_b);
	vec_b = vec_copy(&vec_a);
	TEST_INT_EQ(vec_equal(&vec_a, &vec_b), 1);
	vec_free(&vec_a);
	vec_free(&vec_b);
	TEST_END();
}
static void tst_vec_front_back_at(void)
{
	TEST_BEGIN();
	vec_t vec = vec_alloc(sizeof(uint64_t));
	uint64_t value = 42;
	vec_push(&vec, &value);
	TEST_INT_EQ(*(uint64_t*)vec_front(&vec), 42);
	TEST_INT_EQ(*(uint64_t*)vec_back(&vec), 42);
	TEST_INT_EQ(*(uint64_t*)vec_at(&vec, 0), 42);
	value = 43;
	vec_push(&vec, &value);
	TEST_INT_EQ(*(uint64_t*)vec_front(&vec), 42);
	TEST_INT_EQ(*(uint64_t*)vec_back(&vec), 43);
	TEST_INT_EQ(*(uint64_t*)vec_at(&vec, 1), 43);
	value = 44;
	vec_resize(&vec, 32);
	vec_set(&vec, 31, &value);
	TEST_INT_EQ(*(uint64_t*)vec_back(&vec), 44);
	TEST_INT_EQ(*(uint64_t*)vec_at(&vec, 31), 44);
	vec_resize(&vec, 1);
	TEST_INT_EQ(*(uint64_t*)vec_front(&vec), 42);
	TEST_INT_EQ(*(uint64_t*)vec_at(&vec, 0), 42);
	vec_free(&vec);
	TEST_END();
}
static void tst_vec_remove(void)
{
	TEST_BEGIN();
	vec_t vec = vec_alloc(sizeof(uint64_t));
	uint64_t value;
	value = 42;
	vec_push(&vec, &value);
	value = 1337;
	vec_push(&vec, &value);
	TEST_INT_EQ(*(uint64_t*)vec_front(&vec), 42);
	TEST_INT_EQ(*(uint64_t*)vec_back(&vec), 1337);
	TEST_INT_EQ(vec_count(&vec), 2);
	TEST_INT_EQ(vec_size(&vec), 16);
	TEST_INT_EQ(vec_capacity(&vec), 128);
	value = 42;
	vec_remove(&vec, &value);
	TEST_INT_EQ(*(uint64_t*)vec_front(&vec), 1337);
	TEST_INT_EQ(*(uint64_t*)vec_back(&vec), 1337);
	TEST_INT_EQ(vec_count(&vec), 1);
	TEST_INT_EQ(vec_size(&vec), 8);
	TEST_INT_EQ(vec_capacity(&vec), 128);
	vec_free(&vec);
	TEST_END();
}
static void tst_vec_swap(void)
{
	TEST_BEGIN();
	vec_t vec = vec_alloc(sizeof(uint64_t));
	uint64_t value;
	value = 42;
	vec_push(&vec, &value);
	value = 1337;
	vec_push(&vec, &value);
	TEST_INT_EQ(*(uint64_t*)vec_front(&vec), 42);
	TEST_INT_EQ(*(uint64_t*)vec_back(&vec), 1337);
	vec_swap(&vec, 0, 1);
	TEST_INT_EQ(*(uint64_t*)vec_front(&vec), 1337);
	TEST_INT_EQ(*(uint64_t*)vec_back(&vec), 42);
	vec_free(&vec);
	TEST_END();
}
static void tst_vec_clear(void)
{
	TEST_BEGIN();
	vec_t vec = vec_alloc(sizeof(uint64_t));
	uint64_t value;
	value = 42;
	vec_push(&vec, &value);
	value = 1337;
	vec_push(&vec, &value);
	TEST_INT_EQ(vec_count(&vec), 2);
	TEST_INT_EQ(vec_size(&vec), 16);
	TEST_INT_EQ(vec_capacity(&vec), 128);
	vec_clear(&vec);
	TEST_INT_EQ(vec_count(&vec), 0);
	TEST_INT_EQ(vec_size(&vec), 0);
	TEST_INT_EQ(vec_capacity(&vec), 128);
	uint64_t index = 0;
	uint64_t count = 16;
	while (index < count)
	{
		vec_push(&vec, &value);
		index++;
	}
	vec_clear(&vec);
	TEST_INT_EQ(vec_count(&vec), 0);
	TEST_INT_EQ(vec_size(&vec), 0);
	TEST_INT_EQ(vec_capacity(&vec), 256);
	vec_free(&vec);
	TEST_END();
}
static void tst_vec(void)
{
	test_prologue();
	heap_prologue();
	tst_vec_empty();
	tst_vec_push_pop();
	tst_vec_expand();
	tst_vec_resize();
	tst_vec_copy_equal();
	tst_vec_front_back_at();
	tst_vec_remove();
	tst_vec_swap();
	tst_vec_clear();
	heap_epilogue();
	test_epilogue("vec");
}

#endif // TST_VEC_H
