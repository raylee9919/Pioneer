/* ―――――――――――――――――――――――――――――――――――◆――――――――――――――――――――――――――――――――――――
   $File: $
   $Date: $
   $Revision: $
   $Creator: Sung Woo Lee $
   $Notice: (C) Copyright 2024 by Sung Woo Lee. All Rights Reserved. $
   ―――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――― */

inline u32
ChunkHash(ChunkHashmap *chunkHashmap, Chunk_Position pos)
{
    // TODO: Better hash function.
    u32 bucket = (pos.chunkX * 16 + pos.chunkY * 9 + pos.chunkZ * 4) &
        (array_count(chunkHashmap->chunks) - 1);
    Assert(bucket < sizeof(chunkHashmap->chunks));

    return bucket;
}

inline b32
IsSameChunk(Chunk_Position A, Chunk_Position B)
{
    b32 result = 
        A.chunkX == B.chunkX &&
        A.chunkY == B.chunkY &&
        A.chunkZ == B.chunkZ;
    return result;
}

inline b32
IsSameChunk(Chunk *chunk, Chunk_Position pos)
{
    b32 result = 
        chunk->chunkX == pos.chunkX &&
        chunk->chunkY == pos.chunkY &&
        chunk->chunkZ == pos.chunkZ;
    return result;
}

internal Chunk *
get_chunk(Memory_Arena *arena, ChunkHashmap *hashmap, Chunk_Position pos) 
{
    Chunk *result = 0;

    u32 bucket = ChunkHash(hashmap, pos);
    ChunkList *list = hashmap->chunks + bucket;
    for (Chunk *chunk = list->head;
         chunk != 0;
         chunk = chunk->next) 
    {
        if (IsSameChunk(chunk, pos)) 
        {
            result = chunk;
            break;
        }
    }

    if (!result) 
    {
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
set_flag(Entity *entity, EntityFlag flag) 
{
    entity->flags |= flag;
}

inline b32
is_set(Entity *entity, EntityFlag flag) 
{
    b32 result = (entity->flags & flag);
    return result;
}

internal Entity *
push_entity(Memory_Arena *arena, ChunkHashmap *hashmap,
            Entity_Type type, Chunk_Position pos) 
{
    Entity *entity  = push_struct(arena, Entity);
    entity->type    = type;
    entity->pos     = pos;

    switch (type) 
    {
        case eEntity_XBot: 
        {
            entity->u = 10.0f;
        } break;

        case eEntity_Tile: 
        {
            entity->dim = {0.9f, 0.9f, 0.7f};
        } break;

        INVALID_DEFAULT_CASE
    }

    Chunk *chunk = get_chunk(arena, hashmap, pos);
    EntityList *entities = &chunk->entities;
    if (!entities->head) 
    {
        entities->head = entity;
    } 
    else 
    {
        entity->next = entities->head;
        entities->head = entity;
    }

    return entity;
}

internal void
recalc_pos(Chunk_Position *pos, v3 chunkDim) 
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
                 Chunk_Position oldPos, Chunk_Position newPos) 
{
    Chunk *oldChunk = get_chunk(arena, hashmap, oldPos);
    Chunk *newChunk = get_chunk(arena, hashmap, newPos);
    EntityList *oldEntities = &oldChunk->entities;
    EntityList *newEntities = &newChunk->entities;

    for (Entity *E = oldEntities->head;
         E != 0;
         E = E->next) 
    { 
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
Subtract(Chunk_Position A, Chunk_Position B, v3 chunkDim) 
{
    v3 diff = {};
    diff.x = (f32)(A.chunkX - B.chunkX) * chunkDim.x;
    diff.y = (f32)(A.chunkY - B.chunkY) * chunkDim.y;
    diff.z = (f32)(A.chunkZ - B.chunkZ) * chunkDim.z;
    diff += (A.offset - B.offset);

    return diff;
}

internal void
update_entity_pos(Game_State *game_state, Entity *self, f32 dt, Chunk_Position simMin, Chunk_Position simMax)
{
    Chunk_Position oldPos = self->pos;
    Chunk_Position newPos = self->pos;
    self->accel *= self->u;
    self->accel -= 1.5f * self->velocity;
    self->velocity += dt * self->accel;
    newPos.offset += dt * self->velocity;
    recalc_pos(&newPos, game_state->world->chunkDim);

    // NOTE: Minkowski Collision
    f32 tRemain = dt;
    f32 eps = 0.001f;
    v3 vTotal = self->velocity;
    for (s32 count = 0;
            count < 4 && tRemain > 0.0f;
            ++count) 
    {
        for (s32 Z = simMin.chunkZ;
                Z <= simMax.chunkZ;
                ++Z) 
        {
            for (s32 Y = simMin.chunkY;
                    Y <= simMax.chunkY;
                    ++Y) 
            {
                for (s32 X = simMin.chunkX;
                        X <= simMax.chunkX;
                        ++X) 
                {
                    Chunk *chunk = get_chunk(&game_state->world_arena,
                                             &game_state->world->chunkHashmap, {X, Y, Z});
                    for (Entity *other = chunk->entities.head;
                            other != 0;
                            other = other->next) 
                    {
                        if (self != other && 
                            is_set(self, EntityFlag_Collides) && 
                            is_set(other, EntityFlag_Collides)) 
                        {
                            // NOTE: For now, we will test entities on same level.
                            if (self->pos.chunkZ == other->pos.chunkZ) 
                            {
                                v3 boxDim = self->dim + other->dim;
                                Rect3 box = {v3{0, 0, 0}, boxDim};
                                v3 min = -0.5f * boxDim;
                                v3 max = 0.5f * boxDim;
                                v3 oldRelP = Subtract(oldPos, other->pos, game_state->world->chunkDim);
                                v3 newRelP = Subtract(newPos, other->pos, game_state->world->chunkDim);
                                f32 tUsed = 0.0f;
                                u32 axis = 0;
                                if (in_rect(newRelP, box)) 
                                {
                                    if (vTotal.x != 0) 
                                    {
                                        f32 t = (min.x - oldRelP.x) / vTotal.x;
                                        if (t >= 0 && t <= tRemain) 
                                        {
                                            tUsed = t;
                                            axis = 0;
                                        }
                                        t = (max.x - oldRelP.x) / vTotal.x;
                                        if (t >= 0 && t <= tRemain) 
                                        {
                                            tUsed = t;
                                            axis = 0;
                                        }
                                    }
                                    if (vTotal.y != 0) 
                                    {
                                        f32 t = (min.y - oldRelP.y) / vTotal.y;
                                        if (t >= 0 && t <= tRemain) 
                                        {
                                            tUsed = t;
                                            axis = 1;
                                        }
                                        t = (max.y - oldRelP.y) / vTotal.y;
                                        if (t >= 0 && t <= tRemain) 
                                        {
                                            tUsed = t;
                                            axis = 1;
                                        }
                                    }

                                    tUsed -= eps;
                                    v3 vUsed = tUsed * vTotal;
                                    tRemain -= tUsed;
                                    v3 vRemain = tRemain * vTotal;
                                    if (axis == 0) 
                                    {
                                        vRemain.x *= -1;
                                    } 
                                    else 
                                    {
                                        vRemain.y *= -1;
                                    }
                                    newPos = oldPos;
                                    newPos.offset += (vUsed + vRemain);
                                    recalc_pos(&newPos, game_state->world->chunkDim);
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
    if (!IsSameChunk(oldPos, newPos)) 
    {
        MapEntityToChunk(&game_state->world_arena,
                         &game_state->world->chunkHashmap,
                         self, oldPos, newPos);
    }
}

internal void
update_entities(Game_State *gameState, f32 dt, Chunk_Position simMin, Chunk_Position simMax) 
{
    TIMED_BLOCK();
    for (s32 Z = simMin.chunkZ;
         Z <= simMax.chunkZ;
         ++Z) 
    {
        for (s32 Y = simMin.chunkY;
             Y <= simMax.chunkY;
             ++Y) 
        {
            for (s32 X = simMin.chunkX;
                 X <= simMax.chunkX;
                 ++X) 
            {
                Chunk *chunk = get_chunk(&gameState->world_arena,
                                         &gameState->world->chunkHashmap,
                                         Chunk_Position{X, Y, Z});
                for (Entity *entity = chunk->entities.head;
                     entity != 0;
                     entity = entity->next) 
                {
                    switch (entity->type) 
                    {
                        case eEntity_XBot: 
                        {
                            f32 epsilon = 0.01f;
                            update_entity_pos(gameState, entity, dt, simMin, simMax);
                        }

                        case eEntity_Tile: 
                        {

                        } break;

                        INVALID_DEFAULT_CASE
                    }
                }
            }
        }
    }
}
