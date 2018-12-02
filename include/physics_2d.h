#ifndef REFLECS_SYSTEMS_PHYSICS_2D_H
#define REFLECS_SYSTEMS_PHYSICS_2D_H

#include <reflecs/reflecs.h>
#include <reflecs/components/transform/transform.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct EcsCollision2DInfo {
    EcsVector2D normal;
    float distance;
} EcsCollision2DInfo;

/**
 * Components needed to represent a collider
 *  [0] : EcsVector2D
 *  [1] : EcsCircleCollider (Can be NULL)
 *  [2] : EcsPolygonCollider (Can be NULL)
 */
typedef void* EcsCollider2DData[3];

int8_t EcsPhysis2d_GetCollisionInfo(EcsCollider2DData *collider_a, EcsCollider2DData *collider_b, EcsCollision2DInfo *collision_out); 

#ifdef __cplusplus
}
#endif

#endif