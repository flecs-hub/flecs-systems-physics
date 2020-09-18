#ifndef FLECS_SYSTEMS_PHYSICS_H
#define FLECS_SYSTEMS_PHYSICS_H

/* This generated file contains includes for project dependencies */
#include "flecs-systems-physics/bake_config.h"
#include "flecs-systems-physics/octree.h"
#include "flecs-systems-physics/spatial_query.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __cplusplus

typedef struct EcsSpatialQuery {
    ecs_squery_t *query;
} EcsSpatialQuery;

typedef struct EcsSpatialQueryResult {
    ecs_vector_t *results;
} EcsSpatialQueryResult;

#else

typedef struct EcsSpatialQuery {
    flecs::squery query;
} EcsSpatialQuery;

typedef struct EcsSpatialQueryResult {
    flecs::vector<flecs::squery::entity> results;
} EcsSpatialQueryResult;

#endif

typedef struct FlecsSystemsPhysics {
    ECS_DECLARE_COMPONENT(EcsSpatialQuery);
} FlecsSystemsPhysics;

FLECS_SYSTEMS_PHYSICS_EXPORT
void FlecsSystemsPhysicsImport(
    ecs_world_t *world);

#define FlecsSystemsPhysicsImportHandles(handles)\
    ECS_IMPORT_COMPONENT(EcsSpatialQuery)

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
