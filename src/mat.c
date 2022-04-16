#include "mat.h"

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>

static void mat_rotate_clk_90(mat_t mat)
{
    mat_t orig = mat_clone(mat);
    for(size_t row = 0; row < mat.dim; ++row) {
        for(size_t col = 0; col < mat.dim; ++col) {
            mat.data[col * mat.dim + mat.dim - 1 - row] = orig.data[row * mat.dim + col];
        }
    }
    mat_destroy(orig);
}

static void mat_rotate_aclk_90(mat_t mat)
{
    mat_t orig = mat_clone(mat);
    for(size_t row = 0; row < mat.dim; ++row) {
        for(size_t col = 0; col < mat.dim; ++col) {
            mat.data[(mat.dim - 1 - col) * mat.dim + row] = orig.data[row * mat.dim + col];
        }
    }
    mat_destroy(orig);
}

static void mat_rotate_180(mat_t mat)
{
    mat_mirror(mat, MIRROR_VERT);
    mat_mirror(mat, MIRROR_HORI);
}

static void mat_mirror_vert(mat_t mat)
{
    mat_t orig = mat_clone(mat);
    for(size_t row = 0; row < mat.dim / 2; ++row) {
        memcpy(&mat.data[row * mat.dim], &orig.data[(mat.dim - 1 - row) * mat.dim], mat.dim * sizeof(mat_dtype_t));
        memcpy(&mat.data[(mat.dim - 1 - row) * mat.dim], &orig.data[row * mat.dim], mat.dim * sizeof(mat_dtype_t));
    }
    mat_destroy(orig);
}

static void mat_mirror_hori(mat_t mat)
{
    mat_t orig = mat_clone(mat);
    for(size_t row = 0; row < mat.dim; ++row) {
        for(size_t col = 0; col < mat.dim / 2; ++col) {
            mat.data[row * mat.dim + col] = orig.data[row * mat.dim + mat.dim - 1 - col];
            mat.data[row * mat.dim + mat.dim - 1 - col] = orig.data[row * mat.dim + col];
        }
    }
    mat_destroy(orig);
}

mat_t mat_create(size_t dim)
{
    mat_t mat = {
        .dim = dim,
        .data = (mat_dtype_t*)malloc(dim * dim * sizeof(mat_dtype_t)),
    };
    return mat;
}

void mat_destroy(mat_t mat)
{
    free(mat.data);
}

mat_t mat_clone(mat_t src)
{
    mat_t dst = mat_create(src.dim);
    memcpy(dst.data, src.data, src.dim * src.dim * sizeof(mat_dtype_t));
    return dst;
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

