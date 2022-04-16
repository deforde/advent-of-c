#ifndef MAT_H
#define MAT_H

#include <stdint.h>
#include <stddef.h>

typedef uint8_t mat_dtype_t;

typedef struct {
    size_t dim;
    mat_dtype_t* data;
} mat_t;

typedef enum {
    ROTATION_CLK_90,
    ROTATION_ACLK_90,
    ROTATION_180,
} rotation_t;

typedef enum {
    MIRROR_VERT,
    MIRROR_HORI,
} mirror_t;

mat_t mat_create(size_t dim);
void mat_destroy(mat_t mat);
mat_t mat_clone(mat_t src);
void mat_rotate(mat_t mat, rotation_t rot);
void mat_mirror(mat_t mat, mirror_t mirror);

#endif //MAT_H

