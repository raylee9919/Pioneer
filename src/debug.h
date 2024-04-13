/* ―――――――――――――――――――――――――――――――――――◆――――――――――――――――――――――――――――――――――――
   $File: $
   $Date: $
   $Revision: $
   $Creator: Sung Woo Lee $
   $Notice: (C) Copyright 2024 by Sung Woo Lee. All Rights Reserved. $
   ―――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――― */

#include <stdio.h>

#define TIMED_BLOCK__(Number, ...) Timed_Block timed_block_##Number(__COUNTER__, __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define TIMED_BLOCK_(Number, ...) TIMED_BLOCK__(Number, ##__VA_ARGS__)
#define TIMED_BLOCK(...) TIMED_BLOCK_(__LINE__, ##__VA_ARGS__)

#define DEBUG_LOG_FRAME_COUNT 120
r32 inv_debug_log_frame_count = 1.0f / (r32)DEBUG_LOG_FRAME_COUNT;

r32 g_debug_cam_orbital_pitch;
r32 g_debug_cam_orbital_yaw;
r32 g_debug_cam_z = 10.0f;
v2  g_debug_cam_last_mouse_p;

struct Debug_Record {
    u64 cycles;
    u32 hit;
};

struct Debug_Info {
    const char *filename;
    const char *function;
    s32 line;

    u64 max_cycles;
    u64 min_cycles;
    u64 avg_cycles;
};

struct Debug_Log {
    Debug_Record *debug_records;
    Debug_Info *debug_infos;
    s32 next_frame;
    u32 record_width;
};

Debug_Log g_debug_log;

struct Timed_Block {
    Debug_Record *m_debug_record;
    Debug_Info *m_debug_info;
    u64 m_start_cycles;
    s32 m_counter;

    Timed_Block(s32 counter, const char *filename, const char *function, s32 line, u32 hit_count = 1) {
        m_counter = counter;
        m_debug_record = g_debug_log.debug_records +
            g_debug_log.next_frame * g_debug_log.record_width + counter;
        m_debug_info = g_debug_log.debug_infos + counter;

        // multi-thread-don't care.
        m_debug_info->filename = filename;
        m_debug_info->function = function;
        m_debug_info->line = line;

        m_start_cycles = __rdtsc();
        atomic_add_u32(&m_debug_record->hit, hit_count);
    }

    ~Timed_Block() {
        // average cycles.
        // TODO: there can be several hits in a frame!!!
        m_debug_info->avg_cycles -= (u64)((r32)m_debug_record->cycles * inv_debug_log_frame_count);
        atomic_exchange_u64(&m_debug_record->cycles, __rdtsc() - m_start_cycles);
        m_debug_info->avg_cycles += (u64)((r32)m_debug_record->cycles * inv_debug_log_frame_count);

        // TODO: this is busted... max and min must be the value in last 120 frames!
        atomic_exchange_u64(&m_debug_info->max_cycles, Max(m_debug_info->max_cycles, m_debug_record->cycles));
        atomic_exchange_u64(&m_debug_info->min_cycles, Max(m_debug_info->min_cycles, m_debug_record->cycles));
        
    }
};
