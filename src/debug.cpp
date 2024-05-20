/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Sung Woo Lee $
   $Notice: (C) Copyright 2024 by Sung Woo Lee. All Rights Reserved. $
   ======================================================================== */

#include "debug.h"

f32         inv_debug_log_frame_count = 1.0f / (f32)DEBUG_LOG_FRAME_COUNT;
v2          g_debug_cam_last_mouse_p;
Debug_Log   g_debug_log;

Timed_Block::Timed_Block(s32 counter,
                         const char *filename, const char *function, s32 line,
                         u32 hit_count = 1) 
{
    m_counter = counter;
    m_debug_record = (g_debug_log.debug_records +
                      g_debug_log.next_frame * g_debug_log.record_width +
                      counter);
    m_debug_info = g_debug_log.debug_infos + counter;

    // multi-thread-don't care.
    m_debug_info->filename = filename;
    m_debug_info->function = function;
    m_debug_info->line = line;

    m_start_cycles = __rdtsc();
    atomic_add_u32(&m_debug_record->hit, hit_count);
}

Timed_Block::~Timed_Block() 
{
    // average cycles.
    // TODO: there can be several hits in a frame!!!
    m_debug_info->avg_cycles -= (u64)((f32)m_debug_record->cycles * inv_debug_log_frame_count);
    atomic_exchange_u64(&m_debug_record->cycles, __rdtsc() - m_start_cycles);
    m_debug_info->avg_cycles += (u64)((f32)m_debug_record->cycles * inv_debug_log_frame_count);

    // TODO: this is busted... max and min must be the value in last 120 frames!
    atomic_exchange_u64(&m_debug_info->max_cycles, Max(m_debug_info->max_cycles, m_debug_record->cycles));
    atomic_exchange_u64(&m_debug_info->min_cycles, Max(m_debug_info->min_cycles, m_debug_record->cycles));

}

