#include <include/physics.h>
#include <string.h>
#include <math.h>

/* -- Automatically add colliders to geometry entities with EcsCollider -- */

static
void add_square_collider(
    EcsWorld *world,
    EcsEntity entity,
    EcsEntity EcsPolygon8Collider_h,
    float w,
    float h)
{
    ecs_set(world, entity, EcsPolygon8Collider, {
        .points = {
            {-w, -h},
            {w, -h},
            {w, h},
            {-w, h},
            {-w, -h}
        },
        .point_count = 5
    });
}

void EcsAddColliderForSquare(EcsRows *rows) {
    EcsWorld *world = rows->world;
    EcsEntity EcsPolygon8Collider_h = ecs_component(rows, 2);

    void *row;
    for (row = rows->first; row < rows->last; row = ecs_next(rows, row)) {
        EcsEntity entity = ecs_entity(rows, row, ECS_ROW_ENTITY);
        EcsSquare *s = ecs_data(rows, row, 0);

        float w = s->size / 2.0;
        add_square_collider(world, entity, EcsPolygon8Collider_h, w, w);
    }
}

void EcsAddColliderForRectangle(EcsRows *rows) {
    EcsWorld *world = rows->world;
    EcsEntity EcsPolygon8Collider_h = ecs_component(rows, 2);

    void *row;
    for (row = rows->first; row < rows->last; row = ecs_next(rows, row)) {
        EcsEntity entity = ecs_entity(rows, row, ECS_ROW_ENTITY);
        EcsRectangle *s = ecs_data(rows, row, 0);

        float w = s->width / 2.0;
        float h = s->height / 2.0;

        add_square_collider(world, entity, EcsPolygon8Collider_h, w, h);
    }
}

void EcsAddColliderForCircle(EcsRows *rows) {
    EcsWorld *world = rows->world;
    EcsEntity EcsCircleCollider_h = ecs_component(rows, 2);

    void *row;
    for (row = rows->first; row < rows->last; row = ecs_next(rows, row)) {
        EcsEntity entity = ecs_entity(rows, row, ECS_ROW_ENTITY);
        EcsCircle *s = ecs_data(rows, row, 0);

        ecs_set(world, entity, EcsCircleCollider, {
            .position = {0, 0},
            .radius = s->radius
        });
    }
}


/* -- Add world colliders (colliders transformed to world space) -- */

void EcsAddPolygon8ColliderWorld(EcsRows *rows) {
    EcsWorld *world = rows->world;
    EcsEntity EcsPolygon8ColliderWorld_h = ecs_component(rows, 1);

    void *row;
    for (row = rows->first; row < rows->last; row = ecs_next(rows, row)) {
        EcsEntity entity = ecs_entity(rows, row, ECS_ROW_ENTITY);
        EcsPolygon8Collider *collider = ecs_data(rows, row, 0);
        ecs_set_ptr(world, entity, EcsPolygon8ColliderWorld_h, collider);
    }
}

void EcsAddCircleColliderWorld(EcsRows *rows) {
    EcsWorld *world = rows->world;
    EcsEntity EcsCircleColliderWorld_h = ecs_component(rows, 1);

    void *row;
    for (row = rows->first; row < rows->last; row = ecs_next(rows, row)) {
        EcsEntity entity = ecs_entity(rows, row, ECS_ROW_ENTITY);
        EcsCircleCollider *collider = ecs_data(rows, row, 0);
        ecs_set_ptr(world, entity, EcsCircleColliderWorld_h, collider);
    }
}


/* -- Transform colliders to world space -- */

void EcsTransformPolygon8Colliders(EcsRows *rows) {
    void *row;
    for (row = rows->first; row < rows->last; row = ecs_next(rows, row)) {
        EcsMatTransform2D *m = ecs_data(rows, row, 0);
        EcsPolygon8Collider *c = ecs_data(rows, row, 1);
        EcsPolygon8ColliderWorld *cw = ecs_data(rows, row, 2);
        ecs_mat3x3_transform(m, c->points, cw->points, c->point_count);
        cw->point_count = c->point_count;
    }
}

void EcsTransformCircleColliders(EcsRows *rows) {
    void *row;
    for (row = rows->first; row < rows->last; row = ecs_next(rows, row)) {
        EcsMatTransform2D *m = ecs_data(rows, row, 0);
        EcsCircleCollider *c = ecs_data(rows, row, 1);
        EcsCircleColliderWorld *cw = ecs_data(rows, row, 2);
        ecs_mat3x3_transform(m, &c->position, &cw->position, 1);
    }
}

typedef struct TestColliderParam {
    EcsEntity entity;
    EcsEntity collider_component;
    void *collider_data;
} TestColliderParam;

static
EcsPolygonCollider poly8_to_poly(
    EcsPolygon8Collider *collider)
{
    return (EcsPolygonCollider) {
        .point_count = collider->point_count,
        .points = collider->points
    };
}

void create_collision(
    EcsWorld *world,
    EcsEntity entity_1,
    EcsEntity entity_2,
    EcsCollision2D *collision_data,
    EcsEntity EcsCollision2D_h)
{
    collision_data->entity_1 = entity_1;
    collision_data->entity_2 = entity_2;
    ecs_set_ptr(world, 0, EcsCollision2D_h, collision_data);
}

void EcsTestColliders(EcsRows *rows) {
    TestColliderParam *param = rows->param;
    EcsWorld *world = rows->world;
    EcsEntity collider = ecs_component(rows, 0);
    EcsEntity EcsPolygon8ColliderWorld_h = ecs_component(rows, 1);
    EcsEntity EcsCircleColliderWorld_h = ecs_component(rows, 2);
    EcsEntity EcsCollision2D_h = ecs_component(rows, 3);
    EcsCollision2D collision;
    void *row;

    if (param->collider_component == EcsPolygon8ColliderWorld_h) {
        EcsPolygonCollider c1 = poly8_to_poly(param->collider_data);
        if (collider == EcsPolygon8ColliderWorld_h) {
            for (row = rows->first; row < rows->last; row = ecs_next(rows, row)) {
                EcsPolygonCollider c2 = poly8_to_poly(ecs_data(rows, row, 0));
                EcsEntity entity = ecs_entity(rows, row, ECS_ROW_ENTITY);

                if (entity == param->entity)
                    continue;

                if (ecs_collider2d_poly(&c1, &c2, &collision)) {
                    create_collision(world, entity, param->entity, &collision,
                                     EcsCollision2D_h);
                }
            }
        } else if (collider == EcsCircleColliderWorld_h) {
            for (row = rows->first; row < rows->last; row = ecs_next(rows, row)) {
                EcsCircleColliderWorld *c2 = ecs_data(rows, row, 0);
                EcsEntity entity = ecs_entity(rows, row, ECS_ROW_ENTITY);

                if (entity == param->entity)
                    continue;

                if (ecs_collider2d_poly_circle(&c1, c2, &collision)) {
                    create_collision(world, entity, param->entity, &collision,
                                     EcsCollision2D_h);
                }
            }
        }
    } else if (param->collider_component == EcsCircleColliderWorld_h) {
        EcsCircleColliderWorld *c1 = param->collider_data;
        if (collider == EcsPolygon8ColliderWorld_h) {
            for (row = rows->first; row < rows->last; row = ecs_next(rows, row)) {
                EcsPolygonCollider c2 = poly8_to_poly(ecs_data(rows, row, 0));
                EcsEntity entity = ecs_entity(rows, row, ECS_ROW_ENTITY);

                if (entity == param->entity)
                    continue;

                if (ecs_collider2d_circle_poly(c1, &c2, &collision)) {
                    create_collision(world, entity, param->entity, &collision,
                                     EcsCollision2D_h);
                }
            }
        } else if (collider == EcsCircleColliderWorld_h) {
            for (row = rows->first; row < rows->last; row = ecs_next(rows, row)) {
                EcsCircleColliderWorld *c2 = ecs_data(rows, row, 0);
                EcsEntity entity = ecs_entity(rows, row, ECS_ROW_ENTITY);

                if (entity == param->entity)
                    continue;

                if (ecs_collider2d_circle(c1, c2, &collision)) {
                    create_collision(world, entity, param->entity, &collision,
                                     EcsCollision2D_h);
                }
            }
        }
    }
}

void EcsWalkColliders(EcsRows *rows) {
    EcsWorld *world = rows->world;
    EcsEntity collider = ecs_component(rows, 0);
    EcsEntity EcsTestColliders_h = ecs_component(rows, 1);
    void *row;

    uint32_t n = 1;
    for (row = rows->first; row < rows->last; row = ecs_next(rows, row)) {
        EcsEntity entity = ecs_entity(rows, row, ECS_ROW_ENTITY);
        void *data = ecs_data(rows, row, 0);
        TestColliderParam param = {
            .entity = entity,
            .collider_component = collider,
            .collider_data = data
        };

        ecs_run_w_filter(
            world,
            EcsTestColliders_h,
            rows->delta_time,
            n,
            0,
            0,
            &param);

        n ++;
    }
}

void EcsCleanCollisions(EcsRows *rows) {
    EcsWorld *world = rows->world;

    void *row;
    for (row = rows->first; row < rows->last; row = ecs_next(rows, row)) {
        EcsEntity e = ecs_entity(rows, row, ECS_ROW_ENTITY);
        ecs_delete(world, e);
    }
}
