/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Sung Woo Lee $
   $Notice: (C) Copyright 2024 by Sung Woo Lee. All Rights Reserved. $
   ======================================================================== */


Timed_Block::Timed_Block(s32 counter,
                         const char *filename, const char *function, s32 line, const char *block_name)
{
    m_counter = counter;
    m_debug_record = (g_debug_log.debug_records +
                      g_debug_log.next_frame * g_debug_log.record_width +
                      counter);
    m_debug_info = g_debug_log.debug_infos + counter;

    // multi-thread-don't care.
    m_debug_info->filename  = filename;
    m_debug_info->function  = function;
    m_debug_info->line      = line;
    m_debug_info->block_name= block_name;

    m_start_cycles = __rdtsc();
    atomic_add_u32(&m_debug_record->hit, 1);
}

Timed_Block::~Timed_Block()
{
    end_debug_block(this);
}


internal void
init_debug(Debug_Log *debug_log, Memory_Arena *arena)
{
    // COUNTER must be the last one in the game code translation unit.
    u32 width = __COUNTER__;
    debug_log->record_width = width;
    debug_log->debug_records = push_array(arena, Debug_Record, DEBUG_LOG_FRAME_COUNT * width);
    debug_log->debug_infos = push_array(arena, Debug_Info, width);
    debug_log->next_frame = 0;

    for (u32 idx = 0;
         idx < width;
         ++idx) 
    {
        Debug_Info *info = debug_log->debug_infos + idx;
        info->max_cycles = 0;
        info->min_cycles = UINT64_MAX;
        info->avg_cycles = 0;
    }
}

internal void
end_debug_log(Debug_Log *debug_log, f32 *cen_y)
{
    Debug_Record *records = (debug_log->debug_records +
                             debug_log->next_frame * debug_log->record_width);
    for (u32 record_idx = 0;
         record_idx < debug_log->record_width;
         ++record_idx) 
    {
        Debug_Record *record = records + record_idx;
        atomic_exchange_u32(&record->hit, 0);
    }

    if (++debug_log->next_frame == DEBUG_LOG_FRAME_COUNT) 
    {
        debug_log->next_frame = 0;
    }

    *cen_y = INIT_CEN_Y;
}

internal void
display_debug_info(Debug_Log *debug_log, Render_Group *render_group, Game_Assets *game_assets, Memory_Arena *arena, f32 *cen_y)
{
    for (u32 record_idx = 0;
            record_idx < debug_log->record_width;
            ++record_idx) 
    {
        Debug_Info *info = g_debug_log.debug_infos + record_idx;
        size_t size = 1024;
        char *str = push_array(arena, char, size);
        _snprintf(str, size,
                  "%30s(%4d): %10I64uavg_cyc",
                  info->block_name,
                  info->line,
                  info->avg_cycles);
        push_string(render_group, _v3_(0.0f, 0.0f, 0.0f), str, cen_y, game_assets);

        //
        // DRAW GRAPH
        //
#if 1
        f32 x = 950.0f;
        f32 max_height = (f32)game_assets->v_advance;
        f32 width = 2.0f;
        f32 inv_max_cycles = 0.0f;
        if (info->max_cycles != 0.0f) 
        {
            inv_max_cycles = 1.0f / info->max_cycles;
        }

        for (u32 frame = 0;
             frame < DEBUG_LOG_FRAME_COUNT;
             ++frame)
        {
            Debug_Record *record = g_debug_log.debug_records + frame * g_debug_log.record_width + record_idx;
            f32 bar_w = width * 0.5f;
            f32 bar_h = max_height * record->cycles * inv_max_cycles;
            v3 min = _v3_(x + width * frame, *cen_y, 0);
            v3 max = min + _v3_(bar_w, bar_h, 0);
            push_bitmap(render_group, min, max, 0, _v4_(1.0f, 1.0f, 0.5f, 1.0f));
        }
#endif

    *cen_y = *cen_y - game_assets->v_advance;
    }
}
