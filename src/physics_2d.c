#include <flecs_systems_physics.h>
#include <math.h>

#define AXIS_MIN(minMax) ((minMax).x)
#define AXIS_MAX(minMax) ((minMax).y)

static
void get_projection(
    EcsVec2 *axis,
    EcsPoint2D *vertices, int8_t size,
    EcsVec2 *out)
{
    float min = ecs_vec2_dot(axis, &vertices[0]);
    float max = min;
    for (int i = 1; i < size; i++) {
        float t = ecs_vec2_dot(axis, &vertices[i]);
        if (t < min) {
            min = t;
        }
        if (t > max) {
            max = t;
        }
    }
    out->x = min;
    out->y = max;
}

static
void get_projection_circle(
    EcsVec2 *axis,
    EcsVec2 *center, float radius,
    EcsVec2 *out)
{
    float mid = ecs_vec2_dot(axis, center);
    out->x = mid - radius;
    out->y = mid + radius;
}

static
void get_closest_point(
    EcsVec2 *position,
    EcsPoint2D *vertices, int8_t size,
    EcsVec2 *out)
{
    float distance = MAXFLOAT;
    float currDist;

    for (int i = 0; i < size; i++) {
        currDist = ecs_vec2_distanceSqrt(position, &vertices[i]);
        if (currDist < distance) {
            distance = currDist;
            out->x = vertices[i].x;
            out->y = vertices[i].y;
        }
    }
}

static
int8_t check_circle_circle(
    EcsCircleCollider *circle_a,
    EcsCircleCollider *circle_b,
    EcsCollision2D *collision_out)
{
    float totalRadius = circle_a->radius + circle_b->radius;
    float distSqrt = ecs_vec2_distanceSqrt(
        &circle_a->position, &circle_b->position);

    if (distSqrt > totalRadius * totalRadius) {
        return false;
    }

    ecs_vec2_sub(
        &circle_b->position, &circle_a->position, &(collision_out->normal));

    ecs_vec2_normalize(&(collision_out->normal), &(collision_out->normal));

    collision_out->distance = totalRadius - sqrtf(distSqrt);

    return true;
}

static
void check_axis_sat(
    EcsVec2 *axis,
    EcsVec2 *minMaxA,
    EcsVec2 *minMaxB,
    EcsCollision2D *collision_out)
{
    float distmin;
    if (AXIS_MIN(*minMaxA) < AXIS_MIN(*minMaxB)) {
        distmin = AXIS_MIN(*minMaxB) - AXIS_MAX(*minMaxA);
        if (distmin < 0) {
            distmin = -distmin;
            ecs_vec2_mult(axis, -1, axis);
        }
    } else {
        distmin = AXIS_MIN(*minMaxA) - AXIS_MAX(*minMaxB);
        if (distmin < 0) {
            distmin = -distmin;
        } else {
            ecs_vec2_mult(axis, -1, axis);
        }
    }
    if (distmin <= collision_out->distance) {
        collision_out->distance = distmin;
        collision_out->normal.x = axis->x;
        collision_out->normal.y = axis->y;
    }
}

static
float check_poly_sat_axis(
    EcsPoint2D *vertices_a, int8_t size_a,
    EcsPoint2D *vertices_b, int8_t size_b,
    EcsCollision2D *collision_out, int8_t invert)
{
    EcsVec2 axis;
    EcsVec2 minMaxA;
    EcsVec2 minMaxB;

    for (int8_t i = 0; i < size_a; i++){
        ecs_vec2_sub(
            &vertices_a[(i+1) < size_a ? (i+1) : 0], &vertices_a[i], &axis);
        ecs_vec2_perpendicular(&axis, &axis);
        ecs_vec2_normalize(&axis, &axis);
        if (invert) {
            get_projection(&axis, vertices_a, size_a, &minMaxB);
            get_projection(&axis, vertices_b, size_b, &minMaxA);
        } else {
            get_projection(&axis, vertices_a, size_a, &minMaxA);
            get_projection(&axis, vertices_b, size_b, &minMaxB);
        }

        //max0 < min1 || max1 < min0
        if (AXIS_MAX(minMaxA) < AXIS_MIN(minMaxB) ||
            AXIS_MAX(minMaxB) < AXIS_MIN(minMaxA))
        {
            return INFINITY;
        }

        check_axis_sat(&axis, &minMaxA, &minMaxB, collision_out);
    }
    return collision_out->distance;
}

static
int8_t check_poly_sat(
    EcsPolygonCollider *polygon_a,
    EcsPolygonCollider *polygon_b,
    EcsCollision2D *collision_out)
{
    collision_out->distance = INFINITY;

    if (check_poly_sat_axis(polygon_a->points, polygon_a->point_count,
                            polygon_b->points, polygon_b->point_count,
                            collision_out, false) == INFINITY) {
        return false;
    }

    if (check_poly_sat_axis(polygon_b->points, polygon_b->point_count,
                            polygon_a->points, polygon_a->point_count,
                            collision_out, true) == INFINITY) {
        return false;
    }

    return true;
}

static
int8_t check_circle_poly_sat(
    EcsCircleCollider *circle,
    EcsPolygonCollider *polygon,
    EcsCollision2D *collision_out)
{
    EcsVec2 axis;
    EcsVec2 closestPoint;
    EcsVec2 minMaxA;
    EcsVec2 minMaxB;

    int8_t point_count = polygon->point_count;
    EcsVec2 *points = polygon->points;

    get_closest_point(&circle->position, points, point_count, &closestPoint);
    ecs_vec2_sub(&circle->position, &closestPoint, &axis);
    ecs_vec2_normalize(&axis, &axis);

    get_projection(&axis, points, point_count, &minMaxA);
    get_projection_circle(
        &axis, &circle->position, circle->radius, &minMaxB);

    //max0 < min1 || max1 < min0
    if (AXIS_MAX(minMaxA) < AXIS_MIN(minMaxB) ||
        AXIS_MAX(minMaxB) < AXIS_MIN(minMaxA))
    {
        return false;
    }
    collision_out->distance = INFINITY;
    check_axis_sat(&axis, &minMaxA, &minMaxB, collision_out);

    for (int8_t i = 0; i < point_count; i++){
        ecs_vec2_sub(
            &points[(i+1) < point_count ? (i+1) : 0], &points[i], &axis);

        ecs_vec2_perpendicular(&axis, &axis);
        ecs_vec2_normalize(&axis, &axis);

        get_projection(&axis, points, point_count, &minMaxA);
        get_projection_circle(
            &axis, &circle->position, circle->radius, &minMaxB);

        //max0 < min1 || max1 < min0
        if (AXIS_MAX(minMaxA) < AXIS_MIN(minMaxB) ||
            AXIS_MAX(minMaxB) < AXIS_MIN(minMaxA))
        {
            return false;
        }

        check_axis_sat(&axis, &minMaxA, &minMaxB, collision_out);
    }

    return true;
}

int8_t ecs_collider2d_poly(
    EcsPolygonCollider *poly_a,
    EcsPolygonCollider *poly_b,
    EcsCollision2D *collision_out)
{
    return check_poly_sat(poly_a, poly_b, collision_out);
}

int8_t ecs_collider2d_poly_circle(
    EcsPolygonCollider *poly,
    EcsCircleCollider *circle,
    EcsCollision2D *collision_out)
{
    int8_t result = check_circle_poly_sat(circle, poly, collision_out);
    ecs_vec2_mult(&(collision_out->normal), -1, &(collision_out->normal));
    return result;
}

int8_t ecs_collider2d_circle_poly(
    EcsCircleCollider *circle,
    EcsPolygonCollider *poly,
    EcsCollision2D *collision_out)
{
    return check_circle_poly_sat(circle, poly, collision_out);
}

int8_t ecs_collider2d_circle(
    EcsCircleCollider *circle_a,
    EcsCircleCollider *circle_b,
    EcsCollision2D *collision_out)
{
    return check_circle_circle(circle_a, circle_b, collision_out);
}
