#ifndef REFLECS_SYSTEMS_PHYSICS_H
#define REFLECS_SYSTEMS_PHYSICS_H

#include "bake_config.h"
#include "physics_2d.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct EcsSystemsPhysicsHandles {
   EcsHandle Move2D_w_Rotation;
   EcsHandle Move2D_w_Velocity;
   EcsHandle Move2D;
   EcsHandle Move3D_w_Rotation;
   EcsHandle Move3D_w_Velocity;
   EcsHandle Move3D;
   EcsHandle Move;
   EcsHandle Rotate2D;
   EcsHandle Rotate3D;
   EcsHandle Rotate;
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
    EcsDeclareHandle(handles, Move);\
    EcsDeclareHandle(handles, Rotate2D);\
    EcsDeclareHandle(handles, Rotate3D);\
    EcsDeclareHandle(handles, Rotate);

#ifdef __cplusplus
}
#endif

#endif
