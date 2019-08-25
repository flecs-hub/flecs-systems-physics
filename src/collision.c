#include <flecs_systems_physics.h>
#include <string.h>
#include <math.h>

/* -- Automatically add colliders to geometry entities with EcsCollider -- */

static
void add_rect_collider(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entity_t ecs_entity(EcsPolygon8Collider),
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

void EcsAddColliderForSquare(ecs_rows_t *rows) {
    ecs_world_t *world = rows->world;
    ECS_COLUMN_COMPONENT(rows, EcsPolygon8Collider, 3);

    int i;
    for (i = 0; i < rows->count; i ++) {
        EcsSquare *s = ecs_field(rows, EcsSquare, 1, i);

        float w = s->size / 2.0;
        add_rect_collider(
            world, rows->entities[i], ecs_entity(EcsPolygon8Collider), w, w);
    }
}

void EcsAddColliderForRectangle(ecs_rows_t *rows) {
    ecs_world_t *world = rows->world;
    ECS_COLUMN_COMPONENT(rows, EcsPolygon8Collider, 3);

    int i;
    for (i = 0; i < rows->count; i ++) {
        EcsRectangle *s = ecs_field(rows, EcsRectangle, 1, i);

        float w = s->width / 2.0;
        float h = s->height / 2.0;

        add_rect_collider(world, rows->entities[i], ecs_entity(EcsPolygon8Collider), w, h);
    }
}

void EcsAddColliderForCircle(ecs_rows_t *rows) {
    ecs_world_t *world = rows->world;
    ECS_COLUMN_COMPONENT(rows, EcsCircleCollider, 3);

    int i;
    for (i = 0; i < rows->count; i ++) {
        EcsCircle *s = ecs_field(rows, EcsCircle, 1, i);

        ecs_set(world, rows->entities[i], EcsCircleCollider, {
            .position = {0, 0},
            .radius = s->radius
        });
    }
}


/* -- Add world colliders (colliders transformed to world space) -- */

void EcsAddPolygon8ColliderWorld(ecs_rows_t *rows) {
    ecs_world_t *world = rows->world;
    ECS_COLUMN_COMPONENT(rows, EcsPolygon8ColliderWorld, 2);

    int i;
    for (i = 0; i < rows->count; i ++) {
        EcsPolygon8Collider *collider = ecs_field(rows, EcsPolygon8Collider, 1, i);
        ecs_set_ptr(world, rows->entities[i], EcsPolygon8ColliderWorld, collider);
    }
}

void EcsAddCircleColliderWorld(ecs_rows_t *rows) {
    ecs_world_t *world = rows->world;
    ECS_COLUMN_COMPONENT(rows, EcsCircleColliderWorld, 2);

    int i;
    for (i = 0; i < rows->count; i ++) {
        EcsCircleCollider *collider = ecs_field(rows, EcsCircleCollider, 1, i);
        ecs_set_ptr(world, rows->entities[i], EcsCircleColliderWorld, collider);
    }
}


/* -- Transform colliders to world space -- */

void EcsTransformPolygon8Colliders(ecs_rows_t *rows) {
    int i;
    for (i = 0; i < rows->count; i ++) {
        EcsMatTransform2D *m = ecs_field(rows, EcsMatTransform2D, 1, i);
        EcsPolygon8Collider *c = ecs_field(rows, EcsPolygon8Collider, 2, i);
        EcsPolygon8ColliderWorld *cw = ecs_field(rows, EcsPolygon8ColliderWorld, 3, i);
        ecs_mat3x3_transform(m, c->points, cw->points, c->point_count);
        cw->point_count = c->point_count;
    }
}

void EcsTransformCircleColliders(ecs_rows_t *rows) {
    int i;
    for (i = 0; i < rows->count; i ++) {
        EcsMatTransform2D *m = ecs_field(rows, EcsMatTransform2D, 1, i);
        EcsCircleCollider *c = ecs_field(rows, EcsCircleCollider, 2, i);
        EcsCircleColliderWorld *cw = ecs_field(rows, EcsCircleColliderWorld, 3, i);
        ecs_mat3x3_transform(m, &c->position, &cw->position, 1);
    }
}

typedef struct TestColliderParam {
    ecs_entity_t entity;
    ecs_type_t collider_component;
    void *collider_data;
} TestColliderParam;

static
EcsPolygonCollider poly8_to_poly(
    EcsPolygon8ColliderWorld *collider)
{
    return (EcsPolygonCollider) {
        .point_count = collider->point_count,
        .points = collider->points
    };
}

void create_collision(
    ecs_world_t *world,
    ecs_entity_t entity_1,
    ecs_entity_t entity_2,
    EcsCollision2D *collision_data,
    ecs_entity_t ecs_entity(EcsCollision2D))
{
    ecs_assert(entity_1 != entity_2, ECS_INTERNAL_ERROR, NULL);

    collision_data->entity_1 = entity_1;
    collision_data->entity_2 = entity_2;
    
    ecs_set_ptr(world, 0, EcsCollision2D, collision_data);
}

void EcsTestColliders(ecs_rows_t *rows) {
    TestColliderParam *param = rows->param;
    ecs_world_t *world = rows->world;
    ecs_type_t collider = ecs_column_type(rows, 1);
    ECS_COLUMN_COMPONENT(rows, EcsPolygon8ColliderWorld, 2);
    ECS_COLUMN_COMPONENT(rows, EcsCircleColliderWorld, 3);
    ECS_COLUMN_COMPONENT(rows, EcsCollision2D, 4);
    EcsCollision2D collision;
    int i;

    if (param->collider_component == TEcsPolygon8ColliderWorld) {
        EcsPolygonCollider c1 = poly8_to_poly(param->collider_data);
        if (collider == TEcsPolygon8ColliderWorld) {
            for (i = 0; i < rows->count; i ++) {
                EcsPolygonCollider c2 = poly8_to_poly(
                    ecs_field(rows, EcsPolygon8ColliderWorld, 1, i));

                if (ecs_collider2d_poly(&c1, &c2, &collision)) {
                    create_collision(world, rows->entities[i], param->entity, 
                            &collision, ecs_entity(EcsCollision2D));
                }
            }
        } else if (collider == TEcsCircleColliderWorld) {
            for (i = 0; i < rows->count; i ++) {
                EcsCircleColliderWorld *c2 = ecs_field(rows, EcsCircleColliderWorld, 1, i);

                if (ecs_collider2d_poly_circle(&c1, c2, &collision)) {
                    create_collision(world, rows->entities[i], param->entity, 
                                    &collision, ecs_entity(EcsCollision2D));
                }
            }
        }
    } else if (param->collider_component == TEcsCircleColliderWorld) {
        EcsCircleColliderWorld *c1 = param->collider_data;
        if (collider == TEcsPolygon8ColliderWorld) {
            for (i = 0; i < rows->count; i ++) {
                EcsPolygonCollider c2 = poly8_to_poly(ecs_field(rows, EcsPolygon8ColliderWorld, 1, i));

                if (ecs_collider2d_circle_poly(c1, &c2, &collision)) {
                    create_collision(world, rows->entities[i], param->entity, &collision,
                                     ecs_entity(EcsCollision2D));
                }
            }
        } else if (collider == TEcsCircleColliderWorld) {
            for (i = 0; i < rows->count; i ++) {
                EcsCircleColliderWorld *c2 = ecs_field(rows, EcsCircleColliderWorld, 1, i);

                if (ecs_collider2d_circle(c1, c2, &collision)) {
                    create_collision(world, rows->entities[i], param->entity, 
                                    &collision, ecs_entity(EcsCollision2D));
                }
            }
        }
    }
}

void EcsWalkColliders(ecs_rows_t *rows) {
    ecs_world_t *world = rows->world;
    ecs_type_t collider = ecs_column_type(rows, 1);
    ecs_entity_t EcsTestColliders = ecs_column_entity(rows, 2);
    int i;
    
    for (i = 0; i < rows->count; i ++) {
        void *data = _ecs_field(rows, 0, 1, i);
        
        TestColliderParam param = {
            .entity = rows->entities[i],
            .collider_component = collider,
            .collider_data = data
        };

        ecs_run_w_filter(
            world,
            EcsTestColliders,
            rows->delta_time,
            rows->frame_offset + i + 1,
            0,
            0,
            &param);
    }
}

void EcsCleanCollisions(ecs_rows_t *rows) {
    ecs_world_t *world = rows->world;

    int i;
    for (i = 0; i < rows->count; i ++) {
        ecs_delete(world, rows->entities[i]);
    }
}
