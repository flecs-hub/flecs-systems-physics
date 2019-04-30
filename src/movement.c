#include <include/physics.h>
#include <string.h>
#include <math.h>

#define ECS_g (9.807)
#define ECS_air_density (1.225)

void EcsAddRotate2D(ecs_rows_t *rows) {
    ecs_type_t TEcsRotation2D = ecs_column_type(rows, 2);

    int i;
    for (i = 0; i < rows->count; i ++) {
        ecs_set(rows->world, rows->entities[i], EcsRotation2D, {0});
    }
}

void EcsMove2D_w_Rotation(ecs_rows_t *rows) {
    ECS_COLUMN(rows, EcsPosition2D, p, 1);

    int i;
    for (i = 0; i < rows->count; i ++) {
        EcsSpeed *speed = ecs_field(rows, EcsSpeed, i, 2);
        EcsRotation2D *r = ecs_field(rows, EcsRotation2D, i, 3);
        float x_speed = 1;
        float y_speed = 0;

        x_speed = cos(r->angle) * speed->value;
        y_speed = sin(r->angle) * speed->value;

        p[i].x += x_speed * rows->delta_time;
        p[i].y += y_speed * rows->delta_time;
    }
}

void EcsMove2D_w_Velocity(ecs_rows_t *rows) {
    ECS_COLUMN(rows, EcsPosition2D, p, 1);

    int i;
    for (i = 0; i < rows->count; i ++) {
        EcsSpeed *speed = ecs_field(rows, EcsSpeed, i, 2);
        EcsVelocity2D *v = ecs_field(rows, EcsVelocity2D, i, 3);
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
        EcsAngularSpeed *s = ecs_field(rows, EcsAngularSpeed, i, 2);
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
    ECS_COLUMN_COMPONENT(rows, EcsPosition2D, 2);
    ECS_COLUMN_COMPONENT(rows, EcsVelocity2D, 3);
    ECS_COLUMN_COMPONENT(rows, EcsBounciness, 4);

    int i;
    for (i = 0; i < rows->count; i ++) {
        EcsCollision2D *collision = ecs_field(rows, EcsCollision2D, i, 1);

        ecs_entity_t e1 = collision->entity_1;
        ecs_entity_t e2 = collision->entity_2;

        EcsVec2 normal = collision->normal;
        float distance = collision->distance;

        EcsVelocity2D *v1 = ecs_get_ptr(rows->world, e1, EcsVelocity2D);
        EcsVelocity2D *v2 = ecs_get_ptr(rows->world, e2, EcsVelocity2D);

        EcsBounciness b1 = 1.0, b2 = 1.0;
        EcsBounciness *b1_ptr = ecs_get_ptr(rows->world, e1, EcsBounciness);
        if (b1_ptr) {
            b1 = *b1_ptr;
        }
        
        EcsBounciness *b2_ptr = ecs_get_ptr(rows->world, e2, EcsBounciness);
        if (b2_ptr) {
            b2 = *b2_ptr;
        }
        
        float b = (b1 + b2) / 2.0;

        if (v1) {
            float move = distance;
            if (v2) {
                move *= 0.5;
            }

            EcsPosition2D *p = ecs_get_ptr(rows->world, e1, EcsPosition2D);        
            p->x += move * normal.x;
            p->y += move * normal.y;
            
            EcsVec2 norm = normal;
            ecs_vec2_mult(&norm, 1, &norm);
            ecs_vec2_reflect(v1, &norm, v1);
            ecs_vec2_mult(v1, b, v1);
        }

        if (v2) {
            float move = distance;
            if (v1) {
                move *= 0.5;
            }

            EcsPosition2D *p = ecs_get_ptr(rows->world, e2, EcsPosition2D);    
            p->x -= move * normal.x;
            p->y -= move * normal.y;

            EcsVec2 norm = normal;
            ecs_vec2_mult(&norm, 1, &norm);
            ecs_vec2_reflect(v2, &norm, v2);
            ecs_vec2_mult(v2, b, v2);
        }
    }
}

void EcsMove3D_w_Rotation(ecs_rows_t *rows) {
    ECS_COLUMN(rows, EcsPosition3D, p, 1);

    int i;
    for (i = 0; i < rows->count; i ++) {
        EcsSpeed *speed = ecs_field(rows, EcsSpeed, i, 2);
        EcsRotation3D *r = ecs_field(rows, EcsRotation3D, i, 3);
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
        EcsSpeed *speed = ecs_field(rows, EcsSpeed, i, 2);
        EcsVelocity3D *v = ecs_field(rows, EcsVelocity3D, i, 3);
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
        EcsAngularSpeed *s = ecs_field(rows, EcsAngularSpeed, i, 2);
        EcsAngularVelocity *v = ecs_field(rows, EcsAngularVelocity, i, 3);
        float speed = 1;

        if (s) {
            speed = s->value;
        }

        r[i].x += v->x * speed * rows->delta_time;;
        r[i].y += v->y * speed * rows->delta_time;;
        r[i].z += v->z * speed * rows->delta_time;;
    }
}
