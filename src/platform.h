 /* ―――――――――――――――――――――――――――――――――――◆――――――――――――――――――――――――――――――――――――
    $File: $
    $Date: $
    $Revision: $
    $Creator: Sung Woo Lee $
    $Notice: (C) Copyright 2024 by Sung Woo Lee. All Rights Reserved. $
    ―――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――― */

struct v2;
struct Bitmap;

#include "intrin.h"

#define KB(value) (   value  * 1024ll)
#define MB(value) (KB(value) * 1024ll)
#define GB(value) (MB(value) * 1024ll)
#define TB(value) (GB(value) * 1024ll)

#define Assert(expression)  if(!(expression)) { *(volatile int *)0 = 0; }
#define INVALID_CODE_PATH Assert(!"Invalid Code Path")
#define INVALID_DEFAULT_CASE default: { INVALID_CODE_PATH } break;


///////////////////////////////////////////////////////////////////////////////
//
// Compilers
//
#define __MSVC      1
#define __LLVM      0

#if     __MSVC
  #include <intrin.h>
#elif   __LLVM
  #include <x86intrin.h>
#endif

#if __MSVC
  #define __WRITE_BARRIER__ _WriteBarrier();
#elif   __LLVM

#endif

extern "C" {
    typedef struct DebugReadFileResult {
        u32 content_size;
        void *contents;
    } DebugReadFileResult;

    #define DEBUG_PLATFORM_WRITE_FILE(name) b32 name(const char *filename, u32 size, void *contents)
    typedef DEBUG_PLATFORM_WRITE_FILE(DEBUG_PLATFORM_WRITE_FILE_);

    #define DEBUG_PLATFORM_FREE_MEMORY(name) void name(void *memory)
    typedef DEBUG_PLATFORM_FREE_MEMORY(DEBUG_PLATFORM_FREE_MEMORY_);

    #define DEBUG_PLATFORM_READ_FILE(name) DebugReadFileResult name(const char *filename)
    typedef DEBUG_PLATFORM_READ_FILE(DEBUG_PLATFORM_READ_FILE_);


    typedef struct GameKey {
        b32 is_set;
    } GameKey;

    typedef struct GameInput {
        r32 dt_per_frame;
        GameKey move_up;
        GameKey move_down;
        GameKey move_left;
        GameKey move_right;

        GameKey toggle_debug;
    } GameInput;

    struct PlatformWorkQueue;
    #define PLATFORM_WORK_QUEUE_CALLBACK(Name) void Name(PlatformWorkQueue *queue, void *data)
    typedef PLATFORM_WORK_QUEUE_CALLBACK(PlatformWorkQueueCallback);

    typedef void Platform_Add_Entry(PlatformWorkQueue *queue, PlatformWorkQueueCallback *callback, void *data);
    typedef void Platform_Complete_All_Work(PlatformWorkQueue *queue);

    typedef struct Platform_API {
        Platform_Add_Entry          *platform_add_entry;
        Platform_Complete_All_Work  *platform_complete_all_work;

        DEBUG_PLATFORM_READ_FILE_   *debug_platform_read_file;
        DEBUG_PLATFORM_WRITE_FILE_  *debug_platform_write_file;
        DEBUG_PLATFORM_FREE_MEMORY_ *debug_platform_free_memory;


    } Platform_API;

    typedef struct Render_Batch {
        size_t  size;
        void    *base;
        size_t  used;
    } Render_Batch;

    typedef struct GameMemory {
        // NOTE: Spec memory to be initialized to zero.
        void *permanent_memory;
        u64 permanent_memory_size;

        void *transient_memory;
        u64 transient_memory_size;

        void *debug_memory;
        u64 debug_memory_size;

        PlatformWorkQueue *highPriorityQueue;
        PlatformWorkQueue *lowPriorityQueue;

        Platform_API platform;

        Render_Batch render_batch;
    } GameMemory;

    typedef struct GameScreenBuffer{
        void *memory;
        u32 width;
        u32 height;
        u32 bpp;
        u32 pitch;
    } GameScreenBuffer;

}
