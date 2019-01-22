#include <include/physics.h>
#include <string.h>
#include <math.h>

void EcsMove2D_w_Rotation(EcsRows *rows) {
    void *row;
    for (row = rows->first; row < rows->last; row = ecs_next(rows, row)) {
        EcsPosition2D *p = ecs_data(rows, row, 0);
        EcsSpeed *speed = ecs_data(rows, row, 1);
        EcsRotation2D *r = ecs_data(rows, row, 2);
        float x_speed = 1;
        float y_speed = 0;

        x_speed = cos(r->angle) * speed->value;
        y_speed = sin(r->angle) * speed->value;

        p->x += x_speed * rows->delta_time;
        p->y += y_speed * rows->delta_time;
    }
}

void EcsMove2D_w_Velocity(EcsRows *rows) {
    void *row;
    for (row = rows->first; row < rows->last; row = ecs_next(rows, row)) {
        EcsPosition2D *p = ecs_data(rows, row, 0);
        EcsSpeed *speed = ecs_data(rows, row, 1);
        EcsVelocity2D *v = ecs_data(rows, row, 2);
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

void EcsRotate2D(EcsRows *rows) {
    void *row;
    for (row = rows->first; row < rows->last; row = ecs_next(rows, row)) {
        EcsRotation2D *r = ecs_data(rows, row, 0);
        EcsAngularSpeed *s = ecs_data(rows, row, 1);
        r->angle += s->value * rows->delta_time;;
    }
}

void EcsMove3D_w_Rotation(EcsRows *rows) {
    void *row;
    for (row = rows->first; row < rows->last; row = ecs_next(rows, row)) {
        EcsPosition3D *p = ecs_data(rows, row, 0);
        EcsSpeed *speed = ecs_data(rows, row, 1);
        EcsRotation3D *r = ecs_data(rows, row, 2);
        float x_speed = cos(r->z) * sin(r->y) * speed->value;
        float y_speed = cos(r->x) * sin(r->z) * speed->value;
        float z_speed = cos(r->y) * sin(r->x) * speed->value;
        p->x += x_speed * rows->delta_time;;
        p->y += y_speed * rows->delta_time;;
        p->z += z_speed * rows->delta_time;;
    }
}

void EcsMove3D_w_Velocity(EcsRows *rows) {
    void *row;
    for (row = rows->first; row < rows->last; row = ecs_next(rows, row)) {
        EcsPosition3D *p = ecs_data(rows, row, 0);
        EcsSpeed *speed = ecs_data(rows, row, 1);
        EcsVelocity3D *v = ecs_data(rows, row, 2);
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

void EcsRotate3D(EcsRows *rows) {
    void *row;
    for (row = rows->first; row < rows->last; row = ecs_next(rows, row)) {
        EcsRotation3D *r = ecs_data(rows, row, 0);
        EcsAngularSpeed *s = ecs_data(rows, row, 1);
        EcsAngularVelocity *v = ecs_data(rows, row, 2);
        float speed = 1;

        if (s) {
            speed = s->value;
        }

        r->x += v->x * speed * rows->delta_time;;
        r->y += v->y * speed * rows->delta_time;;
        r->z += v->z * speed * rows->delta_time;;
    }
}
