 /* ―――――――――――――――――――――――――――――――――――◆――――――――――――――――――――――――――――――――――――
    $File: $
    $Date: $
    $Revision: $
    $Creator: Sung Woo Lee $
    $Notice: (C) Copyright 2024 by Sung Woo Lee. All Rights Reserved. $
    ―――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――― */

#include "render.h"

//
// Push to Render Group
//
#define PushRenderEntity(GROUP, STRUCT) \
  (STRUCT *)__push_render_entity(GROUP, sizeof(STRUCT), RenderType_##STRUCT)
internal Render_Entity_Header *
__push_render_entity(Render_Group *renderGroup, u32 size, RenderType type) {
    Assert(size + renderGroup->used <= renderGroup->capacity);

    Render_Entity_Header *header = (Render_Entity_Header *)(renderGroup->base + renderGroup->used);
    header->type = type;
    header->size = size;

    renderGroup->used += size;

    return header;
}

internal void
push_bitmap(Render_Group *render_group,
            v3 origin, v3 axis_x, v3 axis_y,
            Bitmap *bitmap, v4 color = v4{1.0f, 1.0f, 1.0f, 1.0f})
{
    RenderEntityBitmap *piece = PushRenderEntity(render_group, RenderEntityBitmap);
    if (piece)
    {
        piece->origin       = origin;
        piece->axis_x       = axis_x;
        piece->axis_y       = axis_y;
        piece->bitmap       = bitmap;
        piece->color        = color;
    }
}

global_var r32 cen_y = 100.0f;
internal void
push_text(Render_Group *render_group, v3 base,
          const char *str, Game_Assets *game_assets,
          v4 color = v4{1.0f, 1.0f, 1.0f, 1.0f})
{
    r32 left_x = 40.0f;
    r32 kern = 0.0f;
    r32 C = 0.0f;
    r32 A = 0.0f;

    for (const char *ch = str;
         *ch;
         ++ch)
    {
        Asset_Glyph *glyph = game_assets->glyphs[*ch];
        if (glyph)
        {
            C = (r32)game_assets->glyphs[*ch]->C;
            if (*ch != ' ')
            {
                Bitmap *bitmap = &glyph->bitmap;
                r32 w = (r32)bitmap->width;
                r32 h = (r32)bitmap->height;
                // push_bitmap(render_group, v3{0.0f, 0.0f, 0.0f}, v2{left_x, cen_y - glyph->ascent}, v2{w, 0.0f}, v2{0.0f, h}, bitmap);
            }
            if (*(ch + 1))
            {
                kern = (r32)get_kerning(&game_assets->kern_hashmap, *ch, *(ch + 1));
                if (game_assets->glyphs[*(ch + 1)])
                {
                    A = (r32)game_assets->glyphs[*(ch + 1)]->A;
                }
                r32 advance_x = (glyph->B + C + A + kern);
                left_x += advance_x;
            }
        } 
        else if (*ch == ' ')
        {
            // TODO: horizontal advance info in asset.
            left_x += C + 10.0f;
        } 
        else 
        {

        }
    }
    cen_y += game_assets->v_advance;
}

#if 0
internal void
draw_bitmap_slow(Bitmap *buf, v2 min, Bitmap *bmp, v4 color = v4{1.0f, 1.0f, 1.0f, 1.0f}) {
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
#endif

#if 0
internal void
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
            draw_bitmap_slow(buffer, v2{left_x, cen_y - glyph->ascent}, bitmap, info->color);
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
#endif

internal void
push_rect(Render_Group *renderGroup,
          v2 min, v2 max, v4 color) {
    RenderEntityRect *piece = PushRenderEntity(renderGroup, RenderEntityRect);
    if (piece) {
        piece->min = min;
        piece->max = max;
        piece->color = color;
    }
}

internal Render_Group *
alloc_render_group(Memory_Arena *arena, b32 ortho, r32 aspect_ratio) {
    Render_Group *result = PushStruct(arena, Render_Group);
    *result = {};
    result->capacity            = MB(4);
    result->base                = (u8 *)PushSize(arena, result->capacity);
    result->used                = 0;
    result->sort_entry_begin    = result->base + result->capacity;

    Camera *cam = &result->camera;
    cam->orthographic       = ortho;
    cam->focal_length       = 0.5f;

    if (cam->orthographic) {
        r32 a = aspect_ratio;
        cam->projection = m4x4{{
            { 1,  0,  0,  0},
            { 0,  a,  0,  0},
            { 0,  0,  1,  0},
            { 0,  0,  0,  1}
        }};

    } else { // perspective.
        cam->aspect_ratio       = aspect_ratio;

        m4x4 cam_o = (x_rotation(g_debug_cam_orbital_pitch) *
                      z_rotation(g_debug_cam_orbital_yaw));
        v3 cam_translation = v3{0.0f, 0.0f, g_debug_cam_z};

        m4x4 cam_c = camera_transform(get_column(cam_o, 0),
                                      get_column(cam_o, 1),
                                      get_column(cam_o, 2),
                                      cam_o * cam_translation); // focus on origin.

        cam->projection         = cam_c;
    }

    return result;
}


#if 0
internal void
draw_rect(Bitmap *buffer, v2 min, v2 max, v4 color) {
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
#endif

#if 0
internal void
DrawRectSlowAsf(Bitmap *buffer, v2 origin, v2 axisX, v2 axisY, Bitmap *bmp) {

    s32 bufWidthMax = buffer->width - 1;
    s32 bufHeightMax = buffer->height - 1;

    s32 minX = bufWidthMax;
    s32 maxX = 0;
    s32 minY = bufHeightMax;
    s32 maxY = 0;

    v2 Vs[4] = {
        origin,
        origin + axisX,
        origin + axisY,
        origin + axisX + axisY
    };

    for (s32 idx = 0;
            idx < ArrayCount(Vs);
            ++idx) {
        v2 V = Vs[idx];
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
            v2 P = v2{(r32)X, (r32)Y} - origin;
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


#if 0 // pure cpu simd render.
internal void
draw_bitmap_fast(Bitmap *buffer, v2 origin, v2 axisX, v2 axisY, Bitmap *bmp, v4 color) {
    TIMED_BLOCK();

    s32 bufWidthMax = buffer->width - 1;
    s32 bufHeightMax = buffer->height - 1;

    s32 minX = bufWidthMax;
    s32 maxX = 0;
    s32 minY = bufHeightMax;
    s32 maxY = 0;

    v2 Vs[4] = {
        origin,
        origin + axisX,
        origin + axisY,
        origin + axisX + axisY
    };

    for (s32 idx = 0;
            idx < ArrayCount(Vs);
            ++idx) {
        v2 V = Vs[idx];
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
#endif

#if 1
internal void
push_sort_entry(Render_Group *group, u8 *entity, v3 base) {
    group->sort_entry_begin -= sizeof(Sort_Entry);
    Assert(group->sort_entry_begin >= group->base + group->used);
    Sort_Entry *entry = (Sort_Entry *)group->sort_entry_begin;

    entry->render_entity = entity;
    entry->base = base;
}
#endif

#if 0
internal void
sort_render_group(Render_Group *group) {
    TIMED_BLOCK();
    // TODO: bubble sort... for now. O(n^2). Ain't really thrilled about it.
    // comparing r32 for several times cost some shit.
    for (Sort_Entry *bubble = (Sort_Entry *)group->sort_entry_begin + 1;
         (u8 *)bubble < group->base + group->capacity;
         ++bubble) {
        for (Sort_Entry *cmp = bubble - 1;
             (u8 *)cmp >= group->sort_entry_begin;
             --cmp) {
            if (bubble->base.z < cmp->base.z) {
                Sort_Entry tmp = *cmp;
                *cmp = *bubble;
                *bubble = tmp;
                bubble = cmp;
            } else if (bubble->base.y > cmp->base.y) {
                Sort_Entry tmp = *cmp;
                *cmp = *bubble;
                *bubble = tmp;
                bubble = cmp;
            }
        }
    }
}
#endif

internal void
push_render_group(Render_Group *group, Render_Batch *batch) {
    Assert(batch->used + sizeof(Render_Group) <= batch->size);
    *(Render_Group *)((u8 *)batch->base + batch->used) = *group;
    batch->used += sizeof(Render_Group);
}

internal void
RenderGroupToOutput(Render_Group *render_group, Platform_API *platform, Render_Batch *batch) {
    TIMED_BLOCK();

    u8 *at = render_group->base;

    // add sort entries at the back of the render group.
#if 1
    while (at < render_group->base + render_group->used) {
        Render_Entity_Header *header =(Render_Entity_Header *)at;
        push_sort_entry(render_group, at, header->base);
        at += header->size;
    }
#endif

#if 0
    // sort.
    sort_render_group(render_group);
#endif

    // pass to platform to draw.
    push_render_group(render_group, batch);
}
