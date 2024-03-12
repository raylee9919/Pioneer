 /* ========================================================================
    $File: $
    $Date: $
    $Revision: $
    $Creator: Sung Woo Lee $
    $Notice: (C) Copyright 2024 by Sung Woo Lee. All Rights Reserved. $
    ======================================================================== */

#include "render_group.h"

#define PushRenderEntity(GROUP, STRUCT) \
    (STRUCT *)PushRenderEntity_(GROUP, sizeof(STRUCT), RenderType_##STRUCT)
internal RenderEntityHeader *
PushRenderEntity_(RenderGroup *renderGroup, u32 size, RenderType type) {
    ASSERT(size + renderGroup->used <= renderGroup->capacity);
    RenderEntityHeader *header = (RenderEntityHeader *)(renderGroup->base + renderGroup->used);
    header->type = type;
    renderGroup->used += size;
    return header;
}

internal void
PushBmp(RenderGroup *renderGroup, vec2 origin, vec2 axisX, vec2 axisY,
        Bitmap *bmp) {
    RenderEntityBmp *piece = PushRenderEntity(renderGroup, RenderEntityBmp);
    if (piece) {
        piece->origin = origin;
        piece->axisX = axisX;
        piece->axisY = axisY;
        piece->bmp = bmp;
    }
}

internal void
PushRect(RenderGroup *renderGroup,
        vec2 min, vec2 max, vec4 color) {
    RenderEntityRect *piece = PushRenderEntity(renderGroup, RenderEntityRect);
    if (piece) {
        piece->min = min;
        piece->max = max;
        piece->color = color;
    }
}

internal void
PushCoordinateSystem(RenderGroup *renderGroup,
        vec2 origin, vec2 axisX, vec2 axisY, Bitmap *bmp) {
    RenderEntityCoordinateSystem *piece = PushRenderEntity(renderGroup, RenderEntityCoordinateSystem);
    if (piece) {
        piece->origin = origin;
        piece->axisX = axisX;
        piece->axisY = axisY;
        piece->bmp = bmp;
    }
}

internal RenderGroup *
AllocRenderGroup(MemoryArena *memoryArena) {
    RenderGroup *result = PushStruct(memoryArena, RenderGroup);
    *result = {};
    result->capacity = MB(4);
    result->base = (u8 *)PushSize(memoryArena, result->capacity);
    result->used = 0;

    return result;
}

internal void
DrawBmp(Bitmap *buf,
        vec2 min, Bitmap *bmp, r32 CAlpha = 1.0f) {
    s32 minX = RoundR32ToS32(min.x);
    s32 minY = RoundR32ToS32(min.y);
    s32 maxX = minX + bmp->width;
    s32 maxY = minY + bmp->height;

    s32 srcOffsetX = 0;
    if(minX < 0) {
        srcOffsetX = -minX;
        minX = 0;
    }

    s32 srcOffsetY = 0;
    if(minY < 0) {
        srcOffsetY = -minY;
        minY = 0;
    }

    if(maxX > (s32)buf->width) {
        maxX = (s32)buf->width;
    }

    if(maxY > (s32)buf->height) {
        maxY = (s32)buf->height;
    }
    
    u8 *srcRow = (u8 *)bmp->memory +
        srcOffsetY * bmp->pitch +
        BYTES_PER_PIXEL * srcOffsetX;
    u8 *dstRow = ((u8 *)buf->memory +
                      minX * BYTES_PER_PIXEL +
                      minY * buf->pitch);

    for(s32 Y = minY;
        Y < maxY;
        ++Y)
    {
        u32 *dst = (u32 *)dstRow;
        u32 *src = (u32 *)srcRow;
        for(s32 X = minX;
            X < maxX;
            ++X)
        {
            real32 SA = (real32)((*src >> 24) & 0xFF);
            real32 RSA = (SA / 255.0f) * CAlpha;            
            real32 SR = CAlpha*(real32)((*src >> 16) & 0xFF);
            real32 SG = CAlpha*(real32)((*src >> 8) & 0xFF);
            real32 SB = CAlpha*(real32)((*src >> 0) & 0xFF);

            real32 DA = (real32)((*dst >> 24) & 0xFF);
            real32 DR = (real32)((*dst >> 16) & 0xFF);
            real32 DG = (real32)((*dst >> 8) & 0xFF);
            real32 DB = (real32)((*dst >> 0) & 0xFF);
            real32 RDA = (DA / 255.0f);
            
            real32 InvRSA = (1.0f-RSA);
            real32 A = 255.0f*(RSA + RDA - RSA*RDA);
            real32 R = InvRSA*DR + SR;
            real32 G = InvRSA*DG + SG;
            real32 B = InvRSA*DB + SB;

            *dst = (((uint32)(A + 0.5f) << 24) |
                     ((uint32)(R + 0.5f) << 16) |
                     ((uint32)(G + 0.5f) << 8) |
                     ((uint32)(B + 0.5f) << 0));
            
            ++dst;
            ++src;
        }

        dstRow += buf->pitch;
        srcRow += bmp->pitch;
    }
}

internal void
DrawRect(Bitmap *buffer, vec2 min, vec2 max, vec4 color) {
    s32 minX = RoundR32ToS32(min.x);
    s32 minY = RoundR32ToS32(min.y);
    s32 maxX = RoundR32ToS32(max.x);
    s32 maxY = RoundR32ToS32(max.y);

    if (minX < 0) {
        if (maxX < 0) {
            return;
        } else {
            minX = 0;
        }
    }
    if (minY < 0) {
        if (maxY < 0) {
            return;
        } else {
            minY = 0;
        }
    }
    if (maxX > (s32)buffer->width) {
        if (minX > (s32)buffer->width) {
            return;
        } else {
            maxX = (s32)buffer->width; 
        }
    }
    if (maxY > (s32)buffer->width) {
        if (minY > (s32)buffer->width) {
            return;
        } else {
            maxY = (s32)buffer->width; 
        }
    }

    r32 R = color.r;
    r32 G = color.g;
    r32 B = color.b;
    r32 A = color.a;

    u32 C = ( (RoundR32ToU32(A * 255.0f) << 24) | 
              (RoundR32ToU32(R * 255.0f) << 16) |
              (RoundR32ToU32(G * 255.0f) <<  8) |
              (RoundR32ToU32(B * 255.0f) <<  0) );

    u32 *memBegin = (u32 *)buffer->memory;
    u32 *memEnd = (u32 *)buffer->memory +
        buffer->width * buffer->height;

    for (s32 y = minY;
            y < maxY;
            ++y) {
        u32 *row = (u32 *)buffer->memory + y * buffer->width;
        for (s32 x = minX;
                x < maxX;
                ++x) {
            u32 *pixel = row + x;
            if (pixel >= memBegin &&
                    pixel < memEnd) {
                *pixel = C;
            }
        }
    }
}

internal void
DrawRectSlow(Bitmap *buffer, vec2 origin, vec2 axisX, vec2 axisY, Bitmap *bmp) {
    RDTSC_BEGIN(RenderRectSlow);

    s32 minX = buffer->width - 1;
    s32 maxX = 0;
    s32 minY = buffer->height - 1;
    s32 maxY = 0;

    vec2 Vs[4] = {
        origin,
        origin + axisX,
        origin + axisY,
        origin + axisX + axisY
    };

    for (s32 idx = 0;
            idx < ArrayCount(Vs);
            ++idx) {
        vec2 V = Vs[idx];
        s32 floorX = FloorR32ToS32(V.x);
        s32 floorY = FloorR32ToS32(V.y);
        s32 ceilX = CeilR32ToS32(V.x);
        s32 ceilY = CeilR32ToS32(V.y);
        if (floorX < minX) { minX = floorX; }
        if (floorY < minY) { minY = floorY; }
        if (ceilX > maxX)  { maxX = ceilX; }
        if (ceilY > maxY)  { maxY = ceilY; }
    }

    __m128 originX_4x = _mm_set1_ps(origin.x);
    __m128 originY_4x = _mm_set1_ps(origin.y);
    __m128 axisXx_4x = _mm_set1_ps(axisX.x);
    __m128 axisXy_4x = _mm_set1_ps(axisX.y);
    __m128 axisYx_4x = _mm_set1_ps(axisY.x);
    __m128 axisYy_4x = _mm_set1_ps(axisY.y);
    __m128 invLenSquareX_4x = _mm_set1_ps(1.0f / LenSquare(axisX));
    __m128 invLenSquareY_4x = _mm_set1_ps(1.0f / LenSquare(axisY));
    __m128 zero_4x = _mm_set1_ps(0.0f);
    __m128 one_4x = _mm_set1_ps(1.0f);
    __m128i minusOne_4x = _mm_set1_epi32(-1);
    __m128i one_epi32 = _mm_set1_epi32(1);
    __m128 m255_4x = _mm_set1_ps(255.0f);
    __m128 inv255_4x = _mm_set1_ps(1.0f / 255.0f);
    __m128i maskFF_4x = _mm_set1_epi32(0xFF);

    __m128 bmpMaxX_4x = _mm_set1_ps((r32)(bmp->width - 1));
    __m128 bmpMaxY_4x = _mm_set1_ps((r32)(bmp->height - 1));

    __m128i bufWidthMinusOne = _mm_set1_epi32(buffer->width - 1);

    RDTSC_BEGIN(PerPixel);

#define M(m, i)  ((r32 *)&m)[i]
#define Mi(m, i) ((u32 *)&m)[i]
#define mm_clamp_ps(A, lo, hi) _mm_min_ps(_mm_max_ps(A, lo), hi)
#define mm_clamp_epi32(A, lo, hi) _mm_min_epi32(_mm_max_epi32(A, lo), hi)

    for (s32 Y = minY;
            Y <= maxY;
            ++Y) {
        for (s32 X = minX;
                X <= maxX;
                X += 4) {
            if (Y < 0 || Y >= buffer->height) { continue; }

            __m128i Xi_4x = mm_clamp_epi32(_mm_setr_epi32(X, X + 1, X + 2, X + 3), one_epi32, bufWidthMinusOne);
            __m128i Yi_4x = _mm_set1_epi32(Y);

            __m128 X_4x = _mm_cvtepi32_ps(Xi_4x);
            __m128 Y_4x = _mm_set1_ps((r32)Y);

            __m128 Px_4x = _mm_sub_ps(X_4x, originX_4x);
            __m128 Py_4x = _mm_sub_ps(Y_4x, originY_4x);
            __m128 dx_4x = _mm_mul_ps(_mm_add_ps(_mm_mul_ps(Px_4x, axisXx_4x), _mm_mul_ps(Py_4x, axisXy_4x)), invLenSquareX_4x);
            __m128 dy_4x = _mm_mul_ps(_mm_add_ps(_mm_mul_ps(Px_4x, axisYx_4x), _mm_mul_ps(Py_4x, axisYy_4x)), invLenSquareY_4x);
#if 0
            __m128i innerMask_4x = _mm_castps_si128(_mm_and_ps(
            _mm_and_ps( _mm_cmpge_ps(dx_4x, zero_4x), _mm_cmple_ps(dx_4x, one_4x) ),
            _mm_and_ps( _mm_cmpge_ps(dy_4x, zero_4x), _mm_cmple_ps(dy_4x, one_4x) )));
#endif
            dx_4x = _mm_mul_ps(dx_4x, bmpMaxX_4x);
            dy_4x = _mm_mul_ps(dy_4x, bmpMaxY_4x);

            // NOTE: Floor and Ceil to get 4 texels. Clamp 0~bmpDim
            __m128i dxI = _mm_cvttps_epi32(dx_4x);
            __m128i dyI = _mm_cvttps_epi32(dy_4x);
            __m128 u1_4x = mm_clamp_ps(_mm_cvtepi32_ps(dxI), zero_4x, bmpMaxX_4x);
            __m128 v1_4x = mm_clamp_ps(_mm_cvtepi32_ps(dyI), zero_4x, bmpMaxY_4x);
            __m128 u2_4x = mm_clamp_ps(_mm_add_ps(u1_4x, one_4x), zero_4x, bmpMaxX_4x);
            __m128 v2_4x = mm_clamp_ps(_mm_add_ps(v1_4x, one_4x), zero_4x, bmpMaxY_4x);

            // NOTE: Get weight among 4 texels. Clamp 0~1
            __m128 rx = mm_clamp_ps(_mm_mul_ps(_mm_sub_ps(dx_4x, u1_4x), _mm_rcp_ps(_mm_sub_ps(u2_4x, u1_4x))), zero_4x, one_4x);
            __m128 ry = mm_clamp_ps(_mm_mul_ps(_mm_sub_ps(dy_4x, v1_4x), _mm_rcp_ps(_mm_sub_ps(v2_4x, v1_4x))), zero_4x, one_4x);
            __m128 inv_rx = mm_clamp_ps(_mm_sub_ps(one_4x, rx), zero_4x, one_4x);
            __m128 inv_ry = mm_clamp_ps(_mm_sub_ps(one_4x, ry), zero_4x, one_4x);

            __m128i bmpPitch_4x = _mm_set1_epi32(bmp->pitch);
            __m128i bpp_4x = _mm_set1_epi32(BYTES_PER_PIXEL);

            __m128i txlA_4x;
            __m128i txlB_4x;
            __m128i txlC_4x;
            __m128i txlD_4x;

            for (s32 I = 0;
                    I < 4;
                    ++I) {
                u32 *base = (u32 *)( (u8 *)bmp->memory + (s32)M(v1_4x, I) * bmp->pitch + (s32)M(u1_4x, I) * BYTES_PER_PIXEL );
                Mi(txlA_4x, I) = *(base);
                Mi(txlB_4x, I) = *(base + 1);
                Mi(txlC_4x, I) = *(base + bmp->width);
                Mi(txlD_4x, I) = *(base + bmp->width + 1);
            }
            
            __m128 colorAA_4x  = _mm_cvtepi32_ps(_mm_and_si128(_mm_srli_epi32(txlA_4x, 24), maskFF_4x));
            __m128 colorAR_4x  = _mm_cvtepi32_ps(_mm_and_si128(_mm_srli_epi32(txlA_4x, 16), maskFF_4x));
            __m128 colorAG_4x  = _mm_cvtepi32_ps(_mm_and_si128(_mm_srli_epi32(txlA_4x,  8), maskFF_4x));
            __m128 colorAB_4x  = _mm_cvtepi32_ps(_mm_and_si128(txlA_4x, maskFF_4x));

            __m128 colorBA_4x  = _mm_cvtepi32_ps(_mm_and_si128(_mm_srli_epi32(txlB_4x, 24), maskFF_4x));
            __m128 colorBR_4x  = _mm_cvtepi32_ps(_mm_and_si128(_mm_srli_epi32(txlB_4x, 16), maskFF_4x));
            __m128 colorBG_4x  = _mm_cvtepi32_ps(_mm_and_si128(_mm_srli_epi32(txlB_4x,  8), maskFF_4x));
            __m128 colorBB_4x  = _mm_cvtepi32_ps(_mm_and_si128(txlB_4x, maskFF_4x));

            __m128 colorCA_4x  = _mm_cvtepi32_ps(_mm_and_si128(_mm_srli_epi32(txlC_4x, 24), maskFF_4x));
            __m128 colorCR_4x  = _mm_cvtepi32_ps(_mm_and_si128(_mm_srli_epi32(txlC_4x, 16), maskFF_4x));
            __m128 colorCG_4x  = _mm_cvtepi32_ps(_mm_and_si128(_mm_srli_epi32(txlC_4x,  8), maskFF_4x));
            __m128 colorCB_4x  = _mm_cvtepi32_ps(_mm_and_si128(txlC_4x, maskFF_4x));

            __m128 colorDA_4x  = _mm_cvtepi32_ps(_mm_and_si128(_mm_srli_epi32(txlD_4x, 24), maskFF_4x));
            __m128 colorDR_4x  = _mm_cvtepi32_ps(_mm_and_si128(_mm_srli_epi32(txlD_4x, 16), maskFF_4x));
            __m128 colorDG_4x  = _mm_cvtepi32_ps(_mm_and_si128(_mm_srli_epi32(txlD_4x,  8), maskFF_4x));
            __m128 colorDB_4x  = _mm_cvtepi32_ps(_mm_and_si128(txlD_4x, maskFF_4x));

            // NOTE: Lerp
            __m128 SA_4x = _mm_add_ps(
                    _mm_mul_ps(_mm_add_ps( _mm_mul_ps(colorAA_4x, inv_rx), _mm_mul_ps(colorBA_4x, rx) ), inv_ry),
                    _mm_mul_ps(_mm_add_ps( _mm_mul_ps(colorCA_4x, inv_rx), _mm_mul_ps(colorDA_4x, rx) ), ry) 
                    );
            __m128 SR_4x = _mm_add_ps(
                    _mm_mul_ps(_mm_add_ps( _mm_mul_ps(colorAR_4x, inv_rx), _mm_mul_ps(colorBR_4x, rx) ), inv_ry),
                    _mm_mul_ps(_mm_add_ps( _mm_mul_ps(colorCR_4x, inv_rx), _mm_mul_ps(colorDR_4x, rx) ), ry) 
                    );
            __m128 SG_4x = _mm_add_ps(
                    _mm_mul_ps(_mm_add_ps( _mm_mul_ps(colorAG_4x, inv_rx), _mm_mul_ps(colorBG_4x, rx) ), inv_ry),
                    _mm_mul_ps(_mm_add_ps( _mm_mul_ps(colorCG_4x, inv_rx), _mm_mul_ps(colorDG_4x, rx) ), ry) 
                    );
            __m128 SB_4x = _mm_add_ps(
                    _mm_mul_ps(_mm_add_ps( _mm_mul_ps(colorAB_4x, inv_rx), _mm_mul_ps(colorBB_4x, rx) ), inv_ry),
                    _mm_mul_ps(_mm_add_ps( _mm_mul_ps(colorCB_4x, inv_rx), _mm_mul_ps(colorDB_4x, rx) ), ry) 
                    );
            __m128 RSA_4x = _mm_mul_ps(SA_4x, inv255_4x);
            __m128 InvRSA_4x = _mm_sub_ps(one_4x, RSA_4x);
            
            // NOTE: Premultiply alpha.
            SR_4x = _mm_mul_ps(SR_4x, RSA_4x);
            SG_4x = _mm_mul_ps(SG_4x, RSA_4x);
            SB_4x = _mm_mul_ps(SB_4x, RSA_4x);

            u32 *dstBegin = (u32 *)buffer->memory + Y * buffer->width + X;
            __m128i DST_4x = _mm_loadu_si128((__m128i *)dstBegin);
            __m128 DA_4x = _mm_cvtepi32_ps(_mm_and_si128(_mm_srli_epi32(DST_4x, 24), maskFF_4x));
            __m128 DR_4x = _mm_cvtepi32_ps(_mm_and_si128(_mm_srli_epi32(DST_4x, 16), maskFF_4x));
            __m128 DG_4x = _mm_cvtepi32_ps(_mm_and_si128(_mm_srli_epi32(DST_4x,  8), maskFF_4x));
            __m128 DB_4x = _mm_cvtepi32_ps(_mm_and_si128(DST_4x, maskFF_4x));
            __m128 RDA_4x = _mm_mul_ps(DA_4x, inv255_4x);
            
            // NOTE: Premultiply alpha.
            DR_4x = _mm_mul_ps(DR_4x, RDA_4x);
            DG_4x = _mm_mul_ps(DG_4x, RDA_4x);
            DB_4x = _mm_mul_ps(DB_4x, RDA_4x);

            __m128 A_4x = _mm_mul_ps(m255_4x, _mm_sub_ps(_mm_add_ps(RSA_4x, RDA_4x), _mm_mul_ps(RSA_4x, RDA_4x)));
            __m128 R_4x = _mm_add_ps(_mm_mul_ps(InvRSA_4x, DR_4x), SR_4x);
            __m128 G_4x = _mm_add_ps(_mm_mul_ps(InvRSA_4x, DG_4x), SG_4x);
            __m128 B_4x = _mm_add_ps(_mm_mul_ps(InvRSA_4x, DB_4x), SB_4x);

            __m128i Ai_4x = _mm_slli_epi32(_mm_cvtps_epi32(A_4x), 24);
            __m128i Ri_4x = _mm_slli_epi32(_mm_cvtps_epi32(R_4x), 16);
            __m128i Gi_4x = _mm_slli_epi32(_mm_cvtps_epi32(G_4x),  8);
            __m128i Bi_4x = _mm_cvtps_epi32(B_4x);

            __m128i resultColor_4x = _mm_or_si128( _mm_or_si128(Ai_4x, Ri_4x), _mm_or_si128(Gi_4x, Bi_4x) );
            _mm_storeu_si128((__m128i *)dstBegin, resultColor_4x);
        }
    }

    RDTSC_END_ADDCOUNT(PerPixel, (maxX - minX + 1) * (maxY - minY + 1));

    RDTSC_END(RenderRectSlow);
}

struct DrawBmpWorkData {
    Bitmap *buffer;
    vec2 origin;
    vec2 axisX;
    vec2 axisY;
    Bitmap *bmp;
};

internal
PLATFORM_WORK_QUEUE_CALLBACK(DrawBmpWork) {
    DrawBmpWorkData *work = (DrawBmpWorkData *)data;
    DrawRectSlow(work->buffer, work->origin, work->axisX, work->axisY, work->bmp);
}

internal void
RenderGroupToOutput(RenderGroup *renderGroup, Bitmap *drawBuffer, PlatformWorkQueue *renderQueue) {
    vec2 screenCenter = vec2{
        0.5f * (r32)drawBuffer->width,
        0.5f * (r32)drawBuffer->height
    };

    u8 *at = renderGroup->base;

    while (at < renderGroup->base + renderGroup->used) {
        RenderEntityHeader *header =(RenderEntityHeader *)at;
        switch (header->type) {
            case RenderType_RenderEntityClear: {
                RenderEntityClear *piece = (RenderEntityClear *)at;
                at += sizeof(*piece);
            } break;

            case RenderType_RenderEntityBmp: {
                RenderEntityBmp *piece = (RenderEntityBmp *)at;
                vec2 origin = piece->origin;
                vec2 axisX = piece->axisX;
                vec2 axisY = piece->axisY;

                DrawBmpWorkData data = {};
                data.buffer = drawBuffer;
                data.origin = origin;
                data.axisX = axisX;
                data.axisY = axisY;
                data.bmp = piece->bmp;
#if 0
                // Multi-threaded path
                platformAddEntry(renderQueue, DrawBmpWork, &data);
#else
                // Single-threaded path
                // DrawRectSlow(drawBuffer, origin, axisX, axisY, piece->bmp);
                DrawBmpWork(renderQueue, &data);
#endif
                at += sizeof(*piece);
            } break;

            case RenderType_RenderEntityRect: {
                RenderEntityRect *piece = (RenderEntityRect *)at;
                DrawRect(drawBuffer, piece->min, piece->max, piece->color);
                at += sizeof(*piece);
            } break;

            case RenderType_RenderEntityCoordinateSystem: {
                RenderEntityCoordinateSystem *piece = (RenderEntityCoordinateSystem *)at;
                vec2 dim = vec2{4.0f, 4.0f};
                vec2 R = 0.5f * dim;
                vec4 yellow = vec4{1.0f, 1.0f, 0.0f, 1.0f};
                vec4 purple = vec4{1.0f, 0.0f, 1.0f, 1.0f};
                vec2 origin = piece->origin;
                vec2 axisX = piece->axisX;
                vec2 axisY = piece->axisY;
                DrawRectSlow(drawBuffer, origin, axisX, axisY, piece->bmp);
                DrawRect(drawBuffer, origin - R, origin + R, yellow);
                DrawRect(drawBuffer, origin + axisX - R, origin + axisX + R, yellow);
                DrawRect(drawBuffer, origin + axisY - R, origin + axisY + R, yellow);
                DrawRect(drawBuffer, origin + axisX + axisY - R, origin + axisX + axisY + R, yellow);
                at += sizeof(*piece);
            } break;

            INVALID_DEFAULT_CASE
        }
    }
}
