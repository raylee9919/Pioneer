 /* ―――――――――――――――――――――――――――――――――――◆――――――――――――――――――――――――――――――――――――
    $File: $
    $Date: $
    $Revision: $
    $Creator: Sung Woo Lee $
    $Notice: (C) Copyright 2024 by Sung Woo Lee. All Rights Reserved. $
    ―――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――― */

enum Render_Type 
{
    eRender_Quad,
    eRender_Text,
    eRender_Skeletal_Mesh,
    eRender_Static_Mesh
};

struct Render_Entity_Header 
{
    Render_Type     type;
    u32             size;
    v3              base;
};

//
// Render Entities
//

struct Game_Assets;


struct Render_Quad 
{
    Render_Entity_Header header;
    Bitmap *bitmap;
};

struct Render_Text 
{
    Render_Entity_Header header;
    const char *str;
    Game_Assets *game_assets;
    f32 scale;
    v4 color;
};

struct Render_Skeletal_Mesh
{
    Render_Entity_Header    header;
    Asset_Mesh              *mesh;
    Asset_Material          *material;
    m4x4                    world_transform;
    m4x4                    *animation_transforms;
};

struct Render_Static_Mesh
{
    Render_Entity_Header    header;
    Asset_Mesh              *mesh;
    Asset_Material          *material;
    m4x4                    world_transform;
};

enum Camera_Type
{
    eCamera_Type_Perspective,
    eCamera_Type_Orthographic,
};

struct Camera 
{
    Camera_Type     type;
    f32             focal_length;
    f32             w_over_h;

    v3              world_translation;
    qt              world_rotation;

    m4x4            projection;
};

struct Textured_Vertex 
{
    v3      p;
    v2      uv;
    v4      color;
    v3      normal;
};

enum Render_Group_Type
{
    eRender_Group_Skeletal_Mesh,
    eRender_Group_Static_Mesh,
    eRender_Group_Grass,
    eRender_Group_Text,

    eRender_Group_Count
};

struct Render_Group 
{
    Render_Group_Type   type;

    size_t              capacity;
    size_t              used;
    u8                  *base;
    Camera              *camera;

    Textured_Vertex     *vertices;
    size_t              vertex_count;
    size_t              varray_size;
};

