/* ―――――――――――――――――――――――――――――――――――◆――――――――――――――――――――――――――――――――――――
   $File: $
   $Date: $
   $Revision: $
   $Creator: Sung Woo Lee $
   $Notice: (C) Copyright 2024 by Sung Woo Lee. All Rights Reserved. $
   ―――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――― */

#define GL_EXTENSION_LIST           \
    X(GL_EXT_texture_sRGB)          \
    X(GL_EXT_framebuffer_sRGB)

#define X(ext) ext,
enum GL_Extensions {
    GL_EXTENSION_LIST
    GL_EXT_COUNT
};
#undef X

struct GL_Info 
{
    b32     modern;
    char    *vendor;
    char    *renderer;
    char    *version;
    char    *shading_language_version;
    char    *extensions;
    s32     major;
    s32     minor;

    b32     has_ext[GL_EXT_COUNT];
};

struct Skeletal_Mesh_Program
{
    s32     id;

    s32     world_transform;
    s32     mvp;
    s32     texture_sampler;
    s32     cam_pos;
    s32     bone_transforms;
    s32     color_ambient;
    s32     color_diffuse;
    s32     color_specular;
};

struct Static_Mesh_Program
{
    s32     id;

    s32     world_transform;
    s32     mvp;
    s32     texture_sampler;
    s32     cam_pos;
    s32     color_ambient;
    s32     color_diffuse;
    s32     color_specular;
};

struct Grass_Program
{
    s32     id;

    s32     mvp;
    s32     time;
    s32     grass_max_vertex_y;
};

struct GL 
{
    Skeletal_Mesh_Program   skeletal_mesh_program;
    Static_Mesh_Program     static_mesh_program;
    Grass_Program           grass_program;

    GLuint  vao;
    GLuint  vbo;
    GLuint  vio;

    GLuint  grass_vbo;

    Bitmap  white_bitmap;
    u32     white[4][4];
};
