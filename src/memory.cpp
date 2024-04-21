/* ―――――――――――――――――――――――――――――――――――◆――――――――――――――――――――――――――――――――――――
   $File: $
   $Date: $
   $Revision: $
   $Creator: Sung Woo Lee $
   $Notice: (C) Copyright 2024 by Sung Woo Lee. All Rights Reserved. $
   ―――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――― */

internal void *
push_size_(Memory_Arena *arena, size_t size)
{
    Assert((arena->used + size) <= arena->size);
    void *result = arena->base + arena->used;
    arena->used += size;

    return result;
}
#define push_size(arena, size)           push_size_(arena, size)
#define push_struct(arena, type)         (type *)push_size_(arena, sizeof(type))
#define push_array(arena, type, count)   (type *)push_size_(arena, count * sizeof(type))

internal void
init_arena(Memory_Arena *arena, size_t size, u8 *base)
{
    arena->size = size;
    arena->base = base;
    arena->used = 0;
}

internal void
init_sub_arena(Memory_Arena *subArena, Memory_Arena *motherArena, size_t size)
{
    Assert(motherArena->size >= motherArena->used + size);
    init_arena(subArena, size, motherArena->base + motherArena->used);
    motherArena->used += size;
}

internal TemporaryMemory
BeginTemporaryMemory(Memory_Arena *memoryArena)
{
    memoryArena->tempCount++;

    TemporaryMemory result = {};
    result.memoryArena = memoryArena;
    result.used = memoryArena->used;

    return result;
}

internal void
EndTemporaryMemory(TemporaryMemory *temporaryMemory) 
{
    Memory_Arena *arena = temporaryMemory->memoryArena;
    Assert(arena->used >= temporaryMemory->used);
    arena->used = temporaryMemory->used;
    Assert(arena->tempCount > 0);
    arena->tempCount--;
}

internal WorkMemory_Arena *
BeginWorkMemory(TransientState *transState)
{
    WorkMemory_Arena *result = 0;
    for (s32 idx = 0;
         idx < ArrayCount(transState->workArena);
         ++idx)
    {
        WorkMemory_Arena *workSlot = transState->workArena + idx;
        if (!workSlot->isUsed) {
            result = workSlot;
            result->isUsed = true;
            result->flush = BeginTemporaryMemory(&result->memoryArena);
            break;
        }
    }
    return result;
}

internal void
EndWorkMemory(WorkMemory_Arena *workMemory_Arena)
{
    EndTemporaryMemory(&workMemory_Arena->flush);
    __WRITE_BARRIER__
    workMemory_Arena->isUsed = false;
}
