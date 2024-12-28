#include "timer.h"
#include "heap.h"

#include "benchmark_ecs.h"

void benchmark_ecs_for2_proc(ecs_t* ecs, uint64_t entity, vec_t* view)
{
	uint64_t* dummy0 = (uint64_t*)ecs_value(ecs, entity, 0, view);
	uint64_t* dummy1 = (uint64_t*)ecs_value(ecs, entity, 1, view);
	*dummy0 = 42;
	*dummy1 = 42;
}
void benchmark_ecs_for4_proc(ecs_t* ecs, uint64_t entity, vec_t* view)
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
void benchmark_ecs_create_entity(ecs_t* ecs, vec_t* entities)
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
void benchmark_ecs_attach_component(ecs_t* ecs, vec_t* entities)
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
void benchmark_ecs_set_component(ecs_t* ecs, vec_t* entities)
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
void benchmark_ecs_get_component(ecs_t* ecs, vec_t* entities)
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
void benchmark_ecs_detach_component(ecs_t* ecs, vec_t* entities)
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
void benchmark_ecs_delete_entity(ecs_t* ecs, vec_t* entities)
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
void benchmark_ecs_for2(ecs_t* ecs)
{
	vec_t view = ecs_all(ecs, 0b11);
	TIMER_BEGIN();
	ecs_for(ecs, &view, benchmark_ecs_for2_proc);
	TIMER_END();
	vec_free(&view);
}
void benchmark_ecs_for4(ecs_t* ecs)
{
	vec_t view = ecs_all(ecs, 0b1111);
	TIMER_BEGIN();
	ecs_for(ecs, &view, benchmark_ecs_for4_proc);
	TIMER_END();
	vec_free(&view);
}
void benchmark_ecs_with_entity_count(ecs_t* ecs, vec_t* entities, uint64_t entity_count)
{
	vec_resize(entities, entity_count);
	benchmark_ecs_create_entity(ecs, entities);
	benchmark_ecs_attach_component(ecs, entities);
	benchmark_ecs_set_component(ecs, entities);
	benchmark_ecs_get_component(ecs, entities);
	benchmark_ecs_detach_component(ecs, entities);
	{
		uint64_t entity_index = 0;
		uint64_t entity_count = vec_count(entities);
		while (entity_index < entity_count)
		{
			uint64_t entity = *(uint64_t*)vec_at(entities, entity_index);
			ecs_attach(ecs, entity, 0, 0);
			ecs_attach(ecs, entity, 1, 0);
			ecs_attach(ecs, entity, 2, 0);
			ecs_attach(ecs, entity, 3, 0);
			entity_index++;
		}
	}
	benchmark_ecs_for2(ecs);
	benchmark_ecs_for4(ecs);
	ecs_clear(ecs);
}
void benchmark_ecs(void)
{
	heap_prologue();
	ecs_t ecs = ecs_alloc();
	ecs_register(&ecs, 0, sizeof(uint64_t));
	ecs_register(&ecs, 1, sizeof(uint64_t));
	ecs_register(&ecs, 2, sizeof(uint64_t));
	ecs_register(&ecs, 3, sizeof(uint64_t));
	vec_t entities = vec_alloc(sizeof(uint64_t));
	benchmark_ecs_with_entity_count(&ecs, &entities, 100);
	benchmark_ecs_with_entity_count(&ecs, &entities, 10000);
	benchmark_ecs_with_entity_count(&ecs, &entities, 1000000);
	vec_free(&entities);
	ecs_free(&ecs);
	heap_epilogue();
}
