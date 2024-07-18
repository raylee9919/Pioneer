/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Sung Woo Lee $
   $Notice: (C) Copyright 2024 by Sung Woo Lee. All Rights Reserved. $
   ======================================================================== */

struct Debug_ID
{
    void *value[2];
};

enum Debug_Type
{
    eDebug_Type_Unknown,

    eDebug_Type_Frame_Marker,
    eDebug_Type_Begin_Block,
    eDebug_Type_End_Block,

    eDebug_Type_Open_Data_Block,
    eDebug_Type_Close_Data_Block,

    eDebug_Type_Mark_Debug_Value,

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
    char *guid;
    char *block_name; // @TODO: should we remove block name altogether?
    u16 thread_id;
    u16 core_idx;
    u8 type;
    union {
        Debug_ID debug_id;
        Debug_Event *value_Debug_Event;

        b32 value_b32;
        s32 value_s32;
        u32 value_u32;
        f32 value_f32;
        v2 value_v2;
        v3 value_v3;
        v4 value_v4;

        Rect2 value_Rect2;
        Rect3 value_Rect3;

        Bitmap *value_Bitmap;
    };
};

struct Debug_Table
{
    u32             current_event_array_idx;
    u64 volatile    event_array_idx_event_idx;
    Debug_Event     events[2][16*65536];
};

#if __DEVELOPER
extern Debug_Table *g_debug_table;
#endif

#define DEBUG_FRAME_END(name) Debug_Table *name(Game_Memory *memory, Game_Screen_Buffer *game_screen_buffer)
typedef DEBUG_FRAME_END(Debug_Frame_End);

#define unique_file_counter_string__(a, b, c) a "(" #b ")." #c
#define unique_file_counter_string_(a, b, c) unique_file_counter_string__(a, b, c)
#define unique_file_counter_string() unique_file_counter_string_(__FILE__, __LINE__, __COUNTER__)

#if __DEVELOPER
#define record_debug_event(event_type, block) \
    u64 array_idx_event_idx = atomic_add_u64(&g_debug_table->event_array_idx_event_idx, 1); \
    u32 event_idx       = (u32)(array_idx_event_idx & 0xffffffff); \
    Assert(event_idx < array_count(g_debug_table->events[0])); \
    Debug_Event *event = g_debug_table->events[array_idx_event_idx >> 32] + event_idx; \
    event->clock            = __rdtsc(); \
    event->type             = (u8)event_type; \
    event->core_idx         = 0; \
    event->thread_id        = (u16)get_thread_id();\
    event->guid             = unique_file_counter_string();\
    event->block_name  = block;\

#define FRAME_MARKER(seconds_elapsed_init)\
{\
    int counter = __COUNTER__;\
    record_debug_event(eDebug_Type_Frame_Marker, "Frame Marker"); \
    event->value_f32 = seconds_elapsed_init; \
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

#if defined(__cplusplus) && __DEVELOPER

    inline void
    debug_value_set_event_data(Debug_Event *event, f32 value)
    {
        event->type = eDebug_Type_f32;
        event->value_f32 = value;
    }
    
    inline void
    debug_value_set_event_data(Debug_Event *event, s32 value)
    {
        event->type = eDebug_Type_s32;
        event->value_s32 = value;
    }
    
    inline void
    debug_value_set_event_data(Debug_Event *event, u32 value)
    {
        event->type = eDebug_Type_u32;
        event->value_u32 = value;
    }
    
    inline void
    debug_value_set_event_data(Debug_Event *event, v2 value)
    {
        event->type = eDebug_Type_v2;
        event->value_v2 = value;
    }
    
    inline void
    debug_value_set_event_data(Debug_Event *event, v3 value)
    {
        event->type = eDebug_Type_v3;
        event->value_v3 = value;
    }
    
    inline void
    debug_value_set_event_data(Debug_Event *event, v4 value)
    {
        event->type = eDebug_Type_v4;
        event->value_v4 = value;
    }
    
    inline void
    debug_value_set_event_data(Debug_Event *event, Rect2 value)
    {
        event->type = eDebug_Type_Rect2;
        event->value_Rect2 = value;
    }
    
    inline void
    debug_value_set_event_data(Debug_Event *event, Rect3 value)
    {
        event->type = eDebug_Type_Rect3;
        event->value_Rect3 = value;
    }
    
    inline void
    debug_value_set_event_data(Debug_Event *event, Bitmap *value)
    {
        event->type = eDebug_Type_Bitmap;
        event->value_Bitmap = value;
    }
    
    #define DEBUG_BEGIN_DATA_BLOCK(name, id) \
    { \
        record_debug_event(eDebug_Type_Open_Data_Block, name); \
        event->debug_id = id; \
    } \
    
    #define DEBUG_END_DATA_BLOCK() \
    { \
        record_debug_event(eDebug_Type_Close_Data_Block, "End Data Block"); \
    } \
    
    
    #define DEBUG_VALUE(value) \
    { \
        record_debug_event(eDebug_Type_Unknown, #value); \
        debug_value_set_event_data(event, value); \
    } \
    
    #define DEBUG_BEGIN_ARRAY(...)
    #define DEBUG_END_ARRAY(...)
    
    inline Debug_ID DEBUG_POINTER_ID(void *pointer)\
    {\
        Debug_ID id = { pointer };\
        return id;\
    }\
    
    #define DEBUG_UI_ENABLED 1
    
    inline Debug_Event debug_init_value(Debug_Type type, Debug_Event *subevent, char *guid, char *name)
    {
        record_debug_event(eDebug_Type_Mark_Debug_Value, "");

        event->guid = guid;
        event->block_name = name;
        event->value_Debug_Event = subevent;

        subevent->clock = 0;
        subevent->guid = guid;
        subevent->block_name = name;
        subevent->thread_id = 0;
        subevent->core_idx = 0;
        subevent->type = (u8)type;
    
        return *subevent;
    }
    
    #define DEBUG_IF__(path) \
        local_persist Debug_Event debug_value_##path = \
        debug_init_value((debug_value_##path.value_b32 = GlobalConstants_##path, eDebug_Type_b32), \
                         &debug_value_##path, unique_file_counter_string(), #path); \
        if (debug_value_##path.value_b32)
    
    #define DEBUG_VARIABLE__(type, path, variable) \
        local_persist Debug_Event debug_value_##variable = debug_init_value((debug_value_##variable.value_##type = GlobalConstants_##path##_##variable, eDebug_Type_##type), \
                                                                            &debug_value_##variable, unique_file_counter_string(), #path "_" #variable); \
        type variable = debug_value_##variable.value_##type;


#else

    #define DEBUG_BEGIN_DATA_BLOCK(...)
    #define DEBUG_END_DATA_BLOCK(...)
    #define DEBUG_VALUE(...)
    #define DEBUG_BEGIN_ARRAY(...)
    #define DEBUG_END_ARRAY(...)
    #define DEBUG_POINTER_ID(...)
    #define DEBUG_UI_ENABLED 0

#endif


#define DEBUG_IF_(path) DEBUG_IF__(path)
#define DEBUG_IF(path) DEBUG_IF_(path)

#define DEBUG_VARIABLE_(type, path, variable) DEBUG_VARIABLE__(type, path, variable)
#define DEBUG_VARIABLE(type, path, variable) DEBUG_VARIABLE_(type, path, variable)
