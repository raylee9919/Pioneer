/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Sung Woo Lee $
   $Notice: (C) Copyright %s by Sung Woo Lee. All Rights Reserved. $
   ======================================================================== */

inline u32
chunk_hash(Chunk_Hashmap *chunkHashmap, Chunk_Position pos)
{
    TIMED_FUNCTION();
    // TODO: Better hash function.
    u32 bucket = ((pos.x * 16 + pos.y * 9 + pos.z * 4) &
                  (array_count(chunkHashmap->chunks) - 1));
    Assert(bucket < sizeof(chunkHashmap->chunks));

    return bucket;
}

inline b32
is_same_chunk(Chunk_Position A, Chunk_Position B)
{
    TIMED_FUNCTION();
    b32 result = (A.x == B.x &&
                  A.y == B.y &&
                  A.z == B.z);
    return result;
}

inline b32
is_same_chunk(Chunk *chunk, Chunk_Position pos)
{
    TIMED_FUNCTION();
    b32 result = (chunk->x == pos.x &&
                  chunk->y == pos.y &&
                  chunk->z == pos.z);
    return result;
}

internal Chunk *
get_chunk(Memory_Arena *arena, Chunk_Hashmap *hashmap, Chunk_Position pos) 
{
    TIMED_FUNCTION();
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
    TIMED_FUNCTION();
    entity->flags |= flag;
}

inline b32
is_set(Entity *entity, Entity_Flag flag) 
{
    TIMED_FUNCTION();
    b32 result = (entity->flags & flag);
    return result;
}

internal Entity *
push_entity(Memory_Arena *arena, Chunk_Hashmap *hashmap,
            Entity_Type type, Chunk_Position chunk_pos, v3 chunk_dim) 
{
    TIMED_FUNCTION();
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
        case Entity_Type::XBOT: 
        {
            entity->u                   = 200.0f;
        } break;

        case Entity_Type::TILE: 
        {
            entity->world_translation.y -= 0.25f;
            entity->world_scaling       = _v3_(0.48f, 0.25f, 0.48f);
        } break;

        case Entity_Type::LIGHT:
        {
            entity->world_scaling       = _v3_(0.5f, 0.25f, 0.5f);
        } break;

        INVALID_DEFAULT_CASE;
    }

    Chunk *chunk = get_chunk(arena, hashmap, chunk_pos);
    Entity_List *entities = &chunk->entities;
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
    TIMED_FUNCTION();
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
    TIMED_FUNCTION();
    Chunk *oldChunk = get_chunk(arena, hashmap, oldPos);
    Chunk *newChunk = get_chunk(arena, hashmap, newPos);
    Entity_List *oldEntities = &oldChunk->entities;
    Entity_List *newEntities = &newChunk->entities;

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
    TIMED_FUNCTION();
    v3 diff = {};
    diff.x = (f32)(A.x - B.x) * chunk_dim.x;
    diff.y = (f32)(A.y - B.y) * chunk_dim.y;
    diff.z = (f32)(A.z - B.z) * chunk_dim.z;
    diff += (A.offset - B.offset);

    return diff;
}

internal void
update_entity_position(Game_State *game_state, Entity *self, f32 dt)
{
    TIMED_FUNCTION();
    Chunk_Position old_chunk_pos = self->chunk_pos;
    Chunk_Position new_chunk_pos = self->chunk_pos;

    self->accel             -= dt * 400.0f * self->velocity;
    self->velocity          += dt * self->accel;
    self->world_translation += dt * self->velocity;
    self->accel             = v3{};

    new_chunk_pos.offset    += dt * self->velocity;
    recalc_pos(&new_chunk_pos, game_state->world->chunk_dim);
   
    self->chunk_pos = new_chunk_pos;
    if (!is_same_chunk(old_chunk_pos, new_chunk_pos)) 
    {
        MapEntityToChunk(&game_state->world_arena,
                         &game_state->world->chunkHashmap,
                         self, old_chunk_pos, new_chunk_pos);
    }
}

// @TODO: this MUST be revamped.
internal void
update_entities(Game_State *game_state, f32 dt,
                Chunk_Position sim_min, Chunk_Position sim_max) 
{
    TIMED_FUNCTION();
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
                        case Entity_Type::XBOT: 
                        {
                            update_entity_position(game_state, entity, dt);
                        } break;

                        case Entity_Type::TILE: 
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

                        case Entity_Type::LIGHT:
                        {
                            update_entity_position(game_state, entity, dt);
                        } break;

                        INVALID_DEFAULT_CASE
                    }
                }
            }
        }
    }
}
