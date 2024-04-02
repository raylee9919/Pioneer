 /* ―――――――――――――――――――――――――――――――――――◆――――――――――――――――――――――――――――――――――――
    $File: $
    $Date: $
    $Revision: $
    $Creator: Sung Woo Lee $
    $Notice: (C) Copyright 2024 by Sung Woo Lee. All Rights Reserved. $
    ―――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――― */

#include "render.h"

///////////////////////////////////////////////////////////////////////////////
//
// Push to Render Group
//
#define PushRenderEntity(GROUP, STRUCT) \
  (STRUCT *)__push_render_entity(GROUP, sizeof(STRUCT), RenderType_##STRUCT)
internal RenderEntityHeader *
__push_render_entity(RenderGroup *renderGroup, u32 size, RenderType type) {
    Assert(size + renderGroup->used <= renderGroup->capacity);
    RenderEntityHeader *header = (RenderEntityHeader *)(renderGroup->base + renderGroup->used);
    header->type = type;
    renderGroup->used += size;

    return header;
}

internal void
push_bitmap(RenderGroup *renderGroup, vec2 origin, vec2 axisX, vec2 axisY,
        Bitmap *bmp, vec4 color = vec4{1.0f, 1.0f, 1.0f, 1.0f}) {
    RenderEntityBmp *piece = PushRenderEntity(renderGroup, RenderEntityBmp);
    if (piece) {
        piece->origin = origin;
        piece->axisX = axisX;
        piece->axisY = axisY;
        piece->bmp = bmp;
        piece->color = color;
    }
}

internal void
push_text(RenderGroup *renderGroup, const char *str, Game_Assets *gameAssets, r32 scale, vec4 color = vec4{1.0f, 1.0f, 1.0f, 1.0f}) {
    Render_Text *piece = PushRenderEntity(renderGroup, Render_Text);
    if (piece) {
        piece->str = str;
        piece->game_assets = gameAssets;
        piece->scale = scale;
        piece->color = color;
    }
}

internal void
draw_bitmap_slow(Bitmap *buf, vec2 min, Bitmap *bmp, vec4 color = vec4{1.0f, 1.0f, 1.0f, 1.0f}) {
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
            // normalized.
            r32 sa = (r32)((*src >> 24) & 0xff) / 255.0f * color.a;
            r32 sr = (r32)((*src >> 16) & 0xff) / 255.0f;        
            r32 sg = (r32)((*src >>  8) & 0xff) / 255.0f;        
            r32 sb = (r32)((*src >>  0) & 0xff) / 255.0f;        

            // square out from sRGB.
            sr *= sr;
            sg *= sg;
            sb *= sb;

            // premultiply alpha.
            sr *= sa;
            sg *= sa;
            sb *= sa;

            // 0~255
            sr *= 255.0f * color.r;
            sg *= 255.0f * color.g;
            sb *= 255.0f * color.b;

            // normalized.
            r32 da = (r32)((*dst >> 24) & 0xff) / 255.0f;
            r32 dr = (r32)((*dst >> 16) & 0xff) / 255.0f;
            r32 dg = (r32)((*dst >>  8) & 0xff) / 255.0f;
            r32 db = (r32)((*dst >>  0) & 0xff) / 255.0f;

            // square out from sRGB.
            dr *= dr;
            dg *= dg;
            db *= db;

            // premultiply alpha.
            dr *= da;
            dg *= da;
            db *= da;

            // 0~255
            dr *= 255.0f;
            dg *= 255.0f;
            db *= 255.0f;

            // blend.
            r32 inv_sa = (1.0f - sa);
            r32 a = sa + da * inv_sa;
            r32 r = sr + dr * inv_sa;
            r32 g = sg + dg * inv_sa;
            r32 b = sb + db * inv_sa;

            // square-root to sRGB.
            r = sqrt(r / 255.0f) * 255.0f;
            g = sqrt(g / 255.0f) * 255.0f;
            b = sqrt(b / 255.0f) * 255.0f;
            a *= 255.0f;

            *dst = (((u32)(a + 0.5f) << 24) |
                    ((u32)(r + 0.5f) << 16) |
                    ((u32)(g + 0.5f) <<  8) |
                    ((u32)(b + 0.5f) <<  0));
            
            ++dst;
            ++src;
        }

        dstRow += buf->pitch;
        srcRow += bmp->pitch;
    }
}

global_var r32 cen_y = 100.0f;
internal void
// draw_text(RenderGroup *renderGroup, const char *str, Game_Assets *gameAssets, r32 scale, vec4 color = vec4{1.0f, 1.0f, 1.0f, 1.0f}) {
draw_text(Bitmap *buffer, Render_Text *info) {
    r32 left_x = 40.0f;
    r32 kern = 0.0f;
    r32 C = 0.0f;
    r32 A = 0.0f;
    for (const char *ch = info->str;
            *ch;
            ++ch) {
        Asset_Glyph *glyph = info->game_assets->glyphs[*ch];
        Bitmap *bitmap = &glyph->bitmap;
        if (info->game_assets->glyphs[*ch]) {
            C = (r32)info->game_assets->glyphs[*ch]->C;
        }
        if (glyph) {
            r32 w = (r32)bitmap->width;
            r32 h = (r32)bitmap->height;
            draw_bitmap_slow(buffer, vec2{left_x, cen_y - glyph->ascent}, bitmap, info->color);
            if (*(ch + 1)) {
                kern = (r32)get_kerning(&info->game_assets->kern_hashmap, *ch, *(ch + 1));
                if (info->game_assets->glyphs[*(ch + 1)]) {
                    A = (r32)info->game_assets->glyphs[*(ch + 1)]->A;
                }
                r32 advance_x = (glyph->B + C + A + kern);
                left_x += advance_x;
            }
        } else if (*ch == ' ') {
            // TODO: horizontal advance info in asset.
            left_x += C + 10.0f;
        } else {

        }
    }
    cen_y += info->game_assets->v_advance;
}

internal void
push_rect(RenderGroup *renderGroup,
        vec2 min, vec2 max, vec4 color) {
    RenderEntityRect *piece = PushRenderEntity(renderGroup, RenderEntityRect);
    if (piece) {
        piece->min = min;
        piece->max = max;
        piece->color = color;
    }
}

internal void
push_coordinate_system(RenderGroup *renderGroup,
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
AllocRenderGroup(Memory_Arena *memoryArena) {
    RenderGroup *result = PushStruct(memoryArena, RenderGroup);
    *result = {};
    result->capacity = MB(4);
    result->base = (u8 *)PushSize(memoryArena, result->capacity);
    result->used = 0;

    return result;
}


internal void
draw_rect(Bitmap *buffer, vec2 min, vec2 max, vec4 color) {
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

    r32 A = color.a;
    r32 R = color.r;
    r32 G = color.g;
    r32 B = color.b;

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

#if 0
internal void
DrawRectSlowAsf(Bitmap *buffer, vec2 origin, vec2 axisX, vec2 axisY, Bitmap *bmp) {

    s32 bufWidthMax = buffer->width - 1;
    s32 bufHeightMax = buffer->height - 1;

    s32 minX = bufWidthMax;
    s32 maxX = 0;
    s32 minY = bufHeightMax;
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

    if (minX < 0) { minX = 0; }
    if (minY < 0) { minY = 0; }
    if (maxX > bufWidthMax) { maxX = bufWidthMax; }
    if (maxY > bufHeightMax) { maxY = bufHeightMax; }

    for (s32 Y = minY;
            Y <= maxY;
            ++Y) {
        for (s32 X = minX;
                X <= maxX;
                ++X) {
            vec2 P = vec2{(r32)X, (r32)Y} - origin;
            r32 Uf = Inner(P, axisX) * InvLenSquare(axisX);
            r32 Vf = Inner(P, axisY) * InvLenSquare(axisY);
            if (Uf >= 0.0f &&
                Vf >= 0.0f &&
                Uf <= 1.0f &&
                Vf <= 1.0f) {

                Uf *= (bmp->width - 1);
                Vf *= (bmp->height - 1);

                // NOTE: Floor and get weight.
                s32 Ui = FloorR32ToS32(Uf);
                s32 Vi = FloorR32ToS32(Vf);
                r32 Rx = Uf - (r32)Ui;
                r32 Ry = Vf - (r32)Vi;

                u32 *texel0 = (u32*)((u8 *)bmp->memory + Vi * bmp->pitch + Ui * 4);
                u32 *texel1 = texel0 + 1;
                if (Ui == bmp->width - 1) {
                    --texel1;
                }
                u32 *texel2 = (u32 *)((u8 *)texel0 + bmp->pitch);
                if (Vi == bmp->height - 1) {
                    texel2 -= bmp->pitch;
                }
                u32 *texel3 = texel2 + 1;
                if (Ui == bmp->width - 1) {
                    --texel3;
                }

                // NOTE: Fetch 4 texels.
                // Square to move out from sRGB.
                r32 A0 = (r32)((*texel0 >> 24) & 0xFF) / 255.0f; // normalized
                r32 R0 = Square((r32)((*texel0 >> 16) & 0xFF)) / 255.0f;
                r32 G0 = Square((r32)((*texel0 >>  8) & 0xFF)) / 255.0f;
                r32 B0 = Square((r32)((*texel0 >>  0) & 0xFF)) / 255.0f;

                r32 A1 = (r32)((*texel1 >> 24) & 0xFF) / 255.0f; // normalized
                r32 R1 = Square( (r32)((*texel1 >> 16) & 0xFF)) / 255.0f;
                r32 G1 = Square( (r32)((*texel1 >>  8) & 0xFF)) / 255.0f;
                r32 B1 = Square( (r32)((*texel1 >>  0) & 0xFF)) / 255.0f;

                r32 A2 = (r32)((*texel2 >> 24) & 0xFF) / 255.0f; // normalized
                r32 R2 = Square( (r32)((*texel2 >> 16) & 0xFF)) / 255.0f;
                r32 G2 = Square( (r32)((*texel2 >>  8) & 0xFF)) / 255.0f;
                r32 B2 = Square( (r32)((*texel2 >>  0) & 0xFF)) / 255.0f;

                r32 A3 = (r32)((*texel3 >> 24) & 0xFF) / 255.0f; // normalized
                r32 R3 = Square( (r32)((*texel3 >> 16) & 0xFF)) / 255.0f;
                r32 G3 = Square( (r32)((*texel3 >>  8) & 0xFF)) / 255.0f;
                r32 B3 = Square( (r32)((*texel3 >>  0) & 0xFF)) / 255.0f;

                // NOTE: Bilinear filtering and premultiply alpha.
                r32 SA = Lerp(Lerp(A0, A1, Rx), Lerp(A2, A3, Rx), Ry);
                r32 SR = Lerp(Lerp(R0, R1, Rx), Lerp(R2, R3, Rx), Ry) * SA;
                r32 SG = Lerp(Lerp(G0, G1, Rx), Lerp(G2, G3, Rx), Ry) * SA;
                r32 SB = Lerp(Lerp(B0, B1, Rx), Lerp(B2, B3, Rx), Ry) * SA;

                // NOTE: Fetch what was before in the background buffer.
                // Square out from sRGB and premultiply alpha.
                u32 *dst = (u32 *)buffer->memory + Y * buffer->width + X;
                r32 DA = (r32)((*dst >> 24) & 0xFF) / 255.0f; // normalized
                r32 DR = Square((r32)((*dst >> 16) & 0xFF)) / 255.0f * DA;
                r32 DG = Square((r32)((*dst >>  8) & 0xFF)) / 255.0f * DA;
                r32 DB = Square((r32)((*dst >>  0) & 0xFF)) / 255.0f * DA;

                // NOTE: Get FINAL pixel values.
                // SquareRoot to sRGB.
                r32 FR = Sqrt( (SR + DR * (1.0f - SA)) * 255.0f );
                r32 FG = Sqrt( (SG + DG * (1.0f - SA)) * 255.0f );
                r32 FB = Sqrt( (SB + DB * (1.0f - SA)) * 255.0f );
                r32 FA = ( (SA + DA * (1.0f - SA)) * 255.0f );
                
                u32 F = ((u32)FA << 24) |
                        ((u32)FR << 16) |
                        ((u32)FG <<  8) | 
                        ((u32)FB <<  0);

                // NOTE: Write to buffer.
                *dst = F;
            }
        }
    }

}
#endif


internal void
draw_bitmap_fast(Bitmap *buffer, vec2 origin, vec2 axisX, vec2 axisY, Bitmap *bmp, vec4 color) {
    TIMED_BLOCK();

    s32 bufWidthMax = buffer->width - 1;
    s32 bufHeightMax = buffer->height - 1;

    s32 minX = bufWidthMax;
    s32 maxX = 0;
    s32 minY = bufHeightMax;
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

    if (minX < 0) { minX = 0; }
    if (minY < 0) { minY = 0; }
    if (maxX > bufWidthMax) { maxX = bufWidthMax; }
    if (maxY > bufHeightMax) { maxY = bufHeightMax; }


#define M(m, i)  ((r32 *)&m)[i]
#define Mi(m, i) ((u32 *)&m)[i]
#define _mm_clamp01_ps(A) _mm_max_ps(_mm_min_ps(A, Onef), Zerof)
#define _mm_square_ps(A) _mm_mul_ps(A, A)
#define mmLerp(A, B, T)  _mm_add_ps(_mm_mul_ps(T, B), _mm_mul_ps(_mm_sub_ps(Onef, T), A))

    __m128 Zerof = _mm_set1_ps(0.0f);
    __m128 Onef = _mm_set1_ps(1.0f);
    __m128 point_5 = _mm_set1_ps(0.5f);
    __m128i MaskFF = _mm_set1_epi32(0xFF);
    __m128 m255f = _mm_set1_ps(255.0f);
    __m128 inv255f = _mm_set1_ps(1.0f / 255.0f);
    __m128 tint_r = _mm_clamp01_ps(_mm_set1_ps(color.r));
    __m128 tint_g = _mm_clamp01_ps(_mm_set1_ps(color.g));
    __m128 tint_b = _mm_clamp01_ps(_mm_set1_ps(color.b));
    __m128 alphaf = _mm_clamp01_ps(_mm_set1_ps(color.a));

    __m128 Ox = _mm_set1_ps(origin.x);
    __m128 Oy = _mm_set1_ps(origin.y);
    __m128 axisXx = _mm_set1_ps(axisX.x);
    __m128 axisXy = _mm_set1_ps(axisX.y);
    __m128 axisYx = _mm_set1_ps(axisY.x);
    __m128 axisYy = _mm_set1_ps(axisY.y);

    // TODO: This is bit incorrect mathematically, but looks fine.
    __m128 bmpWidthMinusTwo = _mm_set1_ps((r32)(bmp->width - 2));
    __m128 bmpHeightMinusTwo = _mm_set1_ps((r32)(bmp->height - 2));

    __m128 InvLenSquareX = _mm_set1_ps(InvLenSquare(axisX));
    __m128 InvLenSquareY = _mm_set1_ps(InvLenSquare(axisY));


    {
    for (s32 Y = minY;
            Y <= maxY;
            ++Y) {

        __m128i Yi = _mm_set1_epi32(Y);
        __m128 Yf = _mm_cvtepi32_ps(Yi);
        __m128 Py = _mm_sub_ps(Yf, Oy);

        for (s32 X = minX;
                X <= maxX;
                X += 4) {
            TIMED_BLOCK(4);

            // NOTE: Clamp X
            // TODO: Must to it properly.
            if (X + 3 > maxX) { X = maxX - 3; }
            if (X < 0) { X = 0; }

            __m128i Xi = _mm_setr_epi32(X, X + 1, X + 2, X + 3);
            __m128 Xf = _mm_cvtepi32_ps(Xi);
            __m128 Px = _mm_sub_ps(Xf, Ox);

            // NOTE: We'll just clamp U and V to guarantee that
            // we fetch from valid memory. Then, whatever the value is,
            // we'll knock out useless ones with write mask.
            __m128 Uf = _mm_clamp01_ps(_mm_mul_ps(_mm_add_ps(_mm_mul_ps(Px, axisXx), _mm_mul_ps(Py, axisXy)), InvLenSquareX));
            __m128 Vf = _mm_clamp01_ps(_mm_mul_ps(_mm_add_ps(_mm_mul_ps(Px, axisYx), _mm_mul_ps(Py, axisYy)), InvLenSquareY));

            // NOTE: Write Mask from inner product.
            __m128i WriteMask = _mm_castps_si128(_mm_and_ps(
                        _mm_and_ps( _mm_cmpge_ps(Uf, Zerof), _mm_cmple_ps(Uf, Onef) ),
                        _mm_and_ps( _mm_cmpge_ps(Vf, Zerof), _mm_cmple_ps(Vf, Onef) )));

            Uf = _mm_mul_ps(Uf, bmpWidthMinusTwo);
            Vf = _mm_mul_ps(Vf, bmpHeightMinusTwo);



            ///////////////////////////////////////////////////////////////////////////////
            //
            // Bilinear Filtering
            //
            __m128 SA, SR, SG, SB;
#if 1
            // NOTE: Floor and get weight.
            __m128i Ui = _mm_cvttps_epi32(Uf);
            __m128i Vi = _mm_cvttps_epi32(Vf);
            __m128 Rx = _mm_sub_ps(Uf, _mm_cvtepi32_ps(Ui));
            __m128 Ry = _mm_sub_ps(Vf, _mm_cvtepi32_ps(Vi));

            // NOTE: Fetch 4 texels for each pixel.
            // Sadly, there's no such things as SIMD fetch.
            // So the loop will stay.

            __m128i texel0 = _mm_set1_epi32(0);
            __m128i texel1 = _mm_set1_epi32(0);
            __m128i texel2 = _mm_set1_epi32(0);
            __m128i texel3 = _mm_set1_epi32(0);

            for (int I = 0;
                    I < 4;
                    ++I) {
                s32 movY = (Mi(Vi, I) * bmp->pitch);
                s32 movX = (Mi(Ui, I) * sizeof(u32));

                u8 *txl0 = (u8 *)bmp->memory + movY + movX;
                u8 *txl1 = txl0 + sizeof(u32);
                u8 *txl2 = txl0 + bmp->pitch;
                u8 *txl3 = txl2 + sizeof(u32);

                Mi(texel0, I) = *(u32 *)txl0;
                Mi(texel1, I) = *(u32 *)txl1;
                Mi(texel2, I) = *(u32 *)txl2;
                Mi(texel3, I) = *(u32 *)txl3;
            }

            // NOTE: Square to move out from sRGB.
            __m128 A0 =  _mm_mul_ps(_mm_cvtepi32_ps(_mm_and_si128(_mm_srli_epi32(texel0, 24), MaskFF)), inv255f); // 0-1
            __m128 R0 =  _mm_mul_ps(_mm_square_ps(_mm_cvtepi32_ps(_mm_and_si128(_mm_srli_epi32(texel0, 16), MaskFF))), inv255f); // 0-255
            __m128 G0 =  _mm_mul_ps(_mm_square_ps(_mm_cvtepi32_ps(_mm_and_si128(_mm_srli_epi32(texel0,  8), MaskFF))), inv255f); // 0-255
            __m128 B0 =  _mm_mul_ps(_mm_square_ps(_mm_cvtepi32_ps(_mm_and_si128(_mm_srli_epi32(texel0,  0), MaskFF))), inv255f); // 0-255

            __m128 A1 =  _mm_mul_ps(_mm_cvtepi32_ps(_mm_and_si128(_mm_srli_epi32(texel1, 24), MaskFF)), inv255f); // 0-1
            __m128 R1 =  _mm_mul_ps(_mm_square_ps(_mm_cvtepi32_ps(_mm_and_si128(_mm_srli_epi32(texel1, 16), MaskFF))), inv255f); // 0-255
            __m128 G1 =  _mm_mul_ps(_mm_square_ps(_mm_cvtepi32_ps(_mm_and_si128(_mm_srli_epi32(texel1,  8), MaskFF))), inv255f); // 0-255
            __m128 B1 =  _mm_mul_ps(_mm_square_ps(_mm_cvtepi32_ps(_mm_and_si128(_mm_srli_epi32(texel1,  0), MaskFF))), inv255f); // 0-255

            __m128 A2 =  _mm_mul_ps(_mm_cvtepi32_ps(_mm_and_si128(_mm_srli_epi32(texel2, 24), MaskFF)), inv255f); // 0-1
            __m128 R2 =  _mm_mul_ps(_mm_square_ps(_mm_cvtepi32_ps(_mm_and_si128(_mm_srli_epi32(texel2, 16), MaskFF))), inv255f); // 0-255
            __m128 G2 =  _mm_mul_ps(_mm_square_ps(_mm_cvtepi32_ps(_mm_and_si128(_mm_srli_epi32(texel2,  8), MaskFF))), inv255f); // 0-255
            __m128 B2 =  _mm_mul_ps(_mm_square_ps(_mm_cvtepi32_ps(_mm_and_si128(_mm_srli_epi32(texel2,  0), MaskFF))), inv255f); // 0-255

            __m128 A3 =  _mm_mul_ps(_mm_cvtepi32_ps(_mm_and_si128(_mm_srli_epi32(texel3, 24), MaskFF)), inv255f); // 0-1
            __m128 R3 =  _mm_mul_ps(_mm_square_ps(_mm_cvtepi32_ps(_mm_and_si128(_mm_srli_epi32(texel3, 16), MaskFF))), inv255f); // 0-255
            __m128 G3 =  _mm_mul_ps(_mm_square_ps(_mm_cvtepi32_ps(_mm_and_si128(_mm_srli_epi32(texel3,  8), MaskFF))), inv255f); // 0-255
            __m128 B3 =  _mm_mul_ps(_mm_square_ps(_mm_cvtepi32_ps(_mm_and_si128(_mm_srli_epi32(texel3,  0), MaskFF))), inv255f); // 0-255

            // NOTE: Bilinear filtering and premultiply alpha.
            SA = _mm_mul_ps(mmLerp(mmLerp(A0, A1, Rx), mmLerp(A2, A3, Rx), Ry), alphaf); // 0-1
            SR = _mm_mul_ps(mmLerp(mmLerp(R0, R1, Rx), mmLerp(R2, R3, Rx), Ry), SA); // 0-255
            SG = _mm_mul_ps(mmLerp(mmLerp(G0, G1, Rx), mmLerp(G2, G3, Rx), Ry), SA); // 0-255
            SB = _mm_mul_ps(mmLerp(mmLerp(B0, B1, Rx), mmLerp(B2, B3, Rx), Ry), SA); // 0-255
#else
            // Round to nearest pixel
            __m128i Ui = _mm_cvtps_epi32(Uf);
            __m128i Vi = _mm_cvtps_epi32(Vf);

            __m128i texel = _mm_set1_epi32(0);

            for (int I = 0;
                    I < 4;
                    ++I) {
                s32 movY = (Mi(Vi, I) * bmp->pitch);
                s32 movX = (Mi(Ui, I) * sizeof(u32));

                u8 *txl = (u8 *)bmp->memory + movY + movX;
                Mi(texel, I) = *(u32 *)txl;
            }

            // Square out from sRGB area
            SA =  _mm_mul_ps(_mm_cvtepi32_ps(_mm_and_si128(_mm_srli_epi32(texel, 24), MaskFF)), inv255f); // 0-1
            SR =  _mm_mul_ps(_mm_square_ps(_mm_cvtepi32_ps(_mm_and_si128(_mm_srli_epi32(texel, 16), MaskFF))), inv255f); // 0-255
            SG =  _mm_mul_ps(_mm_square_ps(_mm_cvtepi32_ps(_mm_and_si128(_mm_srli_epi32(texel,  8), MaskFF))), inv255f); // 0-255
            SB =  _mm_mul_ps(_mm_square_ps(_mm_cvtepi32_ps(_mm_and_si128(_mm_srli_epi32(texel,  0), MaskFF))), inv255f); // 0-255

            // Premultiply Alpha
            SR = _mm_mul_ps(SR, SA); // 0-255
            SG = _mm_mul_ps(SG, SA); // 0-255
            SB = _mm_mul_ps(SB, SA); // 0-255
#endif

            // tint.
            SR = _mm_mul_ps(SR, tint_r); // 0-255
            SG = _mm_mul_ps(SG, tint_g); // 0-255
            SB = _mm_mul_ps(SB, tint_b); // 0-255

            // NOTE: Fetch what was before in the background buffer.
            // Square out from sRGB and premultiply alpha.
            u32 *dst = (u32 *)buffer->memory + Y * buffer->width + X;
            __m128i D =  _mm_loadu_si128((__m128i *)dst);
            __m128 DA =  _mm_mul_ps(_mm_cvtepi32_ps(_mm_and_si128(_mm_srli_epi32(D, 24), MaskFF)), inv255f); // 0-1
            __m128 DR =  _mm_mul_ps(_mm_mul_ps(_mm_square_ps(_mm_cvtepi32_ps(_mm_and_si128(_mm_srli_epi32(D, 16), MaskFF))), inv255f), DA); // 0-255
            __m128 DG =  _mm_mul_ps(_mm_mul_ps(_mm_square_ps(_mm_cvtepi32_ps(_mm_and_si128(_mm_srli_epi32(D,  8), MaskFF))), inv255f), DA); // 0-255
            __m128 DB =  _mm_mul_ps(_mm_mul_ps(_mm_square_ps(_mm_cvtepi32_ps(_mm_and_si128(_mm_srli_epi32(D,  0), MaskFF))), inv255f), DA); // 0-255

            // NOTE: Get FINAL pixel values.
            // SquareRoot to sRGB.
            __m128i FR =_mm_cvttps_epi32(_mm_sqrt_ps(_mm_mul_ps(_mm_add_ps(SR, _mm_mul_ps(DR, _mm_sub_ps(Onef, SA))), m255f))); // 0-255
            __m128i FG =_mm_cvttps_epi32(_mm_sqrt_ps(_mm_mul_ps(_mm_add_ps(SG, _mm_mul_ps(DG, _mm_sub_ps(Onef, SA))), m255f))); // 0-255
            __m128i FB =_mm_cvttps_epi32(_mm_sqrt_ps(_mm_mul_ps(_mm_add_ps(SB, _mm_mul_ps(DB, _mm_sub_ps(Onef, SA))), m255f))); // 0-255
            __m128i FA =_mm_cvttps_epi32(_mm_mul_ps(_mm_add_ps(SA, _mm_mul_ps(DA, _mm_sub_ps(Onef, SA))), m255f));              // 0-255

            __m128i F =  _mm_or_si128(
                    _mm_or_si128(_mm_slli_epi32(FA, 24), _mm_slli_epi32(FR, 16)), 
                    _mm_or_si128(_mm_slli_epi32(FG,  8), _mm_slli_epi32(FB,  0)) );

            // NOTE: Write to buffer.
            _mm_storeu_si128((__m128i *)dst, _mm_and_si128(F, WriteMask));
        }
    }
    }

}

internal void
RenderGroupToOutput(RenderGroup *renderGroup, Bitmap *outputBuffer,
        TransientState *transState, Platform_API *platform) {
    TIMED_BLOCK();
    vec2 screenCenter = vec2{
        0.5f * (r32)outputBuffer->width,
        0.5f * (r32)outputBuffer->height
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
                draw_bitmap_fast(outputBuffer, piece->origin, piece->axisX, piece->axisY, piece->bmp, piece->color);
                at += sizeof(*piece);
            } break;

            case RenderType_Render_Text: {
                Render_Text *piece = (Render_Text *)at;
                draw_text(outputBuffer, piece);
                at += sizeof(*piece);
            } break;

            case RenderType_RenderEntityRect: {
                RenderEntityRect *piece = (RenderEntityRect *)at;
                draw_rect(outputBuffer, piece->min, piece->max, piece->color);
                at += sizeof(*piece);
            } break;

            case RenderType_RenderEntityCoordinateSystem: {
                RenderEntityCoordinateSystem *piece = (RenderEntityCoordinateSystem *)at;
#if 0
                vec2 dim = vec2{4.0f, 4.0f};
                vec2 R = 0.5f * dim;
                vec4 yellow = vec4{1.0f, 1.0f, 0.0f, 1.0f};
                vec4 purple = vec4{1.0f, 0.0f, 1.0f, 1.0f};
                vec2 origin = piece->origin;
                vec2 axisX = piece->axisX;
                vec2 axisY = piece->axisY;
                draw_rect_fast(outputBuffer, origin, axisX, axisY, piece->bmp);
                DrawRect(outputBuffer, origin - R, origin + R, yellow);
                DrawRect(outputBuffer, origin + axisX - R, origin + axisX + R, yellow);
                DrawRect(outputBuffer, origin + axisY - R, origin + axisY + R, yellow);
                DrawRect(outputBuffer, origin + axisX + axisY - R, origin + axisX + axisY + R, yellow);
#endif
                at += sizeof(*piece);
            } break;

            INVALID_DEFAULT_CASE
        }
    }

    platform->platform_complete_all_work(transState->highPriorityQueue);

}
