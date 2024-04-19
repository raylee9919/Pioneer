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

#include "sim.cpp"


// debug.
internal void
init_debug(Debug_Log *debug_log, Memory_Arena *arena);

internal void
display_debug_info(Debug_Log *debug_log, Render_Group *render_group, Game_Assets *game_assets, Memory_Arena *arena);

internal void
end_debug_log(Debug_Log *debug_log);


internal void
load_font(Memory_Arena *arena, DEBUG_PLATFORM_READ_FILE_ *read_file, Game_Assets *game_assets) {
    DebugReadFileResult read = read_file(ASSET_FILE_NAME);
    u8 *at = (u8 *)read.contents;
    u8 *end = at + read.content_size;

    // parse font header.
    u32 kern_count = ((Asset_Font_Header *)at)->kerning_pair_count;
    game_assets->v_advance = ((Asset_Font_Header *)at)->vertical_advance;
    at += sizeof(Asset_Font_Header);

    // parse kerning pairs.
    for (u32 count = 0; count < kern_count; ++count) {
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
    if(read.content_size != 0) {
        while (at < end) {
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
load_bmp(Memory_Arena *arena, DEBUG_PLATFORM_READ_FILE_ *read_file, const char *filename) {
    Bitmap *result = push_struct(arena, Bitmap);
    *result = {};
    
    DebugReadFileResult read = read_file(filename);
    if(read.content_size != 0) {
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

        r32 inv_255f = 1.0f / 255.0f;
        
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

                r32 r = (r32)((c & r_mask) >> r_shift);
                r32 g = (r32)((c & g_mask) >> g_shift);
                r32 b = (r32)((c & b_mask) >> b_shift);
                r32 a = (r32)((c & a_mask) >> a_shift);

                r32 ra = a * inv_255f;
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
struct Load_Asset_Work_Data {
    Game_Assets *gameAssets;
    Memory_Arena *assetArena;
    Asset_ID assetID;
    const char *fileName;
    WorkMemory_Arena *workSlot;
};
PLATFORM_WORK_QUEUE_CALLBACK(load_asset_work) {
    Load_Asset_Work_Data *workData = (Load_Asset_Work_Data *)data;
    workData->gameAssets->bitmaps[workData->assetID] = load_bmp(workData->assetArena, workData->gameAssets->debug_platform_read_file, workData->fileName);
    workData->gameAssets->bitmapStates[workData->assetID] = Asset_State_Loaded;
    EndWorkMemory(workData->workSlot);
}
internal Bitmap *
GetBitmap(TransientState *transState, Asset_ID assetID,
        PlatformWorkQueue *queue, Platform_API *platform) {
    Bitmap *result = transState->gameAssets.bitmaps[assetID];

    if (!result) {
        if (atomic_compare_exchange_u32((u32 *)&transState->gameAssets.bitmapStates[assetID],
                                        Asset_State_Queued, Asset_State_Unloaded)) {
            WorkMemory_Arena *workSlot = BeginWorkMemory(transState);
            if (workSlot) {
                Load_Asset_Work_Data *workData = push_struct(&workSlot->memoryArena, Load_Asset_Work_Data);
                workData->gameAssets = &transState->gameAssets;
                workData->assetArena = &transState->assetArena;
                workData->assetID = assetID;
                workData->workSlot = workSlot;

                switch(assetID) {
                    case GAI_Tree: {
                        workData->fileName = "tree2_teal.bmp";
                    } break;

                    case GAI_Particle: {
                        workData->fileName = "white_particle.bmp";
                    } break;

                    case GAI_Golem: {
                        workData->fileName = "golem.bmp";
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
            } else {
                return result;
            }
        } else {
            return result;
        }
    } else {
        return result;
    }
}


extern "C"
GAME_MAIN(GameMain)
{
    if (!gameState->is_init) {

        gameState->particleRandomSeries = Seed(254);

        init_arena(&gameState->worldArena,
                  gameMemory->permanent_memory_size - sizeof(GameState),
                  (u8 *)gameMemory->permanent_memory + sizeof(GameState));
        gameState->world = push_struct(&gameState->worldArena, World);
        World *world = gameState->world;
        world->chunkDim = {17.0f, 9.0f, 3.0f};
        Memory_Arena *world_arena = &gameState->worldArena;
        ChunkHashmap *chunk_hashmap = &gameState->world->chunkHashmap;


        gameState->player = push_entity(world_arena, chunk_hashmap, eEntity_Player, Position{});

        push_entity(world_arena, chunk_hashmap, eEntity_Familiar, {0, 0, 0, v3{3.0f, 0.0f, 0.0f}});


#if 0
        push_entity(worldArena, chunkHashmap, EntityType_Golem, {0, 0, 0, v3{3.0f, 3.0f, 0.0f}});
#endif

#if 1
        Chunk *chunk = get_chunk(&gameState->worldArena, chunk_hashmap, gameState->player->pos);
        v3 dim = {1.0f, 1.0f, 1.0f};

        for (s32 X = -8; X <= 8; ++X) {
            for (s32 Y = -4; Y <= 4; ++Y) {
                Position tile_pos = {chunk->chunkX, chunk->chunkY, chunk->chunkZ};
                tile_pos.offset.x += dim.x * X;
                tile_pos.offset.y += dim.y * Y;
                recalc_pos(&tile_pos, gameState->world->chunkDim);
                Entity *tile1 = push_entity(world_arena, chunk_hashmap,
                                           eEntity_Tile, tile_pos);
#if 1
                Entity *tile2 = push_entity(world_arena, chunk_hashmap,
                                           eEntity_Tile, tile_pos);
                tile2->pos.chunkZ--;
#endif
                if (X == -8 || X == 8 || Y == -4 || Y == 4) {
                    if (X != 0 && Y != 0) {
                    // if (X == -8 && Y == -4) {
                        Position pos1 = {chunk->chunkX, chunk->chunkY, chunk->chunkZ};
                        Position pos2 = {chunk->chunkX, chunk->chunkY, chunk->chunkZ - 1};
                        pos1.offset.x += dim.x * X;
                        pos1.offset.y += dim.y * Y;
                        pos2.offset.x += dim.x * X;
                        pos2.offset.y += dim.y * Y;
                        recalc_pos(&pos1, gameState->world->chunkDim);
                        recalc_pos(&pos2, gameState->world->chunkDim);
                        Entity *tree1 = push_entity(world_arena, chunk_hashmap, eEntity_Tree, pos1);
                        Entity *tree2 = push_entity(world_arena, chunk_hashmap, eEntity_Tree, pos2);
                        tree1->dim = dim;
                        tree2->dim = dim;
                    }
                }
            }
        }
#endif

#ifdef __DEBUG
        // debug memory.
        init_arena(&gameState->debug_arena, gameMemory->debug_memory_size, (u8 *)gameMemory->debug_memory);
        init_debug(&g_debug_log, &gameState->debug_arena);
#endif

        gameState->is_init = true;
    }

    TIMED_BLOCK();


    ///////////////////////////////////////////////////////////////////////////
    //
    // Init Transient Memory
    //
    void *transMem = gameMemory->transient_memory;
    u64 transMemCap = gameMemory->transient_memory_size;
    Assert(sizeof(TransientState) < transMemCap);
    TransientState *transState = (TransientState *)transMem;

    if (!transState->isInit)
    {

        // transient arena.
        init_arena(&transState->transientArena,
                MB(100),
                (u8 *)transMem + sizeof(TransientState));
        
        // reserved memory for multi-thread work data.
        for (u32 idx = 0;
             idx < ArrayCount(transState->workArena);
             ++idx) {
            WorkMemory_Arena *workSlot = transState->workArena + idx;
            init_sub_arena(&workSlot->memoryArena, &transState->transientArena, MB(4));
        }
        
        // asset arena.
        init_arena(&transState->assetArena,
                MB(20),
                (u8 *)transMem + sizeof(TransientState) + transState->transientArena.size);

        transState->highPriorityQueue = gameMemory->highPriorityQueue;
        transState->lowPriorityQueue = gameMemory->lowPriorityQueue;
        // TODO: Ain't thrilled about it.
        transState->gameAssets.debug_platform_read_file = gameMemory->platform.debug_platform_read_file;

        transState->gameAssets.playerBmp[0] = load_bmp(&gameState->worldArena, gameMemory->platform.debug_platform_read_file, "hero_red_right.bmp");
        transState->gameAssets.playerBmp[1] = load_bmp(&gameState->worldArena, gameMemory->platform.debug_platform_read_file, "hero_red_left.bmp");
        transState->gameAssets.familiarBmp[0] = load_bmp(&gameState->worldArena, gameMemory->platform.debug_platform_read_file, "hero_blue_right.bmp");
        transState->gameAssets.familiarBmp[1] = load_bmp(&gameState->worldArena, gameMemory->platform.debug_platform_read_file, "hero_blue_left.bmp");

        load_font(&gameState->worldArena, gameMemory->platform.debug_platform_read_file, &transState->gameAssets);

        transState->isInit = true;  
   }

    Bitmap draw_buffer   = {};
    draw_buffer.width    = gameScreenBuffer->width;
    draw_buffer.height   = gameScreenBuffer->height;
    draw_buffer.pitch    = gameScreenBuffer->pitch;
    draw_buffer.memory   = gameScreenBuffer->memory;

    r32 aspect_ratio = safe_ratio((r32)draw_buffer.width,
                                  (r32)draw_buffer.height);

    TemporaryMemory renderMemory = BeginTemporaryMemory(&transState->transientArena);



    Render_Group *render_group = alloc_render_group(&transState->transientArena,
                                                    false,
                                                    aspect_ratio);

    v3 chunkDim = gameState->world->chunkDim;
    r32 dt = gameInput->dt_per_frame;

    //
    // Process Input
    //
    Entity *player = gameState->player;
    player->accel = {};
    if (gameInput->move_up.is_set) { player->accel.y = 1.0f; }
    if (gameInput->move_down.is_set) { player->accel.y = -1.0f; }
    if (gameInput->move_left.is_set) { player->accel.x = -1.0f; }
    if (gameInput->move_right.is_set) { player->accel.x = 1.0f; }
    if (player->accel.x != 0.0f && player->accel.y != 0.0f) {
        player->accel *= 0.707106781187f;
    }

#ifdef __DEBUG
    if (gameInput->toggle_debug.is_set) {
        if (gameState->debug_toggle_delay == 0.0f) {
            gameState->debug_toggle_delay = 0.1f;
            if (gameState->debug_mode) {
                gameState->debug_mode = false;
            } else {
                gameState->debug_mode = true;
            }
        }
    }
    gameState->debug_toggle_delay -= dt;
    if (gameState->debug_toggle_delay < 0.0f) {
        gameState->debug_toggle_delay = 0.0f;
    }
#endif



    Position camPos = Position{};
    v3 camDim = {100.0f, 50.0f, 5.0f};
    Position minPos = camPos;
    Position maxPos = camPos;
    minPos.offset -= 0.5f * camDim;
    maxPos.offset += 0.5f * camDim;
    recalc_pos(&minPos, chunkDim);
    recalc_pos(&maxPos, chunkDim);

    //
    // Update entities
    //
    update_entities(gameState, dt, minPos, maxPos);


    //
    // Render entities
    //
    gameState->time += 0.01f;

#if __DEBUG
    if (gameInput->alt.is_set) {
        Mouse_Input *mouse = &gameInput->mouse;
        r32 c = 2.0f;
        if (mouse->is_down[eMouse_Left]) {
            if (mouse->toggle[eMouse_Left]) {
                g_debug_cam_last_mouse_p = mouse->P;
            }
            g_debug_cam_orbital_yaw     += (g_debug_cam_last_mouse_p.x - mouse->P.x) * c;
            g_debug_cam_orbital_pitch   -= (g_debug_cam_last_mouse_p.y - mouse->P.y) * c;
            g_debug_cam_last_mouse_p    = mouse->P;
        }

        if (mouse->wheel_delta) {
            c = 0.5f;
            g_debug_cam_z -= c * mouse->wheel_delta;
        }
    }

#endif

    //
    // @draw
    //
    Game_Assets *gameAssets = &transState->gameAssets;

    for (s32 Z = minPos.chunkZ;
         Z <= maxPos.chunkZ;
         ++Z) {
        for (s32 Y = minPos.chunkY;
             Y <= maxPos.chunkY;
             ++Y) {
            for (s32 X = minPos.chunkX;
                 X <= maxPos.chunkX;
                 ++X) {
                Chunk *chunk = get_chunk(&gameState->worldArena,
                                        &gameState->world->chunkHashmap, {X, Y, Z});
                for (Entity *entity = chunk->entities.head;
                     entity != 0;
                     entity = entity->next) 
                {

                    v3 base = v3 {
                        entity->pos.chunkX * gameState->world->chunkDim.x + entity->pos.offset.x,
                            entity->pos.chunkY * gameState->world->chunkDim.y + entity->pos.offset.y,
                            entity->pos.chunkZ * gameState->world->chunkDim.z + entity->pos.offset.z,
                    };

                    const r32 tilt_angle_z = 0.16f * pi32;
                    const r32 tilt_angle_y = 0.01f * pi32;

                    switch (entity->type) {
                        case eEntity_Player: {
                            s32 face = entity->face;
                            v2 bmp_dim = v2{(r32)gameAssets->playerBmp[face]->width, (r32)gameAssets->playerBmp[face]->height};
                            r32 bmp_height_over_width = safe_ratio(bmp_dim.x, bmp_dim.y);
                            r32 card_h = 1.8f;
                            r32 card_w = card_h * bmp_height_over_width;
                            push_quad(render_group,
                                        v3{base.x - card_w * 0.5f, base.y, base.z},
                                        v3{card_w * cos(tilt_angle_y), card_w * sin(tilt_angle_y), 0.0f},
                                        v3{0.0f, card_h, sin(tilt_angle_z) * card_h},
                                        gameAssets->playerBmp[face]);

#if 0
                            ///////////////////////////////////////////////////////
                            //
                            // Particle System Demo
                            //
                            v2 O = v2{cen.x, cen.y + 0.5f * bmpDim.y};
                            v2 particleDim = ppm * v2{0.5f, 0.5f}; 
                            r32 restitutionC = 0.8f;
                            r32 gridSide = 0.2f;
                            r32 gridSideInPixel = gridSide * ppm;
                            v2 gridO = v2{O.x - (GRID_X / 2.0f) * gridSideInPixel, O.y};
                            r32 invMax = 1.0f / 15.0f;


#if 0
                            // Draw debug grids showing density.
                            for (s32 gridY = 0;
                                 gridY < GRID_Y;
                                 ++gridY) {
                                for (s32 gridX = 0;
                                     gridX < GRID_X;
                                     ++gridX) {
                                    PushRect(renderGroup,
                                             v2{gridO.x + (r32)gridX * gridSideInPixel, gridO.y - (r32)(gridY + 1) * gridSideInPixel},
                                             v2{gridO.x + (r32)(gridX + 1) * gridSideInPixel, gridO.y - (r32)gridY * gridSideInPixel},
                                             v4{gameState->particleGrid[gridY][gridX].density * invMax, 0.0f, 0.0f, 1.0f});
                                }
                            }
#endif

                            ZeroStruct(gameState->particleGrid);

                            // Create
                            for (s32 cnt = 0;
                                 cnt < 4; // TODO: This actually has to be particles per frame.
                                 ++cnt) {
                                Particle *particle = gameState->particles + gameState->particleNextIdx++;
                                if (gameState->particleNextIdx >= ArrayCount(gameState->particles)) {
                                    gameState->particleNextIdx = 0; 
                                }

                                particle->P = v3{RandRange(&gameState->particleRandomSeries, -0.2f, 0.2f), 0.0f, 0.0f};
                                particle->V = v3{RandRange(&gameState->particleRandomSeries, -0.5f, 0.5f), RandRange(&gameState->particleRandomSeries, 7.0f, 8.0f), 0.0f};
                                particle->alpha = 0.01f;
                                particle->dAlpha = 1.0f;
                            }

                            // Simulate and Render
                            for (s32 particleIdx = 0;
                                 particleIdx < ArrayCount(gameState->particles);
                                 ++particleIdx) {
                                Particle *particle = gameState->particles + particleIdx; 

                                // Integrate gravity.
                                particle->A = v3{0.0f, -9.81f, 0.0f};
                                particle->V += dt * particle->A;

                                // Iterate particles and add grid info.
                                v2 P = O + v2{particle->P.x * ppm, -particle->P.y * ppm} - gridO;
                                s32 gridX = Clamp(FloorR32ToS32(P.x) / (s32)gridSideInPixel, 0, GRID_X - 1);
                                s32 gridY = Clamp(FloorR32ToS32(-P.y) / (s32)gridSideInPixel, 0, GRID_Y - 1);
                                r32 density = particle->alpha;
                                ParticleCel *cel = &gameState->particleGrid[gridY][gridX];
                                cel->density += density;
                                cel->V = particle->V;
                            }

                            for (u32 gridY = 0;
                                 gridY < GRID_Y - 1;
                                 ++gridY) {
                                for (u32 gridX = 0;
                                     gridX < GRID_X - 1;
                                     ++gridX) {
                                    // Projection
                                    ParticleCel *cel = &gameState->particleGrid[gridY][gridX];
                                    ParticleCel *right   = &gameState->particleGrid[gridY][gridX + 1];
                                    ParticleCel *up      = &gameState->particleGrid[gridY + 1][gridX];

                                    r32 overRelaxation = 1.9f;
                                    r32 div = overRelaxation * (
                                                                -cel->V.x
                                                                -cel->V.y
                                                                +right->V.x
                                                                +up->V.y);
                                    r32 quarterD = div * 0.25f;
                                    cel->V.x   += 0.25f * quarterD;
                                    cel->V.y   += 0.25f * quarterD;
                                    right->V.x -= 0.25f * quarterD;
                                    up->V.y    -= 0.25f * quarterD;
                                }
                            }

                            for (s32 particleIdx = 0;
                                 particleIdx < ArrayCount(gameState->particles);
                                 ++particleIdx) {
                                Particle *particle = gameState->particles + particleIdx;
                                v2 P = O + v2{particle->P.x * ppm, -particle->P.y * ppm} - gridO;
                                s32 gridX = Clamp(FloorR32ToS32(P.x) / (s32)gridSideInPixel, 0, GRID_X - 1);
                                s32 gridY = Clamp(FloorR32ToS32(-P.y) / (s32)gridSideInPixel, 0, GRID_Y - 1);
                                particle->V = gameState->particleGrid[gridY][gridX].V;

                                particle->P += dt * particle->V;
                                if (particle->alpha > 0.9f) {
                                    particle->dAlpha *= -1.0f;
                                }
                                particle->alpha += dt * particle->dAlpha;
                                if (particle->alpha <= 0.0f) {
                                    particle->alpha = 0.01f;
                                }

                                // Bounce
                                if (particle->P.y < -0.0f) {
                                    particle->P.y = 0.0f;
                                    particle->V.y *= -restitutionC;
                                }

                                // Render Particle
                                v2 particleCen = cen + ppm * v2{particle->P.x, -particle->P.y};
                                particleCen.y += bmpDim.y * 0.5f;
                                r32 scale = 0.3f;
                                Bitmap *bitmap = GetBitmap(transState, GAI_Particle, transState->lowPriorityQueue, &gameMemory->platform);
                                if (bitmap) {
                                    push_quad(renderGroup, base,
                                                particleCen - 0.5f * particleDim,
                                                v2{particleDim.x * scale, 0}, v2{0, particleDim.y * scale},
                                                bitmap, v4{1.0f, 1.0f, 1.0f, particle->alpha});
                                }

                            }
#endif

                        } break;

                        case eEntity_Tree: {
                            Bitmap *bitmap = GetBitmap(transState, GAI_Tree, transState->lowPriorityQueue, &gameMemory->platform);
                            if (bitmap) {
                                v2 bmp_dim = v2{(r32)bitmap->width, (r32)bitmap->height};
                                r32 bmp_height_over_width = safe_ratio(bmp_dim.x, bmp_dim.y);
                                r32 card_h = 2.0f;
                                r32 card_w = card_h * bmp_height_over_width;
                                push_quad(render_group,
                                            v3{base.x - card_w * 0.5f, base.y, base.z},
                                            v3{card_w * cos(tilt_angle_y), card_w * sin(tilt_angle_y), 0.0f},
                                            v3{0.0f, card_h, sin(tilt_angle_z) * card_h},
                                            bitmap);
                            }
                        } break;

                        case eEntity_Familiar: {
                            s32 face = entity->face;
                            v2 bmp_dim = v2{(r32)gameAssets->familiarBmp[face]->width, (r32)gameAssets->familiarBmp[face]->height};
                            r32 bmp_height_over_width = safe_ratio(bmp_dim.x, bmp_dim.y);
                            r32 card_h = 1.8f;
                            r32 card_w = card_h * bmp_height_over_width;
                            push_quad(render_group,
                                        v3{base.x - card_w * 0.5f, base.y, base.z},
                                        v3{card_w * cos(tilt_angle_y), card_w * sin(tilt_angle_y), 0.0f},
                                        v3{0.0f, card_h, sin(tilt_angle_z) * card_h},
                                        gameAssets->familiarBmp[face]);
                        } break;

                        case eEntity_Golem: {
                            Bitmap *bitmap = GetBitmap(transState, GAI_Golem, transState->lowPriorityQueue, &gameMemory->platform);
                            if (bitmap) {
                            }
                        } break;

                        case eEntity_Tile: {
                            r32 radius = entity->dim.x * 0.5f;
                            r32 height = entity->dim.z;
                            push_cube(render_group, base, radius, height,
                                      v4{0.2f, 0.3f, 0.1f, 1.0f});
                        } break;

                        INVALID_DEFAULT_CASE
                    }

                }
            }
        }
    }

    render_group_to_output_batch(render_group, &gameMemory->render_batch);

    EndTemporaryMemory(&renderMemory);


    //
    // Debug Overlay
    //
#ifdef __DEBUG
    TemporaryMemory debug_render_memory = BeginTemporaryMemory(&gameState->debug_arena);
    Render_Group *debug_render_group = alloc_render_group(&gameState->debug_arena,
                                                          true,
                                                          aspect_ratio);

    if (gameState->debug_mode) {
        display_debug_info(&g_debug_log, debug_render_group, gameAssets, &gameState->debug_arena);
    }
    end_debug_log(&g_debug_log);

    render_group_to_output_batch(debug_render_group, &gameMemory->render_batch);
    EndTemporaryMemory(&debug_render_memory);
#endif
}
// end of main.


internal void
init_debug(Debug_Log *debug_log, Memory_Arena *arena) {
    // COUNTER must be the last one in the game code translation unit.
    u32 width = __COUNTER__;
    debug_log->record_width = width;
    debug_log->debug_records = push_array(arena, Debug_Record, DEBUG_LOG_FRAME_COUNT * width);
    debug_log->debug_infos = push_array(arena, Debug_Info, width);
    debug_log->next_frame = 0;

    for (u32 idx = 0;
            idx < width;
            ++idx) {
        Debug_Info *info = debug_log->debug_infos + idx;
        info->max_cycles = 0;
        info->min_cycles = UINT64_MAX;
        info->avg_cycles = 0;
    }
}

internal void
display_debug_info(Debug_Log *debug_log, Render_Group *render_group, Game_Assets *game_assets, Memory_Arena *arena) {
    for (u32 record_idx = 0;
            record_idx < debug_log->record_width;
            ++record_idx) {
        Debug_Info *info = g_debug_log.debug_infos + record_idx;
        size_t size = 1024;
        char *buf = push_array(arena, char, size);
        // TODO: remove CRT.
        _snprintf(buf, size,
                  "%20s(%4d): %10I64uavg_cyc",
                  info->function,
                  info->line,
                  info->avg_cycles);
        push_text(render_group, v3{0.0f, 0.0f, 0.0f}, buf, game_assets);

#if 1
        // draw graph.
        r32 x = 800.0f;
        r32 max_height = (r32)game_assets->v_advance;
        r32 width = 2.0f;
        r32 inv_max_cycles = 0.0f;
        if (info->max_cycles != 0.0f) {
            inv_max_cycles = 1.0f / info->max_cycles;
        }

        for (u32 frame = 0;
             frame < DEBUG_LOG_FRAME_COUNT;
             ++frame)
        {
            Debug_Record *record = g_debug_log.debug_records + frame * g_debug_log.record_width + record_idx;
            r32 height = max_height * record->cycles * inv_max_cycles;
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
end_debug_log(Debug_Log *debug_log) {
    Debug_Record *records = (debug_log->debug_records +
                             debug_log->next_frame *
                             debug_log->record_width);
    for (u32 record_idx = 0;
         record_idx < debug_log->record_width;
         ++record_idx) {
        Debug_Record *record = records + record_idx;
        atomic_exchange_u32(&record->hit, 0);
    }

    if (++debug_log->next_frame == DEBUG_LOG_FRAME_COUNT) {
        debug_log->next_frame = 0;
    }

    // TODO: remove this mf.
    cen_y = 100.0f;
}
