#ifndef FLECS_SYSTEMS_PHYSICS_SQUERY_H
#define FLECS_SYSTEMS_PHYSICS_SQUERY_H

#include "bake_config.h"
#include "octree.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ecs_squery_t ecs_squery_t;

FLECS_SYSTEMS_PHYSICS_EXPORT
ecs_squery_t* ecs_squery_new(
    ecs_world_t *world,
    const char *expr,
    vec3 center,
    float size);

FLECS_SYSTEMS_PHYSICS_EXPORT
void ecs_squery_free(
    ecs_squery_t *sq);

FLECS_SYSTEMS_PHYSICS_EXPORT
void ecs_squery_findn(
    ecs_squery_t *sq,
    vec3 position,
    float range,
    ecs_vector_t **result);

#ifdef __cplusplus
}
#endif

#endif
