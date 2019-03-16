#ifndef FLECS_SYSTEMS_SYSTEMS_H
#define FLECS_SYSTEMS_SYSTEMS_H

#include <include/bake_config.h>

void EcsAddColliderForSquare(EcsRows *rows);

void EcsAddColliderForRectangle(EcsRows *rows);

void EcsAddColliderForCircle(EcsRows *rows);

void EcsAddPolygon8ColliderWorld(EcsRows *rows);

void EcsAddCircleColliderWorld(EcsRows *rows);

void EcsTransformPolygon8Colliders(EcsRows *rows);

void EcsTransformCircleColliders(EcsRows *rows);

void EcsTestColliders(EcsRows *rows);

void EcsWalkColliders(EcsRows *rows);

void EcsCleanCollisions(EcsRows *rows);

void EcsAddRotate2D(EcsRows *rows);

void EcsMove2D_w_Rotation(EcsRows *rows);

void EcsMove2D_w_Velocity(EcsRows *rows);

void EcsRotate2D(EcsRows *rows);

void EcsMove3D_w_Rotation(EcsRows *rows);

void EcsMove3D_w_Velocity(EcsRows *rows);

void EcsRotate3D(EcsRows *rows);

#endif