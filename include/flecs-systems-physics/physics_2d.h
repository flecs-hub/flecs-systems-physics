#ifndef FLECS_SYSTEMS_PHYSICS_2D_H
#define FLECS_SYSTEMS_PHYSICS_2D_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct EcsPolygonCollider {
    int8_t point_count;
    EcsPoint2D *points;
} EcsPolygonCollider;

int8_t ecs_collider2d_poly(
    EcsPolygonCollider *poly_a,
    EcsPolygonCollider *poly_b,
    EcsCollision2D *collision_out);

int8_t ecs_collider2d_poly_circle(
    EcsPolygonCollider *poly,
    EcsCircleCollider *circle,
    EcsCollision2D *collision_out);

int8_t ecs_collider2d_circle_poly(
    EcsCircleCollider *circle,
    EcsPolygonCollider *poly,
    EcsCollision2D *collision_out);

int8_t ecs_collider2d_circle(
    EcsCircleCollider *circle_a,
    EcsCircleCollider *circle_b,
    EcsCollision2D *collision_out);

#ifdef __cplusplus
}
#endif

#endif
