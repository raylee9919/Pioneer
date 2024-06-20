/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Sung Woo Lee $
   $Notice: (C) Copyright 2024 by Sung Woo Lee. All Rights Reserved. $
   ======================================================================== */

// TODO: stop using stdio
#include <stdio.h>

internal void restart_collation(Debug_State *debug_state, u32 invalid_event_array_idx);

inline Debug_State *
get_debug_state(Game_Memory *memory)
{
    Debug_State *debug_state = (Debug_State *)memory->debug_storage;
    Assert(debug_state->init);

    return debug_state;
}

inline Debug_State *
get_debug_state(void)
{
    Debug_State *result = get_debug_state(g_debug_memory);
    return result;
}

internal void
debug_start(u32 width, u32 height)
{
    TIMED_FUNCTION();

    Debug_State *debug_state = (Debug_State *)g_debug_memory->debug_storage;

    if (debug_state)
    {
        if (!debug_state->init)
        {
            debug_state->high_priority_queue = g_debug_memory->high_priority_queue;
            init_arena(&debug_state->debug_arena,
                       g_debug_memory->debug_storage_size - sizeof(Debug_State),
                       debug_state + 1);

            Camera *debug_overlay_camera = push_camera(&debug_state->debug_arena, eCamera_Type_Orthographic, (f32)width, (f32)height);
            debug_state->render_group = alloc_render_group(&debug_state->debug_arena, MB(16),
                                                           debug_overlay_camera);

            debug_state->paused = false;
            debug_state->scope_to_record = 0;

            debug_state->is_debug_mode = false;

            debug_state->init = true;

            init_sub_arena(&debug_state->collate_arena, &debug_state->debug_arena, MB(32));
            debug_state->collate_tmp = begin_temporary_memory(&debug_state->collate_arena);

            restart_collation(debug_state, 0);
        }

        begin_render(debug_state->render_group);

        debug_state->left_edge = 0.0f;
        debug_state->at_y = 0.5f * height;
        debug_state->width = (f32)width;
        debug_state->height = (f32)height;
    }
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
    if (stat->count)
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
debug_end(Game_Input *input, Game_Assets *game_assets)
{
    TIMED_FUNCTION();

    Debug_State *debug_state = get_debug_state();
    if (debug_state)
    {
        Render_Group *render_group = debug_state->render_group;

        Debug_Record *hot_record = 0;

        v2 mouse_p = input->mouse.P;
        if (input->mouse.is_down[eMouse_Right] &&
            input->mouse.toggle[eMouse_Right])
        {
            debug_state->paused = !debug_state->paused;
        }
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
        if (debug_state->is_debug_mode)
        {
            push_string(render_group, _v3_(debug_state->width * 0.02f, debug_state->height * 0.5f, -1.0f),
                        "DEBUG MODE", game_assets);
        }
        if (debug_state->frame_count)
        {
            char text_buffer[256];
            _snprintf_s(text_buffer, sizeof(text_buffer), 
                        "last frame time: %.02fms",
                        debug_state->frames[debug_state->frame_count - 1].wall_seconds_elapsed * 1000.0f);
            push_string(render_group,
                        _v3_(debug_state->width * 0.02f, debug_state->height * 0.8f, 0.0f),
                        text_buffer,
                        game_assets);
        }

        debug_state->profile_rect = _Rect2_(_v2_(50.0f, 50.0f), _v2_(200.0f, 200.0f));
        push_rect(debug_state->render_group, debug_state->profile_rect, 0.0f, _v4_(0, 0, 0, 0.25f));

        f32 bar_spacing = 4.0f;
        f32 lane_height = 0.0f;
        u32 lane_count = debug_state->frame_bar_lane_count;

        u32 max_frame = debug_state->frame_count;
        if (max_frame > 10)
        {
            max_frame = 10;
        }

        if (lane_count > 0 && max_frame > 0)
        {
            lane_height = ((get_height(debug_state->profile_rect) / (f32)max_frame) - bar_spacing) / (f32)lane_count;
        }

        f32 bar_height = lane_height * lane_count;
        f32 bars_plus_spacing = bar_height + bar_spacing;
        f32 chart_left = debug_state->profile_rect.min.x;
        f32 chart_height = bars_plus_spacing * (f32)max_frame;
        f32 chart_width = get_width(debug_state->profile_rect);
        f32 chart_top = debug_state->profile_rect.max.y;
        f32 scale = chart_width * debug_state->frame_bar_scale;

        v3 colors[] =
        {
            _v3_(1, 0, 0),
            _v3_(0, 1, 0),
            _v3_(0, 0, 1),
            _v3_(1, 1, 0),
            _v3_(0, 1, 1),
            _v3_(1, 0, 1),
            _v3_(1, 0.5f, 0),
            _v3_(1, 0, 0.5f),
            _v3_(0.5f, 1, 0),
            _v3_(0, 1, 0.5f),
            _v3_(0.5f, 0, 1),
            _v3_(0, 0.5f, 1),
        };


        for (u32 frame_idx = 0;
             frame_idx < max_frame;
             ++frame_idx)
        {
            Debug_Frame *frame = debug_state->frames + debug_state->frame_count - (frame_idx + 1);
            f32 stack_x = chart_left;
            f32 stack_y = chart_top - bars_plus_spacing * (f32)frame_idx;

            for (u32 region_idx = 0;
                 region_idx < frame->region_count;
                 ++region_idx)
            {
                Debug_Frame_Region *region = frame->regions + region_idx;

                // v3 color = colors[region_idx % array_count(colors)];
                v3 color = colors[region->color_idx % array_count(colors)];
                f32 this_min_x = stack_x + scale * region->min_t;
                f32 this_max_x = stack_x + scale * region->max_t;

                Rect2 region_rect = _Rect2_(_v2_(this_min_x, stack_y - lane_height * (region->lane_idx + 1)),
                                            _v2_(this_max_x, stack_y - lane_height * region->lane_idx));

                push_rect(render_group, region_rect, 0.0f, _v4_(color, 1.0f));

                if (is_in_rect(region_rect, mouse_p))
                {
                    Debug_Record *record = region->record;
                    char text_buffer[256];
                    _snprintf_s(text_buffer, sizeof(text_buffer), 
                                "%s: %10ucy [%s(%d)]",
                                record->block_name,
                                (u32)region->cycle_count,
                                record->file_name,
                                record->line_number);
                    push_string(render_group, _v3_(mouse_p, -1.0f), text_buffer, game_assets);

                    hot_record = record;
                }
            }
        }

        if (input->mouse.is_down[eMouse_Left] && 
            input->mouse.toggle[eMouse_Left])
        {
            if (hot_record)
            {
                debug_state->scope_to_record = hot_record;
            }
            else
            {
                debug_state->scope_to_record = 0;
            }
            refresh_collation(debug_state);
        }

        render_group_to_output_batch(debug_state->render_group, &g_debug_memory->render_batch);
        end_render(debug_state->render_group);
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

inline Debug_Record *
get_record_from(Open_Debug_Block *block)
{
    Debug_Record *result = block ? block->src : 0;
    return result;
}

internal void
collate_debug_records(Debug_State *debug_state, u32 invalid_event_array_idx)
{
    for (;
         ;
         ++debug_state->collation_array_idx)
    {
        if (debug_state->collation_array_idx == MAX_DEBUG_EVENT_ARRAY_COUNT)
        {
            debug_state->collation_array_idx = 0;
        }

        u32 event_array_idx = debug_state->collation_array_idx;
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
                if (debug_state->collation_frame)
                {
                    debug_state->collation_frame->end_clock = event->clock;
                    debug_state->collation_frame->wall_seconds_elapsed = event->seconds_elapsed;
                    ++debug_state->frame_count;

                    f32 clock_range = (f32)(debug_state->collation_frame->end_clock - debug_state->collation_frame->begin_clock);

#if 0
                    if (clock_range > 0.0f)
                    {
                        f32 frame_bar_scale = 1.0f / clock_range;
                        if (debug_state->frame_bar_scale > frame_bar_scale)
                        {
                            debug_state->frame_bar_scale = frame_bar_scale;
                        }
                    }
#endif
                }

                debug_state->collation_frame = debug_state->frames + debug_state->frame_count;
                debug_state->collation_frame->begin_clock = event->clock;
                debug_state->collation_frame->end_clock = 0;
                debug_state->collation_frame->region_count = 0;
                debug_state->collation_frame->regions = push_array(&debug_state->collate_arena, Debug_Frame_Region, MAX_REGIONS_PER_FRAME);
                debug_state->collation_frame->wall_seconds_elapsed = 0;
            }
            else if (debug_state->collation_frame)
            {
                u32 frame_idx = debug_state->frame_count - 1;
                Debug_Thread *thread = get_debug_thread(debug_state, event->tc.thread_id);
                u64 relative_clock = event->clock - debug_state->collation_frame->begin_clock;

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
                    debug_block->src = src;
                    thread->first_open_block = debug_block;
                    debug_block->next_free = 0;
                }
                else if (event->type == eDebug_Event_End_Block)
                {
                    if (thread->first_open_block)
                    {
                        Open_Debug_Block *matching_block = thread->first_open_block;
                        Debug_Event *opening_event = matching_block->opening_event;
                        if (opening_event->tc.thread_id == event->tc.thread_id &&
                            opening_event->debug_record_idx == event->debug_record_idx &&
                            opening_event->translation_unit == event->translation_unit)
                        {
                            if (matching_block->starting_frame_idx == frame_idx)
                            {
                                if (get_record_from(matching_block->parent) == debug_state->scope_to_record)
                                {
                                    f32 min_t = (f32)(opening_event->clock - debug_state->collation_frame->begin_clock);
                                    f32 max_t = (f32)(event->clock - debug_state->collation_frame->begin_clock);
                                    f32 threshold_t = 0.01f;
                                    if ((max_t - min_t) > threshold_t)
                                    {
                                        Debug_Frame_Region *region = add_region(debug_state, debug_state->collation_frame);
                                        region->record = src;
                                        region->cycle_count = (event->clock - opening_event->clock);
                                        region->lane_idx = (u16)thread->lane_idx;
                                        region->min_t = min_t;
                                        region->max_t = max_t;
                                        region->color_idx = (u16)opening_event->debug_record_idx;
                                    }
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
}

internal void
restart_collation(Debug_State *debug_state, u32 invalid_event_array_idx)
{
    end_temporary_memory(&debug_state->collate_tmp);
    debug_state->collate_tmp = begin_temporary_memory(&debug_state->collate_arena);

    debug_state->first_thread = 0;
    debug_state->first_free_block = 0;

    debug_state->frames = push_array(&debug_state->collate_arena, Debug_Frame, MAX_DEBUG_EVENT_ARRAY_COUNT * 4);
    debug_state->frame_bar_lane_count = 0;
    debug_state->frame_count = 0;
    debug_state->frame_bar_scale = 1.0f / 60000000.0f;

    debug_state->collation_array_idx = invalid_event_array_idx + 1;
    debug_state->collation_frame = 0;
}

internal void
refresh_collation(Debug_State *debug_state)
{
    restart_collation(debug_state, g_debug_table->current_event_array_idx);
    collate_debug_records(debug_state, g_debug_table->current_event_array_idx);
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

    Debug_State *debug_state = get_debug_state(game_memory);
    if (debug_state)
    {
        if (!debug_state->paused)
        {
            if (debug_state->frame_count >= MAX_DEBUG_EVENT_ARRAY_COUNT * 4)
            {
                restart_collation(debug_state, g_debug_table->current_event_array_idx);
            }
            collate_debug_records(debug_state, g_debug_table->current_event_array_idx);
        }
    }

    return g_debug_table;
}
