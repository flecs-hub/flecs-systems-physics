#ifndef REFLECS_SYSTEMS_PHYSICS_2D_H
#define REFLECS_SYSTEMS_PHYSICS_2D_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct EcsCollision2D {
    EcsVec2 normal;
    float distance;
} EcsCollision2D;

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
