/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Sung Woo Lee $
   $Notice: (C) Copyright %s by Sung Woo Lee. All Rights Reserved. $
   ======================================================================== */

struct Game_Assets;
struct Noise_Map;

enum Render_Type 
{
    eRender_Mesh,
    eRender_Grass,
    eRender_Star,
    eRender_Bitmap,
};

struct Render_Entity_Header 
{
    Render_Type     type;
    u32             size;
};


struct Render_Quad 
{
    Render_Entity_Header    header;
    Bitmap                  *bitmap;
};

struct Render_Mesh
{
    Render_Entity_Header    header;
    Mesh              *mesh;
    Material          *material;
    m4x4              world_transform;
    m4x4              *animation_transforms;
};

struct Render_Grass
{
    Render_Entity_Header    header;
    Mesh              *mesh;
    u32                     count;
    m4x4                    *world_transforms;
    f32                     time;
    f32                     grass_max_vertex_y;
    Bitmap                  *turbulence_map;
};

struct Render_Star
{
    Render_Entity_Header    header;
    Mesh              *mesh;
    u32                     count;
    m4x4                    *world_transforms;
    f32                     time;
};

struct Render_Bitmap
{
    Render_Entity_Header    header;
    Bitmap                  *bitmap;
    v4                      color;
    v3                      min;
    v3                      max;
};


struct Textured_Vertex
{
    v3                      pos;
    v2                      uv;
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
    f32             width;
    f32             height;
    f32             N;
    f32             F;

    v3              world_translation;
    qt              world_rotation;

    m4x4            V;
    m4x4            P;
    m4x4            VP;
};

#if 0
enum Render_Group_Type
{
    eRender_Group_Skeletal_Mesh,
    eRender_Group_Static_Mesh,
    eRender_Group_Grass,
    eRender_Group_Star,
    eRender_Group_Text,

    eRender_Group_Count
};
#endif

struct Render_Group 
{
    //Render_Group_Type   type;

    size_t              capacity;
    size_t              used;
    u8                  *base;
    Camera              *camera;
};



#if 0
struct Render_Text 
{
    Render_Entity_Header    header;
    const char              *str;
    Game_Assets             *game_assets;
    f32                     scale;
    v4                      color;
};
#endif
