#include "mat.h"

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>

static void mat_rotate_clk_90(mat_t mat)
{
    mat_t orig;
    mat_clone(mat, orig);
    for(size_t row = 0; row < MAT_DIM; ++row) {
        for(size_t col = 0; col < MAT_DIM; ++col) {
            mat[col][MAT_DIM - 1 - row] = orig[row][col];
        }
    }
}

static void mat_rotate_aclk_90(mat_t mat)
{
    mat_t orig;
    mat_clone(mat, orig);
    for(size_t row = 0; row < MAT_DIM; ++row) {
        for(size_t col = 0; col < MAT_DIM; ++col) {
            mat[MAT_DIM - 1 - col][row] = orig[row][col];
        }
    }
}

static void mat_rotate_180(mat_t mat)
{
    mat_mirror(mat, MIRROR_VERT);
    mat_mirror(mat, MIRROR_HORI);
}

static void mat_mirror_vert(mat_t mat)
{
    mat_t orig;
    mat_clone(mat, orig);
    for(size_t row = 0; row < MAT_DIM / 2; ++row) {
        memcpy(mat[row], orig[MAT_DIM - 1 - row], MAT_DIM * sizeof(mat_dtype_t));
        memcpy(mat[MAT_DIM - 1 - row], orig[row], MAT_DIM * sizeof(mat_dtype_t));
    }
}

static void mat_mirror_hori(mat_t mat)
{
    mat_t orig;
    mat_clone(mat, orig);
    for(size_t row = 0; row < MAT_DIM; ++row) {
        for(size_t col = 0; col < MAT_DIM / 2; ++col) {
            mat[row][col] = orig[row][MAT_DIM - 1 - col];
            mat[row][MAT_DIM - 1 - col] = orig[row][col];
        }
    }
}

void mat_clone(mat_t src, mat_t dst)
{
    memcpy(dst, src, MAT_DIM * MAT_DIM * sizeof(mat_dtype_t));
}

void mat_rotate(mat_t mat, rotation_t rot)
{
    switch(rot) {
        case ROTATION_CLK_90:
            mat_rotate_clk_90(mat);
            break;
        case ROTATION_ACLK_90:
            mat_rotate_aclk_90(mat);
            break;
        case ROTATION_180:
            mat_rotate_180(mat);
            break;
        default:
            assert(false);
            break;
    }
}

void mat_mirror(mat_t mat, mirror_t mirror)
{
    switch(mirror) {
        case MIRROR_VERT:
            mat_mirror_vert(mat);
            break;
        case MIRROR_HORI:
            mat_mirror_hori(mat);
            break;
        default:
            assert(false);
            break;
    }
}

