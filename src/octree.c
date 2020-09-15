#include <flecs-systems-physics/octree.h>

#define MAX_PER_OCTANT (8)

typedef struct cube_t {
    struct cube_t *parent;
    struct cube_t *nodes[8];
    ecs_vector_t *entities;
    int32_t id;
    bool is_leaf;
} cube_t;

struct ecs_octree_t {
    ecs_sparse_t *cubes;
    ecs_vector_t *free_cubes;
    cube_t root;
    vec3 center;
    float size;
};

static
void cube_split(
    ecs_octree_t *ot,
    cube_t *cube,
    vec3 center,
    float size);

static
cube_t *new_cube(
    ecs_octree_t *ot,
    cube_t *parent)
{
    cube_t *result;
    if (!ecs_vector_pop(ot->free_cubes, cube_t*, &result)) {
        result = ecs_sparse_add(ot->cubes, cube_t);
        result->id = (int32_t)ecs_sparse_last_id(ot->cubes);
    }
    
    result->parent = parent;
    result->is_leaf = true;

    return result;
}

static
bool is_inside_dim(
    float center,
    float size,
    float e_pos,
    float e_size)
{
    bool 
    result =  e_pos - e_size >= center - size;
    result &= e_pos + e_size <= center + size;
    return result;
}

static
bool is_inside(
    vec3 center,
    float size,
    vec3 e_pos,
    vec3 e_size)
{
    bool 
    result =  is_inside_dim(center[0], size, e_pos[0], e_size[0]);
    result &= is_inside_dim(center[1], size, e_pos[1], e_size[1]);
    result &= is_inside_dim(center[2], size, e_pos[2], e_size[2]);
    return result;
}

static
int8_t get_side(
    float center,
    float coord)
{
    return coord > center;
}

static
int8_t next_cube_index(
    vec3 center,
    float size,
    vec3 pos)    
{
    int8_t left_right = get_side(center[0], pos[0]);
    int8_t top_bottom = get_side(center[1], pos[1]);
    int8_t front_back = get_side(center[2], pos[2]);
    center[0] += size * (left_right * 2 - 1);
    center[1] += size * (top_bottom * 2 - 1);
    center[2] += size * (front_back * 2 - 1);
    return left_right + top_bottom * 2 + front_back * 4;
}

static
void cube_add_entity(
    cube_t *cube,
    ecs_oct_entity_t *ce)
{
    ecs_oct_entity_t *elem = ecs_vector_add(&cube->entities, ecs_oct_entity_t);
    *elem = *ce;
}

static
cube_t* cube_insert(
    ecs_octree_t *ot,
    ecs_oct_entity_t *ce,
    cube_t *cube,
    vec3 cube_center,
    float cube_size)
{
    /* create private variable that can be modified during iteration */
    vec3 center;
    glm_vec3_copy(cube_center, center);
    float size = cube_size / 2;
    cube_t *cur = cube;

    ecs_assert(
        is_inside(
            center, size, ce->pos, ce->size), ECS_INVALID_PARAMETER, NULL);

    do {
        bool is_leaf = cur->is_leaf;

        /* If cube is a leaf and has space, insert */
        if (is_leaf && ecs_vector_count(cur->entities) < MAX_PER_OCTANT) {
            break;
        }

        /* Find next cube */
        vec3 child_center;
        glm_vec3_copy(center, child_center);
        float child_size = size / 2;
        int8_t cube_i = next_cube_index(center, child_size, ce->pos);
        
        /* If entity does not fit in child cube, insert into current */
        if (!is_inside(child_center, child_size, ce->pos, ce->size)) {
            break;
        }

        /* Entity should not be inserted in current node. Check if node is a
         * leaf. If it is, split it up */
        if (is_leaf) {
            cube_split(ot, cur, center, size);
        }

        cube_t *next = cur->nodes[cube_i];
        if (!next) {
            next = new_cube(ot, cur);
            cur->nodes[cube_i] = next;
            cur = next;

            /* Node is guaranteed to be empty, so stop looking */
            break;
        } else {
            cur = next;
        }

        size = child_size;
        glm_vec3_copy(child_center, center);
    } while (1);

    cube_add_entity(cur, ce);

    return cur;
}

static
void cube_split(
    ecs_octree_t *ot,
    cube_t *cube,
    vec3 center,
    float size)
{
    ecs_oct_entity_t *entities = ecs_vector_first(cube->entities, ecs_oct_entity_t);
    int32_t i, count = ecs_vector_count(cube->entities);

    /* This will force entities to be pushed to child nodes */
    cube->is_leaf = false; 

    for (i = 0; i < count; i ++) {
        if (cube != cube_insert(ot, &entities[i], cube, center, size)) {
            ecs_vector_remove_index(cube->entities, ecs_oct_entity_t, i);
            i --;
            count --;
        }
    }
}

ecs_octree_t* ecs_octree_new(
    vec3 center,
    float size)
{
    ecs_octree_t *result = ecs_os_calloc(sizeof(ecs_octree_t));
    glm_vec3_copy(result->center, center);
    result->size = size;
    result->cubes = ecs_sparse_new(cube_t);
    return result;
}

void ecs_octree_clear(
    ecs_octree_t *ot)
{
    ecs_assert(ot != NULL, ECS_INVALID_PARAMETER, NULL);

    /* Keep existing cubes intact so that we can reuse them when the octree is
     * repopulated. This lets us keep the entity vectors, and should cause the
     * octree memory to stabilize eventually. */
    int32_t i, count = ecs_sparse_count(ot->cubes);
    for (i = 0; i < count; i ++) {
        cube_t *cube = ecs_sparse_get(ot->cubes, cube_t, i);
        ecs_vector_clear(cube->entities);

        cube_t **cptr = ecs_vector_add(&ot->free_cubes, cube_t*);
        *cptr = cube;
    }

    /* Clear entities of root */
    ecs_vector_clear(ot->root.entities);
}

void ecs_octree_free(
    ecs_octree_t *ot)
{
}

int32_t ecs_octree_insert(
    ecs_octree_t *ot,
    ecs_entity_t e,
    vec3 e_pos,
    vec3 e_size)
{
    ecs_oct_entity_t ce;
    ce.e = e;
    glm_vec3_copy(ce.pos, e_pos);
    glm_vec3_copy(ce.size, e_size);
    cube_t *cube = cube_insert(ot, &ce, &ot->root, ot->center, ot->size);
    return cube->id;
}

void ecs_octree_findn(
    ecs_octree_t *ot,
    vec3 pos,
    float range,
    ecs_vector_t **result)
{
}
