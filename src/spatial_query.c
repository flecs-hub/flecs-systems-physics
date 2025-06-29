#include <flecs_systems_physics.h>

struct ecs_squery_t {
    ecs_query_t *q;
    ecs_octree_t *ot;
};

#define EXPR_PREFIX\
    "[in] flecs.components.transform.Position3,"\
    "[in] (flecs.components.physics.Collider, flecs.components.geometry.Box) || flecs.components.geometry.Box,"

ecs_squery_t* ecs_squery_new(
    ecs_world_t *world,
    ecs_id_t filter,
    vec3 center,
    float size)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(size > 0, ECS_INVALID_PARAMETER, NULL);

    ecs_squery_t *result = ecs_os_calloc(sizeof(ecs_squery_t));

    result->q = ecs_query(world, {
        .terms = {
            { ecs_id(EcsPosition3), .inout = EcsIn },
            { ecs_pair(EcsCollider, ecs_id(EcsBox)), .inout = EcsIn, .oper = EcsOr }, 
            { ecs_id(EcsBox) },
            { filter, .inout = EcsIn }
        },
        .cache_kind = EcsQueryCacheAuto
    });

    result->ot = ecs_octree_new(center, size);

    ecs_assert(result->q != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(result->ot != NULL, ECS_INTERNAL_ERROR, NULL);

    return result;
}

void ecs_squery_free(
    ecs_squery_t *sq)
{
    ecs_query_fini(sq->q);
    ecs_octree_free(sq->ot);
    ecs_os_free(sq);
}

void ecs_squery_update(
    ecs_squery_t *sq)
{
    ecs_assert(sq != NULL,     ECS_INVALID_PARAMETER, NULL);
    ecs_assert(sq->q != NULL,  ECS_INVALID_PARAMETER, NULL);
    ecs_assert(sq->ot != NULL, ECS_INVALID_PARAMETER, NULL);

    if (ecs_query_changed(sq->q)) {
        ecs_octree_clear(sq->ot);

        const ecs_world_t *world = ecs_get_world(sq->q);
        ecs_iter_t it = ecs_query_iter(world, sq->q);
        while (ecs_query_next(&it)) {
            EcsPosition3 *p = ecs_field(&it, EcsPosition3, 0);
            EcsBox *b = ecs_field(&it, EcsBox, 1);

            if (ecs_field_is_self(&it, 1)) {
                int i;
                for (i = 0; i < it.count; i ++) {
                    vec3 vp, vs;
                    vp[0] = p[i].x;
                    vp[1] = p[i].y;
                    vp[2] = p[i].z;

                    vs[0] = b[i].width;
                    vs[1] = b[i].height;
                    vs[2] = b[i].depth;

                    ecs_octree_insert(sq->ot, it.entities[i], vp, vs);
                }
            } else {
                int i;
                for (i = 0; i < it.count; i ++) {
                    vec3 vp, vs;
                    vp[0] = p[i].x;
                    vp[1] = p[i].y;
                    vp[2] = p[i].z;

                    vs[0] = b->width;
                    vs[1] = b->height;
                    vs[2] = b->depth;

                    ecs_octree_insert(sq->ot, it.entities[i], vp, vs);
                }
            }
        }
    }
}

void ecs_squery_findn(
    const ecs_squery_t *sq,
    vec3 position,
    float range,
    ecs_vec_t *result)
{
    ecs_assert(sq != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(sq->q != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(sq->ot != NULL, ECS_INVALID_PARAMETER, NULL);    

    ecs_octree_findn(sq->ot, position, range, result);
}
