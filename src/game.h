#ifndef GAME_H
#define GAME_H
/* ========================================================================
$File: $
$Date: $
$Revision: $
$Creator: Sung Woo Lee $
$Notice: (C) Copyright 2024 by Sung Woo Lee. All Rights Reserved. $
======================================================================== */

#include "math.h"
#include "platform.h"

#define Max(a, b) ( (a > b) ? a : b )
#define Min(a, b) ( (a < b) ? a : b )
#define ArrayCount(array) ( sizeof(array) / sizeof(array[0]) )
#define ClearToZeroStruct(Struct) ClearToZero(sizeof(Struct), Struct)
internal void
ClearToZero(size_t size, void *data) {
    u8 *at = (u8 *)data;
    while (size--) {
        *at++ = 0;
    }
}

#include "random.h"

// BMP
#pragma pack(push, 1)
struct BitmapInfoHeader {
    u16 filetype;
    u32 filesize;
    u16 reserved1;
    u16 reserved2;
    u32 bitmap_offset;
    u32 bitmap_info_header_size;
    s32 width;
    s32 height;
    s16 plane;
    u16 bpp; // bits
    u32 compression;
    u32 image_size;
    u32 horizontal_resolution;
    u32 vertical_resolution;
    u32 plt_entry_cnt;
    u32 important;

    u32 rMask;
    u32 gMask;
    u32 bMask;
};
#pragma pack(pop)

struct Bitmap {
    s32 width;
    s32 height;
    s32 pitch;
    void *memory;
};

// Memory
struct MemoryArena {
    size_t size;
    size_t used;
    u8 *base;

    u32 tempCount;
};

struct TemporaryMemory {
    MemoryArena *memoryArena;
    size_t used;
};


struct Position {
    s32 chunkX;
    s32 chunkY;
    s32 chunkZ;
    vec3 offset;
};

struct Camera {
    Position pos;
};

enum EntityType {
    EntityType_Player,
    EntityType_Familiar,
    EntityType_Tree,
    EntityType_Golem
};

enum EntityFlag {
    EntityFlag_Collides = 1
};

struct Entity {
    EntityType type;
    vec3 dim;
    Position pos;
    vec3 velocity;
    vec3 accel;
    r32 u;
    Bitmap bmp;
    u32 flags;

    u32 face;

    Entity *next;
};

struct EntityList {
    Entity *head;
};

struct Chunk { 
    s32 chunkX;
    s32 chunkY;
    s32 chunkZ;
    EntityList entities;
    Chunk *next;
};

struct ChunkList {
    Chunk *head;
    u32 count;
};

struct ChunkHashmap { 
    ChunkList chunks[4096];
};

struct World {
    r32 ppm;

    ChunkHashmap chunkHashmap;
    vec3 chunkDim;

    Entity entities[1024];
    u32 entityCount;
};

struct Particle {
    vec3 P;
    vec3 V;
    vec3 A;
    r32 alpha;
    r32 dAlpha;
};

struct ParticleCel {
    r32 density;
    vec3 VSum;
    vec3 V;
};

struct GameState {
    b32 isInit;
    r32 time;

    RandomSeries particleRandomSeries;

    World *world;
    MemoryArena worldArena;

    MemoryArena renderArena;

    Bitmap drawBuffer;

    Camera camera;

    Entity *player;
    Bitmap playerBmp[2];
    Bitmap familiarBmp[2];
    Bitmap treeBmp;
    Bitmap particleBmp;
    Bitmap golemBmp;

    Particle particles[256];
    s32 particleNextIdx;
#define GRID_X 30
#define GRID_Y 20
    ParticleCel particleGrid[GRID_Y][GRID_X];
};

struct TransientState {
    b32 isInit;
    MemoryArena transientArena;
    PlatformWorkQueue *renderQueue;
};

internal void *
PushSize_(MemoryArena *arena, size_t size) {
    ASSERT((arena->used + size) <= arena->size);
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

#define GAME_MAIN(name) void name(GameMemory *gameMemory, GameState *gameState, \
        GameInput *gameInput, GameScreenBuffer *gameScreenBuffer)
typedef GAME_MAIN(GameMain_);

global_var debug_cycle_counter *g_DebugCycleCounters;
global_var PlatformAddEntry *platformAddEntry;
global_var PlatformCompleteAllWork *platformCompleteAllWork;

#endif
