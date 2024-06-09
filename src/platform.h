/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Sung Woo Lee $
   $Notice: (C) Copyright %s by Sung Woo Lee. All Rights Reserved. $
   ======================================================================== */

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

extern "C" 
{
    typedef struct Entire_File 
    {
        u32     content_size;
        void    *contents;
    } Entire_File;

    #define DEBUG_PLATFORM_WRITE_FILE(name) b32 name(const char *filename, u32 size, void *contents)
    typedef DEBUG_PLATFORM_WRITE_FILE(DEBUG_PLATFORM_WRITE_FILE_);

    #define DEBUG_PLATFORM_FREE_MEMORY(name) void name(void *memory)
    typedef DEBUG_PLATFORM_FREE_MEMORY(DEBUG_PLATFORM_FREE_MEMORY_);

    #define PLATFORM_READ_ENTIRE_FILE(name) Entire_File name(const char *filename)
    typedef PLATFORM_READ_ENTIRE_FILE(Read_Entire_File);

    struct Game_Key 
    {
        b32 is_set;
    };

    enum Mouse_Enum 
    {
        eMouse_Left,
        eMouse_Middle,
        eMouse_Right,

        eMouse_Count
    };

    // these are following game coordinates.
    struct Mouse_Input 
    {
        v2      P;
        b32     is_down[eMouse_Count];
        b32     toggle[eMouse_Count];
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

        Game_Key    toggle_debug;

        Mouse_Input mouse;
    };

    struct PlatformWorkQueue;
    #define PLATFORM_WORK_QUEUE_CALLBACK(Name) void Name(PlatformWorkQueue *queue, void *data)
    typedef PLATFORM_WORK_QUEUE_CALLBACK(PlatformWorkQueueCallback);

    typedef void Platform_Add_Entry(PlatformWorkQueue *queue, PlatformWorkQueueCallback *callback, void *data);
    typedef void Platform_Complete_All_Work(PlatformWorkQueue *queue);

    struct Platform_API 
    {
        Platform_Add_Entry          *platform_add_entry;
        Platform_Complete_All_Work  *platform_complete_all_work;

        Read_Entire_File            *debug_platform_read_file;
        DEBUG_PLATFORM_WRITE_FILE_  *debug_platform_write_file;
        DEBUG_PLATFORM_FREE_MEMORY_ *debug_platform_free_memory;
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
        void                *permanent_memory;
        u64                 permanent_memory_size;

        void                *transient_memory;
        u64                 transient_memory_size;

        void                *debug_memory;
        u64                 debug_memory_size;

        PlatformWorkQueue   *highPriorityQueue;
        PlatformWorkQueue   *lowPriorityQueue;

        Platform_API platform;

        Render_Batch render_batch;
    };

    struct Game_Screen_Buffer
    {
        void    *memory;
        u32     width;
        u32     height;
        u32     bpp;
        u32     pitch;
    };

}
