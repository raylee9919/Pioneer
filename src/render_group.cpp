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

#if 0
internal void
push_vertex(Render_Group *group, v3 P, v2 uv, v4 color, v3 normal)
{
    Assert(group->vertex_count + 1 <= group->varray_size);
    Textured_Vertex vertex = Textured_Vertex{P, uv, color, normal};
    group->vertices[group->vertex_count++] = vertex;
}
#endif

internal void
push_mesh(Render_Group *group, Asset_Mesh *mesh, m4x4 *animtion_transforms = 0)
{
    Render_Mesh *piece          = push_render_entity(group, Render_Mesh);
    piece->mesh                 = mesh;
    piece->animation_transforms = animtion_transforms;
}


#if 0
internal void
push_quad(Render_Group *group, v3 O, v3 ax, v3 ay,
          Bitmap *bitmap, v4 color = v4{1.0f, 1.0f, 1.0f, 1.0f})
{
    Render_Quad *piece = push_render_entity(group, Render_Quad);

    v3 normal = normalize(cross(ax, ay));

    v3 V[4];
    V[0] = O;
    V[1] = O + ax;
    V[2] = O + ay;
    V[3] = O + ax + ay;

    push_vertex(group, V[0], v2{0, 0}, color, normal);
    push_vertex(group, V[1], v2{1, 0}, color, normal);
    push_vertex(group, V[2], v2{0, 1}, color, normal);
    push_vertex(group, V[3], v2{1, 1}, color, normal);

    if (piece) 
    {
        piece->bitmap = bitmap;
    }
}


internal void
push_cube(Render_Group *group,
          v3 base, f32 radius, f32 height, v4 color = v4{1.0f, 1.0f, 1.0f, 1.0f})
{
    f32 min_x = base.x - radius;
    f32 max_x = base.x + radius;
    f32 min_y = base.y - radius;
    f32 max_y = base.y + radius;
    f32 min_z = base.z - height;
    f32 max_z = base.z;
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

global_var f32 cen_y = 100.0f;
internal void
push_text(Render_Group *render_group, v3 base,
          const char *str, Game_Assets *game_assets,
          v4 color = v4{1.0f, 1.0f, 1.0f, 1.0f})
{
    f32 left_x = 40.0f;
    f32 kern = 0.0f;
    f32 C = 0.0f;
    f32 A = 0.0f;

    for (const char *ch = str;
         *ch;
         ++ch)
    {
        Asset_Glyph *glyph = game_assets->glyphs[*ch];
        if (glyph)
        {
            C = (f32)game_assets->glyphs[*ch]->C;
            if (*ch != ' ')
            {
                Bitmap *bitmap = &glyph->bitmap;
                f32 w = (f32)bitmap->width;
                f32 h = (f32)bitmap->height;
                // push_bitmap(render_group, v3{0.0f, 0.0f, 0.0f}, v2{left_x, cen_y - glyph->ascent}, v2{w, 0.0f}, v2{0.0f, h}, bitmap);
            }
            if (*(ch + 1))
            {
                kern = (f32)get_kerning(&game_assets->kern_hashmap, *ch, *(ch + 1));
                if (game_assets->glyphs[*(ch + 1)])
                {
                    A = (f32)game_assets->glyphs[*(ch + 1)]->A;
                }
                f32 advance_x = (glyph->B + C + A + kern);
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
#endif


#if 0
internal void
draw_text(Bitmap *buffer, Render_Text *info) 
{
    f32 left_x = 40.0f;
    f32 kern = 0.0f;
    f32 C = 0.0f;
    f32 A = 0.0f;
    for (const char *ch = info->str;
            *ch;
            ++ch) {
        Asset_Glyph *glyph = info->game_assets->glyphs[*ch];
        Bitmap *bitmap = &glyph->bitmap;
        if (info->game_assets->glyphs[*ch]) {
            C = (f32)info->game_assets->glyphs[*ch]->C;
        }
        if (glyph) {
            f32 w = (f32)bitmap->width;
            f32 h = (f32)bitmap->height;
            draw_bitmap_slow(buffer, v2{left_x, cen_y - glyph->ascent}, bitmap, info->color);
            if (*(ch + 1)) {
                kern = (f32)get_kerning(&info->game_assets->kern_hashmap, *ch, *(ch + 1));
                if (info->game_assets->glyphs[*(ch + 1)]) {
                    A = (f32)info->game_assets->glyphs[*(ch + 1)]->A;
                }
                f32 advance_x = (glyph->B + C + A + kern);
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
alloc_render_group(Memory_Arena *arena, b32 ortho, f32 aspect_ratio)
{
    Render_Group *result = push_struct(arena, Render_Group);
    *result = {};
    result->capacity            = MB(4);
    result->base                = (u8 *)push_size(arena, result->capacity);
    result->used                = 0;
#define VARRAY_SIZE MB(1)
    result->vertices            = push_array(arena, Textured_Vertex, VARRAY_SIZE);
    result->vertex_count        = 0;
    result->varray_size         = VARRAY_SIZE;

    Camera *cam = &result->camera;
    cam->orthographic       = ortho;
    cam->focal_length       = 0.5f;

    if (cam->orthographic) 
    {
        f32 a = aspect_ratio;
        cam->projection = m4x4{{
            { 1,  0,  0,  0},
            { 0,  a,  0,  0},
            { 0,  0,  1,  0},
            { 0,  0,  0,  1}
        }};
    } 
    else 
    {
        cam->w_over_h = aspect_ratio;

        m4x4 cam_o = (x_rotation(g_debug_cam_orbital_pitch) *
                      y_rotation(g_debug_cam_orbital_yaw));
        v3 cam_translation = v3{0.0f, 0.0f, g_debug_cam_z};
        m4x4 cam_c = camera_transform(get_column(cam_o, 0),
                                      get_column(cam_o, 1),
                                      get_column(cam_o, 2),
                                      cam_o * cam_translation); // always focus on origin.

        cam->world_pos = cam_o * cam_translation;

        f32 f = cam->focal_length;
        f32 a = cam->w_over_h * f;
        f32 N = 0.1f;
        f32 F = 100.0f;
        f32 b = (N + F) / (N - F);
        f32 c = (2 * N * F) / (N - F);
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
