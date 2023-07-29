#include "transformer.h"

#include <stddef.h>
#include <stdio.h>
#include <assert.h>

size_t read_points(vec4_t* points, const size_t count)
{
    size_t i;
    for (i = 0; i < count; i++) {
        vec4_t point;
        point.w = 1;
        int num_read = scanf("%f %f %f", &point.x, &point.y, &point.z);
        
        if (num_read == EOF) {
            break;
        }
        
        points[i] = point;
    }

    return i;
}

void print_csv(const vec4_t* points, const size_t count)
{
    for (size_t i = 0; i < count; i++) {
        printf("%.4f,%.4f,%.4f\n", points[i].x, points[i].y, points[i].z);
    }
}

void transpose(mat4_t* mat)
{
    __asm {
        mov             eax, mat

        vmovups         ymm2, [eax];                    ymm2 = { x0, y0, z0, w0, x1, y1, z1, w1 }
        vmovups         ymm3, [eax + 32];               ymm3 = { x2, y2, z2, w2, x3, y3, z3, w3 }
        vperm2i128      ymm0, ymm2, ymm3, 00100000b;    ymm0 = { x0, y0, z0, w0, x2, y2, z2, w2 }
        vperm2i128      ymm1, ymm2, ymm3, 00110001b;    ymm1 = { x1, y1, z1, w1, x3, y3, z3, w3 }

        ; shuffle to each ymm contain the x,z or y,w elements of every row
        vshufps         ymm2, ymm0, ymm1, 10001000b;    ymm2 = { x0, z0, x1, z1, x2, z2, x3, z3 }
        vshufps         ymm3, ymm0, ymm1, 11011101b;    ymm3 = { y0, w0, y1, w1, y2, w2, y3, w3 }

        vinsertf128     ymm4, ymm2, xmm3, 1;            ymm4 = { x0, z0, x1, z1, y0, w0, y1, w1 }
        vperm2f128      ymm5, ymm2, ymm3, 00110001b;    ymm5 = { x2, z2, x3, z3, y2, w2, y3, w3 }

        vshufps         ymm6, ymm4, ymm5, 10001000b;    ymm6 = { x0, x1, x2, x3, y0, y1, y2, y3 }
        vshufps         ymm7, ymm4, ymm5, 11011101b;    ymm7 = { z0, z1, z2, z3, w0, w1, w2, w3 }

        vmovups         [eax], ymm6
        vmovups         [eax + 32], ymm7
    }
}

void transform(vec4_t* dst, const vec4_t* src, const mat4_t* mat_tr)
{
    __asm {
        mov             edi, dst
        mov             esi, src
        mov             eax, mat_tr

        vbroadcastf128  ymm0, [esi];                    ymm0 = { x, y, z, w, x, y, z, w }
        vmovups         ymm1, [eax];                    ymm1 = { x0, y0, z0, w0, x1, y1, z1, w1 }
        vmovups         ymm2, [eax + 32];               ymm2 = { x2, y2, z2, w2, x3, y3, z3, w3 }

        vdpps           ymm1, ymm0, ymm1, 11110001b;    ymm1 = { s0, 0, 0, 0, s1, 0, 0, 0 }
        vdpps           ymm2, ymm0, ymm2, 11110001b;    ymm2 = { s2, 0, 0, 0, s3, 0, 0, 0 }

        vpermq          ymm1, ymm1, 11111000b;          xmm1 = { s0, 0, s1, 0 }
        vpermq          ymm2, ymm2, 11111000b;          xmm2 = { s2, 0, s3, 0 }
        shufps          xmm1, xmm2, 10001000b;          xmm1 = { s0, s1, s2, s3 }

        movups          [edi], xmm1
    }
}

void concatenate(mat4_t* dst, const mat4_t* m0, const mat4_t* m1_tr)
{
    __asm {
        mov             edi, dst
        mov             esi, m0
        mov             eax, m1_tr

        // load m1_tr
        vmovups         ymm2, [eax];                    ymm2 = { x0, y0, z0, w0, x1, y1, z1, w1 }
        vmovups         ymm3, [eax + 32];               ymm3 = { x2, y2, z2, w2, x3, y3, z3, w3 }

        // m0_r0
        vbroadcastf128  ymm0, [esi]
        vdpps           ymm4, ymm0, ymm2, 11110001b;    ymm4 = { x0, 0, 0, 0, y0, 0, 0, 0 }
        vdpps           ymm5, ymm0, ymm3, 11110001b;    ymm5 = { z0, 0, 0, 0, w0, 0, 0, 0 }
        vpermq          ymm4, ymm4, 11111000b;          xmm4 = { x0, 0, y0, 0 }
        vpermq          ymm5, ymm5, 11111000b;          xmm5 = { z0, 0, w0, 0 }
        shufps          xmm4, xmm5, 10001000b;          xmm4 = { x0, y0, z0, w0 }
        movups          [edi], xmm4

        // m0_r1
        vbroadcastf128  ymm0, [esi + 16]
        vdpps           ymm4, ymm0, ymm2, 11110001b;    ymm4 = { x1, 0, 0, 0, y1, 0, 0, 0 }
        vdpps           ymm5, ymm0, ymm3, 11110001b;    ymm5 = { z1, 0, 0, 0, w1, 0, 0, 0 }
        vpermq          ymm4, ymm4, 11111000b;          xmm4 = { x1, 0, y1, 0 }
        vpermq          ymm5, ymm5, 11111000b;          xmm5 = { z1, 0, w1, 0 }
        shufps          xmm4, xmm5, 10001000b;          xmm4 = { x1, y1, z1, w1 }
        movups          [edi + 16], xmm4

        // m0_r2
        vbroadcastf128  ymm0, [esi + 32]
        vdpps           ymm4, ymm0, ymm2, 11110001b;    ymm4 = { x2, 0, 0, 0, y2, 0, 0, 0 }
        vdpps           ymm5, ymm0, ymm3, 11110001b;    ymm5 = { z2, 0, 0, 0, w2, 0, 0, 0 }
        vpermq          ymm4, ymm4, 11111000b;          xmm4 = { x2, 0, y2, 0 }
        vpermq          ymm5, ymm5, 11111000b;          xmm5 = { z2, 0, w2, 0 }
        shufps          xmm4, xmm5, 10001000b;          xmm4 = { x2, y2, z2, w2 }
        movups          [edi + 32], xmm4

        // m0_r3
        vbroadcastf128  ymm0, [esi + 48]
        vdpps           ymm4, ymm0, ymm2, 11110001b;    ymm4 = { x3, 0, 0, 0, y3, 0, 0, 0 }
        vdpps           ymm5, ymm0, ymm3, 11110001b;    ymm5 = { z3, 0, 0, 0, w3, 0, 0, 0 }
        vpermq          ymm4, ymm4, 11111000b;          xmm4 = { x3, 0, y3, 0 }
        vpermq          ymm5, ymm5, 11111000b;          xmm5 = { z3, 0, w3, 0 }
        shufps          xmm4, xmm5, 10001000b;          xmm4 = { x3, y3, z3, w3 }
        movups          [edi + 48], xmm4
    }
}

void run(vec4_t* world_pts, const vec4_t* local_pts, const size_t count, const vec3_t* scale, const vec3_t* rotation, const vec3_t* translation)
{
    // make transformation matrices
    mat4_t m_scale;
    mat_scale(&m_scale, scale->x, scale->y, scale->z);
    
    mat4_t m_rotation_x;
    mat4_t m_rotation_y;
    mat4_t m_rotation_z;
    mat_rotation_x(&m_rotation_x, rotation->x);
    mat_rotation_y(&m_rotation_y, rotation->y);
    mat_rotation_z(&m_rotation_z, rotation->z);

    mat4_t m_translation;
    mat_translation(&m_translation, translation->x, translation->y, translation->z);

    // concatenate tranformation matrices
    mat4_t m_concat;
    
    mat4_t m_scale_tr = m_scale;
    mat4_t m_rot_x_tr = m_rotation_x;
    mat4_t m_rot_y_tr = m_rotation_y;
    mat4_t m_rot_z_tr = m_rotation_z;
    mat4_t m_trans_tr = m_translation;
    transpose(&m_scale_tr);
    transpose(&m_rot_x_tr);
    transpose(&m_rot_y_tr);
    transpose(&m_rot_z_tr);
    transpose(&m_trans_tr);

    concatenate(&m_concat, &m_scale, &m_rot_x_tr);
    concatenate(&m_concat, &m_concat, &m_rot_y_tr);
    concatenate(&m_concat, &m_concat, &m_rot_z_tr);
    concatenate(&m_concat, &m_concat, &m_trans_tr);

    mat4_t m_concat_tr = m_concat;
    transpose(&m_concat_tr);

    // transform
    for (size_t i = 0; i < count; i++) {
        transform(&world_pts[i], &local_pts[i], &m_concat_tr);
    }
}
