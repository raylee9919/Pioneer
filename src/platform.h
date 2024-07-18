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

struct Entire_File 
{
    u32     content_size;
    void    *contents;
};

struct Debug_Executing_Process
{
    u64 os_handle;
};

struct Debug_Process_State
{
    b32 started_successfully;
    b32 is_running;
    s32 return_code;
};

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

enum Platform_Error_Type
{
    FATAL,
    NON_FATAL,
};
#define PLATFORM_ERROR_MESSAGE(name) void name(Platform_Error_Type type, char *message)
typedef PLATFORM_ERROR_MESSAGE(Platform_Error_Message);

struct Input_Key
{
    u32 key;
    b32 is_down;
    b32 was_down;
    b32 alt_was_down;
    b32 shift_was_down;
};
struct Input
{
    Input_Key       keys[256];
    u32             next;
    f32             dt;
};

#if 0
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
    f32         dt;

    Game_Key    W;
    Game_Key    S;
    Game_Key    A;
    Game_Key    D;
    Game_Key    alt;
    Game_Key    shift;
    Game_Key    control;
    Game_Key    Q;
    Game_Key    E;
    Game_Key    tilde;

    Mouse_Input mouse;
};
#endif

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
    Platform_Error_Message      *error_message;
    
#if __DEVELOPER
    DEBUG_PLATFORM_WRITE_FILE_  *debug_platform_write_file;
    DEBUG_PLATFORM_FREE_MEMORY_ *debug_platform_free_memory;
    Debug_Platform_Execute_System_Command *debug_platform_execute_system_command;
    Debug_Platform_Get_Process_State *debug_platform_get_process_state;
#endif
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

//
// Common Utility Functions
//
internal u32
string_length(char *string)
{
    u32 count = 0;
    while (*string++)
    {
        count++;
    }
    return count;
}

#include "debug_interface.h"
