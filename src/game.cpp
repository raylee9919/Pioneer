/* ========================================================================
$File: $
$Date: $
$Revision: $
$Creator: Sung Woo Lee $
$Notice: (C) Copyright 2024 by Sung Woo Lee. All Rights Reserved. $
======================================================================== */


#define BYTES_PER_PIXEL 4

#include "types.h"
#include "game.h"
#include "render_group.cpp"

internal Bitmap
LoadBmp(GameMemory *gameMemory, DEBUG_PLATFORM_READ_FILE_ *ReadFile, const char *filename) {
    Bitmap result = {};
    
    DebugReadFileResult readResult = ReadFile(filename);
    if(readResult.content_size != 0) {
        BitmapInfoHeader *header = (BitmapInfoHeader *)readResult.contents;
        uint32 *pixels = (uint32 *)((uint8 *)readResult.contents + header->bitmap_offset);
        result.memory = pixels;
        result.width = header->width;
        result.height = header->height;

        ASSERT(header->compression == 3);

        u32 RedMask = header->rMask;
        u32 GreenMask = header->gMask;
        u32 BlueMask = header->bMask;
        u32 AlphaMask = ~(RedMask | GreenMask | BlueMask);        
        
        BitScanResult RedScan = FindLeastSignificantSetBit(RedMask);
        BitScanResult GreenScan = FindLeastSignificantSetBit(GreenMask);
        BitScanResult BlueScan = FindLeastSignificantSetBit(BlueMask);
        BitScanResult AlphaScan = FindLeastSignificantSetBit(AlphaMask);
        
        ASSERT(RedScan.found);
        ASSERT(GreenScan.found);
        ASSERT(BlueScan.found);
        ASSERT(AlphaScan.found);

        s32 RedShiftDown = (s32)RedScan.index;
        s32 GreenShiftDown = (s32)GreenScan.index;
        s32 BlueShiftDown = (s32)BlueScan.index;
        s32 AlphaShiftDown = (s32)AlphaScan.index;
        
        u32 *SourceDest = pixels;
        for(s32 Y = 0;
            Y < header->height;
            ++Y)
        {
            for(s32 X = 0;
                X < header->width;
                ++X)
            {
                u32 C = *SourceDest;

                r32 R = (r32)((C & RedMask) >> RedShiftDown);
                r32 G = (r32)((C & GreenMask) >> GreenShiftDown);
                r32 B = (r32)((C & BlueMask) >> BlueShiftDown);
                r32 A = (r32)((C & AlphaMask) >> AlphaShiftDown);

#if 0
                r32 AN = (A / 255.0f);
                R = R * AN;
                G = G * AN;
                B = B * AN;
#endif
                
                *SourceDest++ = (((u32)(A + 0.5f) << 24) |
                                 ((u32)(R + 0.5f) << 16) |
                                 ((u32)(G + 0.5f) << 8) |
                                 ((u32)(B + 0.5f) << 0));
            }
        }
    }

    result.pitch = -result.width * BYTES_PER_PIXEL;
    result.memory = (u8 *)result.memory - result.pitch * (result.height - 1);
    
    return result;
}

inline u32
ChunkHash(ChunkHashmap *chunkHashmap, Position pos) {
    // TODO: Better hash function
    u32 bucket = (pos.chunkX * 16 + pos.chunkY * 9 + pos.chunkZ * 4) &
        (ArrayCount(chunkHashmap->chunks) - 1);
    ASSERT(bucket < sizeof(chunkHashmap->chunks));

    return bucket;
}

inline b32
IsSameChunk(Position A, Position B) {
    b32 result = 
        A.chunkX == B.chunkX &&
        A.chunkY == B.chunkY &&
        A.chunkZ == B.chunkZ;
    return result;
}

inline b32
IsSameChunk(Chunk *chunk, Position pos) {
    b32 result = 
        chunk->chunkX == pos.chunkX &&
        chunk->chunkY == pos.chunkY &&
        chunk->chunkZ == pos.chunkZ;
    return result;
}

internal Chunk *
GetChunk(MemoryArena *arena, ChunkHashmap *hashmap, Position pos) {
    Chunk *result = 0;

    u32 bucket = ChunkHash(hashmap, pos);
    ChunkList *list = hashmap->chunks + bucket;
    for (Chunk *chunk = list->head;
            chunk != 0;
            chunk = chunk->next) {
        if (IsSameChunk(chunk, pos)) {
            result = chunk;
            break;
        }
    }

    if (!result) {
        result = PushStruct(arena, Chunk);
        result->next = list->head;
        result->chunkX = pos.chunkX;
        result->chunkY = pos.chunkY;
        result->chunkZ = pos.chunkZ;
        list->head = result;
        list->count++;
    }

    return result;
}

inline void
SetFlag(Entity *entity, EntityFlag flag) {
    entity->flags |= flag;
}

inline b32
IsSet(Entity *entity, EntityFlag flag) {
    b32 result = (entity->flags & flag);
    return result;
}

internal Entity *
PushEntity(MemoryArena *arena, ChunkHashmap *hashmap, EntityType type, Position pos) {
    Entity *entity = PushStruct(arena, Entity);
    *entity = {};
    entity->pos = pos;
    entity->type = type;

    switch (type) {
        case EntityType_Player: {
            SetFlag(entity, EntityFlag_Collides);
            entity->dim = {0.7f, 0.5f, 1.0f};
            entity->u = 10.0f;
        } break;

        case EntityType_Familiar: {
            entity->dim = {0.7f, 0.5f, 1.0f};
            entity->u = 5.0f;
        } break;

        case EntityType_Tree: {
            SetFlag(entity, EntityFlag_Collides);
        } break;

        case EntityType_Golem: {
            entity->dim = {2.0f, 1.8f, 1.0f};
            SetFlag(entity, EntityFlag_Collides);
        } break;

        INVALID_DEFAULT_CASE
    }

    Chunk *chunk = GetChunk(arena, hashmap, pos);
    EntityList *entities = &chunk->entities;
    if (!entities->head) {
        entities->head = entity;
    } else {
        entity->next = entities->head;
        entities->head = entity;
    }

    return entity;
}

internal void
RecalcPos(Position *pos, vec3 chunkDim) {
    r32 boundX = chunkDim.x * 0.5f;
    r32 boundY = chunkDim.y * 0.5f;
    r32 boundZ = chunkDim.z * 0.5f;

    while (pos->offset.x < -boundX || pos->offset.x >= boundX) {
        if (pos->offset.x < -boundX) {
            pos->offset.x += chunkDim.x;
            --pos->chunkX;
        } else if (pos->offset.x >= boundX) {
            pos->offset.x -= chunkDim.x;
            ++pos->chunkX;
        }
    }

    while (pos->offset.y < -boundY || pos->offset.y >= boundY) {
        if (pos->offset.y < -boundY) {
            pos->offset.y += chunkDim.y;
            --pos->chunkY;
        } else if (pos->offset.y >= boundY) {
            pos->offset.y -= chunkDim.y;
            ++pos->chunkY;
        }
    }

    while (pos->offset.z < -boundZ || pos->offset.z >= boundZ) {
        if (pos->offset.z < -boundZ) {
            pos->offset.z += chunkDim.z;
            --pos->chunkZ;
        } else if (pos->offset.z >= boundZ) {
            pos->offset.z -= chunkDim.z;
            ++pos->chunkZ;
        }
    }
}

internal void
MapEntityToChunk(MemoryArena *arena, ChunkHashmap *hashmap, Entity *entity,
        Position oldPos, Position newPos) {
    Chunk *oldChunk = GetChunk(arena, hashmap, oldPos);
    Chunk *newChunk = GetChunk(arena, hashmap, newPos);
    EntityList *oldEntities = &oldChunk->entities;
    EntityList *newEntities = &newChunk->entities;

    for (Entity *E = oldEntities->head;
            E != 0;
            E = E->next) { 
        if (E == entity) { 
            oldEntities->head = E->next;
            break;
        } else if (E->next == entity) { 
            E->next = entity->next;
            break; 
        }
    }

    if (!newEntities->head) {
        newEntities->head = entity;
        entity->next = 0;
    } else {
        entity->next = newEntities->head;
        newEntities->head = entity;
    }
}

internal vec3
Subtract(Position A, Position B, vec3 chunkDim) {
    vec3 diff = {};
    diff.x = (r32)(A.chunkX - B.chunkX) * chunkDim.x;
    diff.y = (r32)(A.chunkY - B.chunkY) * chunkDim.y;
    diff.z = (r32)(A.chunkZ - B.chunkZ) * chunkDim.z;
    diff += (A.offset - B.offset);

    return diff;
}

internal void
UpdateEntityPos(GameState *gameState, Entity *self, r32 dt, Position simMin, Position simMax) {
    Position oldPos = self->pos;
    Position newPos = self->pos;
    self->accel *= self->u;
    self->accel -= 1.5f * self->velocity;
    self->velocity += dt * self->accel;
    newPos.offset += dt * self->velocity;
    RecalcPos(&newPos, gameState->world->chunkDim);

    // NOTE: Minkowski Collision
    r32 tRemain = dt;
    r32 epsilon = 0.001f;
    vec3 vTotal = self->velocity;
    for (s32 count = 0;
            count < 4 && tRemain > 0.0f;
            ++count) {
        for (s32 Z = simMin.chunkZ;
                Z <= simMax.chunkZ;
                ++Z) {
            for (s32 Y = simMin.chunkY;
                    Y <= simMax.chunkY;
                    ++Y) {
                for (s32 X = simMin.chunkX;
                        X <= simMax.chunkX;
                        ++X) {
                    Chunk *chunk = GetChunk(&gameState->worldArena,
                            &gameState->world->chunkHashmap, {X, Y, Z});
                    for (Entity *other = chunk->entities.head;
                            other != 0;
                            other = other->next) {
                        if (self != other && 
                                IsSet(self, EntityFlag_Collides) && 
                                IsSet(other, EntityFlag_Collides) ) {
                            // NOTE: For now, we will test entities on same level.
                            if (self->pos.chunkZ == other->pos.chunkZ) {
                                vec3 boxDim = self->dim + other->dim;
                                Rect3 box = {vec3{0, 0, 0}, boxDim};
                                vec3 min = -0.5f * boxDim;
                                vec3 max = 0.5f * boxDim;
                                vec3 oldRelP = Subtract(oldPos, other->pos, gameState->world->chunkDim);
                                vec3 newRelP = Subtract(newPos, other->pos, gameState->world->chunkDim);
                                r32 tUsed = 0.0f;
                                u32 axis = 0;
                                if (IsPointInRect(newRelP, box)) {
                                    if (vTotal.x != 0) {
                                        r32 t = (min.x - oldRelP.x) / vTotal.x;
                                        if (t >= 0 && t <= tRemain) {
                                            tUsed = t;
                                            axis = 0;
                                        }
                                        t = (max.x - oldRelP.x) / vTotal.x;
                                        if (t >= 0 && t <= tRemain) {
                                            tUsed = t;
                                            axis = 0;
                                        }
                                    }
                                    if (vTotal.y != 0) {
                                        r32 t = (min.y - oldRelP.y) / vTotal.y;
                                        if (t >= 0 && t <= tRemain) {
                                            tUsed = t;
                                            axis = 1;
                                        }
                                        t = (max.y - oldRelP.y) / vTotal.y;
                                        if (t >= 0 && t <= tRemain) {
                                            tUsed = t;
                                            axis = 1;
                                        }
                                    }

                                    tUsed -= epsilon;
                                    vec3 vUsed = tUsed * vTotal;
                                    tRemain -= tUsed;
                                    vec3 vRemain = tRemain * vTotal;
                                    if (axis == 0) {
                                        vRemain.x *= -1;
                                    } else {
                                        vRemain.y *= -1;
                                    }
                                    newPos = oldPos;
                                    newPos.offset += (vUsed + vRemain);
                                    RecalcPos(&newPos, gameState->world->chunkDim);
                                    self->velocity = vRemain;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
   
    self->pos = newPos;
    if (!IsSameChunk(oldPos, newPos)) {
        MapEntityToChunk(&gameState->worldArena,
                &gameState->world->chunkHashmap,
                self, oldPos, newPos);
    }
}

internal void
UpdateEntities(GameState *gameState, r32 dt, Position simMin, Position simMax) {
    for (s32 Z = simMin.chunkZ;
            Z <= simMax.chunkZ;
            ++Z) {
        for (s32 Y = simMin.chunkY;
                Y <= simMax.chunkY;
                ++Y) {
            for (s32 X = simMin.chunkX;
                    X <= simMax.chunkX;
                    ++X) {
                Chunk *chunk = GetChunk(&gameState->worldArena,
                        &gameState->world->chunkHashmap, {X, Y, Z});
                for (Entity *entity = chunk->entities.head;
                        entity != 0;
                        entity = entity->next) {
                    switch (entity->type) {
                        case EntityType_Player: {
                            r32 epsilon = 0.01f;
                            if (entity->velocity.x > epsilon) {
                                entity->face = 0;
                            }
                            if (entity->velocity.x < -epsilon) {
                                entity->face = 1;
                            }
                            UpdateEntityPos(gameState, entity, dt, simMin, simMax);
                        } break;

                        case EntityType_Familiar: {
                            r32 epsilon = 0.01f;
                            if (entity->velocity.x > epsilon) {
                                entity->face = 0;
                            }
                            if (entity->velocity.x < -epsilon) {
                                entity->face = 1;
                            }
                            vec3 V = Subtract(gameState->player->pos, entity->pos, gameState->world->chunkDim);
                            r32 dist = Len(V);
                            V *= (1.0f / Len(V));
                            entity->accel = (dist > 2.5f) ? V : vec3{};
                            UpdateEntityPos(gameState, entity, dt, simMin, simMax);
                        } break;

                        case EntityType_Tree: {
                        } break;

                        case EntityType_Golem: {
                        } break;

                        INVALID_DEFAULT_CASE
                    }
                }
            }
        }
    }
}

inline TemporaryMemory
BeginTemporaryMemory(MemoryArena *memoryArena) {
    memoryArena->tempCount++;

    TemporaryMemory result = {};
    result.memoryArena = memoryArena;
    result.used = memoryArena->used;

    return result;
}

inline void
EndTemporaryMemory(TemporaryMemory *temporaryMemory) {
    MemoryArena *arena = temporaryMemory->memoryArena;
    ASSERT(arena->used >= temporaryMemory->used);
    arena->used = temporaryMemory->used;
    ASSERT(arena->tempCount > 0);
    arena->tempCount--;
}

extern "C"
GAME_MAIN(GameMain) {
    platformAddEntry = gameMemory->platformAddEntry;
    platformCompleteAllWork = gameMemory->platformCompleteAllWork;

    if (!gameState->isInit) {
        gameState->isInit = true;

        gameState->particleRandomSeries = Seed(254);

        InitArena(&gameState->worldArena,
                gameMemory->permanent_memory_capacity - sizeof(GameState),
                (u8 *)gameMemory->permanent_memory + sizeof(GameState));
        gameState->world = PushStruct(&gameState->worldArena, World);
        World *world = gameState->world;
        world->ppm = 50.0f;
        world->chunkDim = {17.0f, 9.0f, 3.0f};
        MemoryArena *worldArena = &gameState->worldArena;
        ChunkHashmap *chunkHashmap = &gameState->world->chunkHashmap;

        gameState->player = PushEntity(worldArena, chunkHashmap, EntityType_Player, {0, 0, 0});
        gameState->playerBmp[0] = LoadBmp(gameMemory, gameMemory->debug_platform_read_file, "hero_red_right.bmp");
        gameState->playerBmp[1] = LoadBmp(gameMemory, gameMemory->debug_platform_read_file, "hero_red_left.bmp");

        PushEntity(worldArena, chunkHashmap, EntityType_Familiar, {0, 0, 0, vec3{3.0f, 0.0f, 0.0f}});
        gameState->familiarBmp[0] = LoadBmp(gameMemory, gameMemory->debug_platform_read_file, "hero_blue_right.bmp");
        gameState->familiarBmp[1] = LoadBmp(gameMemory, gameMemory->debug_platform_read_file, "hero_blue_left.bmp");

        gameState->treeBmp = LoadBmp(gameMemory, gameMemory->debug_platform_read_file, "tree00.bmp");

        gameState->particleBmp = LoadBmp(gameMemory, gameMemory->debug_platform_read_file, "white_particle.bmp");

#if 0
        PushEntity(worldArena, chunkHashmap, EntityType_Golem, {0, 0, 0, vec3{3.0f, 3.0f, 0.0f}});
        gameState->golemBmp = LoadBmp(gameMemory, gameMemory->debug_platform_read_file, "golem.bmp");
#endif

        gameState->camera = {};
        gameState->camera.pos = {0, 0, 0};

#if 1
        Chunk *chunk = GetChunk(&gameState->worldArena, chunkHashmap, gameState->player->pos);
        for (s32 X = -8; X <= 8; ++X) {
            for (s32 Y = -4; Y <= 4; ++Y) {
                if (X == -8 || X == 8 ||
                        Y == -4 || Y == 4) {
                    if (X == 0 || Y == 0) {
                        continue;
                    }
                    vec3 dim = {1.0f, 1.0f, 1.0f};
                    Position pos = {chunk->chunkX, chunk->chunkY, chunk->chunkZ};
                    pos.offset.x += dim.x * X;
                    pos.offset.y += dim.y * Y;
                    RecalcPos(&pos, gameState->world->chunkDim);
                    Entity *tree = PushEntity(&gameState->worldArena, chunkHashmap, EntityType_Tree, pos);
                    tree->dim = dim;
                }
            }
        }
#endif

        for (s32 idx = 0;
                idx < ArrayCount(gameState->particles);
                ++idx) {
            gameState->particles[idx] = {};
        }
        gameState->particleNextIdx = 0;
    }

    g_DebugCycleCounters = gameMemory->debugCycleCounters;

    RDTSC_BEGIN(GameMain);

    //
    // NOTE: Init Transient Memory
    //
    void *transMem = gameMemory->transient_memory;
    u64 transMemCap = gameMemory->transient_memory_capacity;
    ASSERT(sizeof(TransientState) < transMemCap);
    TransientState *transState = (TransientState *)transMem;

    if (!transState->isInit) {
        transState->isInit = true;

        InitArena(&transState->transientArena,
                transMemCap - sizeof(TransientState),
                (u8 *)transMem + sizeof(TransientState));

        transState->renderQueue = gameMemory->highPriorityQueue;
    }

    TemporaryMemory renderMemory = BeginTemporaryMemory(&transState->transientArena);
    RenderGroup *renderGroup = AllocRenderGroup(&transState->transientArena);

    vec3 chunkDim = gameState->world->chunkDim;
    r32 ppm = gameState->world->ppm;
    r32 dt = gameInput->dt_per_frame;

    Entity *player = gameState->player;
    player->accel = {};
    if (gameInput->move_up.is_set) { player->accel.y = 1.0f; }
    if (gameInput->move_down.is_set) { player->accel.y = -1.0f; }
    if (gameInput->move_left.is_set) { player->accel.x = -1.0f; }
    if (gameInput->move_right.is_set) { player->accel.x = 1.0f; }
    if (player->accel.x != 0.0f && player->accel.y != 0.0f) {
        player->accel *= 0.707106781187f;
    }


#if 1
    gameState->camera.pos = player->pos;
#endif
    Position camPos = gameState->camera.pos;
    vec2 camScreenPos = {600.0f, 300.0f};
    vec3 camDim = {100.0f, 50.0f, 0.0f};
    Position minPos = camPos;
    Position maxPos = camPos;
    minPos.offset -= 0.5f * camDim;
    maxPos.offset += 0.5f * camDim;
    RecalcPos(&minPos, chunkDim);
    RecalcPos(&maxPos, chunkDim);

    //
    // Update entities
    //
    UpdateEntities(gameState, dt, minPos, maxPos);

    // 
    // Render entities
    //
    Bitmap drawBuffer = {};
    drawBuffer.width = gameScreenBuffer->width;
    drawBuffer.height = gameScreenBuffer->height;
    drawBuffer.pitch = gameScreenBuffer->pitch;
    drawBuffer.memory = gameScreenBuffer->memory;

    gameState->time += 0.01f;
    r32 angle = gameState->time;

    PushRect(renderGroup, vec2{},
            vec2{(r32)gameScreenBuffer->width, (r32)gameScreenBuffer->height},
            vec4{0.2f, 0.2f, 0.2f, 1.0f});

    for (s32 Y = minPos.chunkY;
            Y <= maxPos.chunkY;
            ++Y) {
        for (s32 X = minPos.chunkX;
                X <= maxPos.chunkX;
                ++X) {
            Chunk *chunk = GetChunk(&gameState->worldArena,
                    &gameState->world->chunkHashmap, {X, Y, 0});
            for (Entity *entity = chunk->entities.head;
                    entity != 0;
                    entity = entity->next) {
                vec3 diff = Subtract(camPos, entity->pos, chunkDim);
                vec2 cen = camScreenPos;
                cen.x -= diff.x * ppm;
                cen.y += diff.y * ppm;
                vec2 dim = ppm * vec2{entity->dim.x, entity->dim.y};
                vec2 min = cen - 0.5f * dim;
                vec2 max = cen + 0.5f * dim;

                switch (entity->type) {
                    case EntityType_Player: {
                        s32 face = entity->face;
                        vec2 bmpDim = vec2{(r32)gameState->playerBmp[face].width, (r32)gameState->playerBmp[face].height};
#if 1
                        PushBmp(renderGroup, cen - 0.5f * bmpDim, vec2{bmpDim.x, 0}, vec2{0, bmpDim.y}, &gameState->playerBmp[face]);
#else
                        //
                        // Rotation Demo
                        //
                        PushBmp(renderGroup,
                                cen - 0.5f * bmpDim,
                                bmpDim.x * vec2{Cos(angle), Sin(angle)},
                                bmpDim.y * vec2{-Sin(angle), Cos(angle)},
                                &gameState->playerBmp[face]);
                        vec2 dotDim {5.0f, 5.0f};
                        vec2 origin = cen - 0.5f * bmpDim;
                        PushRect(renderGroup, origin - 0.5f * dotDim, origin + 0.5f * dotDim, vec4{1.0f, 1.0f, 1.0f, 1.0f});
                        PushRect(renderGroup, origin + vec2{bmpDim.x * Cos(angle), bmpDim.x * Sin(angle)} - 0.5f * dotDim, origin + vec2{bmpDim.x * Cos(angle), bmpDim.x * Sin(angle)} + 0.5f * dotDim, vec4{1.0f, 0.2f, 0.2f, 1.0f});
                        PushRect(renderGroup, origin + vec2{bmpDim.y * -Sin(angle), bmpDim.y * Cos(angle)} - 0.5f * dotDim, origin + vec2{bmpDim.y * -Sin(angle), bmpDim.y * Cos(angle)} + 0.5f * dotDim, vec4{0.2f, 1.0f, 0.2f, 1.0f});
 #endif

                        //
                        // Particle System Demo
                        //
                        vec2 O = vec2{cen.x, cen.y + 0.5f * bmpDim.y};
                        vec2 particleDim = ppm * vec2{0.5f, 0.5f}; 
                        r32 restitutionC = 0.5f;
                        r32 gridSide = 0.5f;
                        r32 gridSideInPixel = gridSide * ppm;
                        vec2 gridO = vec2{O.x - (GRID_X / 2.0f) * gridSideInPixel, O.y};
                        r32 invMax = 1.0f / 40.0f;


#if 1
                        for (s32 gridY = 0;
                                gridY < GRID_Y;
                                ++gridY) {
                            for (s32 gridX = 0;
                                    gridX < GRID_X;
                                    ++gridX) {
                                PushRect(renderGroup,
                                        vec2{gridO.x + (r32)gridX * gridSideInPixel, gridO.y - (r32)(gridY + 1) * gridSideInPixel},
                                        vec2{gridO.x + (r32)(gridX + 1) * gridSideInPixel, gridO.y - (r32)gridY * gridSideInPixel},
                                        vec4{gameState->particleGrid[gridY][gridX].density * invMax, 0.0f, 0.0f, 1.0f});
                            }
                        }
#endif

                        ClearToZeroStruct(gameState->particleGrid);

                        // Create
                        for (s32 cnt = 0;
                                cnt < 2; // TODO: This actually has to be particles per frame.
                                ++cnt) {
                            Particle *particle = gameState->particles + gameState->particleNextIdx++;
                            if (gameState->particleNextIdx >= ArrayCount(gameState->particles)) {
                                gameState->particleNextIdx = 0; 
                            }

                            particle->pos = vec3{RandRange(&gameState->particleRandomSeries, -0.2f, 0.2f), 0.0f, 0.0f};
                            particle->velocity = vec3{RandRange(&gameState->particleRandomSeries, -0.5f, 0.5f), RandRange(&gameState->particleRandomSeries, 7.0f, 8.0f), 0.0f};
                            particle->alpha = 0.1f;
                            particle->dAlpha = 1.0f;
                        }

                        // Simulate and Render
                        for (s32 idx = 0;
                                idx < ArrayCount(gameState->particles);
                                ++idx) {
                            Particle *particle = gameState->particles + idx; 

                            // Simulate
                            particle->accel = vec3{0.0f, -9.81f, 0.0f};
                            particle->velocity += dt * particle->accel;

                            // Euler
                            vec2 P = O + vec2{particle->pos.x * ppm, -particle->pos.y * ppm} - gridO;
                            s32 gridX = Clamp(FloorR32ToS32(P.x) / (s32)gridSideInPixel, 0, GRID_X - 1);
                            s32 gridY = Clamp(FloorR32ToS32(-P.y) / (s32)gridSideInPixel, 0, GRID_Y - 1);
                            r32 density = particle->alpha;
                            gameState->particleGrid[gridY][gridX].density += density;
                            gameState->particleGrid[gridY][gridX].velocity += particle->velocity;
                            gameState->particleGrid[gridY][gridX].pressure += density * particle->velocity;

                            gridX = Clamp(gridX, 1, GRID_X - 2);
                            gridY = Clamp(gridY, 1, GRID_Y - 2);
                            ParticleCel *right   = &gameState->particleGrid[gridY][gridX + 1];
                            ParticleCel *left    = &gameState->particleGrid[gridY][gridX - 1];
                            ParticleCel *up      = &gameState->particleGrid[gridY + 1][gridX];
                            ParticleCel *down    = &gameState->particleGrid[gridY - 1][gridX];
                            r32 overRelaxation = 1.9f;
                            r32 div = overRelaxation * (
                                -particle->velocity.x
                                -particle->velocity.y
                                +right->pressure.x
                                +up->pressure.y);
                            r32 quarterD = div * 0.25f;
                            particle->velocity.x += 0.25f * quarterD;
                            particle->velocity.y += 0.25f * quarterD;
                            right->velocity.x    -= 0.25f * quarterD;
                            up->velocity.y       -= 0.25f * quarterD;

                            particle->pos += dt * particle->velocity;
                            if (particle->alpha > 0.9f) {
                                particle->dAlpha *= -1.0f;
                            }
                            particle->alpha += dt * particle->dAlpha;
                            if (particle->alpha <= 0.0f) {
                                particle->alpha = 0.001f;
                            }


                            // Bounce
                            if (particle->pos.y < -0.0f) {
                                particle->pos.y = 0.0f;
                                particle->velocity.y *= -restitutionC;
                            }

                            
                            // Render Particle
                            vec2 particleCen = cen + ppm * vec2{particle->pos.x, -particle->pos.y};
                            particleCen.y += bmpDim.y * 0.5f;
                            r32 scale = 0.5f;
                            PushBmp(renderGroup,
                                    particleCen - 0.5f * particleDim,
                                    vec2{particleDim.x * scale, 0}, vec2{0, particleDim.y * scale},
                                    &gameState->particleBmp, particle->alpha);
                        }
                        
                    } break;

                    case EntityType_Tree: {
                        vec2 bmpDim = vec2{(r32)gameState->treeBmp.width, (r32)gameState->treeBmp.height};
                        PushBmp(renderGroup, cen - 0.5f * bmpDim, vec2{bmpDim.x, 0}, vec2{0, bmpDim.y}, &gameState->treeBmp);
                    } break;

                    case EntityType_Familiar: {
                        s32 face = entity->face;
                        vec2 bmpDim = vec2{(r32)gameState->familiarBmp[face].width, (r32)gameState->familiarBmp[face].height};
                        PushBmp(renderGroup, cen - 0.5f * bmpDim, vec2{bmpDim.x, 0}, vec2{0, bmpDim.y}, &gameState->familiarBmp[face]);
                    } break;

                    case EntityType_Golem: {
                    } break;

                    INVALID_DEFAULT_CASE
                }

            }
        }
    }
    

    RenderGroupToOutput(renderGroup, &drawBuffer, transState->renderQueue);


    EndTemporaryMemory(&renderMemory);


    RDTSC_END(GameMain);
}
