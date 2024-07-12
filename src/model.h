#ifndef MODEL_H
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Sung Woo Lee $
   $Notice: (C) Copyright 2024 by Sung Woo Lee. All Rights Reserved. $
   ======================================================================== */

#define MAX_BONE_PER_VERTEX     4
#define MAX_BONE_PER_MESH       100

struct Vertex
{
    v3      pos;
    v3      normal;
    v2      uv;
    v4      color;

    s32     node_ids[MAX_BONE_PER_VERTEX];
    f32     node_weights[MAX_BONE_PER_VERTEX];
};

struct Mesh
{
    u32     vertex_count;
    Vertex  *vertices;

    u32     index_count;
    u32     *indices;

    u32     material_idx;
};

struct Material
{
    v3  color_ambient;
    v3  color_diffuse;
    v3  color_specular;
};

struct Node
{
    s32     id;

    m4x4    offset;
    m4x4    transform;  // transform in parent's bone-space. aiNode

    u32     child_count;
    s32     *child_ids;
};

struct Model
{
    u32         mesh_count;
    Mesh        *meshes;

    u32         material_count;
    Material    *materials;

    u32         node_count;
    s32         root_bone_node_id;
    Node        *nodes;
};
    

#define MODEL_H
#endif // MODEL_H
