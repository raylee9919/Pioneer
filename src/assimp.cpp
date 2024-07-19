/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Sung Woo Lee $
   $Notice: (C) Copyright %s by Sung Woo Lee. All Rights Reserved. $
   ======================================================================== */

//
// @TODO:
// 
//
//
//

#include "stdio.h"
#include <vector>
#include <unordered_map>
#include <string>


#include "third_party/assimp/include/assimp/Importer.hpp"
#include "third_party/assimp/include/assimp/scene.h"
#include "third_party/assimp/include/assimp/postprocess.h"


#include "assimp.h"
#include "asset_model.h"
#include "asset_animation.h"

static v3
aiv3_to_v3(aiVector3D ai_v)
{
    v3 v = {};
    v.x = ai_v.x;
    v.y = ai_v.y;
    v.z = ai_v.z;
    return v;
}

static qt
aiqt_to_qt(aiQuaternion ai_q)
{
    qt q = {};
    q.w = ai_q.w;
    q.x = ai_q.x;
    q.y = ai_q.y;
    q.z = ai_q.z;
    return q;
}

static m4x4
ai_m4x4_to_m4x4(aiMatrix4x4 ai_mat)
{
    m4x4 mat = {};
    mat.e[0][0] = ai_mat.a1;
    mat.e[0][1] = ai_mat.a2;
    mat.e[0][2] = ai_mat.a3;
    mat.e[0][3] = ai_mat.a4;

    mat.e[1][0] = ai_mat.b1;
    mat.e[1][1] = ai_mat.b2;
    mat.e[1][2] = ai_mat.b3;
    mat.e[1][3] = ai_mat.b4;

    mat.e[2][0] = ai_mat.c1;
    mat.e[2][1] = ai_mat.c2;
    mat.e[2][2] = ai_mat.c3;
    mat.e[2][3] = ai_mat.c4;

    mat.e[3][0] = ai_mat.d1;
    mat.e[3][1] = ai_mat.d2;
    mat.e[3][2] = ai_mat.d3;
    mat.e[3][3] = ai_mat.d4;

    return mat;
}

static b32
string_equal(char *a, char *b)
{
    b32 result;
    if (a && b)
    {
        while (*a &&
               *b &&
               (*a == *b))
        {
            ++a;
            ++b;
        }
        result = (*a == *b);
    }
    else
    {
        result = false;
    }
    return result;
}

#if 1
static u32
string_length(char *str)
{
    u32 result = 0;
    for (char *c = str;
         *c;
        ++c, ++result) {}
    return result;
}

static u32
string_length_with_null(char *str)
{
    u32 result = string_length(str) + 1;
    return result;
}
#endif


inline void
print_indent(u32 depth)
{
    for (u32 i = 0; i < depth; ++i) { printf("  "); }
}

static s32 g_node_count;
static void
debug_print_nodes(aiNode *node, u32 depth)
{
    g_node_count++;
    m4x4 transform = ai_m4x4_to_m4x4(node->mTransformation);

    print_indent(depth);
    printf("%s\n", node->mName.data);

#if 1
    for (s32 r = 0; r < 4; ++r)
    {
        print_indent(depth);
        for (s32 c = 0; c < 4; ++c)
        {
            printf("%f ", transform.e[r][c]);
        }
        printf("\n");
    }
#endif

    for (u32 i = 0;
         i < node->mNumChildren;
         ++i)
    {
        debug_print_nodes(node->mChildren[i], depth + 1);
    }
}


#if 0
static void 
build_skeleton(aiNode *node, s32 parent_id)
{
    assert(node);
    std::string name(node->mName.data);
    if (g_bone_map.find(name) != g_bone_map.end())
    {
        u32 id = g_bone_map.at(name);
        if (g_bone_visited.find(id) == g_bone_visited.end())
        {
            if (parent_id != -1)
            {
                g_bone_hierarchy[parent_id].push_back(id);
            }
            for (u32 child_idx = 0;
                 child_idx < node->mNumChildren;
                 ++child_idx)
            {
                aiNode *child = node->mChildren[child_idx];
                build_skeleton(child, id);
            }
            g_bone_visited[id] = 1;
        }
    }
    else
    {
        for (u32 child_idx = 0;
             child_idx < node->mNumChildren;
             ++child_idx)
        {
            aiNode *child = node->mChildren[child_idx];
            build_skeleton(child, parent_id);
        }
    }
}

inline s32
get_bone_id(aiString ai_bone_name)
{
    s32 id = 0;
    std::string bone_name(ai_bone_name.data);
    id = g_bone_map.at(bone_name);
    return id;
}
#endif

static char *
create_out_file_name(char *in_file_name)
{
    char *result = 0;
    u8 length_before_period = 0;
    for (char *c = in_file_name;
         c;
         ++c, ++length_before_period)
    {
        if (*c == '.')
        {
            char next = *(c + 1);
            if (next != '.' && next != '/')
            {
                break;
            }
        }
    }

    char *file_format = ".smsh";
    result = (char *)malloc(length_before_period + string_length_with_null(file_format));
    for (u8 idx = 0;
         idx < length_before_period;
         ++idx)
    {
        result[idx] = in_file_name[idx];
    }
    for (u32 char_idx = 0; char_idx < string_length_with_null(file_format); ++char_idx)
    {
        result[length_before_period + char_idx] = file_format[char_idx];
    }

    return result;
}

static b32
model_has_bone(const aiScene *scene)
{
    b32 result = false;

    for (u32 mesh_idx = 0;
         mesh_idx < scene->mNumMeshes;
         ++mesh_idx)
    {
        aiMesh *mesh = scene->mMeshes[mesh_idx];
        if (mesh->mNumBones)
        {
            result = true;
            break;
        }
    }

    return result;
}

struct Bone_Stack_Frame
{
    aiNode *node;
    aiNode *parent;
    u32 next_child_idx;
};
struct Bone_Stack
{
    Bone_Stack_Frame frames[100];
    u32 top_idx;
};
static aiNode *
find_one_below_root_node(const aiScene *scene, aiNode *root_node)
{
    aiNode *result = 0;

    aiString arbitrary_bone_name;
    for (u32 mesh_idx = 0;
         mesh_idx < scene->mNumMeshes;
         ++mesh_idx)
    {
        aiMesh *mesh = scene->mMeshes[mesh_idx];
        if (mesh->HasBones())
        {
            aiBone *arbitary_bone = mesh->mBones[0];
            arbitrary_bone_name = arbitary_bone->mName;
            assert(arbitrary_bone_name != root_node->mName);
            break;
        }
    }

    aiNode *arbitrary_bone_node = root_node->FindNode(arbitrary_bone_name);

    Bone_Stack stack = {};
    Bone_Stack_Frame *top_frame = stack.frames;
    top_frame->node = root_node;
    top_frame->parent = root_node;
    top_frame->next_child_idx = 0;

    for (Bone_Stack_Frame *frame = stack.frames;
         ;
         frame = stack.frames + stack.top_idx)
    {
        aiNode *node = frame->node;
        if (node->mName == arbitrary_bone_name)
        {
            break;
        }
        else if (frame->next_child_idx != node->mNumChildren)
        {
            Bone_Stack_Frame *next_top_frame = &stack.frames[++stack.top_idx];
            assert(stack.top_idx != array_count(stack.frames));
            next_top_frame->node = node->mChildren[frame->next_child_idx++];
            next_top_frame->parent = node;
            next_top_frame->next_child_idx = 0;
        }
        else
        {
            --stack.top_idx;
        }
    }

    result = (stack.top_idx == 0) ? stack.frames[0].node : stack.frames[1].node;

    return result;
}

static void
print_scene_abstract_info(const aiScene *scene)
{
#if 0
    debug_print_nodes(scene->mRootNode, 0);
    printf("node count: %d\n", g_node_count);
#endif
    printf("  mesh count      : %d\n", scene->mNumMeshes);
    printf("  texture count   : %d\n", scene->mNumTextures);
    printf("  material count  : %d\n", scene->mNumMaterials);
    printf("  animation count : %d\n", scene->mNumAnimations);

    u32 bone_count = 0;
    for (u32 mesh_idx = 0;
         mesh_idx < scene->mNumMeshes;
         ++mesh_idx)
    {
        aiMesh *mesh = scene->mMeshes[mesh_idx];
        bone_count += mesh->mNumBones;
    }
    printf("  bone count      : %d\n", bone_count);
}

static u32
get_next_unfilled_bone_index(Asset_Vertex *asset_vertex)
{
    u32 result = 0;
    for (u32 idx = 0;
         idx < MAX_BONE_PER_VERTEX;
         ++idx)
    {
        if (asset_vertex->node_ids[idx] == -1)
        {
            result = idx;
            break;
        }
    }

    return result;
}

struct Hash_Slot
{
    char        *name;
    s32         node_id; // this isn't the offset starting from the hash table slots!
    Hash_Slot   *next;
};
struct Hash_Entry
{
    Hash_Slot   *first;
};
struct Hash_Table
{
    Hash_Entry  *entries;
    u32         length;
    s32         next_id;

    void debug_print()
    {
        for (u32 idx = 0; idx < length; ++idx)
        {
            Hash_Entry *entry = entries + idx;
            for (Hash_Slot *slot = entry->first;
                 slot;
                )
            {
                printf("%d. %s: %d\n", debug_count++, slot->name, slot->node_id);
                if (slot->next)
                {
                    slot = slot->next;
                }
                else
                {
                    break;
                }
            }
        }
    }

    s32 debug_count = 0;
};

static u32
hash(char *key, u32 len)
{
    u32 result;
    if (key)
    {
        result = 0;
    }
    else
    {
        for (char *c = key;
             *c;
            )
        {
            result += *c;
        }
        result %= len;
    }

    return result;
}

static s32
id_from_name(char *name, Hash_Table *hash_table)
{
    s32 id;
    // @TODO: Better hash function!
    u32 slot_idx = hash(name, hash_table->length);
    Hash_Entry *entry = hash_table->entries + slot_idx;
    Hash_Slot *slot = entry->first;

    if (slot)
    {
        for (;;)
        {
            if (string_equal(name, slot->name))
            {
                id = slot->node_id;
                break;
            }
            else if (slot->next)
            {
                slot = slot->next;
            }
            else
            {
                Hash_Slot *new_slot = malloc_type(Hash_Slot);
                new_slot->node_id = hash_table->next_id++;
                new_slot->next = 0;
                new_slot->name = name;

                slot->next = new_slot;

                id = new_slot->node_id;
                break;
            }
        }
    }
    else
    {
        entry->first = malloc_type(Hash_Slot);
        entry->first->node_id = hash_table->next_id++;
        entry->first->next = 0;
        entry->first->name = name;

        id = entry->first->node_id;
    }

    return id;
}

static void
swap(Asset_Node *nd1, Asset_Node *nd2)
{
    Asset_Node tmp = *nd1;
    *nd1 = *nd2;
    *nd2 = tmp;
}

static void
sort_by_id(Asset_Node *nodes, u32 node_count)
{
    u32 i, j;
    bool swapped;
    for (i = 0; i < node_count - 1; i++) 
    {
        swapped = false;
        for (j = 0; j < node_count - i - 1; j++) 
        {
            if (nodes[j].id > nodes[j + 1].id) 
            {
                swap(&nodes[j], &nodes[j + 1]);
                swapped = true;
            }
        }

        if (swapped == false)
            break;
    }
}

static void
fill_asset_nodes(const aiScene *model, Asset_Model *asset_model, aiNode *one_below_root_node,
                 Hash_Table *hash_table)
{
    // traverse through hierarchy, if certain name was in the hash-table,
    // that slot will give us the index of that node in node array.
    // If it wasn't, write to the hash-table. Collision handling isn't much of
    // a big deal. Then, that slot will contain a 'next_to_write' number of 
    // the node array. Then increment 'next_to_write" by one.
    //
    u32 debug_count = 0;

    Asset_Node *asset_nodes = asset_model->nodes;
    u32 node_count = asset_model->node_count;

    aiNode *root = model->mRootNode;
    u32 front = 1;
    u32 back = 2;
    aiNode *q[300];
    q[front] = one_below_root_node;

    asset_nodes->id             = id_from_name(root->mName.data, hash_table);
    asset_nodes->offset         = identity();
    asset_nodes->transform      = ai_m4x4_to_m4x4(root->mTransformation);

    if (one_below_root_node)
    {
        asset_nodes->child_count    = 1;
        asset_nodes->child_ids      = malloc_type(s32);
        asset_nodes->child_ids[0]   = id_from_name(one_below_root_node->mName.data, hash_table);

        while (front != back)
        {
            aiNode *node = q[front];
            Asset_Node *asset_node = asset_nodes + front;

            asset_node->id            = id_from_name(node->mName.data, hash_table);
            asset_node->offset        = identity();
            asset_node->transform     = ai_m4x4_to_m4x4(node->mTransformation);
            asset_node->child_count   = node->mNumChildren;
            asset_node->child_ids     = malloc_array(s32, asset_node->child_count);

            for (u32 child_idx = 0;
                 child_idx < asset_node->child_count;
                 ++child_idx)
            {
                aiNode *child = node->mChildren[child_idx];
                assert(back != array_count(q));
                q[back++] = child;
                asset_node->child_ids[child_idx] = id_from_name(node->mChildren[child_idx]->mName.data, hash_table);
            }

            ++front;
        }

        sort_by_id(asset_nodes, node_count);

        for (u32 mesh_idx = 0;
             mesh_idx < model->mNumMeshes;
             ++mesh_idx)
        {
            aiMesh *mesh = model->mMeshes[mesh_idx];
            for (u32 bone_idx = 0;
                 bone_idx < mesh->mNumBones;
                 ++bone_idx)
            {
                aiBone *bone = mesh->mBones[bone_idx];
                for (u32 idx = 0;
                     idx < back;
                     ++idx)
                {
                    Asset_Node *asset_node = asset_nodes + idx;
                    if (id_from_name(bone->mName.data, hash_table) == asset_node->id)
                    {
                        asset_node->offset = ai_m4x4_to_m4x4(bone->mOffsetMatrix);
                        ++debug_count;
                        break;
                    }
                }
            }
        }

        asset_model->root_bone_node_id =
            id_from_name(one_below_root_node->mName.data, hash_table);
    }
    else
    {
        asset_nodes->child_count = 0;
    }


    //
    //
    //

    for (s32 i = 0; i < (s32)node_count; ++i)
        assert(asset_model->nodes[i].id == i);

#if 0
    hash_table->debug_print();
    printf("bone-node offset extracted: %d\n", debug_count);

#endif
}

static void
fill_asset_meshes(const aiScene *model, Asset_Model *asset_model, Hash_Table *hash_table)
{
    u32 mesh_count = model->mNumMeshes;
    aiMesh **meshes = model->mMeshes;

    asset_model->mesh_count     = mesh_count;
    asset_model->meshes         = malloc_array(Asset_Mesh, asset_model->mesh_count);

    for (u32 mesh_idx = 0;
         mesh_idx < mesh_count;
         ++mesh_idx)
    {
        Asset_Mesh *asset_mesh          = (asset_model->meshes + mesh_idx);
        aiMesh *mesh                    = meshes[mesh_idx];

        u32 vertex_count                = mesh->mNumVertices;
        asset_mesh->vertex_count        = vertex_count;
        asset_mesh->vertices            = malloc_array(Asset_Vertex, asset_mesh->vertex_count);

        for (u32 vertex_idx = 0;
             vertex_idx < vertex_count;
             ++vertex_idx)
        {
            Asset_Vertex *asset_vertex = asset_mesh->vertices + vertex_idx;

            asset_vertex->pos.x = mesh->mVertices[vertex_idx].x;
            asset_vertex->pos.y = mesh->mVertices[vertex_idx].y;
            asset_vertex->pos.z = mesh->mVertices[vertex_idx].z;

            if (mesh->HasNormals())
            {
                asset_vertex->normal.x = mesh->mNormals[vertex_idx].x;
                asset_vertex->normal.y = mesh->mNormals[vertex_idx].y;
                asset_vertex->normal.z = mesh->mNormals[vertex_idx].z;
            }

            if (mesh->HasTextureCoords(0))
            {
                asset_vertex->uv.x = mesh->mTextureCoords[0][vertex_idx].x;
                asset_vertex->uv.y = mesh->mTextureCoords[0][vertex_idx].y;
            }

            if (mesh->HasVertexColors(0))
            {
                asset_vertex->color.r = mesh->mColors[0][vertex_idx].r;
                asset_vertex->color.g = mesh->mColors[0][vertex_idx].g;
                asset_vertex->color.b = mesh->mColors[0][vertex_idx].b;
                asset_vertex->color.a = mesh->mColors[0][vertex_idx].a;
            }
            else
            {
                asset_vertex->color = _v4_(1.0f, 1.0f, 1.0f, 1.0f);
            }

            for (u32 i = 0; i < MAX_BONE_PER_VERTEX; ++i)
            {
                asset_vertex->node_ids[i] = -1; // @Spec: Renderer AI must agree it to be speced to -1 too.
                asset_vertex->node_weights[i] = 0;
            }
        }

        for (u32 bone_idx = 0;
             bone_idx < mesh->mNumBones;
             ++bone_idx)
        {
            aiBone *bone = mesh->mBones[bone_idx];
            for (u32 vw_idx = 0;
                 vw_idx < bone->mNumWeights;
                 ++vw_idx)
            {
                aiVertexWeight *vw = bone->mWeights + vw_idx;
                u32 vertex_idx  = vw->mVertexId;
                f32 weight      = vw->mWeight;

                Asset_Vertex *asset_vertex = asset_mesh->vertices + vertex_idx;
                u32 next = get_next_unfilled_bone_index(asset_vertex);
                assert(next < MAX_BONE_PER_VERTEX);
                asset_vertex->node_ids[next]     = id_from_name(bone->mName.data, hash_table);
                asset_vertex->node_weights[next] = weight;
            }
        }

        u32 triangle_count              = mesh->mNumFaces;
        u32 index_count                 = triangle_count * 3;
        asset_mesh->index_count         = index_count;
        asset_mesh->indices             = malloc_array(u32, asset_mesh->index_count);

        for (u32 triangle_idx = 0;
             triangle_idx < triangle_count;
             ++triangle_idx)
        {
            aiFace *triangle = (mesh->mFaces + triangle_idx);
            assert(triangle->mNumIndices == 3);
            for (u32 i = 0; i < 3; ++i)
            {
                size_t idx_of_idx = (3 * triangle_idx + i);
                asset_mesh->indices[idx_of_idx] = triangle->mIndices[i];
            }
        }

        asset_mesh->material_idx = mesh->mMaterialIndex;
    }
}

static void
fill_asset_textures(const aiScene *model, Asset_Model *asset_model)
{
    u32 texture_count = model->mNumTextures;
    asset_model->texture_count = texture_count;
    asset_model->textures = malloc_array(Asset_Texture, texture_count);

    for (u32 texture_idx = 0;
         texture_idx < texture_count;
         ++texture_idx)
    {
        aiTexture *texture = model->mTextures[texture_idx];
        u32 width = texture->mWidth;
        u32 height = texture->mHeight;
        aiTexel *data = texture->pcData;

        Asset_Texture *asset_texture = asset_model->textures + texture_idx;
        asset_texture->contents = malloc_array(u32, width * height);

        aiTexel *src = data;
        u32 *dst = (u32 *)asset_texture->contents;
        aiTexel C = *src++;
        assert(sizeof(aiTexel) == sizeof(u32));
        *dst++ = (C.r << 24 | C.g << 16 | C.b << 8 | C.a);
    }
}

static u32
get_node_count_from(aiNode *node)
{
    u32 result = 1;

    if (node->mNumChildren)
    {
        for (u32 child_idx = 0;
             child_idx < node->mNumChildren;
             ++child_idx)
        {
            aiNode *child = node->mChildren[child_idx];
            result += get_node_count_from(child);
        }
    }
    else
    {
    }

    return result;
}


static void
fill_asset_materials(const aiScene *model, Asset_Model *asset_model)
{
    if (model->HasMaterials())
    {
        asset_model->material_count = model->mNumMaterials;
        asset_model->materials =
            (Asset_Material *)malloc(sizeof(Asset_Material) * model->mNumMaterials);

        for (u32 mat_idx = 0;
             mat_idx < model->mNumMaterials;
             ++mat_idx)
        {
            Asset_Material *asset_mat = asset_model->materials + mat_idx;
            aiMaterial *mat = model->mMaterials[mat_idx];

            aiColor3D c;

            mat->Get(AI_MATKEY_COLOR_AMBIENT, c);
            asset_mat->color_ambient = _v3_(c.r, c.g, c.b);

            mat->Get(AI_MATKEY_COLOR_DIFFUSE, c);
            asset_mat->color_diffuse = _v3_(c.r, c.g, c.b);

            mat->Get(AI_MATKEY_COLOR_SPECULAR, c);
            asset_mat->color_specular = _v3_(c.r, c.g, c.b);
        }
    }
}

static void
write_asset_meshes(FILE *model_out, Asset_Model *asset_model)
{
    fwrite_item(asset_model->mesh_count, model_out);

    for (u32 mesh_idx = 0;
         mesh_idx < asset_model->mesh_count;
         ++mesh_idx)
    {
        Asset_Mesh *asset_mesh = (asset_model->meshes + mesh_idx);

        fwrite_item(asset_mesh->vertex_count, model_out);
        for (u32 vertex_idx = 0;
             vertex_idx < asset_mesh->vertex_count;
             ++vertex_idx)
        {
            Asset_Vertex *vertex = asset_mesh->vertices + vertex_idx;
            fwrite_item(vertex->pos, model_out);
            fwrite_item(vertex->normal, model_out);
            fwrite_item(vertex->uv, model_out);
            fwrite_item(vertex->color, model_out);

            fwrite_array(vertex->node_ids, MAX_BONE_PER_VERTEX, model_out);
            fwrite_array(vertex->node_weights, MAX_BONE_PER_VERTEX, model_out);
        }

        fwrite_item(asset_mesh->index_count, model_out);
        fwrite_array(asset_mesh->indices, asset_mesh->index_count, model_out);

        fwrite_item(asset_mesh->material_idx, model_out);
    }
}

static void
write_asset_materials(FILE *model_out, Asset_Model *asset_model)
{
    fwrite_item(asset_model->material_count, model_out);
    if (asset_model->material_count)
    {
        fwrite_array(asset_model->materials, asset_model->material_count, model_out);
    }
}

static void
write_asset_nodes(FILE *model_out, Asset_Model *asset_model)
{
    fwrite_item(asset_model->node_count, model_out);
    if (asset_model->node_count)
    {
        fwrite_item(asset_model->root_bone_node_id, model_out);
        for (u32 node_idx = 0;
             node_idx < asset_model->node_count;
             ++node_idx)
        {
            Asset_Node *asset_node = asset_model->nodes + node_idx;
            fwrite_item(asset_node->id, model_out);
            fwrite_item(asset_node->offset, model_out);
            fwrite_item(asset_node->transform, model_out);
            fwrite_item(asset_node->child_count, model_out);
            fwrite_array(asset_node->child_ids, asset_node->child_count, model_out);
        }
    }
}

static char *
create_anim_out_file_name(char *in_file_name, char *anim_name)
{
    char *result = 0;
    u8 length_before_period = 0;
    for (char *c = in_file_name;
         c;
         ++c, ++length_before_period)
    {
        if (*c == '.')
        {
            char next = *(c + 1);
            if (next != '.' && next != '/')
            {
                break;
            }
        }
    }

    char prefix[256];
    _snprintf(prefix, sizeof(prefix), "_%s.sanm", anim_name);
    result = (char *)malloc(length_before_period + string_length_with_null(prefix));
    for (u8 idx = 0;
         idx < length_before_period;
         ++idx)
    {
        result[idx] = in_file_name[idx];
    }
    for (u32 char_idx = 0;
         char_idx < string_length_with_null(prefix);
         ++char_idx)
    {
        result[length_before_period + char_idx] = prefix[char_idx];
    }

    return result;
}

int main(void)
{
    char *input_file_names[] = {
        "../data/xbot_run.fbx",
        "../data/grass.dae",
        "../data/cube.dae",
        "../data/sphere.fbx",
        "../data/octahedral.dae",
    };

    Assimp::Importer importer;
    importer.SetPropertyBool(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, false);

    for (u32 file_idx = 0;
         file_idx < array_count(input_file_names);
         ++file_idx)
    {
        char *in_file_name = input_file_names[file_idx];
        const aiScene *model = importer.ReadFile(in_file_name, aiProcessPreset_TargetRealtime_Quality);
        if (model)
        {
            printf("\nok: load scene '%s'.\n", in_file_name);
            print_scene_abstract_info(model);

            // Init node-hash-table.
            Hash_Table hash_table = {};
            hash_table.length = 500;
            hash_table.next_id = 0;
            hash_table.entries = malloc_array(Hash_Entry, hash_table.length);
            for (u32 i = 0; i < hash_table.length; ++i) { hash_table.entries[i] = Hash_Entry{}; }

            //
            // Model
            //
            Asset_Model asset_model = {};
            char *out_file_name = create_out_file_name(in_file_name);
            FILE *model_out = fopen(out_file_name, "wb");
            if (model_out)
            {
                // Find the one-below-root-node, which is a bone node.
                aiNode *root_node = model->mRootNode;
                aiNode *one_below_root_node = 0;
                if (model_has_bone(model))
                    one_below_root_node = find_one_below_root_node(model, root_node);

                // Fill in the Asset_Model info.
                asset_model.node_count     = one_below_root_node ? (get_node_count_from(one_below_root_node) + 1) : 1;
                asset_model.nodes          = malloc_array(Asset_Node, asset_model.node_count);
                fill_asset_nodes(model, &asset_model, one_below_root_node, &hash_table);
                fill_asset_meshes(model, &asset_model, &hash_table);
                fill_asset_materials(model, &asset_model);
                fill_asset_textures(model, &asset_model);

                // Write out the Asset_Model info.
                write_asset_meshes(model_out, &asset_model);
                write_asset_materials(model_out, &asset_model);
                write_asset_nodes(model_out, &asset_model);
                //write_asset_textures(model_out, &asset_model);

                // Print status
                printf("ok: written '%s'\n", out_file_name);
                fclose(model_out);
            }
            else
            {
                printf("error: Couldn't open output file %s\n", out_file_name);
                return -1;
            }


            //
            // Animation
            //
            for (u32 anim_idx = 0;
                 anim_idx < model->mNumAnimations;
                 ++anim_idx)
            {
                Asset_Animation asset_animation = {};
                aiAnimation *anim = model->mAnimations[anim_idx];

                char *anim_out_file_name = create_anim_out_file_name(in_file_name, anim->mName.data);
                FILE *anim_out = fopen(anim_out_file_name, "wb");
                if (anim_out)
                {
                    f32 spt                 = 1.0f / (f32)anim->mTicksPerSecond;
                    f32 anim_duration       = (f32)(anim->mDuration / anim->mTicksPerSecond);
                    u32 node_count          = anim->mNumChannels;

                    fwrite(anim->mName.data, sizeof(char) * string_length_with_null(anim->mName.data), 1, anim_out);
                    fwrite_item(anim_duration, anim_out);
                    fwrite_item(node_count, anim_out);
                    for (u32 node_idx = 0;
                         node_idx < node_count;
                         ++node_idx)
                    {
                        aiNodeAnim *node = anim->mChannels[node_idx];


                        s32 node_id = id_from_name(node->mNodeName.data, &hash_table);
                        u32 translation_count = node->mNumPositionKeys;
                        u32 rotation_count = node->mNumRotationKeys;
                        u32 scaling_count = node->mNumScalingKeys;

                        fwrite_item(node_id, anim_out);

                        fwrite_item(translation_count, anim_out);
                        fwrite_item(rotation_count, anim_out);
                        fwrite_item(scaling_count, anim_out);

                        for (u32 idx = 0;
                             idx < translation_count;
                             ++idx)
                        {
                            aiVectorKey *key = node->mPositionKeys + idx;
                            f32 dt = (f32)key->mTime * spt;
                            v3 vec = aiv3_to_v3(key->mValue);
                            dt_v3_Pair dt_v3 = dt_v3_Pair{dt, vec};
                            fwrite_item(dt_v3, anim_out);
                        }

                        for (u32 idx = 0;
                             idx < rotation_count;
                             ++idx)
                        {
                            aiQuatKey *key = node->mRotationKeys + idx;
                            f32 dt = (f32)key->mTime * spt;
                            qt q   = aiqt_to_qt(key->mValue);
                            dt_qt_Pair dt_qt = dt_qt_Pair{dt, q};
                            fwrite_item(dt_qt, anim_out);
                        }

                        for (u32 idx = 0;
                             idx < scaling_count;
                             ++idx)
                        {
                            aiVectorKey *key = node->mScalingKeys + idx;
                            f32 dt = (f32)key->mTime * spt;
                            v3 vec = aiv3_to_v3(key->mValue);
                            dt_v3_Pair dt_v3 = dt_v3_Pair{dt, vec};
                            fwrite_item(dt_v3, anim_out);
                        }
                    }

                    printf("ok: written '%s'\n", anim_out_file_name);
                    fclose(anim_out);
                }
                else
                {
                    printf("error: Couldn't open output file %s\n", anim_out_file_name);
                    return -1;
                }
            }

        }
        else
        {
            printf("error: Couldn't load file %s.\n", in_file_name);
            return -1;
        }
    }

    printf("*** SUCCESSFUL! ***\n");
    return 0;
}
