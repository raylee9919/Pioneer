/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Sung Woo Lee $
   $Notice: (C) Copyright %s by Sung Woo Lee. All Rights Reserved. $
   ======================================================================== */

#include <stdio.h>

#define TIMED_BLOCK__(block_name, Number, ...) Timed_Block timed_block_##Number(__COUNTER__, __FILE__, __FUNCTION__, __LINE__, block_name)
#define TIMED_BLOCK_(block_name, Number, ...) TIMED_BLOCK__(block_name, Number)
#define TIMED_BLOCK(block_name, ...) TIMED_BLOCK_(block_name, __LINE__)

#define TIMED_FUNCTION() TIMED_BLOCK(__FUNCTION__)
#define BEGIN_BLOCK(block_name) Timed_Block timed_block_##block_name(__COUNTER__, __FILE__, __FUNCTION__, __LINE__, #block_name)
#define END_BLOCK(block_name) end_debug_block(&timed_block_##block_name);

#define DEBUG_LOG_FRAME_COUNT 120


struct Debug_Recor
{
    u64             cycles;
    u32             hit;
};

struct Debug_Info 
{
    const char      *filename;
    const char      *function;
    s32             line;
    const char      *block_name;

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

    Timed_Block(s32 counter, const char *filename, const char *function, s32 line, const char *block_name);
    ~Timed_Block();
};

internal void
end_debug_block(Timed_Block *timed_block)
{
#if 0
    // average cycles.
    // TODO: there can be several hits in a frame!!!
    m_debug_info->avg_cycles -= (u64)((f32)m_debug_record->cycles * inv_debug_log_frame_count);
    atomic_exchange_u64(timed_block->m_debug_record->cycles, __rdtsc() - m_start_cycles);
    m_debug_info->avg_cycles += (u64)((f32)m_debug_record->cycles * inv_debug_log_frame_count);

    // TODO: this is busted... max and min must be the value in last 120 frames!
    atomic_exchange_u64(&m_debug_info->max_cycles, Max(m_debug_info->max_cycles, m_debug_record->cycles));
    atomic_exchange_u64(&m_debug_info->min_cycles, Min(m_debug_info->min_cycles, m_debug_record->cycles));
#endif
    atomic_exchange_u64(&timed_block->m_debug_record->cycles, __rdtsc() - timed_block->m_start_cycles);
}


f32         inv_debug_log_frame_count = 1.0f / (f32)DEBUG_LOG_FRAME_COUNT;
v2          g_debug_cam_last_mouse_p;
Debug_Log   g_debug_log;
