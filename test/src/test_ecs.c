#include "test.h"
#include "heap.h"
#include "ecs.h"

#include "test_ecs.h"

void test_ecs_create_delete(void)
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
void test_ecs(void)
{
	heap_prologue();
	test_prologue();
	test_ecs_create_delete();
	test_epilogue("ecs");
	heap_epilogue();
}
