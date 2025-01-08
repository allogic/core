#ifndef CORE_ECS_H
#define CORE_ECS_H

#include <stdint.h>

#include "core_config.h"
#include "core_macros.h"
#include "core_vec.h"
#include "core_dat.h"

typedef struct _ecs_t
{
	uint64_t unique_entity;
	uint64_t active_pools;
	uint64_t active_pool_count;
	dat_t masks;
	vec_t alloc_entities;
	vec_t free_entities;
	vec_t pools;
} ecs_t;

typedef void (*ecs_for_func_t)(ecs_t* ecs, uint64_t entity, vec_t* view);

extern ecs_t ecs_alloc(void);
extern ecs_t ecs_copy(ecs_t* ref);
extern uint8_t ecs_equal(ecs_t* ref);
extern uint64_t ecs_create(ecs_t* ecs);
extern void ecs_delete(ecs_t* ecs, uint64_t entity);
extern void ecs_register(ecs_t* ecs, uint64_t bit, uint64_t value_size);
extern void ecs_unregister(ecs_t* ecs, uint64_t bit);
extern void ecs_attach(ecs_t* ecs, uint64_t entity, uint64_t bit, void const* value);
extern void ecs_detach(ecs_t* ecs, uint64_t entity, uint64_t bit);
extern void ecs_set(ecs_t* ecs, uint64_t entity, uint64_t bit, void const* value);
extern void* ecs_get(ecs_t* ecs, uint64_t entity, uint64_t bit);
extern vec_t ecs_all(ecs_t* ecs, uint64_t mask);
extern uint8_t ecs_contains(ecs_t* ecs, uint64_t entity, vec_t* view);
extern uint64_t ecs_count(ecs_t* ecs);
extern void* ecs_value(ecs_t* ecs, uint64_t entity, uint64_t bit, vec_t* view);
extern dat_t* ecs_smallest(ecs_t* ecs, vec_t* view);
extern void ecs_for(ecs_t* ecs, vec_t* view, ecs_for_func_t for_func);
extern void ecs_clear(ecs_t* ecs);
extern void ecs_print(ecs_t* ecs);
extern void ecs_free(ecs_t* ecs);

#endif // CORE_ECS_H
