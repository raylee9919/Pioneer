 /* ―――――――――――――――――――――――――――――――――――◆――――――――――――――――――――――――――――――――――――
    $File: $
    $Date: $
    $Revision: $
    $Creator: Sung Woo Lee $
    $Notice: (C) Copyright 2024 by Sung Woo Lee. All Rights Reserved. $
    ―――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――― */


#define BYTES_PER_PIXEL 4
#define ASSET_FILE_NAME "asset.pack"

#include "types.h"
#include "game.h"
#include "debug.h"
#include "memory.cpp"
#include "render_group.cpp"

#if __DEBUG
#include "windows.h"
#endif

#include "sim.cpp"

internal void
init_debug(Debug_Log *debug_log, Memory_Arena *arena);

internal void
display_debug_info(Debug_Log *debug_log, Render_Group *render_group, Game_Assets *game_assets, Memory_Arena *arena);

internal void
end_debug_log(Debug_Log *debug_log);


internal void
load_model(Asset_Model **asset_model, const char *file_name,
           Memory_Arena *arena, Read_Entire_File *read_entire_file)
{
    *asset_model = push_struct(arena, Asset_Model);
    Entire_File entire_file = read_entire_file(file_name);
    u8 *at                  = (u8 *)entire_file.contents;
    u8 *end                 = at + entire_file.content_size;

    Asset_Model *model = *asset_model;

    model->mesh_count = *(u32 *)at;
    at += sizeof(model->mesh_count);

    model->meshes = push_array(arena, Asset_Mesh, model->mesh_count);
    Asset_Mesh *mesh = model->meshes;
    size_t tmp_size;
    for (u32 mesh_idx = 0;
         mesh_idx < model->mesh_count;
         ++mesh_idx)
    {
        mesh->vertex_count = *(u32 *)at;
        at += sizeof(u32);

        tmp_size = sizeof(Asset_Vertex) * mesh->vertex_count;
        mesh->vertices = (Asset_Vertex *)push_size(arena, tmp_size);
        memcpy(mesh->vertices, at, tmp_size);
        at += tmp_size;

        mesh->index_count = *(u32 *)at;
        at += sizeof(u32);

        tmp_size = sizeof(u32) * mesh->index_count;
        mesh->indices = (u32 *)push_size(arena, tmp_size);
        memcpy(mesh->indices, at, tmp_size);
        at += tmp_size;

        mesh->material_idx = *(u32 *)at;
        at += sizeof(u32);

        ++mesh;
    }

    // MATERIAL
    model->material_count = *(u32 *)at;
    at += sizeof(u32);

    tmp_size = sizeof(Asset_Material) * model->material_count;
    model->materials = (Asset_Material *)push_size(arena, tmp_size);
    memcpy(model->materials, at, tmp_size);
    at += tmp_size;


    // SKELETAL
    model->bone_count = *(u32 *)at;
    at += sizeof(u32);

    if (model->bone_count)
    {
        model->root_bone_id = *(s32 *)at;
        at += sizeof(s32);

        model->root_transform = *(m4x4 *)at;
        at += sizeof(m4x4);

        tmp_size = sizeof(Asset_Bone) * model->bone_count;
        model->bones = (Asset_Bone *)push_size(arena, tmp_size);
        memcpy(model->bones, at, tmp_size);
        at += tmp_size;
    }

    // ANIMATION
    model->anim_count = *(u32 *)at;
    at += sizeof(u32);

    if (model->anim_count)
    {
        model->anims = push_array(arena, Asset_Animation, model->anim_count);
        for (u32 anim_idx = 0;
             anim_idx < model->anim_count;
             ++anim_idx)
        {
            Asset_Animation *anim = model->anims + anim_idx;
            
            anim->id = *(s32 *)at;
            at += sizeof(s32);

            anim->duration = *(f32 *)at;
            at += sizeof(f32);

            anim->bone_count = *(u32 *)at;
            at += sizeof(u32);

            if (anim->bone_count)
            {
                anim->bones = push_array(arena, Asset_Animation_Bone, anim->bone_count);
            }

            for (u32 bone_idx = 0;
                 bone_idx < anim->bone_count;
                 ++bone_idx)
            {
                Asset_Animation_Bone *anim_bone = anim->bones + bone_idx;

                anim_bone->bone_id = *(s32 *)at;
                at += sizeof(s32);

                anim_bone->translation_count = *(u32 *)at;
                at += sizeof(u32);

                anim_bone->rotation_count = *(u32 *)at;
                at += sizeof(u32);

                anim_bone->scaling_count = *(u32 *)at;
                at += sizeof(u32);

                tmp_size = sizeof(dt_v3_Pair) * anim_bone->translation_count;
                anim_bone->translations = (dt_v3_Pair *)push_size(arena, tmp_size);
                memcpy(anim_bone->translations, at, tmp_size);
                at += tmp_size;

                tmp_size = sizeof(dt_qt_Pair) * anim_bone->rotation_count;
                anim_bone->rotations = (dt_qt_Pair *)push_size(arena, tmp_size);
                memcpy(anim_bone->rotations, at, tmp_size);
                at += tmp_size;

                tmp_size = sizeof(dt_v3_Pair) * anim_bone->scaling_count;
                anim_bone->scalings = (dt_v3_Pair *)push_size(arena, tmp_size);
                memcpy(anim_bone->scalings, at, tmp_size);
                at += tmp_size;
            }
        }


    }

    Assert(at == end);
}

global_var Asset_Bone_Hierarchy g_bone_hierarchy;
internal void
load_bone_hierarchy(Memory_Arena *arena, Read_Entire_File *read_entire_file)
{
    Entire_File entire_file = read_entire_file("bones.pack");
    u8 *at                  = (u8 *)entire_file.contents;
    u8 *end                 = at + entire_file.content_size;

    for (u32 id = 0;
         id < MAX_BONE_PER_MESH;
         ++id)
    {
        Asset_Bone_Info *bone = &g_bone_hierarchy.bone_infos[id];

        bone->child_count = *(u32 *)at;
        at += sizeof(u32);

        if (bone->child_count)
        {
            size_t tmp_size = sizeof(s32) * bone->child_count;
            bone->child_ids = (s32 *)push_size(arena, tmp_size);
            memcpy(bone->child_ids, at, tmp_size);
            at += tmp_size;
        }
    }

    Assert(at == end);
}

internal void
build_animation_transform(Asset_Model *model, s32 bone_id,
                          Asset_Animation *anim, f32 dt,
                          Asset_Bone_Hierarchy *bone_hierarchy,
                          m4x4 *final_transforms,
                          m4x4 parent_transform)
{
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

internal void
load_font(Memory_Arena *arena, Read_Entire_File *read_file, Game_Assets *game_assets)
{
    Entire_File read = read_file(ASSET_FILE_NAME);
    u8 *at = (u8 *)read.contents;
    u8 *end = at + read.content_size;

    // parse font header.
    u32 kern_count = ((Asset_Font_Header *)at)->kerning_pair_count;
    game_assets->v_advance = ((Asset_Font_Header *)at)->vertical_advance;
    at += sizeof(Asset_Font_Header);

    // parse kerning pairs.
    for (u32 count = 0; count < kern_count; ++count) 
    {
        Asset_Kerning *asset_kern = (Asset_Kerning *)at;

        Kerning *kern = push_struct(arena, Kerning);
        *kern = {};
        kern->first = asset_kern->first;
        kern->second = asset_kern->second;
        kern->value = asset_kern->value;

        u32 entry_idx = kerning_hash(&game_assets->kern_hashmap, kern->first, kern->second);
        push_kerning(&game_assets->kern_hashmap, kern, entry_idx);
        at += sizeof(Asset_Kerning);
    }

    // parse glyphs.
    if (read.content_size != 0) 
    {
        while (at < end) 
        {
            Asset_Glyph *glyph = (Asset_Glyph *)at;
            Bitmap *bitmap = &glyph->bitmap;
            game_assets->glyphs[glyph->codepoint] = glyph;
            at += sizeof(Asset_Glyph);
            glyph->bitmap.memory = at + (bitmap->height - 1) * -bitmap->pitch;
            at += glyph->bitmap.size;
        }
    }
}

internal Bitmap *
load_bmp(Memory_Arena *arena, Read_Entire_File *read_file, const char *filename)
{
    Bitmap *result = push_struct(arena, Bitmap);
    *result = {};
    
    Entire_File read = read_file(filename);
    if (read.content_size != 0) 
    {
        BMP_Info_Header *header = (BMP_Info_Header *)read.contents;
        u32 *pixels = (u32 *)((u8 *)read.contents + header->bitmap_offset);

        result->memory = pixels + header->width * (header->height - 1);
        result->width = header->width;
        result->height = header->height;
        result->pitch = -result->width * BYTES_PER_PIXEL;
        result->size = result->width * result->height * BYTES_PER_PIXEL;
        result->handle = 0;

        Assert(header->compression == 3);

        u32 r_mask = header->r_mask;
        u32 g_mask = header->g_mask;
        u32 b_mask = header->b_mask;
        u32 a_mask = ~(r_mask | g_mask | b_mask);        
        
        Bit_Scan_Result r_scan = find_least_significant_set_bit(r_mask);
        Bit_Scan_Result g_scan = find_least_significant_set_bit(g_mask);
        Bit_Scan_Result b_scan = find_least_significant_set_bit(b_mask);
        Bit_Scan_Result a_scan = find_least_significant_set_bit(a_mask);
        
        Assert(r_scan.found);
        Assert(g_scan.found);   
        Assert(b_scan.found);
        Assert(a_scan.found);

        s32 r_shift = (s32)r_scan.index;
        s32 g_shift = (s32)g_scan.index;
        s32 b_shift = (s32)b_scan.index;
        s32 a_shift = (s32)a_scan.index;

        f32 inv_255f = 1.0f / 255.0f;
        
        u32 *at = pixels;
        for(s32 y = 0;
            y < header->height;
            ++y)
        {
            for(s32 x = 0;
                x < header->width;
                ++x)
            {
                u32 c = *at;

                f32 r = (f32)((c & r_mask) >> r_shift);
                f32 g = (f32)((c & g_mask) >> g_shift);
                f32 b = (f32)((c & b_mask) >> b_shift);
                f32 a = (f32)((c & a_mask) >> a_shift);

                f32 ra = a * inv_255f;
#if 1
                r *= ra;
                g *= ra;
                b *= ra;
#endif

                *at++ = (((u32)(a + 0.5f) << 24) |
                         ((u32)(r + 0.5f) << 16) |
                         ((u32)(g + 0.5f) <<  8) |
                         ((u32)(b + 0.5f) <<  0));
            }
        }
    }

    return result;
}

PLATFORM_WORK_QUEUE_CALLBACK(load_asset_work)
{
    Load_Asset_Work_Data *workData = (Load_Asset_Work_Data *)data;
    workData->game_assets->bitmaps[workData->assetID] = load_bmp(workData->assetArena, workData->game_assets->read_entire_file, workData->fileName);
    workData->game_assets->bitmapStates[workData->assetID] = Asset_State_Loaded;
    end_work_memory(workData->workSlot);
}

internal Bitmap *
GetBitmap(Transient_State *trans_state, Asset_ID assetID,
          PlatformWorkQueue *queue, Platform_API *platform)
{
    Bitmap *result = trans_state->game_assets.bitmaps[assetID];

    if (!result) 
    {
        if (atomic_compare_exchange_u32((u32 *)&trans_state->game_assets.bitmapStates[assetID],
                                        Asset_State_Queued, Asset_State_Unloaded)) 
        {
            WorkMemory_Arena *workSlot = begin_work_memory(trans_state);
            if (workSlot) 
            {
                Load_Asset_Work_Data *workData = push_struct(&workSlot->memoryArena, Load_Asset_Work_Data);
                workData->game_assets = &trans_state->game_assets;
                workData->assetArena = &trans_state->assetArena;
                workData->assetID = assetID;
                workData->workSlot = workSlot;

                switch(assetID) 
                {
                    case GAI_Tree: 
                    {
                        workData->fileName = "tree2_teal.bmp";
                    } break;

                    case GAI_Particle: 
                    {
                        workData->fileName = "white_particle.bmp";
                    } break;

                    INVALID_DEFAULT_CASE
                }
#if 1 // multi-thread
                platform->platform_add_entry(queue, load_asset_work, workData);
                return 0; // todo: no bmp...?
#else // single-thread
                load_asset_work(queue, workData);
                return 0; // todo: no bmp...?
#endif
            } 
            else 
            {
                return result;
            }
        } 
        else 
        {
            return result;
        }
    } 
    else 
    {
        return result;
    }
}


internal void
str_format(u32 dst_size, char *dst_init, va_list arg_list)
{
}


extern "C"
GAME_MAIN(game_main)
{
    if (!game_state->is_init) 
    {
        init_arena(&game_state->world_arena,
                   game_memory->permanent_memory_size - sizeof(Game_State),
                   (u8 *)game_memory->permanent_memory + sizeof(Game_State));

        game_state->world               = push_struct(&game_state->world_arena, World);
        World *world                    = game_state->world;
        world->chunk_dim                = {17.0f, 9.0f, 3.0f};
        Memory_Arena *world_arena       = &game_state->world_arena;
        Chunk_Hashmap *chunk_hashmap    = &game_state->world->chunkHashmap;

        Chunk *chunk = get_chunk(&game_state->world_arena, chunk_hashmap, Chunk_Position{0, 0, 0});
        v3 dim = {1.0f, 1.0f, 1.0f};

        for (s32 X = -8; X <= 8; ++X) 
        {
            for (s32 Y = -4; Y <= 4; ++Y) 
            {
                Chunk_Position tile_pos = {chunk->x, chunk->y, chunk->z};
                tile_pos.offset.x += dim.x * X;
                tile_pos.offset.y += dim.y * Y;
                recalc_pos(&tile_pos, game_state->world->chunk_dim);
                Entity *tile1 = push_entity(world_arena, chunk_hashmap,
                                            eEntity_Tile, tile_pos, world->chunk_dim);
                Entity *tile2 = push_entity(world_arena, chunk_hashmap,
                                            eEntity_Tile, tile_pos, world->chunk_dim);
                --tile2->chunk_pos.z;
            }
        }

#ifdef __DEBUG
        // debug memory.
        init_arena(&game_state->debug_arena, game_memory->debug_memory_size, (u8 *)game_memory->debug_memory);
        init_debug(&g_debug_log, &game_state->debug_arena);
#endif

        Entity *xbot = push_entity(world_arena, chunk_hashmap, eEntity_XBot, Chunk_Position{0, 0, 0}, world->chunk_dim);
        game_state->player = xbot;

        load_bone_hierarchy(&game_state->world_arena, game_memory->platform.debug_platform_read_file);

        game_state->is_init = true;
    }

    TIMED_BLOCK();


    ///////////////////////////////////////////////////////////////////////////
    //
    // Init Transient Memory
    //
    void *transMem  = game_memory->transient_memory;
    u64 transMemCap = game_memory->transient_memory_size;
    Assert(sizeof(Transient_State) < transMemCap);
    Transient_State *trans_state = (Transient_State *)transMem;

    if (!trans_state->is_init)
    {
        // transient arena.
        init_arena(&trans_state->transient_arena,
                   MB(100),
                   (u8 *)transMem + sizeof(Transient_State));
        
        // reserved memory for multi-thread work data.
        for (u32 idx = 0;
             idx < array_count(trans_state->workArena);
             ++idx) 
        {
            WorkMemory_Arena *workSlot = trans_state->workArena + idx;
            init_sub_arena(&workSlot->memoryArena, &trans_state->transient_arena, MB(4));
        }
        
        // asset arena.
        init_arena(&trans_state->assetArena,
                   MB(20),
                   (u8 *)transMem + sizeof(Transient_State) + trans_state->transient_arena.size);

        trans_state->highPriorityQueue  = game_memory->highPriorityQueue;
        trans_state->lowPriorityQueue   = game_memory->lowPriorityQueue;
        Game_Assets *game_assets        = &trans_state->game_assets; // TODO: Ain't thrilled about it.
        game_assets->read_entire_file   = game_memory->platform.debug_platform_read_file;

        load_model(&game_assets->xbot_model, "xbot.3d", &game_state->world_arena, game_memory->platform.debug_platform_read_file);
        load_model(&game_assets->cube_model, "cube.3d", &game_state->world_arena, game_memory->platform.debug_platform_read_file);
        load_font(&game_state->world_arena, game_memory->platform.debug_platform_read_file, &trans_state->game_assets);

        trans_state->is_init = true;  
   }

    f32 aspect_ratio = safe_ratio((f32)game_screen_buffer->width,
                                  (f32)game_screen_buffer->height);

    Temporary_Memory render_memory = begin_temporary_memory(&trans_state->transient_arena);



    Render_Group *render_group = alloc_render_group(&trans_state->transient_arena,
                                                    false,
                                                    aspect_ratio);

    v3 chunk_dim = game_state->world->chunk_dim;
    f32 dt = game_input->dt_per_frame;

    //
    // Process Input
    //
    Entity *player = game_state->player;
    player->accel = _v3_(0, 0, 0);
    if (game_input->move_up.is_set)
    {
        m4x4 rotation = to_m4x4(player->world_rotation);
        player->accel = rotation * _v3_(0, 0, dt * 2.0f);
    }
    if (game_input->move_right.is_set)
    {
        f32 theta = acos(player->world_rotation.w) - dt;
        if (theta < 0.0f)
        {
            theta += pi32;
        }
        player->world_rotation = _qt_(cos(theta), 0, sin(theta), 0);
    }
    if (game_input->move_left.is_set)
    {
        f32 theta = acos(player->world_rotation.w) + dt;
        if (theta > pi32)
        {
            theta -= pi32;
        }
        player->world_rotation = _qt_(cos(theta), 0, sin(theta), 0);
    }

#ifdef __DEBUG
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
#endif

    Chunk_Position camPos = Chunk_Position{};
    v3 camDim       = {100.0f, 50.0f, 5.0f};
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
    game_state->time += 0.01f;

#if __DEBUG
    if (game_input->alt.is_set) 
    {
        Mouse_Input *mouse = &game_input->mouse;
        f32 c = 2.0f;
        if (mouse->is_down[eMouse_Left]) 
        {
            if (mouse->toggle[eMouse_Left]) 
            {
                g_debug_cam_last_mouse_p = mouse->P;
            }
            g_debug_cam_orbital_yaw     += (g_debug_cam_last_mouse_p.x - mouse->P.x) * c;
            g_debug_cam_orbital_pitch   -= (g_debug_cam_last_mouse_p.y - mouse->P.y) * c;
            g_debug_cam_last_mouse_p    = mouse->P;
        }

        if (mouse->wheel_delta) 
        {
            c = 0.5f;
            g_debug_cam_z -= c * mouse->wheel_delta;
        }
    }
#endif

    //
    // @draw
    //
    Game_Assets *game_assets = &trans_state->game_assets;

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
                                    m4x4 *final_transforms = push_array(&trans_state->transient_arena, m4x4, MAX_BONE_PER_MESH);
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
                                        push_skeletal_mesh(render_group, mesh, mat,
                                                           world_transform, final_transforms);
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
                        } break;

                        INVALID_DEFAULT_CASE
                    }

                }
            }
        }
    }
#endif




    render_group_to_output_batch(render_group, &game_memory->render_batch);
    end_temporary_memory(&render_memory);


    //
    // Debug Overlay
    //
#ifdef __DEBUG
    Temporary_Memory debug_render_memory = begin_temporary_memory(&game_state->debug_arena);
    Render_Group *debug_render_group = alloc_render_group(&game_state->debug_arena, true, aspect_ratio);

    if (game_state->debug_mode)
    {
        display_debug_info(&g_debug_log, debug_render_group, game_assets, &game_state->debug_arena);
    }
    end_debug_log(&g_debug_log);

    render_group_to_output_batch(debug_render_group, &game_memory->render_batch);
    end_temporary_memory(&debug_render_memory);
#endif
}

internal void
init_debug(Debug_Log *debug_log, Memory_Arena *arena)
{
    // COUNTER must be the last one in the game code translation unit.
    u32 width = __COUNTER__;
    debug_log->record_width = width;
    debug_log->debug_records = push_array(arena, Debug_Record, DEBUG_LOG_FRAME_COUNT * width);
    debug_log->debug_infos = push_array(arena, Debug_Info, width);
    debug_log->next_frame = 0;

    for (u32 idx = 0;
         idx < width;
         ++idx) 
    {
        Debug_Info *info = debug_log->debug_infos + idx;
        info->max_cycles = 0;
        info->min_cycles = UINT64_MAX;
        info->avg_cycles = 0;
    }
}

internal void
display_debug_info(Debug_Log *debug_log, Render_Group *render_group, Game_Assets *game_assets, Memory_Arena *arena)
{
    for (u32 record_idx = 0;
            record_idx < debug_log->record_width;
            ++record_idx) 
    {
        Debug_Info *info = g_debug_log.debug_infos + record_idx;
        size_t size = 1024;
        char *buf = push_array(arena, char, size);
        _snprintf(buf, size,
                  "%20s(%4d): %10I64uavg_cyc",
                  info->function,
                  info->line,
                  info->avg_cycles);
        // push_text(render_group, _v3_(0.0f, 0.0f, 0.0f), buf, game_assets);

#if 1
        // draw graph.
        f32 x = 800.0f;
        f32 max_height = (f32)game_assets->v_advance;
        f32 width = 2.0f;
        f32 inv_max_cycles = 0.0f;
        if (info->max_cycles != 0.0f) 
        {
            inv_max_cycles = 1.0f / info->max_cycles;
        }

        for (u32 frame = 0;
             frame < DEBUG_LOG_FRAME_COUNT;
             ++frame)
        {
            Debug_Record *record = g_debug_log.debug_records + frame * g_debug_log.record_width + record_idx;
            f32 height = max_height * record->cycles * inv_max_cycles;
            v2 min = v2{x + width * frame, 100.0f + max_height * record_idx - height};
            v2 max = min + v2{width * 0.5f, height};
#if 0
            push_rect(render_group,
                      min, max,
                      v4{1.0f, 1.0f, 0.5f, 1.0f});
#endif
        }
#endif

    }

}

internal void
end_debug_log(Debug_Log *debug_log)
{
    Debug_Record *records = (debug_log->debug_records +
                             debug_log->next_frame *
                             debug_log->record_width);
    for (u32 record_idx = 0;
         record_idx < debug_log->record_width;
         ++record_idx) 
    {
        Debug_Record *record = records + record_idx;
        atomic_exchange_u32(&record->hit, 0);
    }

    if (++debug_log->next_frame == DEBUG_LOG_FRAME_COUNT) 
    {
        debug_log->next_frame = 0;
    }

    // TODO: remove this mf.
    // cen_y = 100.0f;
}
