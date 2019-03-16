#ifndef FLECS_SYSTEMS_PHYSICS_H
#define FLECS_SYSTEMS_PHYSICS_H

#include "bake_config.h"
#include "physics_2d.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct EcsSystemsPhysicsHandles {
   ECS_DECLARE_SYSTEM(EcsMove2D_w_Rotation);
   ECS_DECLARE_SYSTEM(EcsMove2D_w_Velocity);
   ECS_DECLARE_COMPONENT(EcsMove2D);
   ECS_DECLARE_SYSTEM(EcsMove3D_w_Rotation);
   ECS_DECLARE_SYSTEM(EcsMove3D_w_Velocity);
   ECS_DECLARE_COMPONENT(EcsMove3D);
   ECS_DECLARE_SYSTEM(EcsRotate2D);
   ECS_DECLARE_SYSTEM(EcsRotate3D);
   ECS_DECLARE_COMPONENT(EcsMove);
   ECS_DECLARE_SYSTEM(EcsCollide);
} EcsSystemsPhysicsHandles;

void EcsSystemsPhysics(
    EcsWorld *world,
    int flags,
    void *handles_out);

#define EcsSystemsPhysics_ImportHandles(handles)\
    ECS_IMPORT_SYSTEM(handles, ECSMove2D_w_Rotation);\
    ECS_IMPORT_SYSTEM(handles, ECSMove2D_w_Velocity);\
    ECS_IMPORT_SYSTEM(handles, ECSMove2D);\
    ECS_IMPORT_SYSTEM(handles, ECSMove3D_w_Rotation);\
    ECS_IMPORT_SYSTEM(handles, ECSMove3D_w_Velocity);\
    ECS_IMPORT_SYSTEM(handles, ECSMove3D);\
    ECS_IMPORT_SYSTEM(handles, ECSRotate2D);\
    ECS_IMPORT_SYSTEM(handles, ECSRotate3D);\
    ECS_IMPORT_SYSTEM(handles, ECSMove);\
    ECS_IMPORT_SYSTEM(handles, ECSCollide);

#ifdef __cplusplus
}
#endif

#endif
