#ifndef FLECS_SYSTEMS_SYSTEMS_H
#define FLECS_SYSTEMS_SYSTEMS_H

#include <flecs_systems_physics.h>

void EcsAddColliderForSquare(ecs_rows_t *rows);

void EcsAddColliderForRectangle(ecs_rows_t *rows);

void EcsAddColliderForCircle(ecs_rows_t *rows);

void EcsAddPolygon8ColliderWorld(ecs_rows_t *rows);

void EcsAddCircleColliderWorld(ecs_rows_t *rows);

void EcsTransformPolygon8Colliders(ecs_rows_t *rows);

void EcsTransformCircleColliders(ecs_rows_t *rows);

void EcsTestColliders(ecs_rows_t *rows);

void EcsWalkColliders(ecs_rows_t *rows);

void EcsCleanCollisions(ecs_rows_t *rows);

void EcsAddRotate2D(ecs_rows_t *rows);

void EcsMove2D_w_Rotation(ecs_rows_t *rows);

void EcsMove2D_w_Velocity(ecs_rows_t *rows);

void EcsRotate2D(ecs_rows_t *rows);

void EcsGravity2D(ecs_rows_t *rows);

void EcsCollide2D(ecs_rows_t *rows);

void EcsMove3D_w_Rotation(ecs_rows_t *rows);

void EcsMove3D_w_Velocity(ecs_rows_t *rows);

void EcsRotate3D(ecs_rows_t *rows);

#endif