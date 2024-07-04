/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Sung Woo Lee $
   $Notice: (C) Copyright 2024 by Sung Woo Lee. All Rights Reserved. $
   ======================================================================== */

enum Debug_Type
{
    eDebug_Type_Frame_Marker,
    eDebug_Type_Begin_Block,
    eDebug_Type_End_Block,

    eDebug_Type_Open_Data_Block,
    eDebug_Type_Close_Data_Block,

    eDebug_Type_f32,
    eDebug_Type_s32,
    eDebug_Type_u32,
    eDebug_Type_b32,
    eDebug_Type_v2,
    eDebug_Type_v3,
    eDebug_Type_v4,
    eDebug_Type_Rect2,
    eDebug_Type_Rect3,

    eDebug_Type_Bitmap,

    //

    eDebug_Type_Counter_Thread_List,
};

struct Debug_Event
{
    u64 clock;
    char *file_name;
    char *block_name;
    u32 line_number;
    u16 thread_id;
    u16 core_idx;
    u8 type;
    union
    {
        void *vec_ptr[2];

        b32 bool32;
        s32 int32;
        u32 uint32;
        f32 float32;
        v2 vector2;
        v3 vector3;
        v4 vector4;

        Rect2 rect2;
        Rect3 rect3;

        Bitmap *bitmap;
    };
};

#define MAX_DEBUG_THREAD_COUNT                  256 
#define MAX_DEBUG_EVENT_ARRAY_COUNT             8
#define MAX_DEBUG_EVENT_COUNT                   (16*65536)
struct Debug_Table
{
    u32             current_event_array_idx;
    u64 volatile    event_array_idx_event_idx;
    u32             event_count[MAX_DEBUG_EVENT_ARRAY_COUNT];
    Debug_Event     events[MAX_DEBUG_EVENT_ARRAY_COUNT][MAX_DEBUG_EVENT_COUNT];
};

#if __INTERNAL
extern Debug_Table *g_debug_table;
#endif

#define DEBUG_FRAME_END(name) Debug_Table *name(Game_Memory *memory, Game_Screen_Buffer *game_screen_buffer, Game_Input *game_input)
typedef DEBUG_FRAME_END(Debug_Frame_End);

#if __INTERNAL
#define record_debug_event(event_type, block) \
    u64 array_idx_event_idx = atomic_add_u64(&g_debug_table->event_array_idx_event_idx, 1); \
    u32 event_idx       = (u32)(array_idx_event_idx & 0xffffffff); \
    Assert(event_idx < MAX_DEBUG_EVENT_COUNT); \
    Debug_Event *event = g_debug_table->events[array_idx_event_idx >> 32] + event_idx; \
    event->clock            = __rdtsc(); \
    event->type             = (u8)event_type; \
    event->core_idx         = 0; \
    event->thread_id        = (u16)get_thread_id(); \
    event->file_name   = __FILE__;\
    event->line_number = __LINE__;\
    event->block_name  = block;\

#define FRAME_MARKER(seconds_elapsed_init)\
{\
    int counter = __COUNTER__;\
    record_debug_event(eDebug_Type_Frame_Marker, "Frame Marker"); \
    event->float32 = seconds_elapsed_init; \
}

#define TIMED_BLOCK__(block_name, number, ...) Timed_Block timed_block_##number(__COUNTER__, __FILE__, __LINE__, block_name, ##__VA_ARGS__)
#define TIMED_BLOCK_(block_name, number, ...) TIMED_BLOCK__(block_name, number, ##__VA_ARGS__)
#define TIMED_BLOCK(block_name, ...) TIMED_BLOCK_(#block_name, __LINE__, ##__VA_ARGS__)
#define TIMED_FUNCTION(...) TIMED_BLOCK_(__FUNCTION__, __LINE__, ##__VA_ARGS__)

#define BEGIN_BLOCK_(counter, file_name_init, line_number_init, block_name_init)\
    { record_debug_event(eDebug_Type_Begin_Block, block_name_init); }

#define END_BLOCK_(counter)\
{ \
    record_debug_event(eDebug_Type_End_Block, "End Block"); \
}

#define BEGIN_BLOCK(name)\
    int counter_##name = __COUNTER__;\
    BEGIN_BLOCK_(counter_##name, __FILE__, __LINE__, #name);\


#define END_BLOCK(name)\
    END_BLOCK_(counter_##name);

struct Timed_Block
{
    int counter;

    Timed_Block(int counter_init, char *file_name, int line_number, char *block_name, int hit_count_init = 1)
    {
        counter = counter_init;
        BEGIN_BLOCK_(counter, file_name, line_number, block_name);
    }

    ~Timed_Block()
    {
        END_BLOCK_(counter);
    }
};
#else
    #define TIMED_BLOCK(...)
    #define TIMED_FUNCTION(...)
    #define BEGIN_BLOCK(...)
    #define END_BLOCK(...)
    #define FRAME_MARKER(...)
#endif

#if defined(__cplusplus) && __INTERNAL

inline void
debug_value_set_event_data(Debug_Event *event, f32 value)
{
    event->type = eDebug_Type_f32;
    event->float32 = value;
}

inline void
debug_value_set_event_data(Debug_Event *event, s32 value)
{
    event->type = eDebug_Type_s32;
    event->int32 = value;
}

inline void
debug_value_set_event_data(Debug_Event *event, u32 value)
{
    event->type = eDebug_Type_u32;
    event->uint32 = value;
}

inline void
debug_value_set_event_data(Debug_Event *event, v2 value)
{
    event->type = eDebug_Type_v2;
    event->vector2 = value;
}

inline void
debug_value_set_event_data(Debug_Event *event, v3 value)
{
    event->type = eDebug_Type_v3;
    event->vector3 = value;
}

inline void
debug_value_set_event_data(Debug_Event *event, v4 value)
{
    event->type = eDebug_Type_v4;
    event->vector4 = value;
}

inline void
debug_value_set_event_data(Debug_Event *event, Rect2 value)
{
    event->type = eDebug_Type_Rect2;
    event->rect2 = value;
}

inline void
debug_value_set_event_data(Debug_Event *event, Rect3 value)
{
    event->type = eDebug_Type_Rect3;
    event->rect3 = value;
}

inline void
debug_value_set_event_data(Debug_Event *event, Bitmap *value)
{
    event->type = eDebug_Type_Bitmap;
    event->bitmap = value;
}

#define DEBUG_BEGIN_DATA_BLOCK(name, ptr0, ptr1) \
{ \
    record_debug_event(eDebug_Type_Open_Data_Block, #name); \
    event->vec_ptr[0] = ptr0; \
    event->vec_ptr[1] = ptr1; \
} \

#define DEBUG_END_DATA_BLOCK() \
{ \
    record_debug_event(eDebug_Type_Close_Data_Block, "End Data Block"); \
} \


#define DEBUG_VALUE(value) \
{ \
    record_debug_event(eDebug_Type_f32, #value); \
    debug_value_set_event_data(event, value); \
} \

#define DEBUG_BEGIN_ARRAY(...)
#define DEBUG_END_ARRAY(...)

#else

#define DEBUG_BEGIN_DATA_BLOCK(...)
#define DEBUG_END_DATA_BLOCK(...)
#define DEBUG_VALUE(...)
#define DEBUG_BEGIN_ARRAY(...)
#define DEBUG_END_ARRAY(...)

#endif

    

