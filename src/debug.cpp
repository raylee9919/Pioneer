/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Sung Woo Lee $
   $Notice: (C) Copyright 2024 by Sung Woo Lee. All Rights Reserved. $
   ======================================================================== */

// TODO: stop using stdio
#include <stdio.h>

struct Debug_Statistic
{
    f64 min;
    f64 max;
    f64 avg;
    u32 count;
};

inline void
begin_debug_statistic(Debug_Statistic *stat)
{
    stat->min   = F32_MAX;
    stat->max   = -F32_MAX;
    stat->avg   = 0.0f;
    stat->count = 0;
}

inline void
end_debug_statistic(Debug_Statistic *stat)
{
    if (stat->count != 0)
    {
        stat->avg /= (f64)stat->count;
    }
    else
    {
        stat->min = 0.0f;
        stat->max = 0.0f;
    }
}

internal void
accumulate_debug_statistic(Debug_Statistic *stat, f64 value)
{
    ++stat->count;
    if (stat->min > value)
    {
        stat->min = value;
    }

    if (stat->max < value)
    {
        stat->max = value;
    }

    stat->avg += value;
}

internal void
debug_overlay(Game_Memory *game_memory, Render_Group *render_group, Game_Assets *game_assets, f32 *cen_y)
{
    Debug_State *debug_state = (Debug_State *)game_memory->debug_memory;
    if (debug_state)
    {
        for (u32 counter_idx = 0;
             counter_idx < debug_state->counter_count;
             ++counter_idx)
        {
            Debug_Counter_State *counter = debug_state->counter_states + counter_idx;

            Debug_Statistic hit_count_stat;
            Debug_Statistic cycle_count_stat;
            Debug_Statistic cycle_over_hit_stat;

            begin_debug_statistic(&hit_count_stat);
            begin_debug_statistic(&cycle_count_stat);
            begin_debug_statistic(&cycle_over_hit_stat);

            for (u32 snapshot_idx = 0;
                 snapshot_idx < DEBUG_SNAPSHOT_COUNT;
                 ++snapshot_idx)
            {
                accumulate_debug_statistic(&hit_count_stat, counter->snapshots[snapshot_idx].hit_count);
                accumulate_debug_statistic(&cycle_count_stat, (u32)counter->snapshots[snapshot_idx].cycle_count);

                f64 cycle_over_hit = 0.0f;
                if (counter->snapshots[snapshot_idx].hit_count)
                {
                    cycle_over_hit = ((f64)counter->snapshots[snapshot_idx].cycle_count /
                                      (f64)counter->snapshots[snapshot_idx].hit_count);
                }
                accumulate_debug_statistic(&cycle_over_hit_stat, cycle_over_hit);
            }
            end_debug_statistic(&hit_count_stat);
            end_debug_statistic(&cycle_count_stat);
            end_debug_statistic(&cycle_over_hit_stat);

            if (counter->block_name)
            {
                char text_buffer[256];
                _snprintf_s(text_buffer, sizeof(text_buffer), 
                            "%32s(%4d): %10ucy %8uh %10ucy/h",
                            counter->block_name,
                            counter->line_number,
                            (u32)cycle_count_stat.avg,
                            (u32)hit_count_stat.avg,
                            (u32)cycle_over_hit_stat.avg);
                push_string(render_group, _v3_(0, 0, 0), text_buffer, cen_y, game_assets);
                *cen_y -= (f32)game_assets->v_advance;

                if (cycle_count_stat.max > 0.0f)
                {

                }
            }
        }

#if 0
        Debug_Frame_End_Info *info = debug_state->frame_end_infos;
        for (u32 timestamp_idx = 0;
             timestamp_idx < info->timestamp_count;
             ++timestamp_idx)
        {
            Debug_Frame_Timestamp *timestamp = info->timestamps + timestamp_idx;
            push_string(render_group, _v3_(0, 0, 0), timestamp->name, cen_y, game_assets);
            *cen_y -= (f32)game_assets->v_advance;
        }

        f32 chart_height = 300.0f;
        f32 scale = chart_height / 0.03333f;
        for (u32 snapshot_idx = 0;
             snapshot_idx < DEBUG_SNAPSHOT_COUNT;
             ++snapshot_idx)
        {
            f32 prev_timestamp_seconds = 0.0f;
            for (u32 timestamp_idx = 0;
                 timestamp_idx < info->timestamp_count;
                 ++timestamp_idx)
            {
                Debug_Frame_Timestamp *timestamp = info->timestamps + timestamp_idx;
                f32 this_timestamp_seconds = (timestamp->seconds - prev_timestamp_seconds);
                prev_timestamp_seconds = timestamp->seconds;
            }
        }
#endif
    }
}


#define DEBUG_RECORDS_MAIN_COUNT __COUNTER__
global_var Debug_Table g_debug_table_;
Debug_Table *g_debug_table = &g_debug_table_;

internal void
update_debug_records(Debug_State *debug_state, u32 counter_count, Debug_Record *counters)
{
    for (u32 counter_idx = 0;
         counter_idx < counter_count;
         ++counter_idx)
    {
        Debug_Record *src = counters + counter_idx;
        Debug_Counter_State *dst = debug_state->counter_states + debug_state->counter_count++;

        u64 hit_count_cycle_count = atomic_exchange_u64(&src->hit_count_cycle_count, 0);
        dst->file_name      = src->file_name;
        dst->block_name     = src->block_name;
        dst->line_number    = src->line_number;
        dst->snapshots[debug_state->snapshot_idx].hit_count       = (u32)(hit_count_cycle_count >> 32);
        dst->snapshots[debug_state->snapshot_idx].cycle_count     = (u32)(hit_count_cycle_count & 0xffffffff);
    }
}


internal void
collate_debug_records(Debug_State *debug_state, u32 event_count, Debug_Event *events)
{
    Debug_Counter_State *counter_array[MAX_DEBUG_TRANSLATION_UNITS];
    Debug_Counter_State *current_counter = debug_state->counter_states;
    u32 total_record_count = 0;

    for (u32 unit_idx = 0;
         unit_idx < MAX_DEBUG_TRANSLATION_UNITS;
         ++unit_idx)
    {
        counter_array[unit_idx] = current_counter;
        total_record_count += g_debug_table->record_count[unit_idx];
        current_counter += g_debug_table->record_count[unit_idx];
    }
    debug_state->counter_count = total_record_count; 

    for (u32 counter_idx = 0;
         counter_idx < debug_state->counter_count;
         ++counter_idx)
    {
        Debug_Counter_State *dst = debug_state->counter_states + counter_idx;
        dst->snapshots[debug_state->snapshot_idx].hit_count = 0;
        dst->snapshots[debug_state->snapshot_idx].cycle_count = 0;
    }

    for (u32 event_idx = 0;
         event_idx < event_count;
         ++event_idx)
    {
        Debug_Event *event = events + event_idx;
        Debug_Counter_State *dst = counter_array[event->translation_unit] + event->debug_record_idx;

        Debug_Record *src   = g_debug_table->records[event->translation_unit] + event->debug_record_idx;
        dst->file_name      = src->file_name;
        dst->block_name     = src->block_name;
        dst->line_number    = src->line_number;

        if (event->type == eDebug_Event_Begin_Block)
        {
            ++dst->snapshots[debug_state->snapshot_idx].hit_count;
            dst->snapshots[debug_state->snapshot_idx].cycle_count -= event->clock;
        }
        else if (event->type == eDebug_Event_End_Block)
        {
            dst->snapshots[debug_state->snapshot_idx].cycle_count += event->clock;
        }
    }
}

extern "C"
DEBUG_FRAME_END(debug_frame_end)
{
    g_debug_table->record_count[0] = DEBUG_RECORDS_MAIN_COUNT;

    ++g_debug_table->current_event_array_idx;
    if (g_debug_table->current_event_array_idx >= array_count(g_debug_table->events))
    {
        g_debug_table->current_event_array_idx = 0;
    }
    u64 event_array_idx_event_idx = atomic_exchange_u64(&g_debug_table->event_array_idx_event_idx,
                                                        (u64)g_debug_table->current_event_array_idx << 32);

    u32 event_array_idx = (event_array_idx_event_idx >> 32);
    u32 event_count     = (event_array_idx_event_idx & 0xffffffff);

    Debug_State *debug_state = (Debug_State *)game_memory->debug_memory;
    if (debug_state)
    {
        debug_state->counter_count = 0;
        collate_debug_records(debug_state, event_count, g_debug_table->events[event_array_idx]);

        ++debug_state->snapshot_idx;
        if (debug_state->snapshot_idx >= DEBUG_SNAPSHOT_COUNT)
        {
            debug_state->snapshot_idx = 0;
        }
    }

    return g_debug_table;
}
