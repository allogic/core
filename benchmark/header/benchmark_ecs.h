#ifndef BENCHMARK_ECS_H
#define BENCHMARK_ECS_H

#include <stdint.h>

#include "vec.h"
#include "ecs.h"

extern void benchmark_ecs_for2_proc(ecs_t* ecs, uint64_t entity, vec_t* view);
extern void benchmark_ecs_for4_proc(ecs_t* ecs, uint64_t entity, vec_t* view);
extern void benchmark_ecs_create_entity(ecs_t* ecs, vec_t* entities);
extern void benchmark_ecs_attach_component(ecs_t* ecs, vec_t* entities);
extern void benchmark_ecs_set_component(ecs_t* ecs, vec_t* entities);
extern void benchmark_ecs_get_component(ecs_t* ecs, vec_t* entities);
extern void benchmark_ecs_detach_component(ecs_t* ecs, vec_t* entities);
extern void benchmark_ecs_delete_entity(ecs_t* ecs, vec_t* entities);
extern void benchmark_ecs_for2(ecs_t* ecs);
extern void benchmark_ecs_for4(ecs_t* ecs);
extern void benchmark_ecs_with_entity_count(ecs_t* ecs, vec_t* entities, uint64_t entity_count);
extern void benchmark_ecs(void);

#endif // BENCHMARK_ECS_H
