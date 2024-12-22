#ifndef TST_ECS_H
#define TST_ECS_H

#include "test.h"
#include "heap.h"
#include "ecs.h"

static void tst_ecs_create_delete(void)
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
static void tst_ecs(void)
{
	test_prologue();
	heap_prologue();
	tst_ecs_create_delete();
	heap_epilogue();
	test_epilogue("ecs");
}

#endif // TST_ECS_H
