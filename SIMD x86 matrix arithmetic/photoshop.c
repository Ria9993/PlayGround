#include <assert.h>
#include <stdint.h>
#include "potushop.h"
#include "image.h"

#define ALIGN32 __declspec(align(32))

ALIGN16 typedef struct vector4 {
    float x;
    float y;
    float z;
    float w;
} vector4_t;

ALIGN16 const static vector4_t GRAYSCALE = { 0.3f, 0.59f, 0.11f, 0.f };

ALIGN16 const static vector4_t SEPIA_R = { 0.393f, 0.769f, 0.189f, 0.f };
ALIGN16 const static vector4_t SEPIA_G = { 0.349f, 0.686f, 0.168f, 0.f };
ALIGN16 const static vector4_t SEPIA_B = { 0.272f, 0.534f, 0.131f, 0.f };

ALIGN32 const static float CLIP_1[8] = { 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f };
ALIGN32 const static float CLIP_0[8] = { 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f };

static float s_brightness; // [-100, 100]

static float s_in_min; // [0, 253]
static float s_in_max; // [2, 255]
static float s_in_range;
static float s_in_range_recp;
static float s_out_min; // [0, 255]
static float s_out_max; // [0, 255]
static float s_out_range;
static float s_in_out_rescale;

void set_brightness_arg(int brightness)
{
    assert(brightness >= -100 && brightness <= 100);
    assert(sizeof(vector4_t) == 16);

    s_brightness = (float)brightness / 255.f;
}

void set_level_args(int in_min, int in_max, int out_min, int out_max)
{
    assert(in_min >= 0 && in_min <= 253);
    assert(in_max >= 2 && in_max <= 255);
    assert(in_min < in_max);

    assert(out_min >= 0 && out_min <= 255);
    assert(out_max >= 0 && out_max <= 255);

    s_in_min = in_min / 255.f;
    s_in_max = in_max / 255.f;
    s_in_range = (in_max - in_min) / 255.f;
    s_in_range_recp = 1.f / s_in_range;
    s_out_min = out_min / 255.f;
    s_out_max = out_max / 255.f;
    s_out_range = (out_max - out_min) / 255.f;
    //s_in_out_rescale = s_in_range_recp * s_out_range;
    s_in_out_rescale = (float)((1.0 / ((in_max - in_min) / 255.0)) * ((out_max - out_min) / 255.0));
}

/*  r = r * 0.3 + g * 0.59 + b * 0.11
    g = r * 0.3 + g * 0.59 + b * 0.11
    b = r * 0.3 + g * 0.59 + b * 0.11  */
void to_grayscale(void)
{
    __asm {
        mov     ecx, [g_num_pixels]
        mov     eax, ecx
        shl     eax, 4; eax *= 16
        mov     esi, offset g_pixels
        mov     edi, esi
        add     edi, eax; edi = g_num_pixels_end_ptr

        // load grayscale
        vbroadcastf128 ymm7, GRAYSCALE

        // continue untill 256bit(AVX) memory-boundary
        shr     ecx, 1
        jnc     CHECK_END
        movaps  xmm0, [esi]
        mulps   xmm0, xmm7
        haddps  xmm0, xmm0
        haddps  xmm0, xmm0
        shufps  xmm0, xmm0, 00000000b
        movaps  [esi], xmm0
        add     esi, 16
    
    CHECK_END:
        cmp     esi, edi
        je      END

    AVX_LOOP:
        vmovaps ymm0, [esi]
        vmulps  ymm0, ymm0, ymm7
        vhaddps ymm0, ymm0, ymm0
        vhaddps ymm0, ymm0, ymm0
        vshufps ymm0, ymm0, ymm0, 00000000b
        vmovaps [esi], ymm0
        add     esi, 32
        cmp     esi, edi
        jne     AVX_LOOP
    END:
    }
}

/*  r = r * 0.393 + g * 0.769 + b * 0.189
    g = r * 0.349 + g * 0.686 + b * 0.168
    b = r * 0.272 + g * 0.534 + b * 0.131   */
void to_sepia(void)
{
    __asm {
        mov     ecx, [g_num_pixels]
        mov     eax, ecx
        shl     eax, 4; eax *= 16
        mov     esi, offset g_pixels
        mov     edi, esi
        add     edi, eax; edi = g_num_pixels_end_ptr

        // load clip
        vmovaps ymm4, CLIP_1

        // load sepia_scale
        vbroadcastf128 ymm5, SEPIA_R
        vbroadcastf128 ymm6, SEPIA_G
        vbroadcastf128 ymm7, SEPIA_B

        // continue untill 256bit(AVX) memory-boundary
        shr     ecx, 1
        jnc     CHECK_END
        movaps  xmm0, [esi]
        movaps  xmm1, xmm0
        movaps  xmm2, xmm0

        mulps   xmm0, xmm5; R
        mulps   xmm1, xmm6; G
        mulps   xmm2, xmm7; B
        haddps  xmm0, xmm1; xmm0 = { RL, RH, GL, GH }
        haddps  xmm2, xmm2; xmm2 = { BL, BH, BL, BH }
        haddps  xmm0, xmm2; xmm0 = { R, G, B, B }

        minps   xmm0, xmm4; clipping as 1.0f
        
        movaps  [esi], xmm0
        add     esi, 16

    CHECK_END:
        cmp     esi, edi
        je      END

    AVX_LOOP:
        vmovaps ymm0, [esi]

        vmulps  ymm1, ymm0, ymm6; G
        vmulps  ymm2, ymm0, ymm7; B
        vmulps  ymm0, ymm0, ymm5; R

        vhaddps ymm0, ymm0, ymm1; ymm0 = { X_RL, X_RH, X_GL, X_GH, Y_RL, Y_RH, Y_GL, Y_GH }
        vhaddps ymm2, ymm2, ymm2; ymm2 = { X_BL, X_BH, X_BL, X_BH, Y_BL, Y_BH, Y_BL, Y_BH }
        vhaddps ymm0, ymm0, ymm2; ymm0 = { X_R, X_G, X_B, X_B, Y_R, Y_G, Y_B, Y_B };

        vminps  ymm0, ymm0, ymm4; clipping as 1.0f

        vmovaps [esi], ymm0
        add     esi, 32
        cmp     esi, edi
        jne     AVX_LOOP
    END:
    }
}

void change_brightness(void)
{
    __asm {
        mov     ecx, [g_num_pixels]
        mov     eax, ecx
        shl     eax, 4; eax *= 16
        mov     esi, offset g_pixels
        mov     edi, esi
        add     edi, eax; edi = g_num_pixels_end_ptr

        // load brightness
        vbroadcastss ymm7, s_brightness
        // load clip
        vmovaps ymm6, CLIP_1
        vmovaps ymm5, CLIP_0

        // continue untill 256bit(AVX) memory-boundary
        shr     ecx, 1
        jnc     CHECK_END

        movaps  xmm0, [esi]

        addps   xmm0, xmm7
        minps   xmm0, xmm6; clipping as [0.0f, 1.0f]
        maxps   xmm0, xmm5;

        movaps  [esi], xmm0
        add     esi, 16

    CHECK_END:
        cmp esi, edi
        je  END

    AVX_LOOP:
        vmovaps ymm0, [esi]

        vaddps  ymm0, ymm0, ymm7
        vminps  ymm0, ymm0, ymm6; clipping as[0.0f, 1.0f]
        vmaxps  ymm0, ymm0, ymm5

        vmovaps [esi], ymm0
        add     esi, 32
        cmp     esi, edi
        jne     AVX_LOOP
    END:
    }
}

void change_levels(void)
{
    __asm {
        mov     ecx, [g_num_pixels]
        mov     eax, ecx
        shl     eax, 4; eax *= 16
        mov     esi, offset g_pixels
        mov     edi, esi
        add     edi, eax; edi = g_num_pixels_end_ptr

        // load range
        vbroadcastss ymm4, s_in_min
        vbroadcastss ymm5, s_in_out_rescale
        vbroadcastss ymm6, s_out_min

        vbroadcastss ymm2, s_in_range_recp
        vbroadcastss ymm3, s_out_range
        vbroadcastss ymm1, s_in_max

        // continue untill 256bit(AVX) memory-boundary
        shr     ecx, 1
        jnc     CHECK_END

        movaps  xmm0, [esi]

        // rescale in_range -> out_range
        maxps   xmm0, xmm4
        minps   xmm0, xmm1
        subps   xmm0, xmm4
        mulps   xmm0, xmm5
        addps   xmm0, xmm6

        movaps  [esi], xmm0
        add     esi, 16

    CHECK_END:
        cmp     esi, edi
        je      END

    AVX_LOOP:
        vmovaps ymm0, [esi]

        // rescale in_range -> out_range
        vmaxps  ymm0, ymm0, ymm4   
        vminps  ymm0, ymm0, ymm1
        vsubps  ymm0, ymm0, ymm4
        vmulps  ymm0, ymm0, ymm5
        vaddps  ymm0, ymm0, ymm6

        vmovaps [esi], ymm0
        add     esi, 32
        cmp     esi, edi
        jne     AVX_LOOP
    END:
    }
}
