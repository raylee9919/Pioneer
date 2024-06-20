/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Sung Woo Lee $
   $Notice: (C) Copyright %s by Sung Woo Lee. All Rights Reserved. $
   ======================================================================== */

#define Max(a, b) ( (a > b) ? a : b )
#define Min(a, b) ( (a < b) ? a : b )
#define array_count(array) ( sizeof(array) / sizeof(array[0]) )
#define zero_struct(Struct) clear_to_zero(sizeof(Struct), Struct)

#include "intrinsics.h"
#include "math.h"

#include "platform.h"

#include "asset_model.h"
#include "asset.h"
#include "random.h"


struct Camera;

internal bool
str_equal(char *A, char *B)
{
    while (*A && *B && (*A == *B))
    {
        ++A;
        ++B;
    }
    
    b32 result = ((*A == 0) && (*B == 0));
    return result;
}

internal void
clear_to_zero(size_t size, void *data) 
{
    u8 *at = (u8 *)data;
    while (size--) 
    {
        *at++ = 0;
    }
}

struct Memory_Arena 
{
    size_t  size;
    size_t  used;
    void    *base;

    u32     tempCount;
};

struct Temporary_Memory 
{
    Memory_Arena *memoryArena;
    // simply stores what was the used amount before.
    // restoring without any precedence issues problem in multi-threading.
    size_t used;
};

struct WorkMemory_Arena 
{
    b32                 isUsed;
    Memory_Arena        memoryArena;
    Temporary_Memory    flush;
};

struct Chunk_Position 
{
    union
    {
        struct 
        {
            s32 x;
            s32 y;
            s32 z;
        };
        v3i xyz;
    };
    v3 offset;
};

enum Entity_Type 
{
    eEntity_XBot,
    eEntity_Tile,
};
enum Entity_Flag 
{
    eEntity_Flag_Collides = 1
};
struct Entity 
{
    Entity_Type         type;
    Chunk_Position      chunk_pos;

    v3                  world_translation;
    qt                  world_rotation;
    v3                  world_scaling;

#if 1
    v3                  dim;
    v3                  velocity;
    v3                  accel;
    f32                 u;
    u32                 flags;
#endif

    Asset_Animation     *cur_anim;
    f32                 anim_dt;

    Entity              *next;
};

struct EntityList 
{
    Entity  *head;
};

struct Chunk 
{
    s32         x;
    s32         y;
    s32         z;
    EntityList  entities;
    Chunk       *next;
};

struct Chunk_List 
{
    Chunk   *head;
    u32     count;
};

struct Chunk_Hashmap 
{
    Chunk_List   chunks[4096];
};

struct World 
{
    Chunk_Hashmap   chunkHashmap;
    v3              chunk_dim;

    Entity          entities[1024];
    u32             entityCount;
};


struct Kerning 
{
    u32         first;
    u32         second;
    s32         value;
    Kerning     *prev;
    Kerning     *next;
};
struct Kerning_List 
{
    Kerning     *first;
    Kerning     *last;
    u32         count;
};
struct Kerning_Hashmap 
{
    Kerning_List entries[1024];
};
inline u32
kerning_hash(Kerning_Hashmap *hashmap, u32 first, u32 second) 
{
    // todo: better hash function.
    u32 result = (first * 12 + second * 33) % array_count(hashmap->entries);
    return result;
}
internal void
push_kerning(Kerning_Hashmap *hashmap, Kerning *kern, u32 entry_idx) 
{
    Assert(entry_idx < array_count(hashmap->entries));
    Kerning_List *list = hashmap->entries + entry_idx;
    if (list->first) 
    {
        list->last->next = kern;
        kern->prev = list->last;
        kern->next = 0;
        list->last = kern;
        ++list->count;
    } 
    else 
    {
        list->first = kern;
        list->last = kern;
        kern->prev = 0;
        kern->next = 0;
        ++list->count;
    }
}
internal s32
get_kerning(Kerning_Hashmap *hashmap, u32 first, u32 second) 
{
    s32 result = 0;
    u32 entry_idx = kerning_hash(hashmap, first, second);
    Assert(entry_idx < array_count(hashmap->entries));
    for (Kerning *at = hashmap->entries[entry_idx].first;
            at;
            at = at->next) 
    {
        if (at->first == first && at->second == second) 
        {
            result = at->value;
        }
    }
    return result;
}

struct Game_Assets 
{
    Asset_State         bitmapStates[GAI_Count];
    Bitmap              *bitmaps[GAI_Count];

    u32                 v_advance;
    Kerning_Hashmap     kern_hashmap;
    Asset_Glyph         *glyphs[256];

    Asset_Model         *xbot_model;
    Asset_Model         *cube_model;
    Asset_Model         *octahedral_model;
    Asset_Model         *grass_model;

    Read_Entire_File    *read_entire_file;
};

struct Load_Asset_Work_Data 
{
    Game_Assets         *game_assets;
    Memory_Arena        *assetArena;
    Asset_ID            assetID;
    const char          *fileName;
    WorkMemory_Arena    *workSlot;
};

struct Game_State 
{
    b32                 init;
    f32                 time;

    Random_Series       random_series;

    Entity              *player;
    Camera              *debug_cam;

    World               *world;
    Memory_Arena        world_arena;
};

struct Transient_State 
{
    b32                     init;
    Memory_Arena            transient_arena;
    Platform_Work_Queue     *high_priority_queue;
    Platform_Work_Queue     *low_priority_queue;

    WorkMemory_Arena        workArena[4];

    Memory_Arena            assetArena;
    Game_Assets             game_assets;
};


#define GAME_MAIN(name) void name(Game_Memory *game_memory,                 \
                                  Game_State *game_state,                   \
                                  Game_Input *game_input,                   \
                                  Game_Screen_Buffer *game_screen_buffer)
typedef GAME_MAIN(Game_Main);


