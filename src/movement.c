#include <flecs_systems_physics.h>
#include <string.h>
#include <math.h>

#define ECS_g (9.807)
#define ECS_air_density (1.225)

void EcsAddRotate2D(ecs_rows_t *rows) {
    ecs_entity_t ecs_entity(EcsRotation2D) = ecs_column_entity(rows, 2);

    int i;
    for (i = 0; i < rows->count; i ++) {
        ecs_set(rows->world, rows->entities[i], EcsRotation2D, {0});
    }
}

void EcsMove2D_w_Rotation(ecs_rows_t *rows) {
    ECS_COLUMN(rows, EcsPosition2D, p, 1);

    int i;
    for (i = 0; i < rows->count; i ++) {
        EcsRotation2D *r = ecs_field(rows, EcsRotation2D, 2, i);
        EcsSpeed *speed = ecs_field(rows, EcsSpeed, 3, i);
        float x_speed = 0;
        float y_speed = 0;

        x_speed = cos(r->angle - M_PI / 2.0) * speed->value;
        y_speed = sin(r->angle - M_PI / 2.0) * speed->value;

        p[i].x += x_speed * rows->delta_time;
        p[i].y += y_speed * rows->delta_time;
    }
}

void EcsMove2D_w_Velocity(ecs_rows_t *rows) {
    ECS_COLUMN(rows, EcsPosition2D, p, 1);

    int i;
    for (i = 0; i < rows->count; i ++) {
        EcsSpeed *speed = ecs_field(rows, EcsSpeed, 2, i);
        EcsVelocity2D *v = ecs_field(rows, EcsVelocity2D, 3, i);
        float x_speed = v->x;
        float y_speed = v->y;

        if (speed) {
            x_speed *= speed->value;
            y_speed *= speed->value;
        }
        
        p[i].x += x_speed * rows->delta_time;
        p[i].y += y_speed * rows->delta_time;
    }
}

void EcsRotate2D(ecs_rows_t *rows) {
    ECS_COLUMN(rows, EcsRotation2D, r, 1);

    int i;
    for (i = 0; i < rows->count; i ++) {
        EcsAngularSpeed *s = ecs_field(rows, EcsAngularSpeed, 2, i);
        r[i].angle += s->value * rows->delta_time;;
    }
}

void EcsGravity2D(ecs_rows_t *rows) {
    ECS_COLUMN(rows, EcsVelocity2D, v, 1);

    int i;
    for (i = 0; i < rows->count; i ++) {
        v[i].y += ECS_g * rows->delta_time;
    }
}

void EcsCollide2D(ecs_rows_t *rows) {
    ecs_world_t *world = rows->world;

    ECS_COLUMN_COMPONENT(rows, EcsPosition2D, 2);
    ECS_COLUMN_COMPONENT(rows, EcsVelocity2D, 3);
    ECS_COLUMN_COMPONENT(rows, EcsBounciness, 4);
    ECS_COLUMN_COMPONENT(rows, EcsFriction, 5);
    ECS_COLUMN_COMPONENT(rows, EcsRigidBody, 6);
    int i;
    for (i = 0; i < rows->count; i ++) {
        EcsCollision2D *collision = ecs_field(rows, EcsCollision2D, 1, i);

        ecs_entity_t e1 = collision->entity_1;
        ecs_entity_t e2 = collision->entity_2;

        bool is_rigid_1 = ecs_has(world, e1, EcsRigidBody);
        bool is_rigid_2 = ecs_has(world, e2, EcsRigidBody);

        EcsVec2 normal = collision->normal;
        float distance = collision->distance + 0.001f;

        EcsVelocity2D *v1 = ecs_get_ptr(world, e1, EcsVelocity2D);
        EcsVelocity2D *v2 = ecs_get_ptr(world, e2, EcsVelocity2D);

        EcsBounciness b1 = 0, b2 = 0;
        EcsBounciness *b1_ptr = ecs_get_ptr(world, e1, EcsBounciness);
        if (b1_ptr) {
            b1 = *b1_ptr;
        }
        
        EcsBounciness *b2_ptr = ecs_get_ptr(world, e2, EcsBounciness);
        if (b2_ptr) {
            b2 = *b2_ptr;
        }
        
        float b = (b1 + b2);

        bool is_dynamic1 = (v1 && is_rigid_1);
        bool is_dynamic2 = (v2 && is_rigid_2);

        if (is_dynamic1 || is_dynamic2) {
            if (!is_dynamic2) {
                EcsPosition2D *p = ecs_get_ptr(world, e1, EcsPosition2D);        
                p->x += (distance) * normal.x;
                p->y += (distance) * normal.y;
                
                EcsVec2 v1x;
                EcsVec2 v1y;

                ecs_vec2_mult(&normal, ecs_vec2_dot(&normal, v1), &v1x);
                ecs_vec2_sub(v1, &v1x, &v1y);

                ecs_vec2_mult(&v1x, -1*b, v1);
                ecs_vec2_add(v1, &v1y, v1);
            } else if (!is_dynamic1) {
                EcsPosition2D *p = ecs_get_ptr(world, e2, EcsPosition2D);    
                p->x -= (distance) * normal.x;
                p->y -= (distance) * normal.y;

                EcsVec2 norm = normal;
                ecs_vec2_mult(&norm, -1, &norm);
                EcsVec2 v1x;
                EcsVec2 v1y;
                ecs_vec2_mult(&norm, ecs_vec2_dot(&norm, v2), &v1x);
                ecs_vec2_sub(v2, &v1x, &v1y);
                
                ecs_vec2_mult(&v1x, -1*b, v2);
                ecs_vec2_add(v2, &v1y, v2);
                
            } else {
                EcsPosition2D *p1 = ecs_get_ptr(world, e1, EcsPosition2D);   
                EcsPosition2D *p2 = ecs_get_ptr(world, e2, EcsPosition2D);   

                EcsVec2 temp = normal;
                ecs_vec2_mult(&temp, distance/2, &temp);
                ecs_vec2_add(p1, &temp, p1);
                ecs_vec2_sub(p2, &temp, p2);

                EcsVec2 v1x;
                EcsVec2 v1y;
                EcsVec2 v2x;
                EcsVec2 v2y;

                ecs_vec2_mult(&normal, ecs_vec2_dot(&normal, v1), &v1x);
                ecs_vec2_sub(v1, &v1x, &v1y);

                ecs_vec2_mult(&normal, -1, &normal);
                ecs_vec2_mult(&normal, ecs_vec2_dot(&normal, v2), &v2x);
                ecs_vec2_sub(v2, &v2x, &v2y);

                float m1 = 1;
                float m2 = 1;
                float cm = m1 + m2;

                ecs_vec2_mult(&v1x, ((m1-m2)/cm) * (b/2), &temp);
                ecs_vec2_mult(&v2x, (( 2*m2)/cm) * (b/2), v1);

                ecs_vec2_add(v1, &temp, v1);
                ecs_vec2_add(v1, &v1y, v1);
                //
                ecs_vec2_mult(&v1x, (( 2*m1)/cm) * (b/2), &temp);
                ecs_vec2_mult(&v2x, ((m2-m1)/cm) * (b/2), v2);

                ecs_vec2_add(v2, &temp, v2);
                ecs_vec2_add(v2, &v2y, v2);
            }
        }
    }
}

void EcsMove3D_w_Rotation(ecs_rows_t *rows) {
    ECS_COLUMN(rows, EcsPosition3D, p, 1);

    int i;
    for (i = 0; i < rows->count; i ++) {
        EcsSpeed *speed = ecs_field(rows, EcsSpeed, 2, i);
        EcsRotation3D *r = ecs_field(rows, EcsRotation3D, 3, i);
        float x_speed = cos(r->z) * sin(r->y) * speed->value;
        float y_speed = cos(r->x) * sin(r->z) * speed->value;
        float z_speed = cos(r->y) * sin(r->x) * speed->value;
        p[i].x += x_speed * rows->delta_time;;
        p[i].y += y_speed * rows->delta_time;;
        p[i].z += z_speed * rows->delta_time;;
    }
}

void EcsMove3D_w_Velocity(ecs_rows_t *rows) {
    ECS_COLUMN(rows, EcsPosition3D, p, 1);

    int i;
    for (i = 0; i < rows->count; i ++) {
        EcsSpeed *speed = ecs_field(rows, EcsSpeed, 2, i);
        EcsVelocity3D *v = ecs_field(rows, EcsVelocity3D, 3, i);
        float x_speed = v->x;
        float y_speed = v->y;
        float z_speed = v->z;

        if (speed) {
            x_speed *= speed->value;
            y_speed *= speed->value;
            z_speed *= speed->value;
        }

        p[i].x += x_speed * rows->delta_time;;
        p[i].y += y_speed * rows->delta_time;;
    }
}

void EcsRotate3D(ecs_rows_t *rows) {
    ECS_COLUMN(rows, EcsRotation3D, r, 1);

    int i;
    for (i = 0; i < rows->count; i ++) {
        EcsAngularSpeed *s = ecs_field(rows, EcsAngularSpeed, 2, i);
        EcsAngularVelocity *v = ecs_field(rows, EcsAngularVelocity, 3, i);
        float speed = 1;

        if (s) {
            speed = s->value;
        }

        r[i].x += v->x * speed * rows->delta_time;;
        r[i].y += v->y * speed * rows->delta_time;;
        r[i].z += v->z * speed * rows->delta_time;;
    }
}
