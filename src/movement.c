#include <include/physics.h>
#include <string.h>
#include <math.h>

void EcsAddRotate2D(ecs_rows_t *rows) {
    EcsType TEcsRotation2D = ecs_column_type(rows, 2);

    int i;
    for (i = 0; i < rows->count; i ++) {
        ecs_set(rows->world, rows->entities[i], EcsRotation2D, {0});
    }
}

void EcsMove2D_w_Rotation(ecs_rows_t *rows) {
    int i;
    for (i = 0; i < rows->count; i ++) {
        EcsPosition2D *p = ecs_field(rows, EcsPosition2D, i, 1);
        EcsSpeed *speed = ecs_field(rows, EcsSpeed, i, 2);
        EcsRotation2D *r = ecs_field(rows, EcsRotation2D, i, 3);
        float x_speed = 1;
        float y_speed = 0;

        x_speed = cos(r->angle) * speed->value;
        y_speed = sin(r->angle) * speed->value;

        p->x += x_speed * rows->delta_time;
        p->y += y_speed * rows->delta_time;
    }
}

void EcsMove2D_w_Velocity(ecs_rows_t *rows) {
    int i;
    for (i = 0; i < rows->count; i ++) {
        EcsPosition2D *p = ecs_field(rows, EcsPosition2D, i, 1);
        EcsSpeed *speed = ecs_field(rows, EcsSpeed, i, 2);
        EcsVelocity2D *v = ecs_field(rows, EcsVelocity2D, i, 3);
        float x_speed = v->x;
        float y_speed = v->y;

        if (speed) {
            x_speed *= speed->value;
            y_speed *= speed->value;
        }

        p->x += x_speed * rows->delta_time;;
        p->y += y_speed * rows->delta_time;;
    }
}

void EcsRotate2D(ecs_rows_t *rows) {
    int i;
    for (i = 0; i < rows->count; i ++) {
        EcsRotation2D *r = ecs_field(rows, EcsRotation2D, i, 1);
        EcsAngularSpeed *s = ecs_field(rows, EcsAngularSpeed, i, 2);
        r->angle += s->value * rows->delta_time;;
    }
}

void EcsMove3D_w_Rotation(ecs_rows_t *rows) {
    int i;
    for (i = 0; i < rows->count; i ++) {
        EcsPosition3D *p = ecs_field(rows, EcsPosition3D, i, 1);
        EcsSpeed *speed = ecs_field(rows, EcsSpeed, i, 2);
        EcsRotation3D *r = ecs_field(rows, EcsRotation3D, i, 3);
        float x_speed = cos(r->z) * sin(r->y) * speed->value;
        float y_speed = cos(r->x) * sin(r->z) * speed->value;
        float z_speed = cos(r->y) * sin(r->x) * speed->value;
        p->x += x_speed * rows->delta_time;;
        p->y += y_speed * rows->delta_time;;
        p->z += z_speed * rows->delta_time;;
    }
}

void EcsMove3D_w_Velocity(ecs_rows_t *rows) {
    int i;
    for (i = 0; i < rows->count; i ++) {
        EcsPosition3D *p = ecs_field(rows, EcsPosition3D, i, 1);
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

        p->x += x_speed * rows->delta_time;;
        p->y += y_speed * rows->delta_time;;
    }
}

void EcsRotate3D(ecs_rows_t *rows) {
    int i;
    for (i = 0; i < rows->count; i ++) {
        EcsRotation3D *r = ecs_field(rows, EcsRotation3D, i, 1);
        EcsAngularSpeed *s = ecs_field(rows, EcsAngularSpeed, i, 2);
        EcsAngularVelocity *v = ecs_field(rows, EcsAngularVelocity, i, 3);
        float speed = 1;

        if (s) {
            speed = s->value;
        }

        r->x += v->x * speed * rows->delta_time;;
        r->y += v->y * speed * rows->delta_time;;
        r->z += v->z * speed * rows->delta_time;;
    }
}
