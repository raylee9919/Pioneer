#ifndef GAME_H
#define GAME_H
 /* ―――――――――――――――――――――――――――――――――――◆――――――――――――――――――――――――――――――――――――
    $File: $
    $Date: $
    $Revision: $
    $Creator: Sung Woo Lee $
    $Notice: (C) Copyright 2024 by Sung Woo Lee. All Rights Reserved. $
    ―――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――― */


#define Max(a, b) ( (a > b) ? a : b )
#define Min(a, b) ( (a < b) ? a : b )
#define ArrayCount(array) ( sizeof(array) / sizeof(array[0]) )
#define ZeroStruct(Struct) clear_to_zero(sizeof(Struct), Struct)
internal void
clear_to_zero(size_t size, void *data) {
    u8 *at = (u8 *)data;
    while (size--) {
        *at++ = 0;
    }
}

#include "platform.h"
#include "intrinsics.h"
#include "math.h"
#include "asset.h"
#include "random.h"

struct Memory_Arena {
    size_t size;
    size_t used;
    u8 *base;

    u32 tempCount;
};

struct TemporaryMemory {
    Memory_Arena *memoryArena;
    // simply stores what was the used amount before.
    // restoring without any precedence issues problem in multi-threading.
    size_t used;
};

struct WorkMemory_Arena {
    b32 isUsed;
    Memory_Arena memoryArena;
    TemporaryMemory flush;
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

struct Kerning {
    u32 first;
    u32 second;
    s32 value;
    Kerning *prev;
    Kerning *next;
};
struct Kerning_List {
    Kerning *first;
    Kerning *last;
    u32 count;
};
struct Kerning_Hashmap {
    Kerning_List entries[1024];
};
inline u32
kerning_hash(Kerning_Hashmap *hashmap, u32 first, u32 second) {
    // todo: better hash function.
    u32 result = (first * 12 + second * 33) % ArrayCount(hashmap->entries);
    return result;
}
internal void
push_kerning(Kerning_Hashmap *hashmap, Kerning *kern, u32 entry_idx) {
    Assert(entry_idx < ArrayCount(hashmap->entries));
    Kerning_List *list = hashmap->entries + entry_idx;
    if (list->first) {
        list->last->next = kern;
        kern->prev = list->last;
        kern->next = 0;
        list->last = kern;
        ++list->count;
    } else {
        list->first = kern;
        list->last = kern;
        kern->prev = 0;
        kern->next = 0;
        ++list->count;
    }
}
internal s32
get_kerning(Kerning_Hashmap *hashmap, u32 first, u32 second) {
    s32 result = 0;
    u32 entry_idx = kerning_hash(hashmap, first, second);
    Assert(entry_idx < ArrayCount(hashmap->entries));
    for (Kerning *at = hashmap->entries[entry_idx].first;
            at;
            at = at->next) {
        if (at->first == first && at->second == second) {
            result = at->value;
        }
    }
    return result;
}

struct Game_Assets {
    Asset_State bitmapStates[GAI_Count];
    Bitmap *bitmaps[GAI_Count];

    Bitmap *playerBmp[2];
    Bitmap *familiarBmp[2];

    u32 v_advance;
    Kerning_Hashmap kern_hashmap;
    Asset_Glyph *glyphs[256];

    DEBUG_PLATFORM_READ_FILE_ *debug_platform_read_file;
};


struct GameState {
    b32 isInit;
    r32 time;

    RandomSeries particleRandomSeries;

    World *world;
    Memory_Arena worldArena;

    Memory_Arena renderArena;


    Bitmap drawBuffer;

    Camera camera;

    Entity *player;

    Particle particles[512];
    s32 particleNextIdx;

#define GRID_X 30
#define GRID_Y 20

    ParticleCel particleGrid[GRID_Y][GRID_X];

    // debug.
    r32 debug_toggle_delay;
    b32 debug_mode;
    Memory_Arena debug_arena;
};

struct TransientState {
    b32 isInit;
    Memory_Arena transientArena;
    PlatformWorkQueue *highPriorityQueue;
    PlatformWorkQueue *lowPriorityQueue;

    WorkMemory_Arena workArena[4];

    Memory_Arena assetArena;
    Game_Assets gameAssets;
};



#define GAME_MAIN(name) void name(GameMemory *gameMemory, GameState *gameState, \
        GameInput *gameInput, GameScreenBuffer *gameScreenBuffer)
typedef GAME_MAIN(GameMain_);


#endif
