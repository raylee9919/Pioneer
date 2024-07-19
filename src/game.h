/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Sung Woo Lee $
   $Notice: (C) Copyright %s by Sung Woo Lee. All Rights Reserved. $
   ======================================================================== */

/*
 * @TODO:
 * - load animation -> build a hash table to get an index to an animation node
 *   given node id as a key.
 *
 *
 *
 *
 *
 */

#define INTROSPECT(params)

#define maximum(a, b) ( (a > b) ? a : b )
#define minimum(a, b) ( (a < b) ? a : b )
#define array_count(array) ( sizeof(array) / sizeof(array[0]) )


#include "intrinsics.h"
#include "math.h"
#include "model.h"
#include "animation.h"
#include "platform.h"
#include "asset.h"
#include "random.h"

struct Camera;
enum Animation_State;

internal b32
string_equal(char *str1, u32 len1, char *str2, u32 len2)
{
    b32 result = (len1 == len2);

    if (result)
    {
        result = true;
        for (u32 idx = 0;
             idx < len1;
             ++idx)
        {
            if (str1[idx] != str2[idx])
            {
                result = false;
                break;
            }
        }
    }

    return result;
}


#define zero_struct(INSTANCE) zero_size(sizeof(INSTANCE), &(INSTANCE))
#define zero_array(COUNT, POINTER) zero_size(COUNT * sizeof((POINTER)[0]), POINTER)
internal void
zero_size(size_t size, void *data) 
{
    u8 *at = (u8 *)data;
    while (size--) 
    {
        *at++ = 0;
    }
}

inline void *
copy(size_t size, void *src_init, void *dst_init)
{
    u8 *src = (u8 *)src_init;
    u8 *dst = (u8 *)dst_init;
    while (size--) { *dst++ = *src++; }
    return dst_init;
}

#define DLIST_INSERT(sentinel, element) \
    (element)->next = (sentinel)->next; \
    (element)->prev = (sentinel); \
    (element)->next->prev = (element); \
    (element)->prev->next = (element);

#define DLIST_INIT(sentinel) \
    (sentinel)->next = (sentinel); \
    (sentinel)->prev = (sentinel);

#define FREELIST_ALLOC(result, free_list_ptr, allocation_code)\
    result = free_list_ptr;\
    if (result) { free_list_ptr = result->next_free; } else { result = allocation_code; }
#define FREELIST_DEALLOC(ptr, free_list_ptr) \
    if (ptr) {ptr->next_free = free_list_ptr; (free_list_ptr) = ptr;}

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

struct Work_Memory_Arena 
{
    b32                 is_used;
    Memory_Arena        arena;
    Temporary_Memory    flush;
};

struct Chunk_Position 
{
    union {
        struct  {
            s32 x;
            s32 y;
            s32 z;
        };
        v3i xyz;
    };
    v3 offset;
};

struct Animation_Channel
{
    Animation *animation;
    f32 dt;
};

enum Entity_Type 
{
    XBOT,
    TILE,
    LIGHT,
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

    v3                  velocity;
    v3                  accel;
    f32                 u;
    u32                 flags;

    Animation_Channel   animation_channels[1];
    m4x4                *animation_transform;

    Entity              *next;
};

struct Entity_List 
{
    Entity  *head;
};

struct Chunk 
{
    s32             x;
    s32             y;
    s32             z;
    Entity_List     entities;
    Chunk           *next;
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

struct Font
{
    u32                 v_advance;
    f32                 ascent;
    f32                 descent;
    f32                 max_width;
    Kerning_Hashmap     kern_hashmap;
    Asset_Glyph         *glyphs[256];
};

struct Game_Assets 
{
    Asset_State         bitmapStates[GAI_Count];
    Bitmap              *bitmaps[GAI_Count];

    Bitmap              *debug_bitmap;

    Font                debug_font;
    Font                menu_font;

    Model               *xbot_model;
    Model               *cube_model;
    Model               *sphere_model;
    Model               *octahedral_model;

    Model               *grass_model;
    f32                 grass_max_vertex_y;
    Bitmap              *turbulence_map;

    Mesh                *star_mesh;

    Animation           *xbot_idle;
    Animation           *xbot_run;

    Read_Entire_File    *read_entire_file;
};

struct Load_Asset_Work_Data 
{
    Game_Assets         *game_assets;
    Memory_Arena        *assetArena;
    Asset_ID            assetID;
    const char          *fileName;
    Work_Memory_Arena   *workSlot;
};

struct Console_Cursor
{
    v2 offset;
    v2 dim;
    v4 color1;
    v4 color2;
};

// @Temporary
struct Console
{
    #define CONSOLE_TARGET_T 0.2f
    f32         dt;
    v2          half_dim;
    v4          bg_color;

    Font        *font;
    v4          text_color;

    Console_Cursor cursor;

    char        cbuf[64];
    u32         cbuf_at;
    v2          input_baseline_offset;

    b32 initted;
};

enum Game_Mode
{
    GAME,
    CONSOLE,
    MENU,
};
struct Game_State 
{
    b32                 init;
    f32                 time;

    Game_Mode           mode;

    Random_Series       random_series;

    Entity              *player;

    World               *world;
    Memory_Arena        world_arena;

    m4x4                *grass_world_transforms;
    s32                 grass_count;

    m4x4                *star_world_transforms;
    s32                 star_count;

    Camera              *using_camera;
    Camera              *player_camera;
    Camera              *free_camera;
    Camera              *orthographic_camera;

    // @TEMPORARY
    Entity              *light;

    // @TEMPORARY: this is meant to be in dev-engine memory.
    Console             console;
};

struct Transient_State 
{
    b32                     init;
    Memory_Arena            transient_arena;
    Platform_Work_Queue     *high_priority_queue;
    Platform_Work_Queue     *low_priority_queue;

    Work_Memory_Arena       work_arenas[4];

    Memory_Arena            asset_arena;
    Game_Assets             game_assets;
};


#define GAME_UPDATE(name) void name(Game_Memory *game_memory,                 \
                                    Game_State *game_state,                   \
                                    Game_Input *input,                        \
                                    Event_Queue *event_queue,                 \
                                    Game_Screen_Buffer *game_screen_buffer)
typedef GAME_UPDATE(Game_Update);



#include "meta.cpp"
