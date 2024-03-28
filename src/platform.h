#ifndef SW_PLATFORM_H
 /* ―――――――――――――――――――――――――――――――――――◆――――――――――――――――――――――――――――――――――――
    $File: $
    $Date: $
    $Revision: $
    $Creator: Sung Woo Lee $
    $Notice: (C) Copyright 2024 by Sung Woo Lee. All Rights Reserved. $
    ―――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――― */

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
#define COMPILER_MSVC 1


#ifdef COMPILER_MSVC
  #define __WRITE_BARRIER__ _WriteBarrier();
  // atomic compare exchange.
  #define ATOMIC_COMPARE_EXCHANGE(Name) int Name(volatile int *dst, int exchange, int comperhand)
  typedef ATOMIC_COMPARE_EXCHANGE(Atomic_Compare_Exchange);

  // atomic add.
  #define ATOMIC_ADD(Name) int Name(volatile int *addend, int value)
  typedef ATOMIC_ADD(Atomic_Add);
#endif

struct DebugReadFileResult {
    u32 content_size;
    void *contents;
};

#define DEBUG_PLATFORM_WRITE_FILE(name) bool32 name(const char *filename, uint32 size, void *contents)
typedef DEBUG_PLATFORM_WRITE_FILE(DEBUG_PLATFORM_WRITE_FILE_);

#define DEBUG_PLATFORM_FREE_MEMORY(name) void name(void *memory)
typedef DEBUG_PLATFORM_FREE_MEMORY(DEBUG_PLATFORM_FREE_MEMORY_);

#define DEBUG_PLATFORM_READ_FILE(name) DebugReadFileResult name(const char *filename)
typedef DEBUG_PLATFORM_READ_FILE(DEBUG_PLATFORM_READ_FILE_);


typedef struct {
    b32 is_set;
} GameKey;

typedef struct {
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

typedef void PlatformAddEntry(PlatformWorkQueue *queue, PlatformWorkQueueCallback *callback, void *data);
typedef void PlatformCompleteAllWork(PlatformWorkQueue *queue);
typedef struct {
    // NOTE: Spec memory to be initialized to zero.
    void *permanent_memory;
    u64 permanent_memory_capacity;

    void *transient_memory;
    u64 transient_memory_capacity;

    PlatformWorkQueue *highPriorityQueue;
    PlatformWorkQueue *lowPriorityQueue;

    PlatformAddEntry *platformAddEntry;
    PlatformCompleteAllWork *platformCompleteAllWork;

    DEBUG_PLATFORM_READ_FILE_ *debug_platform_read_file;
    DEBUG_PLATFORM_WRITE_FILE_ *debug_platform_write_file;
    DEBUG_PLATFORM_FREE_MEMORY_ *debug_platform_free_memory;

    Atomic_Compare_Exchange *atomic_compare_exchange;
    Atomic_Add *atomic_add;

} GameMemory;

typedef struct {
    void *memory;
    u32 width;
    u32 height;
    u32 bpp;
    u32 pitch;
} GameScreenBuffer;

global_var PlatformAddEntry *platformAddEntry;
global_var PlatformCompleteAllWork *platformCompleteAllWork;


#define SW_PLATFORM_H
#endif
