#define TEST_IMPLEMENTATION
#include "test.h"

#define HEAP_IMPLEMENTATION
#include "heap.h"

#define VEC_IMPLEMENTATION
#include "vec.h"

#define DAT_IMPLEMENTATION
#include "dat.h"

#define ECS_IMPLEMENTATION
#include "ecs.h"

static void tst_create_delete(void)
{
	TEST_BEGIN();
	ecs_t ecs = ecs_alloc();
	uint64_t entity;
	entity = ecs_create(&ecs);
	TEST_INT_EQ(ecs_count(&ecs), 1);
	TEST_INT_EQ(entity, 0);
	ecs_delete(&ecs, entity);
	TEST_INT_EQ(ecs_count(&ecs), 0);
	entity = ecs_create(&ecs);
	TEST_INT_EQ(ecs_count(&ecs), 1);
	TEST_INT_EQ(entity, 0);
	ecs_free(&ecs);
	TEST_END();
}

int32_t main(void)
{
	test_prologue();
	heap_prologue();
	tst_create_delete();
	heap_epilogue();
	test_epilogue();
	return 0;
}
