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

#include "intrinsics.h"
#include "math.h"

#include "platform.h"

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
    v3 offset;
};

enum Entity_Type {
    eEntity_Player,
    eEntity_Familiar,
    eEntity_Tree,
    eEntity_Golem,
    eEntity_Tile
};

enum EntityFlag {
    EntityFlag_Collides = 1
};

struct Entity {
    Entity_Type type;
    v3 dim;
    Position pos;
    v3 velocity;
    v3 accel;
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
    ChunkHashmap chunkHashmap;
    v3 chunkDim;

    Entity entities[1024];
    u32 entityCount;
};

struct Particle {
    v3 P;
    v3 V;
    v3 A;
    r32 alpha;
    r32 dAlpha;
};

struct ParticleCel {
    r32 density;
    v3 VSum;
    v3 V;
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
    b32 is_init;
    r32 time;

    RandomSeries particleRandomSeries;

    World *world;
    Memory_Arena worldArena;

    Memory_Arena renderArena;


    Bitmap drawBuffer;

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
        Game_Input *gameInput, GameScreenBuffer *gameScreenBuffer)
typedef GAME_MAIN(GameMain_);
