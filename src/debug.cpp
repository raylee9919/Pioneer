/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Sung Woo Lee $
   $Notice: (C) Copyright 2024 by Sung Woo Lee. All Rights Reserved. $
   ======================================================================== */

// TODO: stop using stdio
#include <stdio.h>

#include "debug_variables.h"

internal void restart_collation(Debug_State *debug_state, u32 invalid_event_array_idx);

inline Debug_State *
debug_get_state(Game_Memory *memory)
{
    Debug_State *debug_state = (Debug_State *)memory->debug_storage;
    Assert(debug_state->init);

    return debug_state;
}

inline Debug_State *
debug_get_state(void)
{
    Debug_State *result = debug_get_state(g_debug_memory);
    return result;
}

internal void
debug_start(u32 width, u32 height, f32 v_advance)
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

            Debug_Variable_Definition_Context context = {};
            context.state = debug_state;
            context.arena = &debug_state->debug_arena;
            context.group = debug_begin_variable_group(&context, "Root");

            debug_begin_variable_group(&context, "Debugging");

            debug_create_variables(&context);
            debug_begin_variable_group(&context, "Profile");
            debug_begin_variable_group(&context, "By Thread");
            Debug_Variable *thread_list = 
                debug_add_variable(&context, eDebug_Variable_Type_Counter_Thread_List, "");
            thread_list->profile.dimension = _v2_(1024.0f, 100.0f);
            debug_end_variable_group(&context);
            debug_begin_variable_group(&context, "By Function");
            Debug_Variable *function_list = 
                debug_add_variable(&context, eDebug_Variable_Type_Counter_Thread_List, "");
            function_list->profile.dimension = _v2_(1024.0f, 200.0f);
            debug_end_variable_group(&context);
            debug_end_variable_group(&context);

            debug_end_variable_group(&context);

            debug_state->root_group = context.group;

            Camera *debug_overlay_camera = push_camera(&debug_state->debug_arena, eCamera_Type_Orthographic, (f32)width, (f32)height);
            debug_state->render_group = alloc_render_group(&debug_state->debug_arena, MB(16),
                                                           debug_overlay_camera);
            debug_state->game_assets = &((Transient_State *)g_debug_memory->transient_memory)->game_assets;

            debug_state->paused = false;
            debug_state->scope_to_record = 0;

            debug_state->init = true;

            init_sub_arena(&debug_state->collate_arena, &debug_state->debug_arena, MB(32));
            debug_state->collate_tmp = begin_temporary_memory(&debug_state->collate_arena);

            restart_collation(debug_state, 0);
        }

        begin_render(debug_state->render_group);

        debug_state->left_edge = 0.0f;
        debug_state->left_edge = (f32)width;
        debug_state->at_y = 0.5f * height;
        debug_state->width = (f32)width;
        debug_state->height = (f32)height;

        debug_state->at_y = (f32)height;
        debug_state->left_edge = 0.0f;

        debug_state->hierarchy.group = debug_state->root_group;
        debug_state->hierarchy.ui_p = _v2_(debug_state->left_edge, debug_state->at_y);
    }
}

internal void
debug_text_line(char *string)
{
    Debug_State *debug_state = debug_get_state();
    if (debug_state)
    {
        Render_Group *render_group = debug_state->render_group;
        Game_Assets *game_assets = debug_state->game_assets;
        Assert(game_assets);
        f32 line_advance = (f32)game_assets->v_advance;

        string_op(eString_Op_Draw, render_group,
                  _v3_(debug_state->left_edge,
                       debug_state->at_y - line_advance,
                       0.0f),
                  string, game_assets, _v4_(1, 1, 1, 1));
        debug_state->at_y -= game_assets->v_advance;
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

internal size_t
debug_variable_to_text(char *buffer, char *end, Debug_Variable *var, u32 flags)
{
    char *at = buffer;

    if (flags & eDebug_Var_To_Text_Add_Debug_UI)
    {
        at += _snprintf_s(at, (size_t)(end - at), (size_t)(end - at),
                          "#define DEBUG_UI_");
    }

    if (flags & eDebug_Var_To_Text_Add_Name)
    {
        at += _snprintf_s(at, (size_t)(end - at), (size_t)(end - at),
                          "%s%s ", var->name, (flags & eDebug_Var_To_Text_Colon) ? ":" : "");
    }

    switch (var->type)
    {
        case eDebug_Variable_Type_b32:
        {
            if (flags & eDebug_Var_To_Text_Pretty_Bools)
            {
            at += _snprintf_s(at, (size_t)(end - at), (size_t)(end - at),
                              "%s", var->bool32 ? "true" : "false");
            }
            else
            {
                at += _snprintf_s(at, (size_t)(end - at), (size_t)(end - at),
                                  "%d", var->bool32);
            }
        } break;

        case eDebug_Variable_Type_s32:
        {
            at += _snprintf_s(at, (size_t)(end - at), (size_t)(end - at),
                              "%d", var->int32);
        } break;

        case eDebug_Variable_Type_u32:
        {
            at += _snprintf_s(at, (size_t)(end - at), (size_t)(end - at),
                              "%u", var->uint32);
        } break;

        case eDebug_Variable_Type_f32:
        {
            at += _snprintf_s(at, (size_t)(end - at), (size_t)(end - at),
                              "%f", var->float32);
            if (flags & eDebug_Var_To_Text_Float_Suffix)
            {
                *at++ = 'f';
            }
        } break;

        case eDebug_Variable_Type_v2:
        {
            at += _snprintf_s(at, (size_t)(end - at), (size_t)(end - at),
                              "_v2_(%f, %f)", var->vector2.x, var->vector2.y);
        } break;

        case eDebug_Variable_Type_v3:
        {
            at += _snprintf_s(at, (size_t)(end - at), (size_t)(end - at),
                              "_v3_(%f, %f, %f)", var->vector3.x, var->vector3.y, var->vector3.z);
        } break;

        case eDebug_Variable_Type_v4:
        {
            at += _snprintf_s(at, (size_t)(end - at), (size_t)(end - at),
                              "_v4_(%f, %f, %f, %f)", var->vector4.r, var->vector4.g, var->vector4.b, var->vector4.a);
        } break;

        case eDebug_Variable_Type_Group:
        {
        } break;

        INVALID_DEFAULT_CASE;
    }

    if (flags & eDebug_Var_To_Text_Line_Feed_End)
    {
        *at++ = '\n';
    }

    if (flags & eDebug_Var_To_Text_Null_Terminator)
    {
        *at++ = 0;
    }

    return (at - buffer);
}

internal void
write_config(Debug_State *debug_state)
{
    // TODO: need a giant buffer here.
    char tmp[4096];
    char *at = tmp;
    char *end = tmp + sizeof(tmp);
    
    int depth = 0;
    Debug_Variable *var = debug_state->root_group->group.first_child;

    while (var)
    {
        if (debug_should_be_written(var->type))
        {
            for (int indent = 0;
                 indent < depth;
                 ++indent)
            {
                *at++ = ' ';
                *at++ = ' ';
                *at++ = ' ';
                *at++ = ' ';
            }

            if (var->type == eDebug_Variable_Type_Group)
            {
                at += _snprintf_s(at, (size_t)(end - at), (size_t)(end - at),
                                  "// ");
            }
            at += debug_variable_to_text(at, end, var, 
                                         eDebug_Var_To_Text_Add_Debug_UI|
                                         eDebug_Var_To_Text_Add_Name|
                                         eDebug_Var_To_Text_Float_Suffix|
                                         eDebug_Var_To_Text_Line_Feed_End);
        }

        if (var->type == eDebug_Variable_Type_Group)
        {
            var = var->group.first_child;
            ++depth;
        }
        else
        {
            while (var)
            {
                if (var->next)
                {
                    var = var->next;
                    break;
                }
                else
                {
                    var = var->parent;
                    --depth;
                }
            }
        }

    }

    Platform_API *platform = &g_debug_memory->platform; 
    platform->debug_platform_write_file("../src/config.h", (u32)(at - tmp), tmp);

    if (!debug_state->compiling)
    {
        debug_state->compiling = true;
        debug_state->compiler = platform->debug_platform_execute_system_command("../src",
                                                                                "c:/windows/system32/cmd.exe",
                                                                                "/C build.bat");
    }
    
}

internal void
draw_profile_in(Debug_State *debug_state, Rect2 profile_rect, v2 mouse_p)
{
    Game_Assets *game_assets = debug_state->game_assets;

    push_rect(debug_state->render_group, profile_rect, 0.0f, _v4_(0, 0, 0, 0.25f));

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
        lane_height = ((get_height(profile_rect) / (f32)max_frame) - bar_spacing) / (f32)lane_count;
    }

    f32 bar_height = lane_height * lane_count;
    f32 bars_plus_spacing = bar_height + bar_spacing;
    f32 chart_left = profile_rect.min.x;
    f32 chart_height = bars_plus_spacing * (f32)max_frame;
    f32 chart_width = get_width(profile_rect);
    f32 chart_top = profile_rect.max.y;
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

            Rect2 region_rect = rect2_min_max(_v2_(this_min_x, stack_y - lane_height * (region->lane_idx + 1)),
                                              _v2_(this_max_x, stack_y - lane_height * region->lane_idx));

            push_rect(debug_state->render_group, region_rect, 0.0f, _v4_(color, 1.0f));

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
                string_op(eString_Op_Draw, debug_state->render_group, _v3_(mouse_p, -1.0f), text_buffer, game_assets);

                // hot_record = record;
            }
        }
    }
}

internal void
debug_draw_main_menu(Debug_State *debug_state, Game_Assets *game_assets, v2 menu_p, v2 mouse_p)
{
    f32 at_x = debug_state->hierarchy.ui_p.x;
    f32 at_y = debug_state->hierarchy.ui_p.y;

    debug_state->next_hot = 0;

    f32 spacing_y = 4.0f;
    int depth = 0;
    Debug_Variable *var = debug_state->hierarchy.group->group.first_child;
    f32 line_advance = (f32)game_assets->v_advance;

    while (var)
    {
        b32 is_hot = (debug_state->hot == var);
        v4 item_color = (is_hot && debug_state->hot_interaction == 0) ? _v4_(1, 1, 0, 1) : _v4_(1, 1, 1, 1);

        Rect2 bounds = {};
        switch (var->type)
        {
            case eDebug_Variable_Type_Counter_Thread_List:
            {
                v2 min_corner = _v2_(at_x + depth * 2.0f * line_advance, at_y - var->profile.dimension.y);
                v2 max_corner = _v2_(min_corner.x + var->profile.dimension.x, at_y);
                v2 size_p = _v2_(max_corner.x, min_corner.y);
                bounds = rect2_min_max(min_corner, max_corner);
                draw_profile_in(debug_state, bounds, mouse_p);

                Rect2 size_box = rect2_cen_half_dim(size_p, _v2_(4.0f, 4.0f));
                push_rect(debug_state->render_group, size_box, 0.0f,
                          (is_hot && debug_state->hot_interaction == eDebug_Interaction_Resize_Profile) ?
                          _v4_(1, 1, 0, 1) : _v4_(1, 1, 1, 1));

                if (is_in_rect(size_box, mouse_p))
                {
                    debug_state->next_hot_interaction = eDebug_Interaction_Resize_Profile;
                    debug_state->next_hot = var;
                }
                else if (is_in_rect(bounds, mouse_p))
                {
                    debug_state->next_hot_interaction = eDebug_Interaction_None;
                    debug_state->next_hot = var;
                }
            } break;

            default:
            {
                char text[256];
                debug_variable_to_text(text, text + sizeof(text), var, 
                                       eDebug_Var_To_Text_Add_Name|
                                       eDebug_Var_To_Text_Null_Terminator|
                                       eDebug_Var_To_Text_Colon|
                                       eDebug_Var_To_Text_Pretty_Bools);

                f32 left_p_x = at_x + depth * 2.0f * line_advance;
                f32 top_p_y = at_y;
                bounds = string_op(eString_Op_Get_Rect2, debug_state->render_group, _v3_(0, 0, 0), text, game_assets);
                bounds = offset(bounds, _v2_(left_p_x, top_p_y - get_dim(bounds).y));

                string_op(eString_Op_Draw,
                          debug_state->render_group,
                          _v3_(_v2_(left_p_x, top_p_y - get_dim(bounds).y), 0.0f),
                          text,
                          game_assets,
                          item_color);

                if (is_in_rect(bounds, mouse_p))
                {
                    debug_state->next_hot_interaction = eDebug_Interaction_None;
                    debug_state->next_hot = var;
                }
            } break;
        }

        at_y = (bounds.min.y - spacing_y);

        if (var->type == eDebug_Variable_Type_Group &&
            var->group.expanded)
        {
            var = var->group.first_child;
            ++depth;
        }
        else
        {
            while (var)
            {
                if (var->next)
                {
                    var = var->next;
                    break;
                }
                else
                {
                    var = var->parent;
                    --depth;
                }
            }
        }
    }

    debug_state->at_y = at_y;

#if 0
    u32 new_hot_menu_idx = array_count(debug_variable_list);
    f32 best_distance_sq = F32_MAX;

    f32 menu_radius = 300.0f;
    f32 dT = (2.0f * pi32) / (f32)array_count(debug_variable_list);

    for (u32 menu_item_idx = 0;
         menu_item_idx < array_count(debug_variable_list);
         ++menu_item_idx)
    {
        Debug_Variable *var = debug_variable_list + menu_item_idx;
        char *text = var->name;

        v4 item_color = var->value ? _v4_(1, 1, 1, 1) : _v4_(0.5f, 0.5f, 0.5f, 1.0f);
        if (menu_item_idx == debug_state->hot_menu_idx)
        {
            item_color = _v4_(1, 1, 0, 1);
        }
        v2 dim = string_op(eString_Op_Get_Dim, debug_state->render_group, _v3_(0,0,0), text, game_assets);
        
        f32 T = (f32)menu_item_idx * dT;
        v2 text_p = menu_p + menu_radius * arm2(T);
        f32 this_dist_sq = len_square(mouse_p - text_p);
        if (this_dist_sq < best_distance_sq)
        {
            new_hot_menu_idx = menu_item_idx;
            best_distance_sq = this_dist_sq;
        }
        string_op(eString_Op_Draw, debug_state->render_group, _v3_(text_p - 0.5f * dim, -1.0f), text, game_assets, item_color);
    }

    if (len_square(mouse_p - menu_p) >= square(menu_radius))
    {
        debug_state->hot_menu_idx = new_hot_menu_idx;
    }
    else
    {
        debug_state->hot_menu_idx = array_count(debug_variable_list);
    }
#endif
}

internal void
debug_begin_interact(Debug_State *debug_state, Game_Input *input, v2 mouse_p)
{
    if (debug_state->hot)
    {
        if (debug_state->hot_interaction)
        {
            debug_state->interaction = debug_state->hot_interaction;
        }
        else
        {
            switch (debug_state->hot->type)
            {
                case eDebug_Variable_Type_b32:
                {
                    debug_state->interaction = eDebug_Interaction_Toggle_Value;
                } break;

                case eDebug_Variable_Type_f32:
                {
                    debug_state->interaction = eDebug_Interaction_Drag_Value;
                } break;

                case eDebug_Variable_Type_Group:
                {
                    debug_state->interaction = eDebug_Interaction_Toggle_Value;
                } break;
            }
        }

        if (debug_state->interaction)
        {
            debug_state->interacting_with = debug_state->hot;
        }
    }
    else
    {
        debug_state->interaction = eDebug_Interaction_NOP;
    }
}

internal void
debug_end_interact(Debug_State *debug_state, Game_Input *input, v2 mouse_p)
{
    if (debug_state->interaction != eDebug_Interaction_NOP)
    {
        Debug_Variable *var = debug_state->interacting_with;
        Assert(var);

        switch (debug_state->interaction)
        {
            case eDebug_Interaction_Toggle_Value:
            {
                switch (var->type)
                {
                    case eDebug_Variable_Type_b32:
                    {
                        var->bool32 = !var->bool32;
                    } break;

                    case eDebug_Variable_Type_Group:
                    {
                        var->group.expanded = !var->group.expanded;
                    } break;
                }
            } break;

            case eDebug_Interaction_Tear_Value:
            {
            } break;
        }

        write_config(debug_state);
    }

    debug_state->interaction = eDebug_Interaction_None;
    debug_state->interacting_with = 0;
}

internal void
debug_interact(Debug_State *debug_state, Game_Input *input, v2 mouse_p, Game_Assets *game_assets)
{
    v2 d_mouse_p = (mouse_p - debug_state->last_mouse_p);

    /*
       if (input->mouse.is_down[eMouse_Right])
       {
       if (input->mouse.toggle[eMouse_Right])
       {
       debug_state->menu_p = mouse_p;
       }
       debug_draw_main_menu(debug_state, game_assets, debug_state->menu_p, mouse_p);
       }
    */

    if (debug_state->interaction)
    {
        Debug_Variable *var = debug_state->interacting_with;

        switch (debug_state->interaction)
        {
            case eDebug_Interaction_Drag_Value:
            {
                switch (var->type)
                {
                    case eDebug_Variable_Type_f32:
                    {
                        var->float32 += 0.1f * d_mouse_p.x;
                    } break;
                }
            } break;

            case eDebug_Interaction_Resize_Profile:
            {
                var->profile.dimension += _v2_(d_mouse_p.x, -d_mouse_p.y);
                var->profile.dimension.x = maximum(var->profile.dimension.x, 10.0f);
                var->profile.dimension.y = maximum(var->profile.dimension.y, 10.0f);
            } break;
        }

        // click interaction.
        if (!input->mouse.is_down[eMouse_Left] &&
            input->mouse.toggle[eMouse_Left])
        {
            debug_end_interact(debug_state, input, mouse_p);
        }
    }
    else
    {
        debug_state->hot = debug_state->next_hot;
        debug_state->hot_interaction = debug_state->next_hot_interaction;

        // TODO: revise as mouse half transition counts;
        if (input->mouse.is_down[eMouse_Left])
        {
            debug_begin_interact(debug_state, input, mouse_p);
        }
        else if (input->mouse.toggle[eMouse_Left])
        {
            debug_begin_interact(debug_state, input, mouse_p);
            debug_end_interact(debug_state, input, mouse_p);
        }
    }

    debug_state->last_mouse_p = mouse_p;
}

internal void
debug_end(Game_Input *input, Game_Assets *game_assets)
{
    TIMED_FUNCTION();

    Debug_State *debug_state = debug_get_state();
    if (debug_state)
    {
        Render_Group *render_group = debug_state->render_group;

        debug_state->next_hot = 0;
        debug_state->next_hot_interaction = eDebug_Interaction_None;
        Debug_Record *hot_record = 0;

        v2 mouse_p = input->mouse.P;
        debug_draw_main_menu(debug_state, game_assets, debug_state->menu_p, mouse_p);
        debug_interact(debug_state, input, mouse_p, game_assets);

        if (debug_state->compiling)
        {
            Debug_Process_State state = g_debug_memory->platform.debug_platform_get_process_state(debug_state->compiler);
            if (state.is_running)
            {
                debug_text_line("COMPILING");
            }
            else
            {
                debug_state->compiling = false;
            }
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

        if (debug_state->frame_count)
        {
            char text_buffer[256];
            _snprintf_s(text_buffer, sizeof(text_buffer), 
                        "last frame time: %.02fms",
                        debug_state->frames[debug_state->frame_count - 1].wall_seconds_elapsed * 1000.0f);
            debug_text_line(text_buffer);
        }

        push_rect(debug_state->render_group, rect2_min_max(_v2_(10, 10), _v2_(40, 40)), 0.0f, _v4_(DEBUG_UI_COLOR, 0, 1, 1));


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

    Debug_State *debug_state = debug_get_state(game_memory);
    if (debug_state)
    {
        if (game_memory->executable_reloaded)
        {
            restart_collation(debug_state, g_debug_table->current_event_array_idx);
        }

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
