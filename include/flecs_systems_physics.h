#ifndef FLECS_SYSTEMS_PHYSICS_H
#define FLECS_SYSTEMS_PHYSICS_H

/* This generated file contains includes for project dependencies */
#include "flecs-systems-physics/bake_config.h"
#include "flecs-systems-physics/octree.h"
#include "flecs-systems-physics/spatial_query.h"

// Don't use reflection, but use utility macro's for auto-exporting variables
#undef ECS_META_IMPL
#ifndef FLECS_SYSTEMS_PHYSICS_IMPL
#define ECS_META_IMPL EXTERN
#else
#define ECS_META_IMPL DECLARE
#endif

#ifdef __cplusplus
extern "C" {
#endif

FLECS_SYSTEMS_PHYSICS_API
ECS_STRUCT(EcsSpatialQuery, {
    vec3 center;
    float size;
    ecs_squery_t *query;
});

FLECS_SYSTEMS_PHYSICS_API
ECS_STRUCT(EcsSpatialQueryResult, {
    ecs_vec_t results;
});

FLECS_SYSTEMS_PHYSICS_API
void FlecsSystemsPhysicsImport(
    ecs_world_t *world);

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
#ifndef FLECS_NO_CPP

namespace flecs {
namespace systems {

class physics {
public:
    using oct_entity_t = ecs_oct_entity_t;

    struct SpatialQuery : EcsSpatialQuery {
        SpatialQuery() {
            ecs_os_zeromem(center);
            size = 0;
            query = nullptr;
        }

        SpatialQuery(ecs_squery_t *q) {
            query = q;
        }

        SpatialQuery(vec3 c, float s, ecs_squery_t *q = nullptr) {
            ecs_os_memcpy_t(center, c, vec3);
            size = s;
            query = q;
        }

        void update() {
            ecs_squery_update(query);
        }

        void findn(vec3 pos, float range, EcsSpatialQueryResult& qr) const {
            ecs_squery_findn(query, pos, range, &qr.results);
        }
    };

    struct SpatialQueryResult : EcsSpatialQueryResult {        
        oct_entity_t* begin() {
            return static_cast<oct_entity_t*>(results.array);
        }

        oct_entity_t* end() {
            return &static_cast<oct_entity_t*>(results.array)[results.count];
        }
    };

    physics(flecs::world& ecs) {
        // Load module contents
        FlecsSystemsPhysicsImport(ecs);

        // Bind C++ types with module contents
        ecs.module<flecs::systems::physics>();
        ecs.component<SpatialQuery>();
        ecs.component<SpatialQueryResult>();
    }
};

}
}

#endif
#endif

#endif
