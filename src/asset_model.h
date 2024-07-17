#ifndef ASSET_MODEL_H
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Sung Woo Lee $
   $Notice: (C) Copyright %s by Sung Woo Lee. All Rights Reserved. $
   ======================================================================== */

#define MAX_BONE_PER_VERTEX     4
#define MAX_BONE_PER_MESH       100

#pragma pack(push, 1)

struct Asset_Vertex
{
    v3      pos;
    v3      normal;
    v2      uv;
    v4      color;

    s32     node_ids[MAX_BONE_PER_VERTEX];
    f32     node_weights[MAX_BONE_PER_VERTEX];
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

struct Asset_Node
{
    s32     id;

    m4x4    offset;     // mesh-space to bone-space. aiBone... if unavailable, set to no-op matrix...?
    m4x4    transform;  // transform in parent's bone-space. aiNode

    u32     child_count;
    s32     *child_ids;
};

struct Asset_Texture
{
    u32     width;
    u32     height;
    void    *contents;
};

struct Asset_Model
{
    u32                     mesh_count;
    Asset_Mesh              *meshes;

    u32                     material_count;
    Asset_Material          *materials;

    u32                     node_count;
    s32                     root_bone_node_id;
    Asset_Node              *nodes;

    u32                     texture_count;
    Asset_Texture           *textures;
};


#pragma pack(pop)

#define ASSET_MODEL_H
#endif
