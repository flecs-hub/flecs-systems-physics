#ifndef FLECS_SYSTEMS_PHYSICS_SQUERY_H
#define FLECS_SYSTEMS_PHYSICS_SQUERY_H

#include "bake_config.h"
#include "octree.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ecs_squery_t ecs_squery_t;

FLECS_SYSTEMS_PHYSICS_EXPORT
ecs_squery_t* ecs_squery_new(
    ecs_world_t *world,
    const char *expr,
    vec3 center,
    float size);

FLECS_SYSTEMS_PHYSICS_EXPORT
void ecs_squery_free(
    ecs_squery_t *sq);

FLECS_SYSTEMS_PHYSICS_EXPORT
void ecs_squery_update(
    ecs_squery_t *sq);    

FLECS_SYSTEMS_PHYSICS_EXPORT
void ecs_squery_findn(
    const ecs_squery_t *sq,
    vec3 position,
    float range,
    ecs_vector_t **result);

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus

namespace flecs {

class squery {
public:
    using entity = ecs_oct_entity_t;

    squery() {
        sq_ = nullptr;
        rc_ = nullptr;
    }

    squery(flecs::world& world, const char *expr, vec3 center, float size) {
        sq_ = ecs_squery_new(world.c_ptr(), expr, center, size);
        rc_ = new int;
        rc_[0] = 1;
    }

    squery(const squery& obj) {
        ecs_assert(rc_ == nullptr, ECS_INVALID_PARAMETER, NULL);
        this->sq_ = obj.sq_;
        this->rc_ = obj.rc_;
        this->rc_[0] ++;
    }

    void update() {
        ecs_squery_update(sq_);
    }

    void findn(vec3 pos, float range, flecs::vector<squery::entity>& results) const {
        ecs_vector_t *v = results.ptr();
        ecs_squery_findn(sq_, pos, range, &v);
        results.ptr(v);
    }

    ~squery() {
        if (rc_) {
            if (--(rc_[0])) {
                ecs_squery_free(sq_);
                delete rc_;
            }
        }
    }

private:
    ecs_squery_t *sq_;
    int *rc_;
};

}

#endif

#endif
