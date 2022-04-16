#ifndef MAT_H
#define MAT_H

#include <stdint.h>

#define MAT_DIM 8

typedef uint8_t mat_dtype_t;
typedef mat_dtype_t mat_t[MAT_DIM][MAT_DIM];

typedef enum {
    ROTATION_CLK_90,
    ROTATION_ACLK_90,
    ROTATION_180,
} rotation_t;

typedef enum {
    MIRROR_VERT,
    MIRROR_HORI,
} mirror_t;

void mat_clone(mat_t src, mat_t dst);
void mat_rotate(mat_t mat, rotation_t rot);
void mat_mirror(mat_t mat, mirror_t mirror);

#endif //MAT_H

