/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Sung Woo Lee $
   $Notice: (C) Copyright 2024 by Sung Woo Lee. All Rights Reserved. $
   ======================================================================== */

// TODO: stop using stdio
#include <stdio.h>

global_var f32 left_edge;
global_var f32 at_y;

internal void
debug_reset(u32 width, u32 height)
{
    TIMED_FUNCTION();
    left_edge = 0.0f;
    at_y = 0.5f * height;
}

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
debug_overlay(Game_Memory *game_memory,
              f32 width, f32 height,
              Game_Assets *game_assets)
{
    Debug_State *debug_state = (Debug_State *)game_memory->debug_memory;
    if (debug_state)
    {
#if 0
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
#endif
        push_string(g_debug_render_group, _v3_(width * 0.3f, height * 0.8f, 0.0f),
                    "DEBUG MODE", game_assets);

        f32 lane_width = 8.0f;
        u32 lane_count = debug_state->frame_bar_lane_count;
        f32 bar_width = lane_width * lane_count;
        f32 bar_spacing = bar_width + 4.0f;
        f32 chart_left = left_edge + 10.0f;
        f32 chart_height = 300.0f;
        f32 chart_width = bar_spacing * (f32)debug_state->frame_count;
        f32 chart_min_y = at_y - (chart_height + 80.0f);
        f32 scale = chart_height * debug_state->frame_bar_scale;

        v3 colors[] = {
            _v3_(1.0f, 0.0f, 0.0f),
            _v3_(0.0f, 1.0f, 0.0f),
            _v3_(0.0f, 0.0f, 1.0f),
            _v3_(1.0f, 1.0f, 0.0f),
            _v3_(0.0f, 1.0f, 1.0f),
            _v3_(1.0f, 0.0f, 1.0f),
            _v3_(1.0f, 0.5f, 0.0f),
            _v3_(1.0f, 0.0f, 0.5f),
            _v3_(0.5f, 1.0f, 0.0f),
            _v3_(0.0f, 1.0f, 0.5f),
            _v3_(0.5f, 0.0f, 1.0f),
            _v3_(0.0f, 0.5f, 1.0f)
        };

        for (u32 frame_idx = 0;
             frame_idx < debug_state->frame_count;
             ++frame_idx)
        {
            Debug_Frame *frame = debug_state->frames + frame_idx;
            f32 stack_x = chart_left + bar_spacing * (f32)frame_idx;
            f32 stack_y = chart_min_y;

            for (u32 region_idx = 0;
                 region_idx < frame->region_count;
                 ++region_idx)
            {
                Debug_Frame_Region *region = frame->regions + region_idx;

                v3 color = colors[region_idx % array_count(colors)];
                f32 this_min_y = stack_y + scale * region->min_t;
                f32 this_max_y = stack_y + scale * region->max_t;
                push_bitmap(g_debug_render_group,
                            _v3_(stack_x + lane_width * region->lane_idx, this_min_y, 0.0f),
                            _v3_(stack_x + lane_width * region->lane_idx + lane_width, this_max_y, 0.0f),
                            0, _v4_(color, 1.0f));
            }
        }
    }
}

#define DEBUG_RECORDS_MAIN_COUNT __COUNTER__
global_var Debug_Table g_debug_table_;
Debug_Table *g_debug_table = &g_debug_table_;

inline u32
get_lane_from_thread(Debug_State *debug_state, u32 thread_idx)
{
    u32 result = 0;

    // TODO: implement thread ID lookup.

    return result;
}

internal Debug_Thread *
get_debug_thread(Debug_State *debug_state, u32 thread_id)
{
    Debug_Thread *result = 0;
    for (Debug_Thread *thread = debug_state->first_thread;
         thread;
         thread = thread->next)
    {
        if (thread->id == thread_id)
        {
            result = thread;
            break;
        }
    }

    if (!result)
    {
        result = push_struct(&debug_state->collate_arena, Debug_Thread);
        result->id = thread_id;
        result->lane_idx = debug_state->frame_bar_lane_count++;
        result->first_open_block = 0;
        result->next = debug_state->first_thread;
        debug_state->first_thread = result;
    }

    return result;
}

internal Debug_Frame_Region *
add_region(Debug_State *debug_state, Debug_Frame *current_frame)
{
    Assert(current_frame->region_count < MAX_REGIONS_PER_FRAME);
    Debug_Frame_Region *result = current_frame->regions + current_frame->region_count++;

    return result;
}


internal void
collate_debug_records(Debug_State *debug_state, u32 invalid_event_array_idx)
{
    debug_state->frames = push_array(&debug_state->collate_arena, Debug_Frame, MAX_DEBUG_EVENT_ARRAY_COUNT * 4);
    debug_state->frame_bar_lane_count = 0;
    debug_state->frame_count = 0;
    debug_state->frame_bar_scale = 1.0f;

    Debug_Frame *current_frame = 0;

    for (u32 event_array_idx = invalid_event_array_idx + 1;
         ;
         ++event_array_idx)
    {
        if (event_array_idx == MAX_DEBUG_EVENT_ARRAY_COUNT)
        {
            event_array_idx = 0;
        }

        if (event_array_idx == invalid_event_array_idx)
        {
            break;
        }

        for (u32 event_idx = 0;
             event_idx < g_debug_table->event_count[event_array_idx];
             ++event_idx)
        {
            Debug_Event *event = g_debug_table->events[event_array_idx] + event_idx;
            Debug_Record *src = (g_debug_table->records[event->translation_unit] +
                                 event->debug_record_idx);

            if (event->type == eDebug_Event_Frame_Marker)
            {
                if (current_frame)
                {
                    current_frame->end_clock = event->clock;
                    f32 clock_range = (f32)(current_frame->end_clock - current_frame->begin_clock);
                    if (clock_range > 0.0f)
                    {
                        f32 frame_bar_scale = 1.0f / clock_range;
                        if (debug_state->frame_bar_scale > frame_bar_scale)
                        {
                            debug_state->frame_bar_scale = frame_bar_scale;
                        }
                    }
                }
                current_frame = debug_state->frames + debug_state->frame_count++;
                current_frame->begin_clock = event->clock;
                current_frame->end_clock = 0;
                current_frame->region_count = 0;
                current_frame->regions = push_array(&debug_state->collate_arena, Debug_Frame_Region, MAX_REGIONS_PER_FRAME);
            }
            else if (current_frame)
            {
                u32 frame_idx = debug_state->frame_count - 1;
                Debug_Thread *thread = get_debug_thread(debug_state, event->thread_id);
                u64 relative_clock = event->clock - current_frame->begin_clock;
                if (event->type == eDebug_Event_Begin_Block)
                {
                    Open_Debug_Block *debug_block = debug_state->first_free_block;
                    if (debug_block)
                    {
                        debug_state->first_free_block = debug_block->next_free;
                    }
                    else
                    {
                        debug_block = push_struct(&debug_state->collate_arena, Open_Debug_Block);
                    }

                    debug_block->starting_frame_idx = frame_idx;
                    debug_block->opening_event = event;
                    debug_block->parent = thread->first_open_block;
                    thread->first_open_block = debug_block;
                    debug_block->next_free = 0;
                }
                else if (event->type == eDebug_Event_End_Block)
                {
                    if (thread->first_open_block)
                    {
                        Open_Debug_Block *matching_block = thread->first_open_block;
                        Debug_Event *opening_event = matching_block->opening_event;
                        if (opening_event->thread_id == event->thread_id &&
                            opening_event->debug_record_idx == event->debug_record_idx &&
                            opening_event->translation_unit == event->translation_unit)
                        {
                            if (matching_block->starting_frame_idx == frame_idx)
                            {
                                if (thread->first_open_block->parent == 0)
                                {
                                    Debug_Frame_Region *region = add_region(debug_state, current_frame);
                                    region->lane_idx = thread->lane_idx;
                                    region->min_t = (f32)(opening_event->clock - current_frame->begin_clock);
                                    region->max_t = (f32)(event->clock - current_frame->begin_clock);
                                }
                            }
                            else
                            {
                                // TODO: record all frames in between and begin/end spans!
                            }

                            thread->first_open_block->next_free = debug_state->first_free_block;
                            debug_state->first_free_block = thread->first_open_block;
                            thread->first_open_block = matching_block->parent;
                        }
                        else
                        {
                            // TODO: record span that goes to the beginning of the frame series?
                        }
                    }
                }
                else
                {
                    Assert(!"Invalid event type");
                }
            }

        }
    }
         
#if 0
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
#endif
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
    g_debug_table->event_count[event_array_idx] = event_count;

    Debug_State *debug_state = (Debug_State *)game_memory->debug_memory;
    if (debug_state)
    {
        if (!debug_state->init)
        {
            init_arena(&debug_state->collate_arena,
                       game_memory->debug_memory_size - sizeof(Debug_State),
                       debug_state + 1);
            debug_state->collate_tmp = begin_temporary_memory(&debug_state->collate_arena);
        }

        end_temporary_memory(&debug_state->collate_tmp);
        debug_state->collate_tmp = begin_temporary_memory(&debug_state->collate_arena);

        debug_state->first_thread = 0;
        debug_state->first_free_block = 0;

        collate_debug_records(debug_state, g_debug_table->current_event_array_idx);
    }

    return g_debug_table;
}
