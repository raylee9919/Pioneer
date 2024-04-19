 /* ―――――――――――――――――――――――――――――――――――◆――――――――――――――――――――――――――――――――――――
    $File: $
    $Date: $
    $Revision: $
    $Creator: Sung Woo Lee $
    $Notice: (C) Copyright 2024 by Sung Woo Lee. All Rights Reserved. $
    ―――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――― */

#include "render_group.h"

#define push_render_entity(GROUP, STRUCT)  (STRUCT *)__push_render_entity(GROUP, sizeof(STRUCT), e##STRUCT)
internal Render_Entity_Header *
__push_render_entity(Render_Group *renderGroup, u32 size, Render_Type type)
{
    Assert(size + renderGroup->used <= renderGroup->capacity);

    Render_Entity_Header *header = (Render_Entity_Header *)(renderGroup->base + renderGroup->used);
    header->type = type;
    header->size = size;

    renderGroup->used += size;

    return header;
}


internal void
push_vertex(Render_Group *group, v3 P, v2 uv, v4 color, v3 normal)
{
    Assert(group->vertex_count + 1 <= group->varray_size);
    Textured_Vertex vertex = Textured_Vertex{P, uv, color, normal};
    group->vertices[group->vertex_count++] = vertex;
}

internal void
push_quad(Render_Group *group, v3 origin, v3 axis_x, v3 axis_y,
            Bitmap *bitmap, v4 color = v4{1.0f, 1.0f, 1.0f, 1.0f})
{
    Render_Quad *piece = push_render_entity(group, Render_Quad);

    // TODO:
    v3 normal = v3{0.0f, 0.0f, 1.0f};

    v3 V[4];
    V[0] = origin;
    V[1] = origin + axis_x;
    V[2] = origin + axis_y;
    V[3] = origin + axis_x + axis_y;

    push_vertex(group, V[0], v2{0, 0}, color, normal);
    push_vertex(group, V[1], v2{1, 0}, color, normal);
    push_vertex(group, V[2], v2{0, 1}, color, normal);
    push_vertex(group, V[3], v2{1, 1}, color, normal);

    if (piece) {
        piece->bitmap = bitmap;
    }
}

internal void
push_cube(Render_Group *group,
          v3 base, r32 radius, r32 height, v4 color = v4{1.0f, 1.0f, 1.0f, 1.0f})
{
    r32 min_x = base.x - radius;
    r32 max_x = base.x + radius;
    r32 min_y = base.y - radius;
    r32 max_y = base.y + radius;
    r32 min_z = base.z - height;
    r32 max_z = base.z;
    v3 V[8] = {
        v3{min_x, min_y, max_z},
        v3{max_x, min_y, max_z},
        v3{max_x, max_y, max_z},
        v3{min_x, max_y, max_z},
        v3{min_x, min_y, min_z},
        v3{max_x, min_y, min_z},
        v3{max_x, max_y, min_z},
        v3{min_x, max_y, min_z},
    };
    push_quad(group, V[4], V[5] - V[4], V[0] - V[4], 0, color);
    push_quad(group, V[5], V[6] - V[5], V[1] - V[5], 0, color);
    push_quad(group, V[6], V[7] - V[6], V[2] - V[6], 0, color);
    push_quad(group, V[7], V[4] - V[7], V[3] - V[7], 0, color);
    push_quad(group, V[0], V[1] - V[0], V[3] - V[0], 0, color);
    push_quad(group, V[5], V[4] - V[5], V[6] - V[5], 0, color);
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

internal Render_Group *
alloc_render_group(Memory_Arena *arena, b32 ortho, r32 aspect_ratio) {
    Render_Group *result = push_struct(arena, Render_Group);
    *result = {};
    result->capacity            = MB(4);
    result->base                = (u8 *)push_size(arena, result->capacity);
    result->used                = 0;
#define VARRAY_SIZE 65536
    result->vertices            = push_array(arena, Textured_Vertex, VARRAY_SIZE);
    result->vertex_count        = 0;
    result->varray_size         = VARRAY_SIZE;

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
        cam->w_over_h       = aspect_ratio;

        m4x4 cam_o = (x_rotation(g_debug_cam_orbital_pitch) *
                      y_rotation(g_debug_cam_orbital_yaw));
        v3 cam_translation = v3{0.0f, 0.0f, g_debug_cam_z};

        m4x4 cam_c = camera_transform(get_column(cam_o, 0),
                                      get_column(cam_o, 1),
                                      get_column(cam_o, 2),
                                      cam_o * cam_translation); // focus on origin.

        r32 f = cam->focal_length;
        r32 a = cam->w_over_h * f;
        r32 N = 0.1f;
        r32 F = 100.0f;
        r32 b = (N + F) / (N - F);
        r32 c = (2 * N * F) / (N - F);
        m4x4 proj = {{
            { f,  0,  0,  0},
            { 0,  a,  0,  0},
            { 0,  0,  b,  c},
            { 0,  0, -1,  0}
        }};

        cam->projection = proj * cam_c;
    }

    return result;
}

internal void
render_group_to_output_batch(Render_Group *group, Render_Batch *batch)
{
    TIMED_BLOCK();
    Assert(batch->used + sizeof(Render_Group) <= batch->size);
    *(Render_Group *)((u8 *)batch->base + batch->used) = *group;
    batch->used += sizeof(Render_Group);
}
