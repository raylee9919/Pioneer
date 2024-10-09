/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Sung Woo Lee $
   $Notice: (C) Copyright %s by Sung Woo Lee. All Rights Reserved. $
   ======================================================================== */

#define X(ext) ext,
#define GL_EXTENSION_LIST           \
    X(GL_ARB_multisample)
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

    b32     has_ext[GL_EXT_COUNT];
};

struct Mesh_Program
{
    s32     id;

    s32     world_transform;
    s32     voxel_VP;
    s32     clip_P;
    s32     persp_VP;
    s32     is_skeletal;
    s32     cam_pos;
    s32     bone_transforms;
    s32     color_ambient;
    s32     color_diffuse;
    s32     color_specular;
    s32     octree_resolution;

    s32     DEBUG_light_P;
    s32     DEBUG_light_color;
    s32     DEBUG_light_strength;
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

struct Voxelization_Program
{
    s32     id;

    s32     world_transform;
    s32     V;
    s32     voxel_P;
    s32     is_skeletal;
    s32     bone_transforms;
    s32     voxel_map;
    s32     ambient;
    s32     diffuse;
    s32     specular;
    s32     octree_level;
    s32     octree_resolution;
    s32     write;

    s32     DEBUG_light_P;
    s32     DEBUG_light_color;
    s32     DEBUG_light_strength;
};

struct Voxel_Program
{
    s32     id;

    s32     world_transform;
    s32     V;
    s32     persp_P;
    s32     ortho_P;
    s32     is_skeletal;
    s32     bone_transforms;
    s32     DEBUG_level;

    s32     octree_level;
    s32     octree_resolution;
};

struct G_Buffer_Program
{
    s32     id;

    s32     world_transform;
    s32     VP;
    s32     is_skeletal;
    s32     bone_transforms;
    s32     ambient;
    s32     diffuse;
    s32     specular;
};

struct Flag_Program
{
    s32     id;

    s32     current_level;
    s32     octree_level;
    s32     octree_resolution;
    s32     fragment_count;
};

struct Alloc_Program
{
    s32     id;

    s32     start;
    s32     alloc_size;
};

struct Init_Program
{
    s32     id;

    s32     start;
    s32     alloc_size;
};

struct Octree_Program
{
    s32     id;

    s32     octree_level;
    s32     octree_resolution;
    s32     fragment_count;
};

struct Flag_Leaf_Program
{
    s32     id;
    
    s32     leaf_start;
    s32     leaf_end;
};

struct Mipmap_Program
{
    s32     id;

    s32     level_start;
    s32     level_end;
};


struct Defer_Program
{
    s32     id;

    s32     gP;
    s32     gN;
    s32     gC;

    s32     cam_P;

    s32     voxel_in_meter;

    s32     DEBUG_light_P;
    s32     DEBUG_light_color;
    s32     DEBUG_light_strength;

    s32     voxel_P;
    s32     octree_level;
    s32     octree_resolution;
};

struct G_Buffer
{
    u32 id;

    u32 Pid;
    u32 Nid;
    u32 Cid;
};


struct GL 
{
    Mesh_Program            mesh_program;
    Grass_Program           grass_program;
    Sprite_Program          sprite_program;
    Star_Program            star_program;

    Voxelization_Program    voxelization_program;
    Voxel_Program           voxel_program;

    G_Buffer_Program        gbuffer_program;

    Flag_Program            flag_program;
    Alloc_Program           alloc_program;
    Init_Program            init_program;
    Octree_Program          octree_program;
    Flag_Leaf_Program       flag_leaf_program;
    Mipmap_Program          mipmap_program;

    Defer_Program           defer_program;

    G_Buffer                gbuffer;

    GLuint  vao;
    GLuint  vbo;
    GLuint  vio;

    GLuint  grass_vbo;
    GLuint  star_vbo;

    u32     shadow_map_fbo;
    u32     shadow_map;

    Bitmap  white_bitmap;
    u32     white[4][4];




    u32     octree_resolution;
    u32     fragment_counter;
    u32     alloc_count;
    u32     max_fragment_count;
    s32     max_compute_work_group_count[3];

    u32     octree_nodes;
    u32     octree_nodes_texture;
    u32     flist_P;
    u32     flist_P_texture;
    u32     flist_diffuse;
    u32     flist_diffuse_texture;

    u32     octree_diffuse;
    u32     octree_diffuse_texture;
};
