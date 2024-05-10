/* ―――――――――――――――――――――――――――――――――――◆――――――――――――――――――――――――――――――――――――
   $File: $
   $Date: $
   $Revision: $
   $Creator: Sung Woo Lee $
   $Notice: (C) Copyright 2024 by Sung Woo Lee. All Rights Reserved. $
   ―――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――― */

#define MAX_BONE_PER_VERTEX     4
#define MAX_BONE_PER_MESH       100

#pragma pack(push, 1)
//
// Mesh
//
struct Asset_Vertex
{
    v3      pos;
    v3      normal;
    v2      uv;
    v4      color;

    s32     bone_ids[MAX_BONE_PER_VERTEX]; // if bone_idx is -1, it means it's empty.
    f32     bone_weights[MAX_BONE_PER_VERTEX];
};

struct Asset_Mesh
{
    u32             vertex_count;
    Asset_Vertex    *vertices;

    u32             index_count;
    u32             *indices;

    u32             material_idx;
};

struct Asset_Material
{
    v3  color_ambient;
    v3  color_diffuse;
    v3  color_specular;
};

struct Asset_Bone
{
    s32     bone_id;    // bone-id is globally bound.
    m4x4    offset;     // mesh-space to bone-space.
    m4x4    transform;  // transform in parent's bone-space.
};


// Animation
struct dt_v3_Pair
{
    f32     dt;
    v3      vec;
};
struct dt_qt_Pair
{
    f32     dt;
    qt      q;
};
struct Asset_Animation_Bone
{
    s32             bone_id;

    u32             translation_count;
    u32             rotation_count;
    u32             scaling_count;

    dt_v3_Pair      *translations;
    dt_qt_Pair      *rotations;
    dt_v3_Pair      *scalings;
};
struct Asset_Animation
{
    s32                     id;
    f32                     duration;

    u32                     bone_count;
    Asset_Animation_Bone    *bones;
};

struct Asset_Model
{
    u32                     mesh_count;
    Asset_Mesh              *meshes;

    u32                     material_count;
    Asset_Material          *materials;

    // skeletal
    u32                     bone_count;
    s32                     root_bone_id;
    m4x4                    root_transform; // TODO: why dafuq is it here?
    Asset_Bone              *bones;

    u32                     anim_count;
    Asset_Animation         *anims;
};


//
// Bone Hierarchy
//
struct Asset_Bone_Info
{
    u32    child_count;
    s32    *child_ids;
};

struct Asset_Bone_Hierarchy
{
    Asset_Bone_Info bone_infos[MAX_BONE_PER_MESH];
};



#pragma pack(pop)
