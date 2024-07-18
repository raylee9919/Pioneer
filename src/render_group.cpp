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
          m4x4 world_transform, v3 light_pos, m4x4 *animation_transforms = 0)
{
    Render_Mesh *piece          = push_render_entity(group, Render_Mesh);
    piece->mesh                 = mesh;
    piece->material             = material;
    piece->world_transform      = world_transform;
    piece->animation_transforms = animation_transforms;
    piece->light_pos            = light_pos;
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

enum Push_String_Flag
{
    DRAW = 0x1,
    GET_RECT = 0x2,
};

internal Rect2
push_string(Push_String_Flag flag, Render_Group *render_group,
            v2 left_bottom, f32 z,
            const char *str, Font *font, v4 color = _v4_(1, 1, 1, 1))
{
    Rect2 result = rect2_inv_inf();

    f32 left_x  = left_bottom.x;
    f32 kern    = 0.0f;
    f32 C       = 0.0f;
    f32 A       = 0.0f;

    for (const char *ch = str;
         *ch;
         ++ch)
    {
        Asset_Glyph *glyph = font->glyphs[*ch];
        if (glyph)
        {
            C = (f32)font->glyphs[*ch]->C;

            if (*ch != ' ')
            {
                Bitmap *bitmap = &glyph->bitmap;
                f32 w = (f32)bitmap->width;
                f32 h = (f32)bitmap->height;
                v3 max = _v3_(left_x + w, left_bottom.y + glyph->ascent, z);
                v3 min = max - _v3_(w, h, 0);

                if (flag & Push_String_Flag::DRAW)
                {
                    push_bitmap(render_group, min, max, bitmap, color);
                }

                if (flag & Push_String_Flag::GET_RECT)
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
            }
            else if (flag & Push_String_Flag::GET_RECT)
            {
                if (result.max.x < left_x + glyph->B + C)
                    result.max.x = left_x + glyph->B + C;
            }

            if (*(ch + 1))
            {
                kern = (f32)get_kerning(&font->kern_hashmap, *ch, *(ch + 1));
                if (font->glyphs[*(ch + 1)])
                {
                    A = (f32)font->glyphs[*(ch + 1)]->A;
                }
                f32 advance_x = (glyph->B + C + A + kern);
                left_x += advance_x;

                if (flag & Push_String_Flag::GET_RECT)
                {
                    if (result.max.x < left_x)
                        result.max.x = left_x;
                }
            }
        } 
        else if (*ch == ' ')
        {
            // TODO: horizontal advance info in asset.
            left_x += (C + 10.0f);
        } 
        else 
        {

        }
    }

    if (result.min.x == F32_MAX)
    {
        result.min = {};
        result.max = {};
    }

    return result;
}

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

        f32 f = camera->focal_length;
        f32 a = safe_ratio(camera->width, camera->height) * f;
        f32 N = 0.1f;
        f32 F = 500.0f;
        f32 b = (N + F) / (N - F);
        f32 c = (2 * N * F) / (N - F);
        m4x4 P = {{
                { f,  0,  0,  0},
                { 0,  a,  0,  0},
                { 0,  0,  b,  c},
                { 0,  0, -1,  0}
        }};
        camera->projection = P * V;
    } 
    else
    {
        f32 a = 2 * safe_ratio(camera->width, camera->height);
        f32 w = camera->width;
        camera->projection = m4x4{{
                { 2,  0,  0, -w},
                { 0,  a,  0, -w},
                { 0,  0,  w,  0},
                { 0,  0,  0,  w}
        }};
    }
}

internal Camera *
push_camera(Memory_Arena *arena, Camera_Type type, f32 width, f32 height,
            f32 focal_length = DEFAULT_FOCAL_LENGTH,
            v3 world_translation = _v3_(0, 0, 0),
            qt world_rotation = _qt_(1, 0, 0, 0))
{
    Camera *result = push_struct(arena, Camera);
    result->type                = type;
    result->width               = width;
    result->height              = height;
    result->focal_length        = focal_length;
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
