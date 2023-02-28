#ifndef FLECS_SYSTEMS_PHYSICS_OCTREE_H
#define FLECS_SYSTEMS_PHYSICS_OCTREE_H

#include "bake_config.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ecs_octree_t ecs_octree_t;

typedef struct ecs_oct_entity_t {
    ecs_entity_t id;
    vec3 pos;
    vec3 size;
} ecs_oct_entity_t;

FLECS_SYSTEMS_PHYSICS_API
ecs_octree_t* ecs_octree_new(
    vec3 center,
    float size);

FLECS_SYSTEMS_PHYSICS_API
void ecs_octree_free(
    ecs_octree_t *ot);

FLECS_SYSTEMS_PHYSICS_API
void ecs_octree_clear(
    ecs_octree_t *ot);    

FLECS_SYSTEMS_PHYSICS_API
int32_t ecs_octree_insert(
    ecs_octree_t *ot,
    ecs_entity_t e,
    vec3 pos,
    vec3 size);

FLECS_SYSTEMS_PHYSICS_API
void ecs_octree_findn(
    ecs_octree_t *ot,
    vec3 pos,
    float range,
    ecs_vec_t *result);

FLECS_SYSTEMS_PHYSICS_API
int32_t ecs_octree_dump(
    ecs_octree_t *ot);

#ifdef __cplusplus
}
#endif

#endif
