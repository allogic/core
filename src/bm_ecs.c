#include "timer.h"

#define HEAP_IMPLEMENTATION
#include "heap.h"

#define VEC_IMPLEMENTATION
#include "vec.h"

#define DAT_IMPLEMENTATION
#include "dat.h"

#define ECS_IMPLEMENTATION
#include "ecs.h"

static void for2_proc(ecs_t* ecs, uint64_t entity, vec_t* view)
{
	uint64_t* dummy0 = (uint64_t*)ecs_value(ecs, entity, 0, view);
	uint64_t* dummy1 = (uint64_t*)ecs_value(ecs, entity, 1, view);

	*dummy0 = 42;
	*dummy1 = 42;
}
static void for4_proc(ecs_t* ecs, uint64_t entity, vec_t* view)
{
	uint64_t* dummy0 = (uint64_t*)ecs_value(ecs, entity, 0, view);
	uint64_t* dummy1 = (uint64_t*)ecs_value(ecs, entity, 1, view);
	uint64_t* dummy2 = (uint64_t*)ecs_value(ecs, entity, 2, view);
	uint64_t* dummy3 = (uint64_t*)ecs_value(ecs, entity, 3, view);

	*dummy0 = 42;
	*dummy1 = 42;
	*dummy2 = 42;
	*dummy3 = 42;
}

static void bm_create(ecs_t* ecs, vec_t* entities)
{
	uint64_t entity_index = 0;
	uint64_t entity_count = vec_count(entities);
	TIMER_BEGIN();
	while (entity_index < entity_count)
	{
		uint64_t* entity = (uint64_t*)vec_at(entities, entity_index);
		*entity = ecs_create(ecs);
		entity_index++;
	}
	TIMER_END();
}
static void bm_attach(ecs_t* ecs, vec_t* entities)
{
	uint64_t dummy = 0;
	uint64_t entity_index = 0;
	uint64_t entity_count = vec_count(entities);
	TIMER_BEGIN();
	while (entity_index < entity_count)
	{
		uint64_t* entity = (uint64_t*)vec_at(entities, entity_index);
		ecs_attach(ecs, *entity, 0, &dummy);
		entity_index++;
	}
	TIMER_END();
}
static void bm_set(ecs_t* ecs, vec_t* entities)
{
	uint64_t dummy = 0;
	uint64_t entity_index = 0;
	uint64_t entity_count = vec_count(entities);
	TIMER_BEGIN();
	while (entity_index < entity_count)
	{
		uint64_t* entity = (uint64_t*)vec_at(entities, entity_index);
		ecs_set(ecs, *entity, 0, &dummy);
		entity_index++;
	}
	TIMER_END();
}
static void bm_get(ecs_t* ecs, vec_t* entities)
{
	uint64_t entity_index = 0;
	uint64_t entity_count = vec_count(entities);
	TIMER_BEGIN();
	while (entity_index < entity_count)
	{
		uint64_t* entity = (uint64_t*)vec_at(entities, entity_index);
		ecs_get(ecs, *entity, 0);
		entity_index++;
	}
	TIMER_END();
}
static void bm_detach(ecs_t* ecs, vec_t* entities)
{
	uint64_t entity_index = 0;
	uint64_t entity_count = vec_count(entities);
	TIMER_BEGIN();
	while (entity_index < entity_count)
	{
		uint64_t* entity = (uint64_t*)vec_at(entities, entity_index);
		ecs_detach(ecs, *entity, 0);
		entity_index++;
	}
	TIMER_END();
}
static void bm_delete(ecs_t* ecs, vec_t* entities)
{
	uint64_t entity_index = 0;
	uint64_t entity_count = vec_count(entities);
	TIMER_BEGIN();
	while (entity_index < entity_count)
	{
		uint64_t* entity = (uint64_t*)vec_at(entities, entity_index);
		ecs_delete(ecs, *entity);
		entity_index++;
	}
	TIMER_END();
}
static void bm_for2(ecs_t* ecs)
{
	vec_t view = ecs_all(ecs, 0b11);
	TIMER_BEGIN();
	ecs_for(ecs, &view, for2_proc);
	TIMER_END();
	vec_free(&view);
}
static void bm_for4(ecs_t* ecs)
{
	vec_t view = ecs_all(ecs, 0b1111);
	TIMER_BEGIN();
	ecs_for(ecs, &view, for4_proc);
	TIMER_END();
	vec_free(&view);
}

int main(void)
{
	heap_prologue();
	ecs_t ecs = ecs_alloc();
	ecs_register(&ecs, 0, sizeof(uint64_t));
	ecs_register(&ecs, 1, sizeof(uint64_t));
	ecs_register(&ecs, 2, sizeof(uint64_t));
	ecs_register(&ecs, 3, sizeof(uint64_t));
	vec_t entities = vec_alloc(sizeof(uint64_t));

	printf("\n");
	printf("TEST 100\n");
	printf("\n");

	vec_resize(&entities, 100);
	bm_create(&ecs, &entities);
	bm_attach(&ecs, &entities);
	bm_set(&ecs, &entities);
	bm_get(&ecs, &entities);
	bm_detach(&ecs, &entities);
	{
		uint64_t entity_index = 0;
		uint64_t entity_count = vec_count(&entities);
		while (entity_index < entity_count)
		{
			uint64_t entity = *(uint64_t*)vec_at(&entities, entity_index);
			ecs_attach(&ecs, entity, 0, 0);
			ecs_attach(&ecs, entity, 1, 0);
			ecs_attach(&ecs, entity, 2, 0);
			ecs_attach(&ecs, entity, 3, 0);
			entity_index++;
		}
	}
	bm_for2(&ecs);
	bm_for4(&ecs);
	ecs_clear(&ecs);

	printf("\n");
	printf("TEST 10000\n");
	printf("\n");

	vec_resize(&entities, 10000);
	bm_create(&ecs, &entities);
	bm_attach(&ecs, &entities);
	bm_set(&ecs, &entities);
	bm_get(&ecs, &entities);
	bm_detach(&ecs, &entities);
	{
		uint64_t entity_index = 0;
		uint64_t entity_count = vec_count(&entities);
		while (entity_index < entity_count)
		{
			uint64_t entity = *(uint64_t*)vec_at(&entities, entity_index);
			ecs_attach(&ecs, entity, 0, 0);
			ecs_attach(&ecs, entity, 1, 0);
			ecs_attach(&ecs, entity, 2, 0);
			ecs_attach(&ecs, entity, 3, 0);
			entity_index++;
		}
	}
	bm_for2(&ecs);
	bm_for4(&ecs);
	ecs_clear(&ecs);

	printf("\n");
	printf("TEST 1000000\n");
	printf("\n");

	vec_resize(&entities, 1000000);
	bm_create(&ecs, &entities);
	bm_attach(&ecs, &entities);
	bm_set(&ecs, &entities);
	bm_get(&ecs, &entities);
	bm_detach(&ecs, &entities);
	{
		uint64_t entity_index = 0;
		uint64_t entity_count = vec_count(&entities);
		while (entity_index < entity_count)
		{
			uint64_t entity = *(uint64_t*)vec_at(&entities, entity_index);
			ecs_attach(&ecs, entity, 0, 0);
			ecs_attach(&ecs, entity, 1, 0);
			ecs_attach(&ecs, entity, 2, 0);
			ecs_attach(&ecs, entity, 3, 0);
			entity_index++;
		}
	}
	bm_for2(&ecs);
	bm_for4(&ecs);
	ecs_clear(&ecs);

	vec_free(&entities);
	ecs_free(&ecs);
	heap_epilogue();
	return 0;
}
