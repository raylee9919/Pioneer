/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Sung Woo Lee $
   $Notice: (C) Copyright %s by Sung Woo Lee. All Rights Reserved. $
   ======================================================================== */

#include <stdio.h>

#define TIMED_BLOCK__(Number, ...) Timed_Block timed_block_##Number(__COUNTER__, __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define TIMED_BLOCK_(Number, ...) TIMED_BLOCK__(Number, ##__VA_ARGS__)
#define TIMED_BLOCK(...) TIMED_BLOCK_(__LINE__, ##__VA_ARGS__)

#define DEBUG_LOG_FRAME_COUNT 120


struct Debug_Record 
{
    u64             cycles;
    u32             hit;
};

struct Debug_Info 
{
    const char      *filename;
    const char      *function;
    s32             line;

    u64             max_cycles;
    u64             min_cycles;
    u64             avg_cycles;
};

struct Debug_Log 
{
    Debug_Record    *debug_records;
    Debug_Info      *debug_infos;
    s32             next_frame;
    u32             record_width;
};

struct Timed_Block 
{
    Debug_Record    *m_debug_record;
    Debug_Info      *m_debug_info;
    u64             m_start_cycles;
    s32             m_counter;

    Timed_Block(s32 counter, const char *filename, const char *function, s32 line, u32 hit_count);
    ~Timed_Block();
};
