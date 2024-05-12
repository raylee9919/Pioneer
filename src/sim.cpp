/* ―――――――――――――――――――――――――――――――――――◆――――――――――――――――――――――――――――――――――――
   $File: $
   $Date: $
   $Revision: $
   $Creator: Sung Woo Lee $
   $Notice: (C) Copyright 2024 by Sung Woo Lee. All Rights Reserved. $
   ―――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――― */

inline u32
chunk_hash(Chunk_Hashmap *chunkHashmap, Chunk_Position pos)
{
    // TODO: Better hash function.
    u32 bucket = ((pos.x * 16 + pos.y * 9 + pos.z * 4) &
                  (array_count(chunkHashmap->chunks) - 1));
    Assert(bucket < sizeof(chunkHashmap->chunks));

    return bucket;
}

inline b32
is_same_chunk(Chunk_Position A, Chunk_Position B)
{
    b32 result = (A.x == B.x &&
                  A.y == B.y &&
                  A.z == B.z);
    return result;
}

inline b32
is_same_chunk(Chunk *chunk, Chunk_Position pos)
{
    b32 result = (chunk->x == pos.x &&
                  chunk->y == pos.y &&
                  chunk->z == pos.z);
    return result;
}

internal Chunk *
get_chunk(Memory_Arena *arena, Chunk_Hashmap *hashmap, Chunk_Position pos) 
{
    Chunk *result = 0;

    u32 bucket = chunk_hash(hashmap, pos);
    Chunk_List *list = hashmap->chunks + bucket;
    for (Chunk *chunk = list->head;
         chunk != 0;
         chunk = chunk->next) 
    {
        if (is_same_chunk(chunk, pos)) 
        {
            result = chunk;
            break;
        }
    }

    if (!result) 
    {
        result          = push_struct(arena, Chunk);
        result->next    = list->head;
        result->x       = pos.x;
        result->y       = pos.y;
        result->z       = pos.z;
        list->head      = result;
        list->count++;
    }

    return result;
}

inline void
set_flag(Entity *entity, Entity_Flag flag) 
{
    entity->flags |= flag;
}

inline b32
is_set(Entity *entity, Entity_Flag flag) 
{
    b32 result = (entity->flags & flag);
    return result;
}

internal Entity *
push_entity(Memory_Arena *arena, Chunk_Hashmap *hashmap,
            Entity_Type type, Chunk_Position chunk_pos, v3 chunk_dim) 
{
    Entity *entity              = push_struct(arena, Entity);
    entity->type                = type;
    entity->chunk_pos           = chunk_pos;
    entity->world_translation   = _v3_(chunk_pos.x * chunk_dim.x + chunk_pos.offset.x,
                                       chunk_pos.y * chunk_dim.y + chunk_pos.offset.y,
                                       chunk_pos.z * chunk_dim.z + chunk_pos.offset.z);
    entity->world_rotation      = _qt_(1, 0, 0, 0);
    entity->world_scaling       = _v3_(1, 1, 1);

    switch (type) 
    {
        case eEntity_XBot: 
        {
            entity->u                   = 200.0f;
        } break;

        case eEntity_Tile: 
        {
            entity->world_translation.y -= 0.25f;
            entity->world_scaling       = _v3_(0.48f, 0.25f, 0.48f);
        } break;

        case eEntity_Grass: 
        {
            entity->world_scaling       = _v3_(1, 1, 1);
        } break;

        INVALID_DEFAULT_CASE
    }

    Chunk *chunk = get_chunk(arena, hashmap, chunk_pos);
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
recalc_pos(Chunk_Position *pos, v3 chunk_dim) 
{
    f32 boundX = chunk_dim.x * 0.5f;
    f32 boundY = chunk_dim.y * 0.5f;
    f32 boundZ = chunk_dim.z * 0.5f;

    while (pos->offset.x < -boundX ||
           pos->offset.x >= boundX) 
    {
        if (pos->offset.x < -boundX) 
        {
            pos->offset.x += chunk_dim.x;
            --pos->x;
        } 
        else if (pos->offset.x >= boundX) 
        {
            pos->offset.x -= chunk_dim.x;
            ++pos->x;
        }
    }

    while (pos->offset.y < -boundY || pos->offset.y >= boundY) 
    {
        if (pos->offset.y < -boundY) 
        {
            pos->offset.y += chunk_dim.y;
            --pos->y;
        } 
        else if (pos->offset.y >= boundY) 
        {
            pos->offset.y -= chunk_dim.y;
            ++pos->y;
        }
    }

    while (pos->offset.z < -boundZ || pos->offset.z >= boundZ) 
    {
        if (pos->offset.z < -boundZ) 
        {
            pos->offset.z += chunk_dim.z;
            --pos->z;
        } 
        else if (pos->offset.z >= boundZ) 
        {
            pos->offset.z -= chunk_dim.z;
            ++pos->z;
        }
    }
}

internal void
MapEntityToChunk(Memory_Arena *arena, Chunk_Hashmap *hashmap, Entity *entity,
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
        if (E == entity) 
        {
            oldEntities->head = E->next;
            break;
        }
        else if (E->next == entity) 
        { 
            E->next = entity->next;
            break; 
        }
    }

    if (!newEntities->head) 
    {
        newEntities->head = entity;
        entity->next = 0;
    } 
    else 
    {
        entity->next = newEntities->head;
        newEntities->head = entity;
    }
}

internal v3
subtract(Chunk_Position A, Chunk_Position B, v3 chunk_dim) 
{
    v3 diff = {};
    diff.x = (f32)(A.x - B.x) * chunk_dim.x;
    diff.y = (f32)(A.y - B.y) * chunk_dim.y;
    diff.z = (f32)(A.z - B.z) * chunk_dim.z;
    diff += (A.offset - B.offset);

    return diff;
}

internal void
update_entity_pos(Game_State *game_state, Entity *self, f32 dt, Chunk_Position simMin, Chunk_Position simMax)
{
    Chunk_Position old_chunk_pos = self->chunk_pos;
    Chunk_Position new_chunk_pos = self->chunk_pos;

    self->accel             *= dt * self->u;
    self->accel             -= dt * 150.0f * self->velocity;
    self->velocity          += dt * self->accel;

    self->world_translation += dt * self->velocity;

    new_chunk_pos.offset    += dt * self->velocity;
    recalc_pos(&new_chunk_pos, game_state->world->chunk_dim);

    // NOTE: Minkowski Collision
    f32 t_remain = dt;
    f32 eps = 0.001f;
    v3 v_total = self->velocity;
    for (s32 count = 0;
         count < 4 && t_remain > 0.0f;
         ++count) 
    {
        for (s32 Z = simMin.z;
             Z <= simMax.z;
             ++Z) 
        {
            for (s32 Y = simMin.y;
                 Y <= simMax.y;
                 ++Y) 
            {
                for (s32 X = simMin.x;
                     X <= simMax.x;
                     ++X) 
                {
                    Chunk *chunk = get_chunk(&game_state->world_arena,
                                             &game_state->world->chunkHashmap, {X, Y, Z});
                    for (Entity *other = chunk->entities.head;
                            other != 0;
                            other = other->next) 
                    {
                        if (self != other && 
                            is_set(self, eEntity_Flag_Collides) && 
                            is_set(other, eEntity_Flag_Collides)) 
                        {
                            // NOTE: For now, we will test entities on same level.
                            if (self->chunk_pos.z == other->chunk_pos.z) 
                            {
                                v3 boxDim = self->dim + other->dim;
                                Rect3 box = {v3{0, 0, 0}, boxDim};
                                v3 min = -0.5f * boxDim;
                                v3 max = 0.5f * boxDim;
                                v3 oldRelP = subtract(old_chunk_pos, other->chunk_pos, game_state->world->chunk_dim);
                                v3 newRelP = subtract(new_chunk_pos, other->chunk_pos, game_state->world->chunk_dim);
                                f32 t_used = 0.0f;
                                u32 axis = 0;
                                if (in_rect(newRelP, box)) 
                                {
                                    if (v_total.x != 0) 
                                    {
                                        f32 t = (min.x - oldRelP.x) / v_total.x;
                                        if (t >= 0 && t <= t_remain) 
                                        {
                                            t_used = t;
                                            axis = 0;
                                        }
                                        t = (max.x - oldRelP.x) / v_total.x;
                                        if (t >= 0 && t <= t_remain) 
                                        {
                                            t_used = t;
                                            axis = 0;
                                        }
                                    }
                                    if (v_total.y != 0) 
                                    {
                                        f32 t = (min.y - oldRelP.y) / v_total.y;
                                        if (t >= 0 && t <= t_remain) 
                                        {
                                            t_used = t;
                                            axis = 1;
                                        }
                                        t = (max.y - oldRelP.y) / v_total.y;
                                        if (t >= 0 && t <= t_remain) 
                                        {
                                            t_used = t;
                                            axis = 1;
                                        }
                                    }

                                    t_used -= eps;
                                    v3 vUsed = t_used * v_total;
                                    t_remain -= t_used;
                                    v3 vRemain = t_remain * v_total;
                                    if (axis == 0) 
                                    {
                                        vRemain.x *= -1;
                                    } 
                                    else 
                                    {
                                        vRemain.y *= -1;
                                    }
                                    new_chunk_pos = old_chunk_pos;
                                    new_chunk_pos.offset += (vUsed + vRemain);
                                    recalc_pos(&new_chunk_pos, game_state->world->chunk_dim);
                                    self->velocity = vRemain;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
   
    self->chunk_pos = new_chunk_pos;
    if (!is_same_chunk(old_chunk_pos, new_chunk_pos)) 
    {
        MapEntityToChunk(&game_state->world_arena,
                         &game_state->world->chunkHashmap,
                         self, old_chunk_pos, new_chunk_pos);
    }
}

// TODO: this MUST be revamped.
internal void
update_entities(Game_State *game_state, f32 dt,
                Chunk_Position sim_min, Chunk_Position sim_max) 
{
    TIMED_BLOCK();
    for (s32 Z = sim_min.z;
         Z <= sim_max.z;
         ++Z) 
    {
        for (s32 Y = sim_min.y;
             Y <= sim_max.y;
             ++Y) 
        {
            for (s32 X = sim_min.x;
                 X <= sim_max.x;
                 ++X) 
            {
                Chunk *chunk = get_chunk(&game_state->world_arena,
                                         &game_state->world->chunkHashmap,
                                         Chunk_Position{X, Y, Z});
                for (Entity *entity = chunk->entities.head;
                     entity != 0;
                     entity = entity->next) 
                {
                    switch (entity->type) 
                    {
                        case eEntity_XBot: 
                        {
                            update_entity_pos(game_state, entity, dt, sim_min, sim_max);
                        } break;

                        case eEntity_Tile: 
                        {
#if 0
                            f32 theta = acos(entity->world_rotation.w);
                            theta += dt;
                            if (theta > pi32)
                            {
                                theta -= pi32;
                            }
                            entity->world_rotation = _qt_(cos(theta), 0, sin(theta), 0);
#endif
                        } break;

                        case eEntity_Grass: 
                        {
                        } break;

                        INVALID_DEFAULT_CASE
                    }
                }
            }
        }
    }
}
