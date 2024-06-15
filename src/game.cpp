/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Sung Woo Lee $
   $Notice: (C) Copyright %s by Sung Woo Lee. All Rights Reserved. $
   ======================================================================== */


#define ASSET_FILE_NAME "asset.pack"

#include "types.h"
#include "game.h"
#include "memory.cpp"
#include "debug.h"
#include "render_group.cpp"
#include "sim.cpp"
#include "asset.cpp"

internal void
build_animation_transform(Asset_Model *model, s32 bone_id,
                          Asset_Animation *anim, f32 dt,
                          Asset_Bone_Hierarchy *bone_hierarchy,
                          m4x4 *final_transforms,
                          m4x4 parent_transform)
{
    TIMED_FUNCTION();
    Assert(dt >= 0.0f);

    Asset_Bone *bone = 0;
    // TODO: this is stupid.
    for (u32 bone_idx = 0;
         bone_idx < model->bone_count;
         ++bone_idx)
    {
        Asset_Bone *at = model->bones + bone_idx;
        if (at->bone_id == bone_id)
        {
            bone = at;
            break;
        }
    }
    Assert(bone);

    m4x4 node_transform = bone->transform;

    // TODO: this is stupid.
    for (u32 bone_idx = 0;
         bone_idx < anim->bone_count;
         ++bone_idx)
    {
        Asset_Animation_Bone *anim_bone = (anim->bones + bone_idx);
        if (bone_id == anim_bone->bone_id)
        {
            // lerp translation.
            v3 lerped_translation = (anim_bone->translations + (anim_bone->translation_count - 1))->vec;
            for (u32 translation_idx = 0;
                 translation_idx < anim_bone->translation_count;
                 ++translation_idx)
            {
                dt_v3_Pair *hi_key = anim_bone->translations + translation_idx;
                if (hi_key->dt > dt)
                {
                    dt_v3_Pair *lo_key = (hi_key - 1);
                    f32 t = (dt - lo_key->dt) / (hi_key->dt - lo_key->dt);
                    lerped_translation = lerp(lo_key->vec, t, hi_key->vec);
                    break;
                }
                else if (hi_key->dt == dt)
                {
                    lerped_translation = hi_key->vec;
                    break;
                }
            }

            // slerp rotation.
            qt slerped_rotation = (anim_bone->rotations + (anim_bone->rotation_count - 1))->q;
            for (u32 rotation_idx = 0;
                 rotation_idx < anim_bone->rotation_count;
                 ++rotation_idx)
            {
                dt_qt_Pair *hi_key = anim_bone->rotations + rotation_idx;
                if (hi_key->dt > dt)
                {
                    dt_qt_Pair *lo_key = (hi_key - 1);
                    f32 t = (dt - lo_key->dt) / (hi_key->dt - lo_key->dt);
                    slerped_rotation = slerp(lo_key->q, t, hi_key->q);
                    break;
                }
                else if (hi_key->dt == dt)
                {
                    slerped_rotation = hi_key->q;
                    break;
                }
            }

            // lerp scaling.
            v3 lerped_scaling = (anim_bone->scalings + (anim_bone->scaling_count - 1))->vec;
            for (u32 scaling_idx = 0;
                 scaling_idx < anim_bone->scaling_count;
                 ++scaling_idx)
            {
                dt_v3_Pair *hi_key = anim_bone->scalings + scaling_idx;
                if (hi_key->dt > dt)
                {
                    dt_v3_Pair *lo_key = (hi_key - 1);
                    f32 t = (dt - lo_key->dt) / (hi_key->dt - lo_key->dt);
                    lerped_scaling = lerp(lo_key->vec, t, hi_key->vec);
                    break;
                }
                else if (hi_key->dt == dt)
                {
                    lerped_scaling = hi_key->vec;
                    break;
                }
            }

            node_transform = trs_to_transform(lerped_translation, slerped_rotation, lerped_scaling);

            break;
        }
    }

    m4x4 global_transform = parent_transform * node_transform;
    m4x4 final_transform  = global_transform * bone->offset;
    final_transforms[bone->bone_id] = final_transform;

    Asset_Bone_Info *bone_info = bone_hierarchy->bone_infos + bone_id;
    u32 child_count = bone_info->child_count;
    for (u32 child_idx = 0;
         child_idx < child_count;
         ++child_idx)
    {
        s32 child_bone_id = bone_info->child_ids[child_idx];
        build_animation_transform(model, child_bone_id,
                                  anim, dt,
                                  bone_hierarchy,
                                  final_transforms,
                                  global_transform);
    }
}


global_var m4x4 *grass_world_transforms;
global_var s32 grass_count;
global_var Asset_Mesh *grass_mesh;
global_var f32 grass_max_vertex_y;
global_var Bitmap *turbulence_map;

#define GRASS_COUNT_MAX 100'000
#define GRASS_DENSITY 10
#define GRASS_RANDOM_OFFSET 0.10f
#define TURBULENCE_MAP_SIDE 256 

global_var m4x4 *star_world_transforms;
global_var s32 star_count;
global_var Asset_Mesh *star_mesh;

#define STAR_COUNT_MAX 100'000

internal Bitmap *
gen_turbulence_map(Memory_Arena *arena, Random_Series *series, u32 side)
{
    Bitmap *result  = 0;
    result          = push_struct(arena, Bitmap);
    result->width   = side;
    result->height  = side;
    result->pitch   = sizeof(u32) * side;
    result->size    = sizeof(u32) * side * side;
    result->memory  = push_array(arena, u32, side * side);
    for (u32 y = 0; y < side; ++y)
    {
        for (u32 x = 0; x < side; ++x)
        {
            u32 *at = (u32 *)result->memory + (y * side + x);
            u8 gray = u8(rand_range(series, 0.0f, 255.0f) + 0.5f);
            *at = (0xff << 24 | gray << 16 | gray << 8 | gray);
        }
    }
    return result;
}

global_var Render_Group *g_debug_render_group;

extern "C"
GAME_MAIN(game_main)
{
    if (!game_state->init) 
    {
        init_arena(&game_state->world_arena,
                   game_memory->permanent_memory_size - sizeof(Game_State),
                   (u8 *)game_memory->permanent_memory + sizeof(Game_State));

        game_state->world               = push_struct(&game_state->world_arena, World);
        World *world                    = game_state->world;
        world->chunk_dim                = _v3_(10.0f, 3.0f, 10.0f);
        Memory_Arena *world_arena       = &game_state->world_arena;
        Chunk_Hashmap *chunk_hashmap    = &game_state->world->chunkHashmap;

        game_state->random_series = seed(6974);

        Chunk *chunk = get_chunk(&game_state->world_arena, chunk_hashmap, Chunk_Position{0, 0, 0});
        v3 dim = _v3_(1.0f, 1.0f, 1.0f);

        grass_world_transforms = push_array(&game_state->world_arena, m4x4, GRASS_COUNT_MAX); 

        s32 hX = round_f32_to_s32(game_state->world->chunk_dim.x * 0.5f);
        s32 hZ = round_f32_to_s32(game_state->world->chunk_dim.z * 0.5f);
        for (s32 X = -hX; X <= hX; ++X) 
        {
            for (s32 Z = -hZ; Z <= hZ; ++Z) 
            {
                Chunk_Position tile_pos = {chunk->x, chunk->y, chunk->z};
                tile_pos.offset.x += dim.x * X;
                tile_pos.offset.z += dim.z * Z;
                recalc_pos(&tile_pos, game_state->world->chunk_dim);
                Entity *tile1 = push_entity(world_arena, chunk_hashmap,
                                            eEntity_Tile, tile_pos, world->chunk_dim);
#if 1
                for (s32 z = -GRASS_DENSITY; z <= GRASS_DENSITY; ++z)
                {
                    for (s32 x = -GRASS_DENSITY; x <= GRASS_DENSITY; ++x)
                    {
                        Chunk_Position grass_pos = tile_pos;
                        grass_pos.offset.z += ((1.0f / GRASS_DENSITY) * dim.z * z +
                                               rand_range(&game_state->random_series, -GRASS_RANDOM_OFFSET, GRASS_RANDOM_OFFSET));
                        grass_pos.offset.x += ((1.0f / GRASS_DENSITY) * dim.x * x + 
                                               rand_range(&game_state->random_series, -GRASS_RANDOM_OFFSET, GRASS_RANDOM_OFFSET));
                        recalc_pos(&grass_pos, game_state->world->chunk_dim);

                        v3 translation      = _v3_(grass_pos.x * world->chunk_dim.x + grass_pos.offset.x,
                                                   grass_pos.y * world->chunk_dim.y + grass_pos.offset.y,
                                                   grass_pos.z * world->chunk_dim.z + grass_pos.offset.z);
                        f32 theta           = rand_range(&game_state->random_series, 0, pi32 * 0.5f);
                        qt rotation         = _qt_(cos(theta), 0, sin(theta), 0);
                        f32 scale           = rand_range(&game_state->random_series, 0.75f, 1.0f);
                        v3 scaling          = _v3_(scale, scale, scale);

                        Assert(grass_count != GRASS_COUNT_MAX);
                        grass_world_transforms[grass_count++] = transpose(trs_to_transform(translation, rotation, scaling));
                    }
                }
#endif
            }
        }

        Entity *xbot = push_entity(world_arena, chunk_hashmap, eEntity_XBot, Chunk_Position{0, 0, 0}, world->chunk_dim);
        game_state->player = xbot;

        load_bone_hierarchy(&game_state->world_arena, game_memory->platform.debug_platform_read_file);


        game_state->debug_cam = push_struct(&game_state->world_arena, Camera);
        Camera *cam = game_state->debug_cam;
        cam->type = eCamera_Type_Perspective;
        cam->focal_length = 0.5f;
        f32 T = pi32 * 0.1;
        f32 D = 5;
        cam->world_translation = _v3_(0, D * sin(T * 2.0f), D * cos(T * 2.0f));
        cam->world_rotation = _qt_(cos(T), -sin(T), 0, 0);

        //
        // NOISE MAP
        //
#if 0
        turbulence_map = gen_turbulence_map(&game_state->world_arena, &game_state->random_series, TURBULENCE_MAP_SIDE);
#else
        turbulence_map = load_bmp(&game_state->world_arena, game_memory->platform.debug_platform_read_file, "turbulence.bmp");
#endif



        // STAR
        star_world_transforms = push_array(&game_state->world_arena, m4x4, STAR_COUNT_MAX);
#define STAR_COUNT 10'000
#define STAR_SCALE 0.2f
#define STAR_DIST  200.0f
        for (u32 cnt = 0; cnt < STAR_COUNT; ++cnt)
        {
            Assert(star_count < STAR_COUNT_MAX);
            f32 x = rand_bilateral(&game_state->random_series);
            f32 y = rand_bilateral(&game_state->random_series);
            f32 z = rand_bilateral(&game_state->random_series);
            f32 theta = rand_range(&game_state->random_series, 0.0f, pi32 * 0.5f);
            v3 v = normalize(_v3_(x, y, z));
            v3 translation  = STAR_DIST * v;
            qt rotation     = _qt_(cos(theta), sin(theta) * v);
            v3 scaling      = _v3_(STAR_SCALE, STAR_SCALE, STAR_SCALE);
            star_world_transforms[star_count++] = transpose(trs_to_transform(translation, rotation, scaling));
        }



        game_state->init = true;
    }


    // @dt
    f32 dt = game_input->dt_per_frame;
    game_state->time += dt;


    ///////////////////////////////////////////////////////////////////////////
    //
    // Init Transient Memory
    //
    void *transMem  = game_memory->transient_memory;
    u64 transMemCap = game_memory->transient_memory_size;
    Assert(sizeof(Transient_State) < transMemCap);
    Transient_State *transient_state = (Transient_State *)transMem;

    if (!transient_state->init)
    {
        // transient arena.
        init_arena(&transient_state->transient_arena,
                   MB(200),
                   (u8 *)transMem + sizeof(Transient_State));

        // reserved memory for multi-thread work data.
        for (u32 idx = 0;
             idx < array_count(transient_state->workArena);
             ++idx) 
        {
            WorkMemory_Arena *workSlot = transient_state->workArena + idx;
            init_sub_arena(&workSlot->memoryArena, &transient_state->transient_arena, MB(4));
        }

        // asset arena.
        init_arena(&transient_state->assetArena,
                   MB(20),
                   (u8 *)transMem + sizeof(Transient_State) + transient_state->transient_arena.size);

        transient_state->high_priority_queue    = game_memory->high_priority_queue;
        transient_state->low_priority_queue     = game_memory->low_priority_queue;
        Game_Assets *game_assets            = &transient_state->game_assets; // TODO: Ain't thrilled about it.
        game_assets->read_entire_file       = game_memory->platform.debug_platform_read_file;

        load_model(&game_assets->xbot_model, "xbot.3d", &game_state->world_arena, game_memory->platform.debug_platform_read_file);
        load_model(&game_assets->cube_model, "cube.3d", &game_state->world_arena, game_memory->platform.debug_platform_read_file);
        load_model(&game_assets->octahedral_model, "octahedral.3d", &game_state->world_arena, game_memory->platform.debug_platform_read_file);
        load_model(&game_assets->grass_model, "grass.3d", &game_state->world_arena, game_memory->platform.debug_platform_read_file);
        grass_mesh = game_assets->grass_model->meshes;
        for (u32 vertex_idx = 0;
             vertex_idx < grass_mesh->vertex_count;
             ++vertex_idx)
        {
            Asset_Vertex *vertex = grass_mesh->vertices + vertex_idx;
            grass_max_vertex_y = Max(grass_max_vertex_y, vertex->pos.y);
        }

        star_mesh = game_assets->octahedral_model->meshes;

        load_font(&game_state->world_arena, game_memory->platform.debug_platform_read_file, &transient_state->game_assets);


        transient_state->init = true;  
    }

    Temporary_Memory render_memory = begin_temporary_memory(&transient_state->transient_arena);

    Camera *debug_overlay_camera = push_camera(&transient_state->transient_arena,
                                               eCamera_Type_Orthographic,
                                               (f32)game_screen_buffer->width,
                                               (f32)game_screen_buffer->height);
    g_debug_render_group = alloc_render_group(&transient_state->transient_arena,
                                              debug_overlay_camera);
    if (g_debug_render_group)
    {
        debug_reset(game_screen_buffer->width, game_screen_buffer->height);
    }

    game_state->debug_cam->width    = (f32)game_screen_buffer->width;
    game_state->debug_cam->height   = (f32)game_screen_buffer->height;

    Render_Group *render_group = alloc_render_group(&transient_state->transient_arena,
                                                    game_state->debug_cam);


    v3 chunk_dim = game_state->world->chunk_dim;

    //
    // Process Input
    //
    Entity *player = game_state->player;
    player->accel = _v3_(0, 0, 0);
    if (!game_state->debug_mode) 
    {
        if (game_input->W.is_set)
        {
            m4x4 rotation = to_m4x4(player->world_rotation);
            player->accel = rotation * _v3_(0, 0, 1);
        }
        if (game_input->D.is_set)
        {
            player->world_rotation = _qt_(cos(dt), 0,-sin(dt), 0) * player->world_rotation;
        }
        if (game_input->A.is_set)
        {
            player->world_rotation = _qt_(cos(dt), 0, sin(dt), 0) * player->world_rotation;
        }
    }
    else
    {
        Camera *cam = game_state->debug_cam;
        f32 C = dt * 3.0f;
        if (game_input->W.is_set)
        {
            m4x4 rotation = to_m4x4(cam->world_rotation);
            cam->world_translation += rotation * _v3_(0, 0, -C);
        }
        if (game_input->S.is_set)
        {
            m4x4 rotation = to_m4x4(cam->world_rotation);
            cam->world_translation += rotation * _v3_(0, 0, C);
        }
        if (game_input->D.is_set)
        {
            m4x4 rotation = to_m4x4(cam->world_rotation);
            cam->world_translation += rotation * _v3_(C, 0, 0);
        }
        if (game_input->A.is_set)
        {
            m4x4 rotation = to_m4x4(cam->world_rotation);
            cam->world_translation += rotation * _v3_(-C, 0, 0);
        }

        if (game_input->Q.is_set)
        {
            m4x4 rotation = to_m4x4(cam->world_rotation);
            cam->world_translation += rotation * _v3_(0, -C, 0);
        }
        if (game_input->E.is_set)
        {
            m4x4 rotation = to_m4x4(cam->world_rotation);
            cam->world_translation += rotation * _v3_(0, C, 0);
        }

    }

    if (game_input->toggle_debug.is_set)
    {
        if (game_state->debug_toggle_delay == 0.0f) 
        {
            game_state->debug_toggle_delay = 0.1f;
            if (game_state->debug_mode) 
            {
                game_state->debug_mode = false;
            } 
            else 
            {
                game_state->debug_mode = true;
            }
        }
    }
    game_state->debug_toggle_delay -= dt;
    if (game_state->debug_toggle_delay < 0.0f) 
    {
        game_state->debug_toggle_delay = 0.0f;
    }



    Chunk_Position camPos = Chunk_Position{};
    v3 camDim       = {100.0f, 5.0f, 50.0f};
    Chunk_Position minPos = camPos;
    Chunk_Position maxPos = camPos;
    minPos.offset -= 0.5f * camDim;
    maxPos.offset += 0.5f * camDim;
    recalc_pos(&minPos, chunk_dim);
    recalc_pos(&maxPos, chunk_dim);


    //
    // Update entities
    //
    update_entities(game_state, dt, minPos, maxPos);


    //
    // Render entities
    //

    //
    // @draw
    //
    Game_Assets *game_assets = &transient_state->game_assets;

#if 1
    for (s32 Z = minPos.z;
         Z <= maxPos.z;
         ++Z) 
    {
        for (s32 Y = minPos.y;
             Y <= maxPos.y;
             ++Y) 
        {
            for (s32 X = minPos.x;
                 X <= maxPos.x;
                 ++X) 
            {
                Chunk *chunk = get_chunk(&game_state->world_arena,
                                         &game_state->world->chunkHashmap,
                                         Chunk_Position{X, Y, Z});
                for (Entity *entity = chunk->entities.head;
                     entity != 0;
                     entity = entity->next) 
                {

                    m4x4 world_transform = trs_to_transform(entity->world_translation,
                                                            entity->world_rotation,
                                                            entity->world_scaling);

                    switch (entity->type) 
                    {
                        case eEntity_XBot: 
                        {
                            Asset_Model *model = game_assets->xbot_model;
                            if (model)
                            {
                                if (entity->cur_anim)
                                {
                                    m4x4 *final_transforms = push_array(&transient_state->transient_arena, m4x4, MAX_BONE_PER_MESH);
                                    build_animation_transform(model, model->root_bone_id,
                                                              entity->cur_anim, entity->anim_dt,
                                                              &g_bone_hierarchy,
                                                              final_transforms,
                                                              model->root_transform);
                                    entity->anim_dt += dt;
                                    if (entity->anim_dt > entity->cur_anim->duration)
                                    {
                                        entity->anim_dt = 0.0f;
                                    }

                                    for (u32 mesh_idx = 0;
                                         mesh_idx < model->mesh_count;
                                         ++mesh_idx)
                                    {
                                        Asset_Mesh *mesh    = model->meshes + mesh_idx;
                                        Asset_Material *mat = model->materials + mesh->material_idx;
                                        push_mesh(render_group, mesh, mat, world_transform, final_transforms);
                                    }
                                }
                                else
                                {
                                    // TODO: for now.
                                    entity->cur_anim = model->anims;
                                }

                            }
                        } break;

                        case eEntity_Tile: 
                        {
                            Asset_Model *model = game_assets->cube_model;
                            if (model)
                            {
                                for (u32 mesh_idx = 0;
                                     mesh_idx < model->mesh_count;
                                     ++mesh_idx)
                                {
                                    Asset_Mesh *mesh = model->meshes + mesh_idx;
                                    Asset_Material *mat = model->materials + mesh->material_idx;
                                    push_mesh(render_group, mesh, mat, world_transform);
                                }
                            }
                        } break;

                        INVALID_DEFAULT_CASE
                    }

                }
            }
        }
    }
#endif

    push_grass(render_group, grass_mesh, grass_count, grass_world_transforms, game_state->time, grass_max_vertex_y, turbulence_map);
    push_star(render_group, star_mesh, star_count, star_world_transforms, game_state->time);
    






#if __DEBUG
    if (game_state->debug_mode)
    {
        debug_overlay(game_memory, (f32)game_screen_buffer->width, (f32)game_screen_buffer->height, &transient_state->game_assets);
    }
#endif


    render_group_to_output_batch(render_group, &game_memory->render_batch);
    render_group_to_output_batch(g_debug_render_group, &game_memory->render_batch);
    end_temporary_memory(&render_memory);
}

#include "debug.cpp"
