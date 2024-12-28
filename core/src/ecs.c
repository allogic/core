#include <stdio.h>
#include <string.h>

#include "ecs.h"

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

#ifndef ECS_INVALID_ENTITY
	#define ECS_INVALID_ENTITY (DAT_TOMBSTONE)
#endif // ECS_INVALID_ENTITY

#ifndef ECS_MAX_POOL_COUNT
	#define ECS_MAX_POOL_COUNT (0xFFFFFFFFFFFFFFFF)
#endif // ECS_MAX_POOL_COUNT

#ifndef ECS_MAX_POOLS
	#define ECS_MAX_POOLS (64)
#endif // ECS_MAX_POOLS

#ifndef ECS_IS_BIT_SET
	#define ECS_IS_BIT_SET(VALUE, BIT) (((uint64_t)(VALUE)) & (((uint64_t)1) << (BIT)))
#endif // ECS_IS_BIT_SET

#ifndef ECS_IS_BIT_NOT_SET
	#define ECS_IS_BIT_NOT_SET(VALUE, BIT) (!(((uint64_t)(VALUE)) & (((uint64_t)1) << (BIT))))
#endif // ECS_IS_BIT_NOT_SET

#ifndef ECS_SET_BIT
	#define ECS_SET_BIT(VALUE, BIT) (((uint64_t)(VALUE)) | (((uint64_t)1) << (BIT)))
#endif // ECS_SET_BIT

#ifndef ECS_CLEAR_BIT
	#define ECS_CLEAR_BIT(VALUE, BIT) (((uint64_t)(VALUE)) & (~(((uint64_t)1) << (BIT))))
#endif // ECS_CLEAR_BIT

#ifndef ECS_TOGGLE_BIT
	#define ECS_TOGGLE_BIT(VALUE, BIT) (((uint64_t)(VALUE)) ^ (((uint64_t)1) << (BIT)))
#endif // ECS_TOGGLE_BIT

ecs_t ecs_alloc(void)
{
	ecs_t ecs;
	memset(&ecs, 0, sizeof(ecs_t));
	ecs.masks = dat_alloc(sizeof(uint64_t));
	ecs.alloc_entities = vec_alloc(sizeof(uint64_t));
	ecs.free_entities = vec_alloc(sizeof(uint64_t));
	ecs.pools = vec_alloc(sizeof(dat_t));
	vec_resize(&ecs.pools, ECS_MAX_POOLS);
	return ecs;
}
ecs_t ecs_copy(ecs_t* ref)
{
	// TODO
	ecs_t ecs;
	memset(&ecs, 0, sizeof(ecs_t));
	return ecs;
}
uint8_t ecs_equal(ecs_t* ref)
{
	// TODO
	return 0;
}
uint64_t ecs_create(ecs_t* ecs)
{
	uint64_t entity = ECS_INVALID_ENTITY;
	uint64_t free_entity_count = vec_count(&ecs->free_entities);
	if (free_entity_count)
	{
		vec_pop(&ecs->free_entities, &entity);
	}
	else
	{
		entity = ecs->unique_entity++;
	}
	vec_push(&ecs->alloc_entities, &entity);
	uint64_t mask = 0;
	dat_set(&ecs->masks, entity, &mask);
	return entity;
}
void ecs_delete(ecs_t* ecs, uint64_t entity)
{
	uint64_t mask = *(uint64_t*)dat_get(&ecs->masks, entity);
	uint64_t bit = 0;
	while (bit < ECS_MAX_POOLS)
	{
		if (ECS_IS_BIT_SET(mask, bit))
		{
			dat_t* pool = (dat_t*)vec_at(&ecs->pools, bit);
			dat_remove(pool, entity);
		}
		bit++;
	}
	dat_remove(&ecs->masks, entity);
	vec_remove(&ecs->alloc_entities, &entity); // TODO
	vec_push(&ecs->free_entities, &entity);
}
void ecs_register(ecs_t* ecs, uint64_t bit, uint64_t value_size)
{
	dat_t* pool = (dat_t*)vec_at(&ecs->pools, bit);
	if (ECS_IS_BIT_SET(ecs->active_pools, bit))
	{
		dat_free(pool);
		ecs->active_pool_count--;
	}
	ecs->active_pools = ECS_SET_BIT(ecs->active_pools, bit);
	ecs->active_pool_count++;
	*pool = dat_alloc(value_size);
}
void ecs_unregister(ecs_t* ecs, uint64_t bit)
{
	// TODO
}
void ecs_attach(ecs_t* ecs, uint64_t entity, uint64_t bit, void const* value)
{
	dat_t* pool = (dat_t*)vec_at(&ecs->pools, bit);
	dat_set(pool, entity, value);
	uint64_t mask = *(uint64_t*)dat_get(&ecs->masks, entity);
	mask = ECS_SET_BIT(mask, bit);
	dat_set(&ecs->masks, entity, &mask);
}
void ecs_detach(ecs_t* ecs, uint64_t entity, uint64_t bit)
{
	dat_t* pool = (dat_t*)vec_at(&ecs->pools, bit);
	dat_remove(pool, entity);
	uint64_t mask = *(uint64_t*)dat_get(&ecs->masks, entity);
	mask = ECS_CLEAR_BIT(mask, bit);
	dat_set(&ecs->masks, entity, &mask);
}
void ecs_set(ecs_t* ecs, uint64_t entity, uint64_t bit, void const* value)
{
	dat_t* pool = (dat_t*)vec_at(&ecs->pools, bit);
	dat_set(pool, entity, value);
}
void* ecs_get(ecs_t* ecs, uint64_t entity, uint64_t bit)
{
	dat_t* pool = (dat_t*)vec_at(&ecs->pools, bit);
	return dat_get(pool, entity);
}
vec_t ecs_all(ecs_t* ecs, uint64_t mask)
{
	vec_t view = vec_alloc(sizeof(dat_t*));
	uint64_t bit = 0;
	while (bit < ECS_MAX_POOLS)
	{
		dat_t* pool = (dat_t*)vec_at(&ecs->pools, bit);
		if (ECS_IS_BIT_SET(mask, bit))
		{
			vec_push(&view, &pool);
		}
		bit++;
	}
	return view;
}
uint8_t ecs_contains(ecs_t* ecs, uint64_t entity, vec_t* view)
{
	uint64_t view_index = 0;
	uint64_t view_count = vec_count(view);
	while (view_index < view_count)
	{
		dat_t* pool = *(dat_t**)vec_at(view, view_index);;
		if (dat_contains(pool, entity) == 0)
		{
			return 0;
		}
		view_index++;
	}
	return 1;
}
uint64_t ecs_count(ecs_t* ecs)
{
	return vec_count(&ecs->alloc_entities);
}
void* ecs_value(ecs_t* ecs, uint64_t entity, uint64_t bit, vec_t* view)
{
	dat_t* pool = *(dat_t**)vec_at(view, bit);
	return dat_value(pool, entity);
}
dat_t* ecs_smallest(ecs_t* ecs, vec_t* view)
{
	dat_t* smallest_pool = 0;
	uint64_t smallest_pool_count = ECS_MAX_POOL_COUNT;
	uint64_t view_index = 0;
	uint64_t view_count = vec_count(view);
	while (view_index < view_count)
	{
		dat_t* pool = *(dat_t**)vec_at(view, view_index);
		uint64_t pool_count = dat_count(pool);
		if (pool_count < smallest_pool_count)
		{
			smallest_pool = pool;
			smallest_pool_count = pool_count;
		}
		view_index++;
	}
	return smallest_pool;
}
void ecs_for(ecs_t* ecs, vec_t* view, proc_t proc)
{
	dat_t* smallest_pool = ecs_smallest(ecs, view);
	uint64_t entity_index = 0;
	uint64_t entity_count = dat_count(smallest_pool);
	while (entity_index < entity_count)
	{
		uint64_t entity = dat_id(smallest_pool, entity_index);
		proc(ecs, entity, view);
		entity_index++;
	}
}
void ecs_clear(ecs_t* ecs)
{
	uint64_t bit = 0;
	while (bit < ECS_MAX_POOLS)
	{
		if (ECS_IS_BIT_SET(ecs->active_pools, bit))
		{
			dat_t* pool = (dat_t*)vec_at(&ecs->pools, bit);
			dat_clear(pool);
		}
		bit++;
	}
	ecs->unique_entity = 0;
	dat_clear(&ecs->masks);
	vec_clear(&ecs->alloc_entities);
	vec_clear(&ecs->free_entities);
}
void ecs_print(ecs_t* ecs)
{
	printf("entity_component_system:\n");
	printf("\tunique_entity: %zu\n", ecs->unique_entity);
	printf("\tactive_pools: %zu\n", ecs->active_pool_count);
	printf("\t\t[");
	uint64_t bit;
	bit = 0;
	while (bit < ECS_MAX_POOLS)
	{
		printf("%3zu", 64 - bit);
		bit++;
	}
	printf("]\n");
	printf("\t\t[");
	bit = 0;
	while (bit < ECS_MAX_POOLS)
	{
		printf("%3zu", ECS_IS_BIT_SET(ecs->active_pools, 65 - bit));
		bit++;
	}
	printf("]\n");
	uint64_t entity_index;
	uint64_t alloc_entity_count = vec_count(&ecs->alloc_entities);
	uint64_t free_entity_count = vec_count(&ecs->free_entities);
	printf("\talloc_entities: %zu\n", alloc_entity_count);
	entity_index = 0;
	while (entity_index < alloc_entity_count)
	{
		printf("\t\t%zu -> %zu\n", entity_index, *(uint64_t*)vec_at(&ecs->alloc_entities, entity_index));
		entity_index++;
	}
	printf("\tfree_entities: %zu\n", free_entity_count);
	entity_index = 0;
	while (entity_index < free_entity_count)
	{
		printf("\t\t%zu -> %zu\n", entity_index, *(uint64_t*)vec_at(&ecs->free_entities, entity_index));
		entity_index++;
	}
}
void ecs_free(ecs_t* ecs)
{
	uint64_t bit = 0;
	while (bit < ECS_MAX_POOLS)
	{
		if (ECS_IS_BIT_SET(ecs->active_pools, bit))
		{
			dat_t* pool = (dat_t*)vec_at(&ecs->pools, bit);
			dat_free(pool);
		}
		bit++;
	}
	dat_free(&ecs->masks);
	vec_free(&ecs->alloc_entities);
	vec_free(&ecs->free_entities);
	vec_free(&ecs->pools);
	memset(ecs, 0, sizeof(ecs_t));
}
