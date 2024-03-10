#ifndef SW_PLATFORM_H
/* ========================================================================
$File: $
$Date: $
$Revision: $
$Creator: Sung Woo Lee $
$Notice: (C) Copyright 2024 by Sung Woo Lee. All Rights Reserved. $
======================================================================== */

#define KB(value) (   value  * 1024ll)
#define MB(value) (KB(value) * 1024ll)
#define GB(value) (MB(value) * 1024ll)
#define TB(value) (GB(value) * 1024ll)

#define ASSERT(expression)  if(!(expression)) { *(int *)0 = 0; }
#define INVALID_CODE_PATH ASSERT(!"Invalid Code Path")
#define INVALID_DEFAULT_CASE default: { INVALID_CODE_PATH } break;

//
// Compilers
//
#define MSVC_COMPILER   1
#define MSVC_LLVM       0


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

typedef struct{
    r32 dt_per_frame;
    GameKey move_up;
    GameKey move_down;
    GameKey move_left;
    GameKey move_right;
} GameInput;

typedef struct {
    void *memory;
    u32 width;
    u32 height;
    u32 bpp;
    u32 pitch;
} GameScreenBuffer;

#ifdef MSVC_COMPILER
    #include "intrin.h"
    
    typedef struct {
        s64 cyclesElapsed;
        s32 hitCount;
    } debug_cycle_counter;
    
    enum DebugCycleCounter {
        DebugCycleCounter_GameMain,
        DebugCycleCounter_RenderRectSlow,
        DebugCycleCounter_PerPixel,
    };
    
    #define RDTSC_BEGIN(name) \
        RdtscBegin_(g_debugCycleCounters, DebugCycleCounter_##name);
    #define RDTSC_END(name) \
        RdtscEnd_(g_debugCycleCounters, DebugCycleCounter_##name);
    #define RDTSC_END_ADDCOUNT(name, count) \
        RdtscEnd_(g_debugCycleCounters, DebugCycleCounter_##name); \
        g_debugCycleCounters[DebugCycleCounter_##name].hitCount += (count - 1);
    
    internal void
    RdtscBegin_(debug_cycle_counter *debugCycleCounters, s32 idx) {
        s64 val = __rdtsc();
        debugCycleCounters[idx].hitCount++;
        debugCycleCounters[idx].cyclesElapsed -= val;
    }
    
    internal void
    RdtscEnd_(debug_cycle_counter *debugCycleCounters, s32 idx) {
        s64 val = __rdtsc();
        debugCycleCounters[idx].cyclesElapsed += val;
    }

#else
    #define RDTSC_BEGIN(name)
    #define RDTSC_END(name)
#endif

#define SW_PLATFORM_H
#endif
