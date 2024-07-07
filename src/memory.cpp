/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Sung Woo Lee $
   $Notice: (C) Copyright %s by Sung Woo Lee. All Rights Reserved. $
   ======================================================================== */

inline b32
arena_has_room_for(Memory_Arena *arena, size_t size)
{
    b32 result = ((arena->used + size) <= arena->size);
    return result;
}

internal void *
push_size_(Memory_Arena *arena, size_t size)
{
    Assert((arena->used + size) <= arena->size);
    void *result = (u8 *)arena->base + arena->used;
    arena->used += size;

    return result;
}
#define push_size(arena, size)              push_size_(arena, size)
#define push_struct(arena, type)            (type *)push_size_(arena, sizeof(type))
#define push_array(arena, type, count)      (type *)push_size_(arena, count * sizeof(type))
#define push_copy(arena, src, size)         copy(push_size(arena, size), src, size)

internal void
init_arena(Memory_Arena *arena, size_t size, void *base)
{
    arena->size = size;
    arena->base = base;
    arena->used = 0;
}

inline size_t
get_arena_size_remaining(Memory_Arena *arena)
{
    size_t result = arena->size - arena->used;
    return result;
}

internal void
init_sub_arena(Memory_Arena *sub_arena, Memory_Arena *mom_arena, size_t size)
{
    Assert(mom_arena->size >= mom_arena->used + size);
    init_arena(sub_arena, size, (u8 *)mom_arena->base + mom_arena->used);
    mom_arena->used += size;
}

internal Temporary_Memory
begin_temporary_memory(Memory_Arena *memoryArena)
{
    memoryArena->tempCount++;

    Temporary_Memory result = {};
    result.memoryArena = memoryArena;
    result.used = memoryArena->used;

    return result;
}

internal void
end_temporary_memory(Temporary_Memory *temporaryMemory) 
{
    Memory_Arena *arena = temporaryMemory->memoryArena;
    Assert(arena->used >= temporaryMemory->used);
    arena->used = temporaryMemory->used;
    Assert(arena->tempCount > 0);
    arena->tempCount--;
}

internal Work_Memory_Arena *
begin_work_memory(Transient_State *transState)
{
    Work_Memory_Arena *result = 0;
    for (s32 idx = 0;
         idx < array_count(transState->work_arenas);
         ++idx)
    {
        Work_Memory_Arena *work_slot = transState->work_arenas + idx;
        if (!work_slot->is_used) 
        {
            result = work_slot;
            result->is_used = true;
            result->flush = begin_temporary_memory(&result->arena);
            break;
        }
    }
    return result;
}

internal void
end_work_memory(Work_Memory_Arena *workMemory_Arena)
{
    end_temporary_memory(&workMemory_Arena->flush);
    __WRITE_BARRIER__
    workMemory_Arena->is_used = false;
}

internal void *
copy(void *dst, void *src, size_t size)
{
    if (size)
    {
        u8 *dst_at = (u8 *)dst;
        u8 *src_at = (u8 *)src;
        for (size_t i = 0; i < size; ++i)
        {
            *dst_at++ = *src_at++;
        }
    }

    return dst;
}
