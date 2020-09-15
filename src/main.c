#include <flecs_systems_physics.h>

static
void EcsMove2(ecs_iter_t *it) {
    EcsPosition2 *p = ecs_column(it, EcsPosition2, 1);
    EcsVelocity2 *v = ecs_column(it, EcsVelocity2, 1);

    int i;
    for (i = 0; i < it->count; i ++) {
        p[i].x += v[i].x;
        p[i].y += v[i].y;
    }
}

static
void EcsMove3(ecs_iter_t *it) {
    EcsPosition3 *p = ecs_column(it, EcsPosition3, 1);
    EcsVelocity3 *v = ecs_column(it, EcsVelocity3, 2);

    int i;
    for (i = 0; i < it->count; i ++) {
        p[i].x += v[i].x;
        p[i].y += v[i].y;
        p[i].z += v[i].z;
    }
}

void FlecsSystemsPhysicsImport(
    ecs_world_t *world)
{
    ECS_MODULE(world, FlecsSystemsPhysics);

    ECS_IMPORT(world, FlecsComponentsTransform);
    ECS_IMPORT(world, FlecsComponentsPhysics);

    ecs_set_name_prefix(world, "Ecs");

    ECS_SYSTEM(world, EcsMove2, EcsOnUpdate, 
        flecs.components.transform.Position2,
        flecs.components.physics.Velocity2,
        SYSTEM:Hidden);

    ECS_SYSTEM(world, EcsMove3, EcsOnUpdate, 
        flecs.components.transform.Position3,
        flecs.components.physics.Velocity3,
        SYSTEM:Hidden);
}
