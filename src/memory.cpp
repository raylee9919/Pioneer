/* ―――――――――――――――――――――――――――――――――――◆――――――――――――――――――――――――――――――――――――
   $File: $
   $Date: $
   $Revision: $
   $Creator: Sung Woo Lee $
   $Notice: (C) Copyright 2024 by Sung Woo Lee. All Rights Reserved. $
   ―――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――― */

internal void *
PushSize_(Memory_Arena *arena, size_t size) {
    Assert((arena->used + size) <= arena->size);
    void *result = arena->base + arena->used;
    arena->used += size;

    return result;
}
#define PushSize(arena, size) PushSize_(arena, size)
#define PushStruct(arena, type) \
    (type *)PushSize_(arena, sizeof(type))
#define PushArray(arena, type, count) \
    (type *)PushSize_(arena, count * sizeof(type))

internal void
InitArena(Memory_Arena *arena, size_t size, u8 *base) {
    arena->size = size;
    arena->base = base;
    arena->used = 0;
}

internal void
InitSubArena(Memory_Arena *subArena, Memory_Arena *motherArena, size_t size) {
    Assert(motherArena->size >= motherArena->used + size);
    InitArena(subArena, size, motherArena->base + motherArena->used);
    motherArena->used += size;
}

inline TemporaryMemory
BeginTemporaryMemory(Memory_Arena *memoryArena) {
    memoryArena->tempCount++;

    TemporaryMemory result = {};
    result.memoryArena = memoryArena;
    result.used = memoryArena->used;

    return result;
}
inline void
EndTemporaryMemory(TemporaryMemory *temporaryMemory) {
    Memory_Arena *arena = temporaryMemory->memoryArena;
    Assert(arena->used >= temporaryMemory->used);
    arena->used = temporaryMemory->used;
    Assert(arena->tempCount > 0);
    arena->tempCount--;
}

inline WorkMemory_Arena *
BeginWorkMemory(TransientState *transState) {
    WorkMemory_Arena *result = 0;
    for (s32 idx = 0;
            idx < ArrayCount(transState->workArena);
            ++idx) {
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
inline void
EndWorkMemory(WorkMemory_Arena *workMemory_Arena) {
    EndTemporaryMemory(&workMemory_Arena->flush);
    __WRITE_BARRIER__
    workMemory_Arena->isUsed = false;
}
