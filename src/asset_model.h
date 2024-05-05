/* ―――――――――――――――――――――――――――――――――――◆――――――――――――――――――――――――――――――――――――
   $File: $
   $Date: $
   $Revision: $
   $Creator: Sung Woo Lee $
   $Notice: (C) Copyright 2024 by Sung Woo Lee. All Rights Reserved. $
   ―――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――― */

#define MAX_BONE_WEIGHT_PER_VERTEX              4
#define MAX_BONE_BINDING_COUNT                  100

#pragma pack(push, 1)
//
// Mesh
//
struct Asset_Vertex
{
    v3  pos;
    v3  normal;
    v2  uv;
    v4  color;

    // if bone_idx is -1, it means it's empty.
    s32    bone_ids[MAX_BONE_WEIGHT_PER_VERTEX];
    f32    bone_weights[MAX_BONE_WEIGHT_PER_VERTEX];
};

struct Asset_Bone
{
    s32     bone_id;    // bone-id is globally bound.
    m4x4    offset;     // local-space to bone-space.
    m4x4    transform;  // transform in parent's bone-space.
};

struct Asset_Mesh
{
    // vertices
    u32             vertex_count;
    Asset_Vertex    *vertices;

    // vertex indices
    u32             index_count;
    u32             *indices;

    // skeletal
    u32             bone_count;
    s32             root_bone_id;
    Asset_Bone      *bones;         // first bone must be the root bone.
};

struct Asset_Model
{
    u32                     mesh_count;
    Asset_Mesh              *meshes;
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
    Asset_Bone_Info bone_infos[MAX_BONE_BINDING_COUNT];
};


//
// Animation
//
struct dt_v3_Pair
{
    f32     dt;
    v3      vec;
};
struct dt_quat_Pair
{
    f32     dt;
    quat    q;
};
struct Asset_Animation_Bone
{
    s32             bone_id;

    u32             translation_count;
    u32             rotation_count;
    u32             scaling_count;

    dt_v3_Pair      *translations;
    dt_quat_Pair    *rotations;
    dt_v3_Pair      *scalings;
};

struct Asset_Animation
{
    s32     id;
    u32     frame_count;
    f32     fps;

    u32                     bone_count;
    Asset_Animation_Bone   *bones;
};

#pragma pack(pop)
