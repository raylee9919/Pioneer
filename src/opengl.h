/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Sung Woo Lee $
   $Notice: (C) Copyright %s by Sung Woo Lee. All Rights Reserved. $
   ======================================================================== */

#define GL_EXTENSION_LIST           \
    X(GL_EXT_texture_sRGB)          \
    X(GL_EXT_framebuffer_sRGB)      \
    X(GL_ARB_multisample)

#define X(ext) ext,
enum GL_Extensions 
{
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

    s32     texture_internal_format;
    b32     is_framebuffer_srgb;

    b32     has_ext[GL_EXT_COUNT];
};

struct Mesh_Program
{
    s32     id;

    s32     world_transform;
    s32     mvp;
    s32     is_skeletal;
    s32     texture_sampler;
    s32     cam_pos;
    s32     bone_transforms;
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
    s32     turbulence_map;
};

struct Star_Program
{
    s32     id;

    s32     mvp;
    s32     time;
};

struct Sprite_Program
{
    s32     id;

    s32     mvp;
    s32     color;
    s32     texture;
};

struct GL 
{
    Mesh_Program            mesh_program;
    Grass_Program           grass_program;
    Sprite_Program          sprite_program;
    Star_Program            star_program;

    GLuint  vao;
    GLuint  vbo;
    GLuint  vio;

    GLuint  grass_vbo;
    GLuint  star_vbo;

    Bitmap  white_bitmap;
    u32     white[4][4];
};
