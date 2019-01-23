#ifndef REFLECS_SYSTEMS_PHYSICS_H
#define REFLECS_SYSTEMS_PHYSICS_H

#include "bake_config.h"
#include "physics_2d.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct EcsSystemsPhysicsHandles {
   EcsEntity Move2D_w_Rotation;
   EcsEntity Move2D_w_Velocity;
   EcsEntity Move2D;
   EcsEntity Move3D_w_Rotation;
   EcsEntity Move3D_w_Velocity;
   EcsEntity Move3D;
   EcsEntity Rotate2D;
   EcsEntity Rotate3D;
   EcsEntity Move;
   EcsEntity Collide;
} EcsSystemsPhysicsHandles;

void EcsSystemsPhysics(
    EcsWorld *world,
    int flags,
    void *handles_out);

#define EcsSystemsPhysics_DeclareHandles(handles)\
    EcsDeclareHandle(handles, Move2D_w_Rotation);\
    EcsDeclareHandle(handles, Move2D_w_Velocity);\
    EcsDeclareHandle(handles, Move2D);\
    EcsDeclareHandle(handles, Move3D_w_Rotation);\
    EcsDeclareHandle(handles, Move3D_w_Velocity);\
    EcsDeclareHandle(handles, Move3D);\
    EcsDeclareHandle(handles, Rotate2D);\
    EcsDeclareHandle(handles, Rotate3D);\
    EcsDeclareHandle(handles, Move);\
    EcsDeclareHandle(handles, Collide);

#ifdef __cplusplus
}
#endif

#endif
