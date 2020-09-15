#ifndef FLECS_SYSTEMS_PHYSICS_H
#define FLECS_SYSTEMS_PHYSICS_H

/* This generated file contains includes for project dependencies */
#include "flecs-systems-physics/bake_config.h"
#include "flecs-systems-physics/octree.h"
#include "flecs-systems-physics/spatial_query.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct FlecsSystemsPhysics {
    int dummy_;
} FlecsSystemsPhysics;

FLECS_SYSTEMS_PHYSICS_EXPORT
void FlecsSystemsPhysicsImport(
    ecs_world_t *world);

#define FlecsSystemsPhysicsImportHandles(handles)

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus

namespace flecs {
namespace systems {

class physics : FlecsSystemsPhysics {
public:
    physics(flecs::world& ecs) {
        FlecsSystemsPhysicsImport(ecs.c_ptr());

        ecs.module<flecs::systems::physics>();
    }
};

}
}

#endif

#endif
