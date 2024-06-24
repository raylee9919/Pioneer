/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Sung Woo Lee $
   $Notice: (C) Copyright %s by Sung Woo Lee. All Rights Reserved. $
   ======================================================================== */

#include "config.h"

union v2;
struct Bitmap;

#define KB(value) (   value  * 1024ll)
#define MB(value) (KB(value) * 1024ll)
#define GB(value) (MB(value) * 1024ll)
#define TB(value) (GB(value) * 1024ll)

#define Assert(expression)  if(!(expression)) { *(volatile int *)0 = 0; }
#define INVALID_CODE_PATH Assert(!"Invalid Code Path")
#define INVALID_DEFAULT_CASE default: { INVALID_CODE_PATH } break;


#define offset_of(Type, Member) (size_t)&(((Type *)0)->Member)


///////////////////////////////////////////////////////////////////////////////
//
// Compilers
//
#if     __MSVC
  #include <intrin.h>
#elif   __LLVM
  #include <x86intrin.h>
#endif

#if __MSVC
  #define __WRITE_BARRIER__ _WriteBarrier();
#elif   __LLVM

#endif


#ifdef __cplusplus
extern "C"
{
#endif
    inline u32
    atomic_compare_exchange_u32(u32 volatile *value, u32 _new, u32 expected) 
    {
        u32 result = _InterlockedCompareExchange((long *)value, _new, expected);
        return result;
    }

    inline u32
    atomic_exchange_u32(u32 volatile *value, u32 _new) 
    {
        u32 result = _InterlockedExchange((long *)value, _new);
        return result;
    }

    inline u64
    atomic_exchange_u64(u64 volatile *value, u64 _new) 
    {
        u64 result = _InterlockedExchange64((long long *)value, _new);
        return result;
    }

    inline u32
    atomic_add_u32(u32 volatile *value, u32 addend) 
    {
        u32 result = _InterlockedExchangeAdd((long *)value, addend);
        return result;
    }

    inline u64
    atomic_add_u64(u64 volatile *value, u64 addend) 
    {
        u64 result = _InterlockedExchangeAdd64((long long *)value, addend);
        return result;
    }

    typedef struct Entire_File 
    {
        u32     content_size;
        void    *contents;
    } Entire_File;

    typedef struct Debug_Executing_Process
    {
        u64 os_handle;
    } Debug_Executing_Process;

    typedef struct Debug_Process_State
    {
        b32 started_successfully;
        b32 is_running;
        s32 return_code;
    } Debug_Process_State;

    #define DEBUG_PLATFORM_WRITE_FILE(name) b32 name(const char *filename, u32 size, void *contents)
    typedef DEBUG_PLATFORM_WRITE_FILE(DEBUG_PLATFORM_WRITE_FILE_);

    #define DEBUG_PLATFORM_FREE_MEMORY(name) void name(void *memory)
    typedef DEBUG_PLATFORM_FREE_MEMORY(DEBUG_PLATFORM_FREE_MEMORY_);

    #define PLATFORM_READ_ENTIRE_FILE(name) Entire_File name(const char *filename)
    typedef PLATFORM_READ_ENTIRE_FILE(Read_Entire_File);

    #define DEBUG_PLATFORM_EXECUTE_SYSTEM_COMMAND(name) Debug_Executing_Process name(char *path, char *command, char* command_line)
    typedef DEBUG_PLATFORM_EXECUTE_SYSTEM_COMMAND(Debug_Platform_Execute_System_Command);

    // TODO: do we want a formal release mechanism here?
    #define DEBUG_PLATFORM_GET_PROCESS_STATE(name) Debug_Process_State name(Debug_Executing_Process process)
    typedef DEBUG_PLATFORM_GET_PROCESS_STATE(Debug_Platform_Get_Process_State);

    struct Game_Key 
    {
        b32 is_set;
    };

    enum Mouse_Enum 
    {
        eMouse_Left,
        eMouse_Middle,
        eMouse_Right,
        eMouse_Extended0,
        eMouse_Extended1,

        eMouse_Count
    };

    // these are following game coordinates.
    struct Mouse_Input 
    {
        b32     is_down[eMouse_Count];
        b32     toggle[eMouse_Count];

        v2      P;
        v2      click_p[eMouse_Count];
        s32     wheel_delta;
    };

    struct Game_Input 
    {
        f32         dt_per_frame;

        Game_Key    W;
        Game_Key    S;
        Game_Key    A;
        Game_Key    D;
        Game_Key    alt;
        Game_Key    Q;
        Game_Key    E;
        Game_Key    tilde;

        Mouse_Input mouse;
    };

    struct Platform_Work_Queue;
    #define PLATFORM_WORK_QUEUE_CALLBACK(Name) void Name(Platform_Work_Queue *queue, void *data)
    typedef PLATFORM_WORK_QUEUE_CALLBACK(Platform_Work_QueueCallback);

    typedef void Platform_Add_Entry(Platform_Work_Queue *queue, Platform_Work_QueueCallback *callback, void *data);
    typedef void Platform_Complete_All_Work(Platform_Work_Queue *queue);

    struct Platform_API 
    {
        Platform_Add_Entry          *platform_add_entry;
        Platform_Complete_All_Work  *platform_complete_all_work;

        Read_Entire_File            *debug_platform_read_file;
        DEBUG_PLATFORM_WRITE_FILE_  *debug_platform_write_file;
        DEBUG_PLATFORM_FREE_MEMORY_ *debug_platform_free_memory;
        Debug_Platform_Execute_System_Command *debug_platform_execute_system_command;
        Debug_Platform_Get_Process_State *debug_platform_get_process_state;
    };

    struct Render_Batch 
    {
        size_t  size;
        void    *base;
        size_t  used;
    };

    struct Game_Memory 
    {
        // NOTE: Spec memory to be initialized to zero.
        void                    *permanent_memory;
        u64                     permanent_memory_size;

        void                    *transient_memory;
        u64                     transient_memory_size;

        void                    *debug_storage;
        u64                     debug_storage_size;

        Platform_Work_Queue     *high_priority_queue;
        Platform_Work_Queue     *low_priority_queue;

        b32                     executable_reloaded;
        Platform_API            platform;

        Render_Batch            render_batch;
    };

    struct Game_Screen_Buffer
    {
        void    *memory;
        u32     width;
        u32     height;
        u32     bpp;
        u32     pitch;
    };
    

    struct Debug_Record
    {
        char    *file_name;
        char    *block_name;

        u32     line_number;
        u32     reserved;
    };

    enum Debug_Event_Type
    {
        eDebug_Event_Frame_Marker,
        eDebug_Event_Begin_Block,
        eDebug_Event_End_Block
    };

    struct Thread_ID_Core_Idx
    {
        u16 thread_id;
        u16 core_idx;
    };

    struct Debug_Event
    {
        u64                 clock;
        union
        {
            Thread_ID_Core_Idx  tc;
            f32                 seconds_elapsed;
        };
        u16                 debug_record_idx;
        u8                  translation_unit;
        u8                  type;
    };

    #define MAX_DEBUG_THREAD_COUNT                  256 
    #define MAX_DEBUG_EVENT_ARRAY_COUNT             8
    #define MAX_DEBUG_TRANSLATION_UNITS             2
    #define MAX_DEBUG_EVENT_COUNT                   (16*65536)
    #define MAX_DEBUG_RECORD_COUNT                  (65536)
    struct Debug_Table
    {
        u32             current_event_array_idx;
        u64 volatile    event_array_idx_event_idx;
        u32             event_count[MAX_DEBUG_EVENT_ARRAY_COUNT];
        Debug_Event     events[MAX_DEBUG_EVENT_ARRAY_COUNT][MAX_DEBUG_EVENT_COUNT];

        u32             record_count[MAX_DEBUG_TRANSLATION_UNITS];
        Debug_Record    records[MAX_DEBUG_TRANSLATION_UNITS][MAX_DEBUG_RECORD_COUNT];
    };

    extern Debug_Table *g_debug_table;

    #define DEBUG_FRAME_END(name) Debug_Table *name(Game_Memory *game_memory)
    typedef DEBUG_FRAME_END(Debug_Frame_End);

    #define record_debug_event_common(record_idx, event_type) \
        u64 array_idx_event_idx = atomic_add_u64(&g_debug_table->event_array_idx_event_idx, 1); \
        u32 event_idx       = (u32)(array_idx_event_idx & 0xffffffff); \
        Assert(event_idx < MAX_DEBUG_EVENT_COUNT); \
        Debug_Event *event = g_debug_table->events[array_idx_event_idx >> 32] + event_idx; \
        event->clock            = __rdtsc(); \
        event->debug_record_idx = (u16)record_idx; \
        event->translation_unit = TRANSLATION_UNIT_IDX; \
        event->type             = (u8)event_type; \

    #define record_debug_event(record_idx, event_type) \
    { \
        record_debug_event_common(record_idx, event_type); \
        event->tc.core_idx         = 0; \
        event->tc.thread_id        = (u16)get_thread_id(); \
    } \

    #define FRAME_MARKER(seconds_elapsed_init)\
    {\
        int counter = __COUNTER__;\
        record_debug_event_common(counter, eDebug_Event_Frame_Marker); \
        event->seconds_elapsed = seconds_elapsed_init; \
        Debug_Record *record = g_debug_table->records[TRANSLATION_UNIT_IDX] + counter;\
        record->file_name   = __FILE__;\
        record->line_number = __LINE__;\
        record->block_name  = "frame_marker";\
    }

#if __PROFILE
    #define TIMED_BLOCK__(block_name, number, ...) Timed_Block timed_block_##number(__COUNTER__, __FILE__, __LINE__, block_name, ##__VA_ARGS__)
    #define TIMED_BLOCK_(block_name, number, ...) TIMED_BLOCK__(block_name, number, ##__VA_ARGS__)
    #define TIMED_BLOCK(block_name, ...) TIMED_BLOCK_(#block_name, __LINE__, ##__VA_ARGS__)
    #define TIMED_FUNCTION(...) TIMED_BLOCK_(__FUNCTION__, __LINE__, ##__VA_ARGS__)

    #define BEGIN_BLOCK_(counter, file_name_init, line_number_init, block_name_init)\
    {\
        Debug_Record *record = g_debug_table->records[TRANSLATION_UNIT_IDX] + counter;\
        record->file_name       = file_name_init;\
        record->line_number     = line_number_init;\
        record->block_name      = block_name_init;\
        record_debug_event(counter, eDebug_Event_Begin_Block);\
    }

    #define BEGIN_BLOCK(name)\
        int counter_##name = __COUNTER__;\
        BEGIN_BLOCK_(counter_##name, __FILE__, __LINE__, #name);\

    #define END_BLOCK_(counter)\
        record_debug_event(counter, eDebug_Event_End_Block);

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
    #define TIMED_BLOCK(block_name, ...)
    #define TIMED_FUNCTION(...)
    #define BEGIN_BLOCK(name)
    #define END_BLOCK(name)
#endif

//
// Common Utility Functions
//
internal u32
string_length(char *string)
{
    u32 result = 0;
    while (*string++)
    {
        result++;
    }
    return result;
}


#ifdef __cplusplus
}
#endif
