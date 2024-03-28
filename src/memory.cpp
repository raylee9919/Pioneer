/* ―――――――――――――――――――――――――――――――――――◆――――――――――――――――――――――――――――――――――――
   $File: $
   $Date: $
   $Revision: $
   $Creator: Sung Woo Lee $
   $Notice: (C) Copyright 2024 by Sung Woo Lee. All Rights Reserved. $
   ―――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――― */

internal void *
PushSize_(MemoryArena *arena, size_t size) {
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
InitArena(MemoryArena *arena, size_t size, u8 *base) {
    arena->size = size;
    arena->base = base;
    arena->used = 0;
}

internal void
InitSubArena(MemoryArena *subArena, MemoryArena *motherArena, size_t size) {
    Assert(motherArena->size >= motherArena->used + size);
    InitArena(subArena, size, motherArena->base + motherArena->used);
    motherArena->used += size;
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
    Assert(arena->used >= temporaryMemory->used);
    arena->used = temporaryMemory->used;
    Assert(arena->tempCount > 0);
    arena->tempCount--;
}

inline WorkMemoryArena *
BeginWorkMemory(TransientState *transState) {
    WorkMemoryArena *result = 0;
    for (s32 idx = 0;
            idx < ArrayCount(transState->workArena);
            ++idx) {
        WorkMemoryArena *workSlot = transState->workArena + idx;
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
EndWorkMemory(WorkMemoryArena *workMemoryArena) {
    EndTemporaryMemory(&workMemoryArena->flush);
    __WRITE_BARRIER__
    workMemoryArena->isUsed = false;
}
