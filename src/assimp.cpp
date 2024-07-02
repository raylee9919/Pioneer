/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Sung Woo Lee $
   $Notice: (C) Copyright %s by Sung Woo Lee. All Rights Reserved. $
   ======================================================================== */

#include <vector>
#include <unordered_map>
#include <string>


#include "third_party/assimp/include/assimp/Importer.hpp"
#include "third_party/assimp/include/assimp/scene.h"
#include "third_party/assimp/include/assimp/postprocess.h"


#define KB(X) (   X *  1024ll)
#define MB(X) (KB(X) * 1024ll)
#define GB(X) (MB(X) * 1024ll)
#define TB(X) (GB(X) * 1024ll)
#define max(A, B) (A > B ? A : B)
#define min(A, B) (A < B ? A : B)
#define assert(EXP) if (!(EXP)) { *(volatile int *)0 = 0; }

#include "types.h"
#include "assimp.h"
#include "asset_model.h"

static std::unordered_map<std::string, s32> g_bone_map;
static u32 g_bone_map_used;
static std::vector<s32> g_bone_hierarchy[MAX_BONE_PER_MESH];
static std::unordered_map<s32, u8> g_bone_visited;

static Memory_Arena
init_memory_arena()
{
    size_t total_memory_size = GB(2);
    Memory_Arena arena = {};
    arena.size = total_memory_size;
    arena.used = 0;
    arena.base = malloc(total_memory_size);
    memset(arena.base, 0, total_memory_size);

    return arena;
}

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

static aiNode *
find_root_bone_node(aiNode *node)
{
    aiNode *result = 0;

    std::string name(node->mName.data);
    if (g_bone_map.find(name) != g_bone_map.end())
    {
        result = node;
    }
    else
    {
        for (u32 child_idx = 0;
             child_idx < node->mNumChildren;
             ++child_idx)
        {
            aiNode *child = node->mChildren[child_idx];
            aiNode *tmp = find_root_bone_node(child);
            if (tmp)
            {
                result = tmp;
                break;
            }
        }
    }

    return result;
}

inline void
print_indent(u32 depth)
{
    for (u32 i = 0; i < depth; ++i) { printf("  "); }
}

static s32 g_node_count;
static void
print_nodes(aiNode *node, u32 depth)
{
    g_node_count++;
    m4x4 transform = ai_m4x4_to_m4x4(node->mTransformation);

    print_indent(depth);
    printf("%s\n", node->mName.data);

    for (s32 r = 0; r < 4; ++r)
    {
        print_indent(depth);
        for (s32 c = 0; c < 4; ++c)
        {
            printf("%f ", transform.e[r][c]);
        }
        printf("\n");
    }

    for (u32 i = 0;
         i < node->mNumChildren;
         ++i)
    {
        print_nodes(node->mChildren[i], depth + 1);
    }
}


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

static void
parse_model_file_name(char *model_file_name, const char *file_name)
{
    const char *src_at = file_name;
    char *dst_at = model_file_name;
    while (*src_at != '.')
    {
        *dst_at++ = *src_at++;
    }
    *dst_at++ = '.';
    *dst_at++ = '3';
    *dst_at++ = 'd';
    *dst_at++ = 0;
}

int
main(int argc, char **argv)
{
    Memory_Arena arena = init_memory_arena();
    Assimp::Importer importer;

    b32 write_skeleton = 0; // (*)

    for (u32 file_idx = 0;
         file_idx < 1;
         ++file_idx)
    {
        const char *file_name = "../data/octahedral.dae"; // (*)
        const aiScene *model = importer.ReadFile(file_name, aiProcessPreset_TargetRealtime_Quality);

        if (model)
        {
            Asset_Model asset_model = {};
            print_nodes(model->mRootNode, 0);
            printf("node count: %d\n", g_node_count);

            printf("success: load model '%s'.\n", file_name);
            printf("  mesh count      : %d\n", model->mNumMeshes);
            printf("  texture count   : %d\n", model->mNumTextures);
            printf("  animation count : %d\n", model->mNumAnimations);


            char model_file_name[32];
            parse_model_file_name(model_file_name, file_name);
            FILE *model_out = fopen(model_file_name, "wb");
            if (model_out)
            {
                std::unordered_map<s32, Asset_Bone> model_bones;

                if (model->HasMeshes())
                {
                    u32 mesh_count = model->mNumMeshes;
                    aiMesh **meshes = model->mMeshes;

                    asset_model.mesh_count    = mesh_count;
                    asset_model.meshes        = push_array(&arena, Asset_Mesh, asset_model.mesh_count);

                    for (u32 mesh_idx = 0;
                         mesh_idx < mesh_count;
                         ++mesh_idx)
                    {
                        Asset_Mesh *asset_mesh          = (asset_model.meshes + mesh_idx);
                        aiMesh *mesh                    = meshes[mesh_idx];

                        u32 vertex_count                = mesh->mNumVertices;
                        asset_mesh->vertex_count        = vertex_count;
                        asset_mesh->vertices            = push_array(&arena, Asset_Vertex, asset_mesh->vertex_count);

                        b32 mesh_has_normals = mesh->HasNormals();
                        b32 mesh_has_uvs     = mesh->HasTextureCoords(0);
                        b32 mesh_has_colors  = mesh->HasVertexColors(0);
                        b32 mesh_has_bones   = mesh->HasBones();

                        for (u32 vertex_idx = 0;
                             vertex_idx < vertex_count;
                             ++vertex_idx)
                        {
                            Asset_Vertex *asset_vertex = asset_mesh->vertices + vertex_idx;

                            asset_vertex->pos.x = mesh->mVertices[vertex_idx].x;
                            asset_vertex->pos.y = mesh->mVertices[vertex_idx].y;
                            asset_vertex->pos.z = mesh->mVertices[vertex_idx].z;

                            if (mesh_has_normals)
                            {
                                asset_vertex->normal.x = mesh->mNormals[vertex_idx].x;
                                asset_vertex->normal.y = mesh->mNormals[vertex_idx].y;
                                asset_vertex->normal.z = mesh->mNormals[vertex_idx].z;
                            }
                            else
                            {
                            }

                            if (mesh_has_uvs)
                            {
                                asset_vertex->uv.x = mesh->mTextureCoords[0][vertex_idx].x;
                                asset_vertex->uv.y = mesh->mTextureCoords[0][vertex_idx].y;
                            }
                            else
                            {
                            }

                            if (mesh_has_colors)
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
                                asset_vertex->bone_ids[i] = -1;
                            }
                        }

                        if (mesh_has_bones)
                        {
                            u32 bone_count = mesh->mNumBones;
                            for (u32 i = 0;
                                 i < bone_count;
                                 ++i)
                            {
                                aiBone *bone = mesh->mBones[i];
                                std::string bone_name(bone->mName.data);
                                m4x4        bone_offset = ai_m4x4_to_m4x4(bone->mOffsetMatrix);
                                s32         bone_id;
                                if (g_bone_map.find(bone_name) == g_bone_map.end())
                                {
                                    assert(g_bone_map_used != 100);
                                    bone_id = g_bone_map_used++;
                                    g_bone_map[bone_name] = bone_id;
                                }
                                else
                                {
                                    bone_id = g_bone_map.at(bone_name);
                                }

                                if (model_bones.find(bone_id) == model_bones.end())
                                {
                                    Asset_Bone model_bone = {};
                                    model_bones[bone_id] = model_bone;
                                }
                            }

                            // go through mesh->mBones, write offset mat4 to asset_bone
                            for (u32 bone_idx = 0;
                                 bone_idx < bone_count;
                                 ++bone_idx)
                            {
                                aiBone *bone            = mesh->mBones[bone_idx];
                                s32 bone_id             = get_bone_id(bone->mName);
                                Asset_Bone *model_bone  = &model_bones.at(bone_id);
                                model_bone->bone_id     = bone_id;
                                model_bone->offset      = ai_m4x4_to_m4x4(bone->mOffsetMatrix);
                                model_bone->transform   = ai_m4x4_to_m4x4(model->mRootNode->FindNode(bone->mName)->mTransformation);
                            }

                            // per mesh, there are vertices.
                            // per vertex, there're bone-ids and bone-weights.
                            for (u32 bone_idx = 0;
                                 bone_idx < bone_count;
                                 ++bone_idx)
                            {
                                aiBone *bone = mesh->mBones[bone_idx];
                                s32 bone_id  = get_bone_id(bone->mName);
                                for (u32 count = 0;
                                     count < bone->mNumWeights;
                                     ++count)
                                {
                                    aiVertexWeight *vw = bone->mWeights + count;
                                    u32 v  = vw->mVertexId;
                                    f32 w  = vw->mWeight;
                                    Asset_Vertex *asset_vertex = (asset_mesh->vertices + v);
                                    for (u32 i = 0;
                                         i < MAX_BONE_PER_VERTEX;
                                         ++i)
                                    {
                                        if (asset_vertex->bone_ids[i] == -1)
                                        {
                                            asset_vertex->bone_ids[i]       = bone_id;
                                            asset_vertex->bone_weights[i]   = w;
                                            break;
                                        }
                                    }
                                }
                            }

                        }

                        u32 triangle_count              = mesh->mNumFaces;
                        u32 index_count                 = triangle_count * 3;
                        asset_mesh->index_count         = index_count;
                        asset_mesh->indices             = push_array(&arena, u32, asset_mesh->index_count);

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


                    Asset_Material *asset_materials = 0;
                    if (model->HasMaterials())
                    {
                        asset_materials = push_array(&arena, Asset_Material, 2);
                        for (u32 mat_idx = 0;
                             mat_idx < model->mNumMaterials;
                             ++mat_idx)
                        {
                            Asset_Material *asset_mat = asset_materials + mat_idx;
                            aiMaterial *mat = model->mMaterials[mat_idx];

                            aiColor3D c;
                            mat->Get(AI_MATKEY_COLOR_AMBIENT, c);
                            asset_mat->color_ambient = _v3_(c.r, c.g, c.b);

                            mat->Get(AI_MATKEY_COLOR_DIFFUSE, c);
                            asset_mat->color_diffuse = _v3_(c.r, c.g, c.b);

                            mat->Get(AI_MATKEY_COLOR_SPECULAR, c);
                            asset_mat->color_specular = _v3_(c.r, c.g, c.b);

                            printf("msvc piece of shit\n");
                        }
                    }
                    else
                    {
                    }


                    //
                    // write.
                    // TODO: this is janky asf.
                    //
                    fwrite(&asset_model.mesh_count, sizeof(asset_model.mesh_count), 1, model_out);

                    for (u32 mesh_idx = 0;
                         mesh_idx < asset_model.mesh_count;
                         ++mesh_idx)
                    {
                        Asset_Mesh *asset_mesh = (asset_model.meshes + mesh_idx);

                        fwrite(&asset_mesh->vertex_count, sizeof(u32), 1, model_out);
                        fwrite(asset_mesh->vertices, sizeof(Asset_Vertex) * asset_mesh->vertex_count, 1, model_out);

                        fwrite(&asset_mesh->index_count, sizeof(u32), 1, model_out);
                        fwrite(asset_mesh->indices, sizeof(u32) * asset_mesh->index_count, 1, model_out);

                        fwrite(&asset_mesh->material_idx, sizeof(u32), 1, model_out);
                    }

                    u32 mat_count = model->mNumMaterials;
                    fwrite(&mat_count, sizeof(u32), 1, model_out);
                    if (mat_count)
                    {
                        fwrite(asset_materials, sizeof(Asset_Material) * mat_count, 1, model_out);
                    }

                    u32  model_bone_count               = (u32)model_bones.size();
                    fwrite(&model_bone_count, sizeof(u32), 1, model_out);
                    if (model_bone_count)
                    {
                        s32  model_root_bone_id     = get_bone_id(find_root_bone_node(model->mRootNode)->mName);
                        m4x4 model_root_transform   = ai_m4x4_to_m4x4(model->mRootNode->mTransformation);
                        fwrite(&model_root_bone_id, sizeof(s32), 1, model_out);
                        fwrite(&model_root_transform, sizeof(m4x4), 1, model_out);
                        for (auto &[id, model_bone] : model_bones)
                        {
                            fwrite(&model_bone.bone_id, sizeof(s32), 1, model_out);
                            fwrite(&model_bone.offset, sizeof(m4x4), 1, model_out);
                            fwrite(&model_bone.transform, sizeof(m4x4), 1, model_out);
                        }
                    }

                }
                else
                {
                }

                if (model->HasTextures())
                {
                }
                else
                {
                }




                //
                // Animation
                //
                u32 anim_count = model->mNumAnimations;
                fwrite(&anim_count, sizeof(u32), 1, model_out);
                if (anim_count)
                {
                    for (s32 anim_id = 0;
                         anim_id < (s32)anim_count;
                         ++anim_id)
                    {
                        aiAnimation *anim       = model->mAnimations[anim_id];
                        f32 spt                 = 1.0f / (f32)anim->mTicksPerSecond;
                        f32 anim_duration       = (f32)(anim->mDuration / anim->mTicksPerSecond);
                        u32 bone_count          = anim->mNumChannels;

                        fwrite(&anim_id, sizeof(s32), 1, model_out);
                        fwrite(&anim_duration, sizeof(f32), 1, model_out);
                        fwrite(&bone_count, sizeof(u32), 1, model_out);

                        for (u32 bone_idx = 0;
                             bone_idx < bone_count;
                             ++bone_idx)
                        {
                            aiNodeAnim *bone                 = anim->mChannels[bone_idx];

                            s32 bone_id = get_bone_id(bone->mNodeName);
                            fwrite(&bone_id, sizeof(s32), 1, model_out);

                            u32 translation_count = bone->mNumPositionKeys;
                            fwrite(&translation_count, sizeof(u32), 1, model_out);

                            u32 rotation_count = bone->mNumRotationKeys;
                            fwrite(&rotation_count, sizeof(u32), 1, model_out);

                            u32 scaling_count = bone->mNumScalingKeys;
                            fwrite(&scaling_count, sizeof(u32), 1, model_out);

                            for (u32 idx = 0;
                                 idx < translation_count;
                                 ++idx)
                            {
                                aiVectorKey *key = bone->mPositionKeys + idx;
                                f32 dt = (f32)key->mTime * spt;
                                v3 vec = aiv3_to_v3(key->mValue);
                                fwrite(&dt, sizeof(f32), 1, model_out);
                                fwrite(&vec, sizeof(v3), 1, model_out);
                            }
                            for (u32 idx = 0;
                                 idx < rotation_count;
                                 ++idx)
                            {
                                aiQuatKey *key = bone->mRotationKeys + idx;
                                f32 dt = (f32)key->mTime * spt;
                                qt q   = aiqt_to_qt(key->mValue);
                                fwrite(&dt, sizeof(f32), 1, model_out);
                                fwrite(&q, sizeof(qt), 1, model_out);
                            }
                            for (u32 idx = 0;
                                 idx < scaling_count;
                                 ++idx)
                            {
                                aiVectorKey *key = bone->mScalingKeys + idx;
                                f32 dt = (f32)key->mTime * spt;
                                v3 vec = aiv3_to_v3(key->mValue);
                                fwrite(&dt, sizeof(f32), 1, model_out);
                                fwrite(&vec, sizeof(v3), 1, model_out);
                            }
                        }
                    }
                }


                build_skeleton(model->mRootNode, -1);


                printf("success: written '%s'\n", model_file_name);
                fclose(model_out);
            }
            else
            {
                printf("error: couldn't open output file %s\n", model_file_name);
                exit(1);
            }




        }
        else
        {
            printf("error: couldn't load model %s.\n", file_name);
            exit(1);
        }
    }


    //
    // Global Bone Hierarchy
    //
    if (write_skeleton)
    {
        Asset_Bone_Hierarchy asset_bone_hierarchy = {};
        for (s32 bone_id = 0;
             bone_id < MAX_BONE_PER_MESH;
             ++bone_id)
        {
            std::vector<s32> bone_info  = g_bone_hierarchy[bone_id];
            u32 child_count             = (u32)bone_info.size();
            Asset_Bone_Info *asset_bone = &asset_bone_hierarchy.bone_infos[bone_id];
            asset_bone->child_count     = child_count;
            asset_bone->child_ids       = push_array(&arena, s32, child_count);
            for (u32 child_id = 0;
                 child_id < child_count;
                 ++child_id)
            {
                asset_bone->child_ids[child_id] = bone_info[child_id];
            }
        }

        const char *bones_file_name = "bones.pack";
        FILE *bones_out = fopen(bones_file_name, "wb");
        if (bones_out)
        {
            for (u32 count = 0;
                 count < MAX_BONE_PER_MESH;
                 ++count)
            {
                Asset_Bone_Info *asset_bone = &asset_bone_hierarchy.bone_infos[count];
                fwrite(&asset_bone->child_count, sizeof(u32), 1, bones_out);
                fwrite(asset_bone->child_ids, sizeof(s32) * asset_bone->child_count, 1, bones_out);
            }

            fclose(bones_out);
            printf("success: written '%s'.\n", bones_file_name);
        }
        else
        {
            printf("error: couldn't open file %s.\n", bones_file_name);
            exit(1);
        }
    }

    printf("*** SUCCESSFUL! ***\n");
    return 0;
}
