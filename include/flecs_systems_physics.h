#ifndef FLECS_SYSTEMS_PHYSICS_H
#define FLECS_SYSTEMS_PHYSICS_H

#include <flecs-systems-physics/bake_config.h>
#include <flecs-systems-physics/physics_2d.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct FlecsSystemsPhysics {
   ECS_DECLARE_ENTITY(EcsMove2D_w_Rotation);
   ECS_DECLARE_ENTITY(EcsMove2D_w_Velocity);
   ECS_DECLARE_ENTITY(EcsMove2D);
   ECS_DECLARE_ENTITY(EcsMove3D_w_Rotation);
   ECS_DECLARE_ENTITY(EcsMove3D_w_Velocity);
   ECS_DECLARE_ENTITY(EcsMove3D);
   ECS_DECLARE_ENTITY(EcsRotate2D);
   ECS_DECLARE_ENTITY(EcsRotate3D);
   ECS_DECLARE_ENTITY(EcsMove);
   ECS_DECLARE_ENTITY(EcsCollide);
} FlecsSystemsPhysics;

void FlecsSystemsPhysicsImport(
    ecs_world_t *world,
    int flags);

#define FlecsSystemsPhysicsImportHandles(handles)\
    ECS_IMPORT_ENTITY(handles, EcsMove2D_w_Rotation);\
    ECS_IMPORT_ENTITY(handles, EcsMove2D_w_Velocity);\
    ECS_IMPORT_ENTITY(handles, EcsMove2D);\
    ECS_IMPORT_ENTITY(handles, EcsMove3D_w_Rotation);\
    ECS_IMPORT_ENTITY(handles, EcsMove3D_w_Velocity);\
    ECS_IMPORT_ENTITY(handles, EcsMove3D);\
    ECS_IMPORT_ENTITY(handles, EcsRotate2D);\
    ECS_IMPORT_ENTITY(handles, EcsRotate3D);\
    ECS_IMPORT_ENTITY(handles, EcsMove);\
    ECS_IMPORT_ENTITY(handles, EcsCollide);

#ifdef __cplusplus
}
#endif

#endif
