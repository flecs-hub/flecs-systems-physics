#include <include/physics.h>
#include <math.h>

#define AXIS_MIN(minMax) ((minMax).x)
#define AXIS_MAX(minMax) ((minMax).y)

#define ERR -1
#define POSITION 0
#define CIRCLE 1
#define POLYGON 2

#define GET_COLLIDER_TYPE(collider) (((*collider)[CIRCLE] != NULL) ? CIRCLE :\
                                     ((*collider)[POLYGON] != NULL) ? POLYGON : ERR)

typedef struct ColliderData {
    EcsPoint2D *position;
    EcsCircleCollider *circle;
    EcsPolygonCollider *polygon;
} ColliderData_t;

static int8_t EcsPhysis2dCollisionCheckCircleCircle(
    ColliderData_t *circle_a,
    ColliderData_t *circle_b,
    EcsCollision2DInfo *collision_out);
static int8_t EcsPhysis2dCollisionCheckPolygonSat(
    ColliderData_t *polygon_a,
    ColliderData_t *polygon_b,
    EcsCollision2DInfo *collision_out);
static int8_t EcsPhysis2dCollisionCheckCirclePolygonSat(
    ColliderData_t *circle,
    ColliderData_t *polygon,
    int8_t invert,
    EcsCollision2DInfo *collision_out);
static float EcsPhysis2dCollisionCheckPolygonSatAxis(
    EcsPoint2D *vertices_a, int8_t size_a,
    EcsPoint2D *vertices_b, int8_t size_b,
    EcsCollision2DInfo *collision_out, int8_t invert);
static void EcsPhysis2dCollisionCheckAxisSat(
    EcsVector2D *axis,
    EcsVector2D *minMaxA,
    EcsVector2D *minMaxB,
    EcsCollision2DInfo *collision_out);
static void EcsPhysis2d_getProjection(
    EcsVector2D *axis,
    EcsPoint2D *vertices, int8_t size,
    EcsVector2D *out);
static void EcsPhysis2d_getProjectionCircle(
    EcsVector2D *axis,
    EcsVector2D *center, float radius,
    EcsVector2D *out);
static void EcsPhysis2d_getClosestPoint(
    EcsVector2D *position,
    EcsPoint2D *vertices, int8_t size,
    EcsVector2D *out
);

int8_t EcsPhysis2d_GetCollisionInfo(
    EcsCollider2DData *collider_a,
    EcsCollider2DData *collider_b,
    EcsCollision2DInfo *collision_out)
{
    if (collider_a == NULL || collider_b == NULL || (*collider_a)[POSITION] == NULL || (*collider_b)[POSITION] == NULL) {
        return false;
    }

    int8_t type_a = GET_COLLIDER_TYPE(collider_a);
    int8_t type_b = GET_COLLIDER_TYPE(collider_b);

    if (type_a == ERR || type_b == ERR) {
        return false;
    } else if (type_a == type_b) {
        if (type_a == CIRCLE) {
            return EcsPhysis2dCollisionCheckCircleCircle((ColliderData_t*)collider_a, (ColliderData_t*)collider_b, collision_out);
        } else if (type_b == POLYGON) {
            return EcsPhysis2dCollisionCheckPolygonSat((ColliderData_t*)collider_a, (ColliderData_t*)collider_b, collision_out);
        }
    } else {
        if (type_a == CIRCLE) {
            return EcsPhysis2dCollisionCheckCirclePolygonSat((ColliderData_t*)collider_a, (ColliderData_t*)collider_b, true, collision_out);
        }
        return EcsPhysis2dCollisionCheckCirclePolygonSat((ColliderData_t*)collider_b, (ColliderData_t*)collider_a, false, collision_out);
    }
    return false;
}

static int8_t EcsPhysis2dCollisionCheckCircleCircle(
    ColliderData_t *circle_a,
    ColliderData_t *circle_b,
    EcsCollision2DInfo *collision_out)
{
    float totalRadius = circle_a->circle->radius + circle_b->circle->radius;
    float distSqrt = EcsVector2D_distanceSqrt(circle_a->position, circle_b->position);
    if (distSqrt > totalRadius*totalRadius) {
        return false;
    }
    EcsVector2D_sub(circle_b->position, circle_a->position, &(collision_out->normal));
    EcsVector2D_normalize(&(collision_out->normal), &(collision_out->normal));
    collision_out->distance = sqrtf(distSqrt)-totalRadius;
    return true;
}

static int8_t EcsPhysis2dCollisionCheckPolygonSat(
    ColliderData_t *polygon_a,
    ColliderData_t *polygon_b,
    EcsCollision2DInfo *collision_out)
{
    EcsPoint2D vertices_a[128];
    EcsPoint2D vertices_b[128];
    EcsMatrix3x3 transfor = {{{1,0, polygon_a->position->x}, {0,1, polygon_a->position->y}, {0,0,1}}};

    EcsMatrix3x3_transform(&transfor, polygon_a->polygon->points, vertices_a, polygon_a->polygon->point_count);
    transfor.data[0][2] = polygon_b->position->x;
    transfor.data[1][2] = polygon_b->position->y;
    EcsMatrix3x3_transform(&transfor, polygon_b->polygon->points, vertices_b, polygon_b->polygon->point_count);

    collision_out->distance = INFINITY;
    if (EcsPhysis2dCollisionCheckPolygonSatAxis(vertices_a, polygon_a->polygon->point_count,
                                                vertices_b, polygon_b->polygon->point_count,
                                                collision_out, false) == INFINITY) {
        return false;
    }
    if (EcsPhysis2dCollisionCheckPolygonSatAxis(vertices_b, polygon_b->polygon->point_count,
                                                vertices_a, polygon_a->polygon->point_count,
                                                collision_out, true) == INFINITY) {
        return false;
    }
    return true;
}

static int8_t EcsPhysis2dCollisionCheckCirclePolygonSat(
    ColliderData_t *circle,
    ColliderData_t *polygon,
    int8_t invert,
    EcsCollision2DInfo *collision_out)
{
    EcsPoint2D vertices_a[128];
    int8_t size_a = polygon->polygon->point_count;
    EcsMatrix3x3 transfor = {{{1,0, polygon->position->x}, {0,1, polygon->position->y}, {0,0,1}}};
    EcsMatrix3x3_transform(&transfor, polygon->polygon->points, vertices_a, size_a);

    EcsVector2D axis;
    EcsVector2D closestPoint;
    EcsVector2D minMaxA;
    EcsVector2D minMaxB;

    EcsPhysis2d_getClosestPoint(circle->position, vertices_a, size_a, &closestPoint);
    EcsVector2D_sub(circle->position, &closestPoint, &axis);
    EcsVector2D_normalize(&axis, &axis);

    EcsPhysis2d_getProjection(&axis, vertices_a, size_a, &minMaxA);
    EcsPhysis2d_getProjectionCircle(&axis, circle->position, circle->circle->radius, &minMaxB);

    //max0 < min1 || max1 < min0
    if (AXIS_MAX(minMaxA) < AXIS_MIN(minMaxB) || AXIS_MAX(minMaxB) < AXIS_MIN(minMaxA)) {
        return false;
    }
    collision_out->distance = INFINITY;
    EcsPhysis2dCollisionCheckAxisSat(&axis, &minMaxA, &minMaxB, collision_out);

    for (int8_t i = 0; i < size_a; i++){
        EcsVector2D_sub(&vertices_a[(i+1) < size_a ? (i+1) : 0], &vertices_a[i], &axis);
        EcsVector2D_perpendicular(&axis, &axis);
        EcsVector2D_normalize(&axis, &axis);

        EcsPhysis2d_getProjection(&axis, vertices_a, size_a, &minMaxA);
        EcsPhysis2d_getProjectionCircle(&axis, circle->position, circle->circle->radius, &minMaxB);

        //max0 < min1 || max1 < min0
        if (AXIS_MAX(minMaxA) < AXIS_MIN(minMaxB) || AXIS_MAX(minMaxB) < AXIS_MIN(minMaxA)) {
            return false;
        }

        EcsPhysis2dCollisionCheckAxisSat(&axis, &minMaxA, &minMaxB, collision_out);
    }

    if (invert) {
        EcsVector2D_mult(&(collision_out->normal), -1, &(collision_out->normal));
    }

    return true;
}

static float EcsPhysis2dCollisionCheckPolygonSatAxis(
    EcsPoint2D *vertices_a, int8_t size_a,
    EcsPoint2D *vertices_b, int8_t size_b,
    EcsCollision2DInfo *collision_out, int8_t invert)
{
    EcsVector2D axis;
    EcsVector2D minMaxA;
    EcsVector2D minMaxB;

    for (int8_t i = 0; i < size_a; i++){
        EcsVector2D_sub(&vertices_a[(i+1) < size_a ? (i+1) : 0], &vertices_a[i], &axis);
        EcsVector2D_perpendicular(&axis, &axis);
        EcsVector2D_normalize(&axis, &axis);
        if (invert) {
            EcsPhysis2d_getProjection(&axis, vertices_a, size_a, &minMaxB);
            EcsPhysis2d_getProjection(&axis, vertices_b, size_b, &minMaxA);
        } else {
            EcsPhysis2d_getProjection(&axis, vertices_a, size_a, &minMaxA);
            EcsPhysis2d_getProjection(&axis, vertices_b, size_b, &minMaxB);
        }

        //max0 < min1 || max1 < min0
        if (AXIS_MAX(minMaxA) < AXIS_MIN(minMaxB) || AXIS_MAX(minMaxB) < AXIS_MIN(minMaxA)) {
            return INFINITY;
        }

        EcsPhysis2dCollisionCheckAxisSat(&axis, &minMaxA, &minMaxB, collision_out);
    }
    return collision_out->distance;
}

static void EcsPhysis2dCollisionCheckAxisSat(
    EcsVector2D *axis,
    EcsVector2D *minMaxA,
    EcsVector2D *minMaxB,
    EcsCollision2DInfo *collision_out)
{
    float distmin;
    if (AXIS_MIN(*minMaxA) < AXIS_MIN(*minMaxB)) {
        distmin = AXIS_MIN(*minMaxB) - AXIS_MAX(*minMaxA);
        if (distmin < 0) {
            distmin = -distmin;
            EcsVector2D_mult(axis, -1, axis);
        }
    } else {
        distmin = AXIS_MIN(*minMaxA) - AXIS_MAX(*minMaxB);
        if (distmin < 0) {
            distmin = -distmin;
        } else {
            EcsVector2D_mult(axis, -1, axis);
        }
    }
    if (distmin <= collision_out->distance) {
        collision_out->distance = distmin;
        collision_out->normal.x = axis->x;
        collision_out->normal.y = axis->y;
    }
}

static void EcsPhysis2d_getProjection(
    EcsVector2D *axis,
    EcsPoint2D *vertices, int8_t size,
    EcsVector2D *out)
{
    float min = EcsVector2D_dot(axis, &vertices[0]);
    float max = min;
    for (int i = 1; i < size; i++) {
        float t = EcsVector2D_dot(axis, &vertices[i]);
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

static void EcsPhysis2d_getProjectionCircle(
    EcsVector2D *axis,
    EcsVector2D *center, float radius,
    EcsVector2D *out)
{
    float mid = EcsVector2D_dot(axis, center);
    out->x = mid - radius;
    out->y = mid + radius;
}

static void EcsPhysis2d_getClosestPoint(
    EcsVector2D *position,
    EcsPoint2D *vertices, int8_t size,
    EcsVector2D *out)
{
    float distance = MAXFLOAT;
    float currDist;

    for (int i = 0; i < size; i++) {
        currDist = EcsVector2D_distanceSqrt(position, &vertices[i]);
        if (currDist < distance) {
            distance = currDist;
            out->x = vertices[i].x;
            out->y = vertices[i].y;
        }
    }
}
