/* ―――――――――――――――――――――――――――――――――――◆――――――――――――――――――――――――――――――――――――
   $File: $
   $Date: $
   $Revision: $
   $Creator: Sung Woo Lee $
   $Notice: (C) Copyright 2024 by Sung Woo Lee. All Rights Reserved. $
   ―――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――― */

inline u32
ChunkHash(ChunkHashmap *chunkHashmap, Position pos)
{
    // TODO: Better hash function.
    u32 bucket = (pos.chunkX * 16 + pos.chunkY * 9 + pos.chunkZ * 4) &
        (ArrayCount(chunkHashmap->chunks) - 1);
    Assert(bucket < sizeof(chunkHashmap->chunks));

    return bucket;
}

inline b32
IsSameChunk(Position A, Position B)
{
    b32 result = 
        A.chunkX == B.chunkX &&
        A.chunkY == B.chunkY &&
        A.chunkZ == B.chunkZ;
    return result;
}

inline b32
IsSameChunk(Chunk *chunk, Position pos)
{
    b32 result = 
        chunk->chunkX == pos.chunkX &&
        chunk->chunkY == pos.chunkY &&
        chunk->chunkZ == pos.chunkZ;
    return result;
}

internal Chunk *
get_chunk(Memory_Arena *arena, ChunkHashmap *hashmap, Position pos) 
{
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
        result = push_struct(arena, Chunk);
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
SetFlag(Entity *entity, EntityFlag flag) 
{
    entity->flags |= flag;
}

inline b32
IsSet(Entity *entity, EntityFlag flag) 
{
    b32 result = (entity->flags & flag);
    return result;
}

internal Entity *
push_entity(Memory_Arena *arena, ChunkHashmap *hashmap,
            Entity_Type type, Position pos) 
{
    Entity *entity = push_struct(arena, Entity);
    entity->pos     = pos;
    entity->type    = type;

    switch (type) {
        case eEntity_Player: {
            SetFlag(entity, EntityFlag_Collides);
            entity->dim = {0.7f, 0.5f, 1.0f};
            entity->u = 10.0f;
        } break;

        case eEntity_Familiar: {
            entity->dim = {0.7f, 0.5f, 1.0f};
            entity->u = 5.0f;
        } break;

        case eEntity_Tree: {
            SetFlag(entity, EntityFlag_Collides);
        } break;

        case eEntity_Golem: {
            entity->dim = {2.0f, 1.8f, 1.0f};
            SetFlag(entity, EntityFlag_Collides);
        } break;

        case eEntity_Tile: {
            entity->dim = {0.9f, 0.9f, 0.7f};
        } break;

        INVALID_DEFAULT_CASE
    }

    Chunk *chunk = get_chunk(arena, hashmap, pos);
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
recalc_pos(Position *pos, v3 chunkDim) 
{
    f32 boundX = chunkDim.x * 0.5f;
    f32 boundY = chunkDim.y * 0.5f;
    f32 boundZ = chunkDim.z * 0.5f;

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
MapEntityToChunk(Memory_Arena *arena, ChunkHashmap *hashmap, Entity *entity,
                 Position oldPos, Position newPos) 
{
    Chunk *oldChunk = get_chunk(arena, hashmap, oldPos);
    Chunk *newChunk = get_chunk(arena, hashmap, newPos);
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

internal v3
Subtract(Position A, Position B, v3 chunkDim) 
{
    v3 diff = {};
    diff.x = (f32)(A.chunkX - B.chunkX) * chunkDim.x;
    diff.y = (f32)(A.chunkY - B.chunkY) * chunkDim.y;
    diff.z = (f32)(A.chunkZ - B.chunkZ) * chunkDim.z;
    diff += (A.offset - B.offset);

    return diff;
}

internal void
UpdateEntityPos(GameState *gameState, Entity *self, f32 dt, Position simMin, Position simMax)
{
    Position oldPos = self->pos;
    Position newPos = self->pos;
    self->accel *= self->u;
    self->accel -= 1.5f * self->velocity;
    self->velocity += dt * self->accel;
    newPos.offset += dt * self->velocity;
    recalc_pos(&newPos, gameState->world->chunkDim);

    // NOTE: Minkowski Collision
    f32 tRemain = dt;
    f32 eps = 0.001f;
    v3 vTotal = self->velocity;
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
                    Chunk *chunk = get_chunk(&gameState->worldArena,
                            &gameState->world->chunkHashmap, {X, Y, Z});
                    for (Entity *other = chunk->entities.head;
                            other != 0;
                            other = other->next) {
                        if (self != other && 
                                IsSet(self, EntityFlag_Collides) && 
                                IsSet(other, EntityFlag_Collides) ) {
                            // NOTE: For now, we will test entities on same level.
                            if (self->pos.chunkZ == other->pos.chunkZ) {
                                v3 boxDim = self->dim + other->dim;
                                Rect3 box = {v3{0, 0, 0}, boxDim};
                                v3 min = -0.5f * boxDim;
                                v3 max = 0.5f * boxDim;
                                v3 oldRelP = Subtract(oldPos, other->pos, gameState->world->chunkDim);
                                v3 newRelP = Subtract(newPos, other->pos, gameState->world->chunkDim);
                                f32 tUsed = 0.0f;
                                u32 axis = 0;
                                if (IsPointInRect(newRelP, box)) {
                                    if (vTotal.x != 0) {
                                        f32 t = (min.x - oldRelP.x) / vTotal.x;
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
                                        f32 t = (min.y - oldRelP.y) / vTotal.y;
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

                                    tUsed -= eps;
                                    v3 vUsed = tUsed * vTotal;
                                    tRemain -= tUsed;
                                    v3 vRemain = tRemain * vTotal;
                                    if (axis == 0) {
                                        vRemain.x *= -1;
                                    } else {
                                        vRemain.y *= -1;
                                    }
                                    newPos = oldPos;
                                    newPos.offset += (vUsed + vRemain);
                                    recalc_pos(&newPos, gameState->world->chunkDim);
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
update_entities(GameState *gameState, f32 dt, Position simMin, Position simMax) 
{
    TIMED_BLOCK();
    for (s32 Z = simMin.chunkZ; Z <= simMax.chunkZ; ++Z) {
        for (s32 Y = simMin.chunkY; Y <= simMax.chunkY; ++Y) {
            for (s32 X = simMin.chunkX; X <= simMax.chunkX; ++X) {
                Chunk *chunk = get_chunk(&gameState->worldArena,
                                         &gameState->world->chunkHashmap, {X, Y, Z});
                for (Entity *entity = chunk->entities.head;
                     entity != 0;
                     entity = entity->next) {
                    switch (entity->type) {
                        case eEntity_Player: {
                            f32 epsilon = 0.01f;
                            if (entity->velocity.x > epsilon) {
                                entity->face = 0;
                            }
                            if (entity->velocity.x < -epsilon) {
                                entity->face = 1;
                            }
                            UpdateEntityPos(gameState, entity, dt, simMin, simMax);
                        } break;

                        case eEntity_Familiar: {
                            f32 epsilon = 0.01f;
                            if (entity->velocity.x > epsilon) {
                                entity->face = 0;
                            }
                            if (entity->velocity.x < -epsilon) {
                                entity->face = 1;
                            }
                            v3 V = Subtract(gameState->player->pos, entity->pos, gameState->world->chunkDim);
                            f32 dist = len(V);
                            V *= (1.0f / len(V));
                            entity->accel = (dist > 2.5f) ? V : v3{};
                            UpdateEntityPos(gameState, entity, dt, simMin, simMax);
                        } break;

                        case eEntity_Tree: {
                        } break;

                        case eEntity_Golem: {
                        } break;

                        case eEntity_Tile: {
                        } break;

                        INVALID_DEFAULT_CASE
                    }
                }
            }
        }
    }
}
