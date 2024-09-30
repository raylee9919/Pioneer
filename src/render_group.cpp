/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Sung Woo Lee $
   $Notice: (C) Copyright %s by Sung Woo Lee. All Rights Reserved. $
   ======================================================================== */

#include "render_group.h"

#define push_render_entity(GROUP, STRUCT)  \
    (STRUCT *)__push_render_entity(GROUP, sizeof(STRUCT), e##STRUCT)

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
begin_render(Render_Group *render_group)
{
    // TODO:
}

internal void
end_render(Render_Group *render_group)
{
    if (render_group)
    {
        render_group->used = 0;
    }
}

internal void
push_mesh(Render_Group *group, Mesh *mesh, Material *material,
          m4x4 world_transform, m4x4 *animation_transforms = 0)
{
    Render_Mesh *piece          = push_render_entity(group, Render_Mesh);
    piece->mesh                 = mesh;
    piece->material             = material;
    piece->world_transform      = world_transform;
    piece->animation_transforms = animation_transforms;
}

internal void
push_grass(Render_Group *group,
           Mesh *mesh,
           u32 count,
           m4x4 *world_transforms,
           f32 time,
           f32 grass_max_vertex_y,
           Bitmap *turbulence_map)
{
    Render_Grass *piece         = push_render_entity(group, Render_Grass);
    piece->mesh                 = mesh;
    piece->count                = count;
    piece->world_transforms     = world_transforms;
    piece->time                 = time;
    piece->grass_max_vertex_y   = grass_max_vertex_y;
    piece->turbulence_map       = turbulence_map;
}

internal void
push_star(Render_Group *group,
           Mesh *mesh,
           u32 count,
           m4x4 *world_transforms,
           f32 time)
{
    Render_Star *piece          = push_render_entity(group, Render_Star);
    piece->mesh                 = mesh;
    piece->count                = count;
    piece->world_transforms     = world_transforms;
    piece->time                 = time;
}

internal void
push_bitmap(Render_Group *group,
            v3 min, v3 max,
            Bitmap *bitmap = 0, v4 color = _v4_(1, 1, 1, 1))
{
    Render_Bitmap *piece        = push_render_entity(group, Render_Bitmap);
    piece->min                  = min;
    piece->max                  = max;
    piece->bitmap               = bitmap;
    piece->color                = color;
}

inline void
push_rect(Render_Group *group, Rect2 rect, f32 z, v4 color)
{
    push_bitmap(group, _v3_(rect.min, z), _v3_(rect.max, z), 0, color);
}

enum String_Op : u8
{
    DRAW        = 0x1,
    GET_RECT    = 0x2,
};

internal Rect2
string_op(u8 flag, Render_Group *render_group,
          v3 left_bottom,
          const char *str, Font *font, v4 color = v4{1, 1, 1, 1})
{
    Rect2 result = rect2_inv_inf();

    f32 left_x  = left_bottom.x;
    f32 kern    = 0.0f;
    f32 A       = 0.0f;

    for (const char *ch = str;
         *ch;
         ++ch)
    {
        Asset_Glyph *glyph = font->glyphs[*ch];
        if (glyph)
        {
            f32 B = (f32)font->glyphs[*ch]->B;
            f32 C = (f32)font->glyphs[*ch]->C;
    
            if (*ch != ' ')
            {
                Bitmap *bitmap = &glyph->bitmap;
                f32 w = (f32)bitmap->width;
                f32 h = (f32)bitmap->height;
                v3 max = v3{left_x + w, left_bottom.y + glyph->ascent, left_bottom.z};
                v3 min = max - v3{w, h, 0};

                if (flag & String_Op::DRAW)
                {
                    push_bitmap(render_group, min, max, bitmap, color);
                }

                if (flag & String_Op::GET_RECT)
                {
                    if (result.min.x > min.x)
                        result.min.x = min.x;
                    if (result.min.y > min.y)
                        result.min.y = min.y;
                    if (result.max.x < max.x)
                        result.max.x = max.x;
                    if (result.max.y < max.y)
                        result.max.y = max.y;
                }

                left_x += w;
            }
            else
            {
                f32 max_x = left_x + B + C;
                f32 min_x = left_x;
                left_x += (B + C);

                if (flag & String_Op::GET_RECT)
                {
                    if (result.min.x > min_x)
                        result.min.x = min_x;
                    if (result.max.x < max_x)
                        result.max.x = max_x;
                }
            }

            if (*(ch + 1))
            {
                kern = (f32)get_kerning(&font->kern_hashmap, *ch, *(ch + 1));
                if (font->glyphs[*(ch + 1)])
                {
                    A = (f32)font->glyphs[*(ch + 1)]->A;
                }
                f32 advance_x = (A + kern);
                left_x += advance_x;
            }
        } 
    }

    if (result.min.x == F32_MAX)
        result = {};

    return result;
}

internal m4x4
perspective_m4x4(f32 width, f32 height, f32 focal_length, f32 near, f32 far)
{
    f32 f = focal_length;
    f32 N = near;
    f32 F = far;
    f32 a = safe_ratio(2.0f * f, width);
    f32 b = safe_ratio(2.0f * f, height);
    f32 c = (N + F) / (N - F);
    f32 d = (2 * N * F) / (N - F);
    m4x4 P = {{
        { a,  0,  0,  0},
        { 0,  b,  0,  0},
        { 0,  0,  c,  d},
        { 0,  0, -1,  0}
    }};
    return P;
}

internal m4x4
orthographic_m4x4(f32 width, f32 height, f32 near, f32 far)
{
    f32 w = safe_ratio(2.0f, width);
    f32 h = safe_ratio(2.0f, height);
    f32 N = near;
    f32 F = far;
    f32 a = safe_ratio(2.0f, N-F);
    f32 b = safe_ratio(N+F, N-F);
    m4x4 P = m4x4{{
        { w,  0,  0,  0},
        { 0,  h,  0,  0},
        { 0,  0,  a,  b},
        { 0,  0,  0,  1}
    }};
    
    return P;
}

#define DEBUG_FAR 500.0f

internal void
set_camera_projection(Camera *camera)
{
    if (camera->type == eCamera_Type_Perspective) 
    {
        m4x4 camera_rotation = to_m4x4(camera->world_rotation);
        m4x4 V = camera_transform(get_column(camera_rotation, 0),
                                  get_column(camera_rotation, 1),
                                  get_column(camera_rotation, 2),
                                  camera->world_translation);
        m4x4 P = perspective_m4x4(camera->width, camera->height, camera->focal_length, camera->focal_length, DEBUG_FAR);
        camera->V   = V;
        camera->P   = P;
        camera->VP  = P*V;
    } 
    else // @TODO: this is busted asf.
    {
        m4x4 camera_rotation = to_m4x4(camera->world_rotation);
        m4x4 V = camera_transform(get_column(camera_rotation, 0),
                                  get_column(camera_rotation, 1),
                                  get_column(camera_rotation, 2),
                                  camera->world_translation);

        f32 w = safe_ratio(2.0f, camera->width);
        f32 h = safe_ratio(2.0f, camera->height);
        f32 N = camera->focal_length;
        f32 F = DEBUG_FAR;
        f32 a = safe_ratio(2.0f, N-F);
        f32 b = safe_ratio(N+F, N-F);
        m4x4 P = m4x4{{
                { w,  0,  0, -1},
                { 0,  h,  0, -1},
                { 0,  0,  a,  b},
                { 0,  0,  0,  1}
        }};

        camera->VP = P*V;
    }
}

internal Camera *
push_camera(Memory_Arena *arena, Camera_Type type, f32 width, f32 height,
            f32 focal_length, f32 N, f32 F,
            v3 world_translation = _v3_(0, 0, 0),
            qt world_rotation = _qt_(1, 0, 0, 0))
{
    Camera *result = push_struct(arena, Camera);
    result->type                = type;
    result->width               = width;
    result->height              = height;
    result->focal_length        = focal_length;
    result->N                   = N;
    result->F                   = F;
    result->world_translation   = world_translation;
    result->world_rotation      = world_rotation;

    return result;
}

internal Render_Group *
alloc_render_group(Memory_Arena *arena, size_t size, Camera *camera)
{
    Render_Group *result = push_struct(arena, Render_Group);
    *result = {};
    result->capacity            = size;
    result->base                = (u8 *)push_size(arena, result->capacity);
    result->used                = 0;
    result->camera = camera;

    set_camera_projection(camera);

    return result;
}

internal void
render_group_to_output_batch(Render_Group *group, Render_Batch *batch)
{
    Assert(batch->used + sizeof(Render_Group) <= batch->size);

    *(Render_Group *)((u8 *)batch->base + batch->used) = *group;
    batch->used += sizeof(Render_Group);
}
