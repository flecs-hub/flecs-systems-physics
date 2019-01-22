#include <include/physics.h>
#include <string.h>
#include <math.h>

void EcsSystemsPhysics(
    EcsWorld *world,
    int flags,
    void *handles_out)
{
    EcsSystemsPhysicsHandles *handles = handles_out;
    bool do_2d = !flags || flags & ECS_2D;
    bool do_3d = !flags || flags & ECS_3D;

    memset(handles, 0, sizeof(EcsSystemsPhysicsHandles));

    ECS_IMPORT(world, EcsComponentsGeometry, flags);

    if (do_2d) {
        ECS_SYSTEM(world, EcsMove2D_w_Rotation, EcsOnFrame,
            EcsPosition2D, EcsRotation2D, EcsSpeed, !EcsVelocity2D);

        ECS_SYSTEM(world, EcsMove2D_w_Velocity, EcsOnFrame,
            EcsPosition2D, ?EcsSpeed, EcsVelocity2D);

        ECS_SYSTEM(world, EcsRotate2D, EcsOnFrame,
            EcsRotation2D, EcsAngularSpeed);

        ECS_SYSTEM(world, EcsAddRotate2D, EcsOnLoad, EcsAngularSpeed, !EcsRotation2D);

        ECS_FAMILY(world, EcsMove2D, EcsMove2D_w_Rotation, EcsMove2D_w_Velocity);

        /* Auto-add colliders to geometry entities that have EcsCollider */

        /* TODO: when EcsCollider is defined on a prefab, colliders should be
         *       added to prefab as well instead of the derived entity */

        ECS_SYSTEM(world, EcsAddColliderForSquare,    EcsOnLoad, EcsSquare, EcsCollider, !EcsPolygon8Collider);
        ECS_SYSTEM(world, EcsAddColliderForRectangle, EcsOnLoad, EcsRectangle, EcsCollider, !EcsPolygon8Collider);
        ECS_SYSTEM(world, EcsAddColliderForCircle,    EcsOnLoad, EcsCircle, EcsCollider, !EcsCircleCollider);

        /* Add world space colliders */
        ECS_SYSTEM(world, EcsAddPolygon8ColliderWorld,  EcsOnLoad, EcsPolygon8Collider, !EcsPolygon8ColliderWorld);
        ECS_SYSTEM(world, EcsAddCircleColliderWorld,    EcsOnLoad, EcsCircleCollider, !EcsCircleColliderWorld);

        /* TODO: world space colliders should only be added to entities, never
         *       to prefabs */

        /* Transform colliders to world space */
        ECS_SYSTEM(world, EcsTransformPolygon8Colliders,  EcsPostFrame, EcsMatTransform2D, EcsPolygon8Collider, EcsPolygon8ColliderWorld);
        ECS_SYSTEM(world, EcsTransformCircleColliders,    EcsPostFrame, EcsMatTransform2D, EcsCircleCollider, EcsCircleColliderWorld);

        /* Do collision testing */
        ECS_SYSTEM(world, EcsTestColliders, EcsOnDemand,
            EcsPolygon8ColliderWorld | EcsCircleColliderWorld,
            ID.EcsPolygon8ColliderWorld,
            ID.EcsCircleColliderWorld,
            ID.EcsCollision2D);
        ECS_SYSTEM(world, EcsCleanCollisions, EcsPostFrame, EcsCollision2D);
        ECS_SYSTEM(world, EcsWalkColliders, EcsPostFrame, EcsPolygon8ColliderWorld | EcsCircleColliderWorld, ID.EcsTestColliders);

        ecs_add(world, EcsMove2D_w_Rotation_h, EcsHidden_h);
        ecs_add(world, EcsMove2D_w_Velocity_h, EcsHidden_h);
        ecs_add(world, EcsRotate2D_h, EcsHidden_h);
        ecs_add(world, EcsMove2D_h, EcsHidden_h);

        ecs_add(world, EcsAddColliderForSquare_h, EcsHidden_h);
        ecs_add(world, EcsAddColliderForRectangle_h, EcsHidden_h);
        ecs_add(world, EcsAddColliderForCircle_h, EcsHidden_h);

        handles->Move2D_w_Rotation = EcsMove2D_w_Rotation_h;
        handles->Move2D_w_Velocity = EcsMove2D_w_Velocity_h;
        handles->Move2D = EcsMove2D_h;
        handles->Rotate2D = EcsRotate2D_h;
    }

    if (do_3d) {
        ECS_SYSTEM(world, EcsMove3D_w_Rotation, EcsOnFrame,
            EcsPosition3D, ?EcsRotation3D, EcsSpeed, !EcsVelocity3D);

        ECS_SYSTEM(world, EcsMove3D_w_Velocity, EcsOnFrame,
            EcsPosition3D, ?EcsSpeed, EcsVelocity3D);

        ECS_SYSTEM(world, EcsRotate3D, EcsOnFrame,
            EcsRotation3D, ?EcsAngularSpeed, EcsAngularVelocity);

        ECS_FAMILY(world, EcsMove3D, EcsMove3D_w_Rotation, EcsMove3D_w_Velocity);

        ecs_add(world, EcsMove3D_w_Rotation_h, EcsHidden_h);
        ecs_add(world, EcsMove3D_w_Velocity_h, EcsHidden_h);
        ecs_add(world, EcsRotate3D_h, EcsHidden_h);
        ecs_add(world, EcsMove3D_h, EcsHidden_h);

        handles->Move3D_w_Rotation = EcsMove3D_w_Rotation_h;
        handles->Move3D_w_Velocity = EcsMove3D_w_Velocity_h;
        handles->Move3D = EcsMove3D_h;
        handles->Rotate3D = EcsRotate3D_h;
    }

    if (do_2d && do_3d) {
        ECS_FAMILY(world, EcsMove, EcsMove2D, EcsMove3D);
        ECS_FAMILY(world, EcsRotate, Rotate2D, EcsRotate3D);

        handles->Move = EcsMove_h;
        handles->Rotate = EcsRotate_h;
    } else if (!do_2d) {
        ECS_FAMILY(world, EcsMove, EcsMove3D);
        ECS_FAMILY(world, EcsRotate, EcsRotate3D);

        handles->Move = EcsMove_h;
        handles->Rotate = EcsRotate_h;
    } else if (!do_3d) {
        ECS_FAMILY(world, EcsMove, EcsMove2D);
        ECS_FAMILY(world, EcsRotate, EcsRotate2D);

        handles->Move = EcsMove_h;
        handles->Rotate = EcsRotate_h;
    }
}
