#ifndef FLECS_SYSTEMS_PHYSICS_H
#define FLECS_SYSTEMS_PHYSICS_H

/* This generated file contains includes for project dependencies */
#include "flecs-systems-physics/bake_config.h"
#include "flecs-systems-physics/octree.h"
#include "flecs-systems-physics/spatial_query.h"

// Don't use reflection, but use utility macro's for auto-exporting variables
#undef ECS_META_IMPL
#ifndef flecs_systems_physics_EXPORTS
#define ECS_META_IMPL EXTERN
#else
#define ECS_META_IMPL DECLARE
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __cplusplus

ECS_STRUCT(EcsSpatialQuery, {
    ecs_squery_t *query;
});

ECS_STRUCT(EcsSpatialQueryResult, {
    ecs_vector_t *results;
});

#else

typedef struct EcsSpatialQuery {
    flecs::squery query;
} EcsSpatialQuery;

typedef struct EcsSpatialQueryResult {
    flecs::vector<flecs::squery::entity> results;
} EcsSpatialQueryResult;

#endif

FLECS_SYSTEMS_PHYSICS_API
void FlecsSystemsPhysicsImport(
    ecs_world_t *world);

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus

namespace flecs {
namespace systems {

class physics : FlecsSystemsPhysics {
public:
    using SpatialQuery = EcsSpatialQuery;
    using SpatialQueryResult = EcsSpatialQueryResult;

    physics(flecs::world& ecs) {
        FlecsSystemsPhysicsImport(ecs.c_ptr());

        ecs.module<flecs::systems::physics>();

        ecs.component<SpatialQuery>("flecs::systems::physics::SpatialQuery");
        ecs.component<SpatialQueryResult>("flecs::systems::physics::SpatialQueryResult");
    }
};

}
}

#endif

#endif
