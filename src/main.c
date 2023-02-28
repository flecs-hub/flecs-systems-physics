#define FLECS_SYSTEMS_PHYSICS_IMPL
#include <flecs_systems_physics.h>

ECS_CTOR(EcsSpatialQuery, ptr, {
    ptr->query = NULL;
})

ECS_MOVE(EcsSpatialQuery, dst, src, {
    if (dst->query) {
        ecs_squery_free(dst->query);
    }

    dst->query = src->query;
    src->query = NULL;
})

ECS_DTOR(EcsSpatialQuery, ptr, {
    if (ptr->query) {
        ecs_squery_free(ptr->query);
    }
})

ECS_CTOR(EcsSpatialQueryResult, ptr, {
    ptr->results = (ecs_vec_t){0};
})

ECS_MOVE(EcsSpatialQueryResult, dst, src, {
    if (dst->results.array) {
        ecs_vec_fini_t(NULL, &dst->results, ecs_oct_entity_t);
    }

    dst->results = src->results;
    src->results = (ecs_vec_t){0};
})

ECS_DTOR(EcsSpatialQueryResult, ptr, {
    if (ptr->results.array) {
        ecs_vec_fini_t(NULL, &ptr->results, ecs_oct_entity_t);
    }
})

static
void EcsMove2(ecs_iter_t *it) {
    EcsPosition2 *p = ecs_field(it, EcsPosition2, 1);
    EcsVelocity2 *v = ecs_field(it, EcsVelocity2, 2);

    int i;
    for (i = 0; i < it->count; i ++) {
        p[i].x += v[i].x * it->delta_time;
        p[i].y += v[i].y * it->delta_time;
    }
}

static
void EcsMove3(ecs_iter_t *it) {
    EcsPosition3 *p = ecs_field(it, EcsPosition3, 1);
    EcsVelocity3 *v = ecs_field(it, EcsVelocity3, 2);

    int i;
    for (i = 0; i < it->count; i ++) {
        p[i].x += v[i].x * it->delta_time;
        p[i].y += v[i].y * it->delta_time;
        p[i].z += v[i].z * it->delta_time;
    }
}

static
void EcsRotate3(ecs_iter_t *it) {
    EcsRotation3 *r = ecs_field(it, EcsRotation3, 1);
    EcsAngularVelocity *a = ecs_field(it, EcsAngularVelocity, 2);

    int i;
    for (i = 0; i < it->count; i ++) {
        r[i].x += a[i].x * it->delta_time;
        r[i].y += a[i].y * it->delta_time;
        r[i].z += a[i].z * it->delta_time;
    }
}

static
void EcsAddBoxCollider(ecs_iter_t *it) {
    EcsBox *box = ecs_field(it, EcsBox, 2);
    ecs_entity_t C = ecs_field_id(it, 1);
    ecs_entity_t B = ecs_field_id(it, 2);

    int i;
    if (ecs_field_is_self(it, 2)) {
        for (i = 0; i < it->count; i ++) {
            ecs_entity_t pair = ecs_pair(C, B);
            EcsBox *collider = ecs_get_mut_id(
                it->world, it->entities[i], pair);
            ecs_os_memcpy_t(collider, &box[i], EcsBox);
        }
    } else {
        for (i = 0; i < it->count; i ++) {
            ecs_entity_t pair = ecs_pair(C, B);
            EcsBox *collider = ecs_get_mut_id(
                it->world, it->entities[i], pair);
            ecs_os_memcpy_t(collider, box, EcsBox);
        }
    }
}

static
void EcsOnSetSpatialQuery(ecs_iter_t *it) {
    EcsSpatialQuery *q = ecs_field(it, EcsSpatialQuery, 1);
    ecs_id_t id = ecs_field_id(it, 1);
    ecs_id_t filter = ecs_pair_second(it->world, id);

    for (int i = 0; i < it->count; i ++) {
        q[i].query = ecs_squery_new(it->world, filter, q[i].center, q[i].size);
        if (!q[i].query) {
            char *filter_str = ecs_id_str(it->world, filter);
            ecs_err("failed to create query for filter '%s'", filter_str);
            ecs_os_free(filter_str);
        }
    }
}

static
void EcsUpdateSpatialQuery(ecs_iter_t *it) {
    EcsSpatialQuery *q = ecs_field(it, EcsSpatialQuery, 1);

    int i;
    for (i = 0; i < it->count; i ++) {
        if (!q->query) {
            char *filter_str = ecs_id_str(it->world, ecs_field_id(it, 1));
            ecs_err("missing spatial query for '%s'", filter_str);
            ecs_os_free(filter_str);
        }

        ecs_squery_update(q->query);
    }
}

void FlecsSystemsPhysicsImport(
    ecs_world_t *world)
{
    ECS_MODULE(world, FlecsSystemsPhysics);
    ECS_IMPORT(world, FlecsComponentsTransform);
    ECS_IMPORT(world, FlecsComponentsPhysics);
    ECS_IMPORT(world, FlecsComponentsGeometry);

    ecs_set_name_prefix(world, "Ecs");

    ECS_COMPONENT_DEFINE(world, EcsSpatialQuery);
    ECS_COMPONENT_DEFINE(world, EcsSpatialQueryResult);

    ecs_set_hooks(world, EcsSpatialQuery, {
        .ctor = ecs_ctor(EcsSpatialQuery),
        .dtor = ecs_dtor(EcsSpatialQuery),
        .move = ecs_move(EcsSpatialQuery)
    });

    ecs_set_hooks(world, EcsSpatialQueryResult, {
        .ctor = ecs_ctor(EcsSpatialQueryResult),
        .dtor = ecs_dtor(EcsSpatialQueryResult),
        .move = ecs_move(EcsSpatialQueryResult)
    });    

    ECS_SYSTEM(world, EcsMove2, EcsOnUpdate, 
        [inout] flecs.components.transform.Position2,
        [in] flecs.components.physics.Velocity2);

    ECS_SYSTEM(world, EcsMove3, EcsOnUpdate, 
        [inout] flecs.components.transform.Position3,
        [in] flecs.components.physics.Velocity3);

    ECS_SYSTEM(world, EcsRotate3, EcsOnUpdate, 
        [inout] flecs.components.transform.Rotation3,
        [in] flecs.components.physics.AngularVelocity);

    ECS_SYSTEM(world, EcsAddBoxCollider, EcsPostLoad, 
        flecs.components.physics.Collider,
        flecs.components.geometry.Box(self|up),
        !(flecs.components.physics.Collider, flecs.components.geometry.Box));

    ECS_OBSERVER(world, EcsOnSetSpatialQuery, EcsOnSet,
        SpatialQuery(self, *), ?Prefab);

    ECS_SYSTEM(world, EcsUpdateSpatialQuery, EcsPreUpdate, 
        SpatialQuery(self, *), ?Prefab);

    ecs_system(world, { .entity = EcsMove2, .query.filter.instanced = true });
    ecs_system(world, { .entity = EcsMove3, .query.filter.instanced = true });
    ecs_system(world, { .entity = EcsRotate3, .query.filter.instanced = true });
}
