/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Sung Woo Lee $
   $Notice: (C) Copyright 2024 by Sung Woo Lee. All Rights Reserved. $
   ======================================================================== */

// TODO: stop using stdio
#include <stdio.h>

#include "debug.h"

inline b32 arena_has_room_for(Memory_Arena *arena, size_t size);

inline b32
debug_ids_are_equal(Debug_ID a, Debug_ID b)
{
    b32 result = (a.value[0] == b.value[0] &&
                  a.value[1] == b.value[1]);
    return result;
}

inline Debug_ID
debug_id_from_link(Debug_Tree *tree, Debug_Variable_Link *link)
{
    Debug_ID result = {};

    result.value[0] = tree;
    result.value[1] = link;

    return result;
}

inline Debug_ID
debug_id_from_guid(Debug_Tree *tree, char *guid)
{
    Debug_ID result = {};

    result.value[0] = tree;
    result.value[1] = guid;

    return result;
}

inline Debug_State *
debug_get_state(Game_Memory *memory)
{
    Debug_State *debug_state = 0;
    if (memory)
    {
        debug_state = (Debug_State *)memory->debug_storage;
        if (!debug_state->init)
        {
            debug_state = 0;
        }
    }

    return debug_state;
}

inline Debug_State *
debug_get_state(void)
{
    Debug_State *result = debug_get_state(g_debug_memory);

    return result;
}

internal Debug_Tree *
add_tree(Debug_State *debug_state, Debug_Variable_Group *group, v2 at_p)
{
    Debug_Tree *tree = push_struct(&debug_state->debug_arena, Debug_Tree);

    tree->ui_p = at_p;
    tree->group = group;

    DLIST_INSERT(&debug_state->tree_sentinel, tree);

    return tree;
}

internal void
debug_text_line(char *string)
{
    Debug_State *debug_state = debug_get_state();
    if (debug_state)
    {
        Render_Group *render_group = debug_state->render_group;
        Font *font = debug_state->font;
        f32 line_advance = (f32)font->v_advance;

        string_op(String_Op::DRAW, render_group,
                  _v3_(debug_state->left_edge,
                       debug_state->at_y - line_advance, 0.0f),
                  string, font, _v4_(1, 1, 1, 1));
        debug_state->at_y -= font->v_advance;
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
debug_event_to_text(char *buffer, char *end, Debug_Event *event, u32 flags)
{
    char *at = buffer;
    char *name = event->block_name;

    if (flags & eDebug_Var_To_Text_Add_Debug_UI)
    {
        at += _snprintf_s(at, (size_t)(end - at), (size_t)(end - at),
                          "#define DEBUG_UI_");
    }

    if (flags & eDebug_Var_To_Text_Add_Name)
    {
        at += _snprintf_s(at, (size_t)(end - at), (size_t)(end - at),
                          "%s%s ", name, (flags & eDebug_Var_To_Text_Colon) ? ":" : "");
    }

    switch (event->type)
    {
        case eDebug_Type_b32:
        {
            if (flags & eDebug_Var_To_Text_Pretty_Bools)
            {
                at += _snprintf_s(at, (size_t)(end - at), (size_t)(end - at),
                                  "%s", event->value_b32 ? "true" : "false");
            }
            else
            {
                at += _snprintf_s(at, (size_t)(end - at), (size_t)(end - at),
                                  "%d", event->value_b32);
            }
        } break;

        case eDebug_Type_s32:
        {
            at += _snprintf_s(at, (size_t)(end - at), (size_t)(end - at),
                              "%d", event->value_s32);
        } break;

        case eDebug_Type_u32:
        {
            at += _snprintf_s(at, (size_t)(end - at), (size_t)(end - at),
                              "%u", event->value_u32);
        } break;

        case eDebug_Type_f32:
        {
            at += _snprintf_s(at, (size_t)(end - at), (size_t)(end - at),
                              "%f", event->value_f32);
            if (flags & eDebug_Var_To_Text_Float_Suffix)
            {
                *at++ = 'f';
            }
        } break;

        case eDebug_Type_v2:
        {
            at += _snprintf_s(at, (size_t)(end - at), (size_t)(end - at),
                              "_v2_(%f, %f)", event->value_v2.x, event->value_v2.y);
        } break;

        case eDebug_Type_v3:
        {
            at += _snprintf_s(at, (size_t)(end - at), (size_t)(end - at),
                              "_v3_(%f, %f, %f)", event->value_v3.x, event->value_v3.y, event->value_v3.z);
        } break;

        case eDebug_Type_v4:
        {
            at += _snprintf_s(at, (size_t)(end - at), (size_t)(end - at),
                              "_v4_(%f, %f, %f, %f)", event->value_v4.r, event->value_v4.g, event->value_v4.b, event->value_v4.a);
        } break;

        case eDebug_Type_Rect2:
        {
            at += _snprintf_s(at, (size_t)(end - at), (size_t)(end - at),
                              "Rect2(%f, %f -> %f, %f)",
                              event->value_Rect2.min.x,
                              event->value_Rect2.min.y,
                              event->value_Rect2.max.x,
                              event->value_Rect2.max.y);
        } break;

        case eDebug_Type_Rect3:
        {
            at += _snprintf_s(at, (size_t)(end - at), (size_t)(end - at),
                              "Rect3(cen:(%f, %f, %f), dim:(%f, %f, %f))",
                              event->value_Rect3.cen.x,
                              event->value_Rect3.cen.y,
                              event->value_Rect3.cen.z,
                              event->value_Rect3.dim.x,
                              event->value_Rect3.dim.y,
                              event->value_Rect3.dim.z);
        } break;

        case eDebug_Type_Counter_Thread_List:
        case eDebug_Type_Bitmap:
        {
        } break;
        case eDebug_Type_Open_Data_Block:
        {
            at += _snprintf_s(at, (size_t)(end - at), (size_t)(end - at),
                              "%s", event->block_name);
        } break;

        default:
        {
            at += _snprintf_s(at, (size_t)(end - at), (size_t)(end - at),
                              "UNHANDLED: %s", event->block_name);
        } break;
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

struct Debug_Variable_Iterator
{
    Debug_Variable_Link *link;
    Debug_Variable_Link *sentinel;
};

internal void
write_config(Debug_State *debug_state)
{
#if 0
    // TODO: need a giant buffer here.
    char tmp[4096];
    char *at = tmp;
    char *end = tmp + sizeof(tmp);
    
    int depth = 0;
    Debug_Variable_Iterator stack[DEBUG_MAX_VARIABLE_STACK_DEPTH];

    stack[depth].link = debug_state->root_group->var_group.next;
    stack[depth].sentinel = &debug_state->root_group->var_group;
    ++depth;
    while (depth > 0)
    {
        Debug_Variable_Iterator *iter = stack + (depth - 1);
        if (iter->link == iter->sentinel)
        {
            --depth;
        }
        else
        {
            Debug_Variable *var = iter->link->var;
            iter->link = iter->link->next;

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

                if (var->type == eDebug_Type_Var_Group)
                {
                    at += _snprintf_s(at, (size_t)(end - at), (size_t)(end - at),
                                      "// ");
                }
                at += debug_event_to_text(at, end, var, 
                                             eDebug_Var_To_Text_Add_Debug_UI|
                                             eDebug_Var_To_Text_Add_Name|
                                             eDebug_Var_To_Text_Float_Suffix|
                                             eDebug_Var_To_Text_Line_Feed_End);
            }

            if (var->type == eDebug_Type_Var_Group)
            {
                iter = stack + depth;
                iter->link = var->var_group.next;
                iter->sentinel = &var->var_group;
                ++depth;
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
#endif
}

internal void
draw_profile_in(Debug_State *debug_state, Rect2 profile_rect, v2 mouse_p)
{
    Game_Assets *game_assets = debug_state->game_assets;

    push_rect(debug_state->render_group, profile_rect, 0.0f, _v4_(0, 0, 0, 0.25f));

    f32 bar_spacing = 4.0f;
    f32 lane_height = 0.0f;
    u32 lane_count = debug_state->frame_bar_lane_count;
    f32 frame_bar_scale = F32_MAX;

    u32 frame_idx = 0;
    for (Debug_Frame *frame = debug_state->oldest_frame;
         frame;
         frame = frame->next, ++frame_idx)
    {
        if (frame_bar_scale < frame->frame_bar_scale)
        {
            frame_bar_scale = frame->frame_bar_scale;
        }
    }

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
    f32 scale = chart_width * frame_bar_scale;

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


#if 0
    u32 frame_idx = 0;
    for (Debug_Frame *frame = debug_state->oldest_frame;
         frame;
         frame = frame->next, ++frame_idx)
    {
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
                Debug_Event *event = region->event;
                char text_buffer[256];
                _snprintf_s(text_buffer, sizeof(text_buffer), 
                            "%s: %10ucy [%s(%d)]",
                            event->block_name,
                            (u32)region->cycle_count,
                            event->file_name,
                            event->line_number);
                string_op(eString_Op_Draw, debug_state->render_group, _v3_(mouse_p, -1.0f), text_buffer, game_assets);

                // hot_record = record;
            }
        }
    }
#endif
}

inline b32
interactions_are_equal(Debug_Interaction a, Debug_Interaction b)
{
    b32 result = (debug_ids_are_equal(a.id, b.id) &&
                  (a.type == b.type) &&
                  (a.generic == b.generic));
    return result;
}

inline b32
interaction_is_hot(Debug_State *debug_state, Debug_Interaction interaction)
{
    b32 result = interactions_are_equal(debug_state->hot_interaction, interaction);
    return result;
}

struct Layout
{
    Debug_State *debug_state;
    v2 mouse_p;
    v2 at;
    int depth;
    f32 line_advance;
    f32 spacing_y;
};

struct Layout_Element
{
    // storage
    Layout *layout;
    v2 *dim;
    v2 *size;
    Debug_Interaction interaction;

    // out
    Rect2 bounds;
};

inline Layout_Element
begin_element_rect(Layout *layout, v2 *dim)
{
    Layout_Element element = {};

    element.layout = layout;
    element.dim = dim;

    return element;
}

inline void
make_element_sizeable(Layout_Element *element)
{
    element->size = element->dim;
}

inline void
default_interaction(Layout_Element *element, Debug_Interaction interaction)
{
    element->interaction = interaction;
}

inline void
end_element(Layout_Element *element)
{
    Layout *layout = element->layout;
    Debug_State *debug_state = layout->debug_state;

    f32 size_handle_pixels = 4.0f;

    v2 frame = {0, 0};
    if (element->size)
    {
        frame.x = size_handle_pixels;
        frame.y = size_handle_pixels;
    }

    v2 total_dim = *element->dim + 2.0f * frame;

    v2 total_min_corner = _v2_(layout->at.x + layout->depth * 2.0f * layout->line_advance,
                               layout->at.y - total_dim.y);
    v2 total_max_corner = total_min_corner + total_dim;

    v2 interior_min_corner = total_min_corner + frame;
    v2 interior_max_corner = interior_min_corner + *element->dim;

    Rect2 total_bounds = rect2_min_max(total_min_corner, total_max_corner);
    element->bounds = rect2_min_max(interior_min_corner, interior_max_corner);

    if (element->interaction.type && is_in_rect(element->bounds, layout->mouse_p))
    {
        debug_state->next_hot_interaction = element->interaction;
    }

    if (element->size)
    {
        push_rect(debug_state->render_group, rect2_min_max(_v2_(total_min_corner.x, interior_min_corner.y),
                                                           _v2_(interior_min_corner.x, interior_max_corner.y)), 0.0f,
                  _v4_(0, 0, 0, 1));
        push_rect(debug_state->render_group, rect2_min_max(_v2_(interior_max_corner.x, interior_min_corner.y),
                                                           _v2_(total_max_corner.x, interior_max_corner.y)), 0.0f,
                  _v4_(0, 0, 0, 1));
        push_rect(debug_state->render_group, rect2_min_max(_v2_(interior_min_corner.x, total_min_corner.y),
                                                           _v2_(interior_max_corner.x, interior_min_corner.y)), 0.0f,
                  _v4_(0, 0, 0, 1));
        push_rect(debug_state->render_group, rect2_min_max(_v2_(interior_min_corner.x, interior_max_corner.y),
                                                           _v2_(interior_max_corner.x, total_max_corner.y)), 0.0f,
                  _v4_(0, 0, 0, 1));

        Debug_Interaction size_interaction = {};
        size_interaction.type = eDebug_Interaction_Resize;
        size_interaction.p = element->size;

        Rect2 size_box = rect2_min_max(_v2_(interior_max_corner.x, total_min_corner.y),
                                       _v2_(total_max_corner.x, interior_min_corner.y));
        push_rect(debug_state->render_group, size_box, 0.0f,
                  interaction_is_hot(debug_state, size_interaction) ? _v4_(1, 1, 0, 1) : _v4_(1, 1, 1, 1));
        if (is_in_rect(size_box, layout->mouse_p))
        {
            debug_state->next_hot_interaction = size_interaction; 
        }
    }

    f32 spacing_y = layout->spacing_y;
    if (0)
    {
        spacing_y = 0.0f;
    }
    layout->at.y = total_bounds.min.y - spacing_y;
}


internal void
free_variable_group(Debug_State *debug_state, Debug_Variable_Group *group)
{
    Assert(!"not implemented");
}

internal Debug_Variable_Link *
add_element_to_group(Debug_State *debug_state, Debug_Variable_Group *parent, Debug_Element *element)
{
    Debug_Variable_Link *link = push_struct(&debug_state->debug_arena, Debug_Variable_Link);

    DLIST_INSERT(&parent->sentinel, link);
    link->children = 0;
    link->element = element;

    return link;
}

internal Debug_Variable_Link *
add_group_to_group(Debug_State *debug_state, Debug_Variable_Group *parent, Debug_Variable_Group *group)
{
    Debug_Variable_Link *link = push_struct(&debug_state->debug_arena, Debug_Variable_Link);

    DLIST_INSERT(&parent->sentinel, link);
    link->children = group;
    link->element = 0;

    return link;
}

internal Debug_Variable_Group *
create_variable_group(Debug_State *debug_state, u32 name_length, char *name)
{
    Debug_Variable_Group *group = push_struct(&debug_state->debug_arena, Debug_Variable_Group);
    DLIST_INIT(&group->sentinel);

    group->name_length = name_length;
    group->name = name;

    return group;
}


internal Debug_View *
get_or_create_debug_view_for(Debug_State *debug_state, Debug_ID id)
{
    // TODO: better hash function!
    u32 hash_idx = (u32)((((u64)id.value[0] >> 2) + ((u64)id.value[1] >> 2)) % array_count(debug_state->view_hash));
    Debug_View **hash_slot = debug_state->view_hash + hash_idx;

    Debug_View *result = 0;
    for (Debug_View *search = *hash_slot;
         search;
         search = search->next_in_hash)
    {
        if (debug_ids_are_equal(search->id, id))
        {
            result = search;
            break;
        }
    }

    if (!result)
    {
        result = push_struct(&debug_state->debug_arena, Debug_View);
        result->id = id;
        result->type = eDebug_View_Type_Unknown;
        result->next_in_hash = *hash_slot;
        *hash_slot = result;
    }

    return result;
}

inline Debug_Interaction
event_interaction(Debug_State *debug_state, Debug_ID debug_id, Debug_Interaction_Type type, Debug_Event *event)
{
    Debug_Interaction item_interaction = {};
    item_interaction.id = debug_id;
    item_interaction.type = type;
    item_interaction.event = event;

    return item_interaction;
}

inline Debug_Interaction
debug_id_interaction(Debug_Interaction_Type type, Debug_ID id)
{
    Debug_Interaction item_interaction = {};
    item_interaction.id = id;
    item_interaction.type = type;

    return item_interaction;
}

internal void
DEBUG_HIT(Debug_ID id, f32 z_value)
{
    Debug_State *debug_state = debug_get_state();
    if (debug_state)
    {
        debug_state->next_hot_interaction = debug_id_interaction(eDebug_Interaction_Select, id);
    }
}

internal b32
is_selected(Debug_State *debug_state, Debug_ID id)
{
    b32 result = false;

    for (u32 idx = 0;
         idx < debug_state->selected_id_count;
         ++idx)
    {
        if (debug_ids_are_equal(id, debug_state->selected_id[idx]))
        {
            result = true;
            break;
        }
    }

    return result;
}

internal void
clear_selection(Debug_State *debug_state)
{
    debug_state->selected_id_count = 0;
}

internal void
add_to_selection(Debug_State *debug_state, Debug_ID id)
{
    if (debug_state->selected_id_count < array_count(debug_state->selected_id) &&
        !is_selected(debug_state, id))
    {
        debug_state->selected_id[debug_state->selected_id_count++] = id;
    }
}

internal b32
DEBUG_HIGHLIGHTED(Debug_ID id, v4 *color)
{
    b32 result = false;

    Debug_State *debug_state = debug_get_state();
    if (debug_state)
    {
        if (is_selected(debug_state, id))
        {
            *color = _v4_(0, 1, 1, 1);
            result = true;
        }

        if (debug_ids_are_equal(debug_state->hot_interaction.id, id))
        {
            *color = _v4_(1, 1, 0, 1);
            result = true;
        }
    }

    return result;
}

internal b32
DEBUG_REQUESTED(Debug_ID id)
{
    b32 result = false;

    Debug_State *debug_state = debug_get_state();
    if (debug_state)
    {
        result = (is_selected(debug_state, id) ||
                  debug_ids_are_equal(debug_state->hot_interaction.id, id));
    }

    return result;
}

internal void
debug_draw_event(Layout *layout, Debug_Stored_Event *stored_event, Debug_ID debug_id)
{
    Debug_State *debug_state = layout->debug_state;
    Render_Group *render_group = debug_state->render_group;
    Game_Assets *game_assets = debug_state->game_assets;

    if (stored_event)
    {
        Debug_Event *event = &stored_event->event;
        Debug_Interaction item_interaction =
            event_interaction(debug_state, debug_id, eDebug_Interaction_Auto_Modify_Variable, event);

        b32 is_hot = interaction_is_hot(debug_state, item_interaction);
        v4 item_color = is_hot ? _v4_(1, 1, 0, 1) : _v4_(1, 1, 1, 1);

        Debug_View *view = get_or_create_debug_view_for(debug_state, debug_id);
        switch (event->type)
        {
            case eDebug_Type_Bitmap:
            {
                Bitmap *bitmap = event->value_Bitmap;
                f32 bitmap_scale = view->inline_block.dim.y;
                if (bitmap)
                {
                    view->inline_block.dim.x = (bitmap_scale * bitmap->width) / bitmap->height;
                }

                Debug_Interaction tear_interaction =
                    event_interaction(debug_state, debug_id, eDebug_Interaction_Tear_Value, event);

                Layout_Element element = begin_element_rect(layout, &view->inline_block.dim);
                make_element_sizeable(&element);
                default_interaction(&element, tear_interaction);
                end_element(&element);

                push_rect(debug_state->render_group, element.bounds, 0.0f, _v4_(0, 0, 0, 1.0f));
                push_bitmap(debug_state->render_group, _v3_(element.bounds.min, 0.0f), _v3_(element.bounds.max, 0.0f), bitmap);
            } break;

            default:
            {
                char text[256];
                debug_event_to_text(text, text + sizeof(text), event, 
                                    eDebug_Var_To_Text_Add_Name|
                                    eDebug_Var_To_Text_Null_Terminator|
                                    eDebug_Var_To_Text_Colon|
                                    eDebug_Var_To_Text_Pretty_Bools);

                Font *font = debug_state->font;
                Rect2 text_bounds = string_op(String_Op::GET_RECT, debug_state->render_group, _v3_(0, 0, 0), text, font);
                v2 dim = _v2_(get_dim(text_bounds).x, layout->line_advance);

                Layout_Element element = begin_element_rect(layout, &dim);
                default_interaction(&element, item_interaction);
                end_element(&element);

                string_op(String_Op::DRAW,
                          debug_state->render_group,
                          _v3_(_v2_(element.bounds.min.x,
                                    element.bounds.max.y - font->v_advance),
                               0.0f),
                          text,
                          font,
                          item_color);

            } break;
        }
    }
}

internal void
debug_draw_element(Layout *layout, Debug_Tree *tree, Debug_Element *element, Debug_ID debug_id)
{
    Debug_State *debug_state = layout->debug_state;

    Debug_Stored_Event *oldest_event = element->oldest_event;
    if (oldest_event)
    {
        Debug_View *view = get_or_create_debug_view_for(debug_state, debug_id);
        switch (oldest_event->event.type)
        {
            case eDebug_Type_Counter_Thread_List:
            {
                Layout_Element layout_element = begin_element_rect(layout, &view->inline_block.dim);
                make_element_sizeable(&layout_element);
                // default_interaction(&element, item_interaction);
                end_element(&layout_element);

                draw_profile_in(debug_state, layout_element.bounds, layout->mouse_p);
            } break;

            case eDebug_Type_Open_Data_Block:
            {
                Debug_Stored_Event *last_open_block = oldest_event;
                for (Debug_Stored_Event *event = oldest_event;
                     event;
                     event = event->next)
                {
                    if (event->event.type == eDebug_Type_Open_Data_Block)
                    {
                        last_open_block = event;
                    }
                }

                for (Debug_Stored_Event *event = last_open_block;
                     event;
                     event = event->next)
                {
                    Debug_ID new_id = debug_id_from_guid(tree, event->event.guid);
                    debug_draw_event(layout, event, new_id);
                }
            } break;

            default:
            {
                Debug_Stored_Event *event = element->most_recent_event;
                debug_draw_event(layout, event, debug_id);
            } break;
        }
    }
}

internal void
debug_draw_main_menu(Debug_State *debug_state, v2 menu_p, v2 mouse_p)
{
    Game_Assets *game_assets = debug_state->game_assets;
    Font *font = debug_state->font;

    for (Debug_Tree *tree = debug_state->tree_sentinel.next;
         tree != &debug_state->tree_sentinel;
         tree = tree->next)
    {
        Layout layout = {};
        layout.debug_state = debug_state;
        layout.mouse_p = mouse_p;
        layout.at = tree->ui_p;
        layout.line_advance = (f32)font->v_advance;
        layout.spacing_y = 4.0f;

        int depth = 0;
        Debug_Variable_Iterator stack[DEBUG_MAX_VARIABLE_STACK_DEPTH];

        Debug_Variable_Group *group = tree->group;
        if (group)
        {
            stack[depth].link = group->sentinel.next;
            stack[depth].sentinel = &group->sentinel;
            ++depth;
            while (depth > 0)
            {
                Debug_Variable_Iterator *iter = stack + (depth - 1);
                if (iter->link == iter->sentinel)
                {
                    --depth;
                }
                else
                {
                    layout.depth = depth;

                    Debug_Variable_Link *link = iter->link;
                    iter->link = iter->link->next;

                    if (link->children)
                    {
                        Debug_ID id = debug_id_from_link(tree, link);
                        Debug_View *view = get_or_create_debug_view_for(debug_state, id);
                        Debug_Interaction item_interaction = debug_id_interaction(eDebug_Interaction_Toggle_Expansion, id);

                        char text[256];
                        Assert((link->children->name_length + 1) < array_count(text));
                        copy(link->children->name_length, link->children->name, text);
                        text[link->children->name_length] = 0;

                        Rect2 text_bounds = string_op(String_Op::GET_RECT, debug_state->render_group, _v3_(0, 0, 0), text, font);
                        v2 dim = _v2_(get_dim(text_bounds).x, layout.line_advance);


                        Layout_Element element = begin_element_rect(&layout, &dim);
                        default_interaction(&element, item_interaction);
                        end_element(&element);

                        b32 is_hot = interaction_is_hot(debug_state, item_interaction);
                        v4 item_color = is_hot ? _v4_(1, 1, 0, 1) : _v4_(1, 1, 1, 1);

                        string_op(String_Op::DRAW,
                                  debug_state->render_group,
                                  _v3_(_v2_(element.bounds.min.x,
                                            element.bounds.max.y - font->v_advance),
                                       0.0f),
                                  text,
                                  font,
                                  item_color);

                        if (view->collapsible.expanded_always)
                        {
                            iter = stack + depth;
                            iter->link = link->children->sentinel.next;
                            iter->sentinel = &link->children->sentinel;
                            ++depth;
                        }
                    }
                    else
                    {
                        Debug_ID debug_id = debug_id_from_link(tree, link);
                        debug_draw_element(&layout, tree, link->element, debug_id);
                    }
                }
            }
        }

        debug_state->at_y = layout.at.y;

        if (1)
        {
            Debug_Interaction move_interaction = {};
            move_interaction.type = eDebug_Interaction_Move;
            move_interaction.p = &tree->ui_p;

            Rect2 move_box = rect2_cen_half_dim(tree->ui_p - _v2_(4.0f, 4.0f), _v2_(4.0f, 4.0f));
            push_rect(debug_state->render_group, move_box, 0.0f,
                      interaction_is_hot(debug_state, move_interaction) ? _v4_(1, 1, 0, 1) : _v4_(1, 1, 1, 1));

            if (is_in_rect(move_box, mouse_p))
            {
                debug_state->next_hot_interaction = move_interaction;
            }
        }

    }

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
debug_begin_interact(Debug_State *debug_state, Game_Input *input, v2 mouse_p, b32 alt_ui)
{
    if (debug_state->hot_interaction.type)
    {
        if (debug_state->hot_interaction.type == eDebug_Interaction_Auto_Modify_Variable)
        {
            switch (debug_state->hot_interaction.event->type)
            {
                case eDebug_Type_b32:
                {
                    debug_state->hot_interaction.type = eDebug_Interaction_Toggle_Value;
                } break;

                case eDebug_Type_f32:
                {
                    debug_state->hot_interaction.type = eDebug_Interaction_Drag_Value;
                } break;

                case eDebug_Type_Open_Data_Block:
                {
                    debug_state->hot_interaction.type = eDebug_Interaction_Toggle_Value;
                } break;
            }

            if (alt_ui)
            {
                debug_state->hot_interaction.type = eDebug_Interaction_Tear_Value;
            }
        }

        switch (debug_state->hot_interaction.type)
        {
            case eDebug_Interaction_Tear_Value:
            {
#if 0
                Debug_Variable *root_group = debug_add_root_group(debug_state, "New_User_Group");
                debug_add_variable_to_group(debug_state, root_group, debug_state->hot_interaction.var);
                Debug_Tree *tree = add_tree(debug_state, root_group, _v2_(0, 0));
                tree->ui_p = mouse_p;
                debug_state->hot_interaction.type = eDebug_Interaction_Move;
                debug_state->hot_interaction.p = &tree->ui_p;
#endif
            } break;

            case eDebug_Interaction_Select:
            {
                if (!input->keys[KEY_LEFTSHIFT].is_down)
                {
                    clear_selection(debug_state);
                }
                add_to_selection(debug_state, debug_state->hot_interaction.id);
            } break;
        }

        debug_state->interaction = debug_state->hot_interaction;
    }
    else
    {
        debug_state->interaction.type = eDebug_Interaction_NOP;
    }
}

internal void
debug_end_interact(Debug_State *debug_state, Game_Input *input, v2 mouse_p)
{
    switch(debug_state->interaction.type)
    {
        case eDebug_Interaction_Toggle_Expansion:
        {
            Debug_View *view = get_or_create_debug_view_for(debug_state, debug_state->interaction.id);
            view->collapsible.expanded_always = !view->collapsible.expanded_always;
        } break;

        case eDebug_Interaction_Toggle_Value:
        {
            Debug_Event *event = debug_state->interaction.event;
            Assert(event);
            switch (event->type)
            {
                case eDebug_Type_b32:
                {
                    event->value_b32 = !event->value_b32;
                } break;
            }
        } break;
    }

    write_config(debug_state);

    debug_state->interaction.type = eDebug_Interaction_None;
    debug_state->interaction.generic = 0;
}

internal void
debug_interact(Debug_State *debug_state, Game_Input *input, v2 mouse_p)
{
    Game_Assets *game_assets = debug_state->game_assets;
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

    if (debug_state->interaction.type)
    {
        Debug_Event *event = debug_state->interaction.event;
        Debug_Tree *tree = debug_state->interaction.tree;
        v2 *p = debug_state->interaction.p;

        switch (debug_state->interaction.type)
        {
            case eDebug_Interaction_Drag_Value:
            {
                switch (event->type)
                {
                    case eDebug_Type_f32:
                    {
                        event->value_f32 += 0.1f * d_mouse_p.x;
                    } break;
                }
            } break;

            case eDebug_Interaction_Resize:
            {
                *p += _v2_(d_mouse_p.x, -d_mouse_p.y);
                p->x = maximum(p->x, 10.0f);
                p->y = maximum(p->y, 10.0f);
            } break;

            case eDebug_Interaction_Move:
            {
                *p += _v2_(d_mouse_p.x, d_mouse_p.y);
            } break;
        }
        b32 alt_ui = input->mouse.is_down[eMouse_Right];

        // click interaction.
        if (!input->mouse.is_down[eMouse_Left] &&
            input->mouse.toggle[eMouse_Left])
        {
            debug_end_interact(debug_state, input, mouse_p);
        }
    }
    else
    {
        debug_state->hot_interaction = debug_state->next_hot_interaction;

        b32 alt_ui = input->mouse.is_down[eMouse_Right];

        // TODO: revise as mouse half transition counts;
        if (input->mouse.is_down[eMouse_Left])
        {
            debug_begin_interact(debug_state, input, mouse_p, alt_ui);
        }
        else if (input->mouse.toggle[eMouse_Left])
        {
            debug_begin_interact(debug_state, input, mouse_p, alt_ui);
            debug_end_interact(debug_state, input, mouse_p);
        }
    }

    debug_state->last_mouse_p = mouse_p;
}

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
        FREELIST_ALLOC(result, debug_state->first_free_thread, push_struct(&debug_state->debug_arena, Debug_Thread););

        result->id = thread_id;
        result->lane_idx = debug_state->frame_bar_lane_count++;
        result->first_open_code_block = 0;
        result->first_open_data_block = 0;
        result->next = debug_state->first_thread;
        debug_state->first_thread = result;
    }

    return result;
}

#if 0
internal Debug_Frame_Region *
add_region(Debug_State *debug_state, Debug_Frame *current_frame)
{
    Assert(current_frame->region_count < MAX_REGIONS_PER_FRAME);
    Debug_Frame_Region *result = current_frame->regions + current_frame->region_count++;

    return result;
}
#endif

inline Open_Debug_Block *
alloc_open_debug_block(Debug_State *debug_state, Debug_Element *element, u32 frame_idx, Debug_Event *event, Open_Debug_Block **first_open_block)
{
    Open_Debug_Block *result = 0;
    FREELIST_ALLOC(result, debug_state->first_free_block, push_struct(&debug_state->debug_arena, Open_Debug_Block));

    result->starting_frame_idx = frame_idx;
    result->opening_event = event;
    result->element = element;
    result->next_free = 0;

    result->parent = *first_open_block;
    *first_open_block = result;

    return result;
}

inline void
deallocate_open_debug_block(Debug_State *debug_state, Open_Debug_Block **first_open_block)
{
    Open_Debug_Block *free_block = *first_open_block;
    *first_open_block = free_block->parent;

    free_block->next_free = debug_state->first_free_block;
    debug_state->first_free_block = free_block;
}

inline b32
events_match(Debug_Event a, Debug_Event b)
{
    // TODO: Have counters for blocks?
    b32 result = (a.thread_id == b.thread_id);
    return result;
}

internal Debug_Event *
create_variable(Debug_State *state, Debug_Type type, char *name)
{
    Debug_Event *var = push_struct(&state->debug_arena, Debug_Event);
    zero_struct(*var);
    var->type = (u8)type;
    var->block_name = (char *)push_copy(&state->debug_arena, name, string_length(name) + 1);

    return var;
}

internal Debug_Variable_Group *
get_or_create_group_with_name(Debug_State *debug_state, Debug_Variable_Group *parent, u32 name_length, char *name)
{
    Debug_Variable_Group *result = 0;
    for (Debug_Variable_Link *link = parent->sentinel.next;
         link != &parent->sentinel;
         link = link->next)
    {
        if (link->children && strings_are_equal(link->children->name_length, link->children->name,
                                                name_length, name))
        {
            result = link->children;
        }
    }

    if (!result)
    {
        result = create_variable_group(debug_state, name_length, name);
        add_group_to_group(debug_state, parent, result);
    }

    return result;
}

internal Debug_Variable_Group *
get_group_for_hierarchical_name(Debug_State *debug_state, Debug_Variable_Group *parent, char *name)
{
    Debug_Variable_Group *result = parent;

    char *first_underscore = 0;
    for (char *scan = name;
         *scan;
         ++scan)
    {
        if (*scan == '_')
        {
            first_underscore = scan;
            break;
        }
    }

    if (first_underscore)
    {
        Debug_Variable_Group *sub_group = get_or_create_group_with_name(debug_state, parent, (u32)(first_underscore - name), name);
        result = get_group_for_hierarchical_name(debug_state, sub_group, first_underscore + 1);
    }

    return result;
}

internal void
free_frame(Debug_State *debug_state, Debug_Frame *frame)
{
    for (u32 element_hash_idx = 0;
         element_hash_idx < array_count(debug_state->element_hash);
         ++element_hash_idx)
    {
        for (Debug_Element *element = debug_state->element_hash[element_hash_idx];
             element;
             element = element->next_in_hash)
        {
            while (element->oldest_event &&
                   element->oldest_event->frame_idx <= frame->frame_idx)
            {
                Debug_Stored_Event *free_event = element->oldest_event;
                element->oldest_event = free_event->next;
                if (element->most_recent_event == free_event)
                {
                    Assert(free_event->next == 0);
                    element->most_recent_event = 0;
                }

                FREELIST_DEALLOC(free_event, debug_state->first_free_stored_event);
            }
        }
    }

    FREELIST_DEALLOC(frame, debug_state->first_free_frame);
}

internal void
free_oldest_frame(Debug_State *debug_state)
{
    if (debug_state->oldest_frame)
    {
        Debug_Frame *frame = debug_state->oldest_frame;
        debug_state->oldest_frame = frame->next;
        if (debug_state->most_recent_frame)
        {
            Assert(frame->next == 0);
            debug_state->most_recent_frame = 0;
        }

        free_frame(debug_state, frame);
    }
}


internal Debug_Frame *
new_frame(Debug_State *debug_state, u64 begin_clock)
{
    Debug_Frame *result = 0;
    while (!result)
    {
        result = debug_state->first_free_frame;
        if (result)
        {
            debug_state->first_free_frame = result->next_free;
        }
        else
        {
            if (arena_has_room_for(&debug_state->per_frame_arena, sizeof(Debug_Frame)))
            {
                result = push_struct(&debug_state->per_frame_arena, Debug_Frame);
            }
            else
            {
                Assert(debug_state->oldest_frame);
                free_oldest_frame(debug_state);
            }
        }
    }

    zero_struct(*result);
    result->frame_idx = debug_state->total_frame_count++;
    result->frame_bar_scale = 1.0f;
    result->begin_clock = begin_clock;

    return result;
}


internal Debug_Stored_Event *
store_event(Debug_State *debug_state, Debug_Element *element, Debug_Event *event)
{
    Debug_Stored_Event *result = 0;
    while (!result)
    {
        result = debug_state->first_free_stored_event;
        if (result)
        {
            debug_state->first_free_stored_event = result->next_free;
        }
        else
        {
            if (arena_has_room_for(&debug_state->per_frame_arena, sizeof(Debug_Stored_Event)))
            {
                result = push_struct(&debug_state->per_frame_arena, Debug_Stored_Event);
            }
            else
            {
                Assert(debug_state->oldest_frame);
                free_oldest_frame(debug_state);
            }
        }
    }

    result->next = 0;
    result->frame_idx = debug_state->collation_frame->frame_idx;
    result->event = *event;

    if (element->most_recent_event)
    {
        element->most_recent_event = element->most_recent_event->next = result;
    }
    else 
    {
        element->oldest_event = element->most_recent_event = result;
    }

    return result;
}

internal Debug_Element *
get_element_from_event(Debug_State *debug_state, Debug_Event *event)
{
    Assert(event->guid);

    size_t hash_value = (u32)((size_t)event->guid >> 2);
    // TODO: verify this turns into an and (not a mod)
    u32 idx = (hash_value % array_count(debug_state->element_hash));

    Debug_Element *result = 0;

    for (Debug_Element *chain = debug_state->element_hash[idx];
         chain;
         chain = chain->next_in_hash)
    {
        if (chain->guid == event->guid)
        {
            result = chain;
            break;
        }
    }

    if (!result)
    {
        result = push_struct(&debug_state->debug_arena, Debug_Element);

        result->guid = event->guid;
        result->next_in_hash = debug_state->element_hash[idx];
        debug_state->element_hash[idx] = result;

        result->oldest_event = result->most_recent_event = 0;

        Debug_Variable_Group *parent_group =
            get_group_for_hierarchical_name(debug_state, debug_state->root_group, event->block_name);
        add_element_to_group(debug_state, parent_group, result);
    }

    return result;
}

internal void
collate_debug_records(Debug_State *debug_state, u32 event_count, Debug_Event *event_array)
{
    for (u32 event_idx = 0;
         event_idx < event_count;
         ++event_idx)
    {
        Debug_Event *event = event_array + event_idx;
        Debug_Element *element = get_element_from_event(debug_state, event);

        if (!debug_state->collation_frame)
        {
            debug_state->collation_frame = new_frame(debug_state, event->clock);
        }

        if (event->type == eDebug_Type_Mark_Debug_Value)
        {
            store_event(debug_state, element, event);
        }
        else if (event->type == eDebug_Type_Frame_Marker)
        {
            Assert(debug_state->collation_frame);

            debug_state->collation_frame->end_clock = event->clock;
            debug_state->collation_frame->wall_seconds_elapsed = event->value_f32;

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

            if (debug_state->paused)
            {
                free_frame(debug_state, debug_state->collation_frame);
            }
            else
            {
                if (debug_state->most_recent_frame)
                {
                    debug_state->most_recent_frame = debug_state->most_recent_frame->next = debug_state->collation_frame;
                }
                else
                {
                    debug_state->oldest_frame = debug_state->most_recent_frame = debug_state->collation_frame;
                }
                ++debug_state->frame_count;
            }

            debug_state->collation_frame = new_frame(debug_state, event->clock);
        }
        else
        {
            Assert(debug_state->collation_frame);

            u32 frame_idx = debug_state->frame_count - 1;
            Debug_Thread *thread = get_debug_thread(debug_state, event->thread_id);
            u64 relative_clock = event->clock - debug_state->collation_frame->begin_clock;

            switch (event->type)
            {
                case eDebug_Type_Begin_Block:
                {
                    Open_Debug_Block *debug_block =
                        alloc_open_debug_block(debug_state, element, frame_idx, event, &thread->first_open_code_block);
                } break;

                case eDebug_Type_End_Block:
                {
                    if (thread->first_open_code_block)
                    {
                        Open_Debug_Block *matching_block = thread->first_open_code_block;
                        Debug_Event *opening_event = matching_block->opening_event;
                        if (events_match(*opening_event, *event))
                        {
                            if (matching_block->starting_frame_idx == frame_idx)
                            {
                                char *match_name =
                                    matching_block->parent ? matching_block->parent->opening_event->block_name : 0;
                                if (match_name == debug_state->scope_to_record)
                                {
#if 0
                                    f32 min_t = (f32)(opening_event->clock - debug_state->collation_frame->begin_clock);
                                    f32 max_t = (f32)(event->clock - debug_state->collation_frame->begin_clock);
                                    f32 threshold_t = 0.01f;
                                    if ((max_t - min_t) > threshold_t)
                                    {
                                        Debug_Frame_Region *region = add_region(debug_state, debug_state->collation_frame);
                                        region->event = opening_event;
                                        region->cycle_count = (event->clock - opening_event->clock);
                                        region->lane_idx = (u16)thread->lane_idx;
                                        region->min_t = min_t;
                                        region->max_t = max_t;
                                        region->color_idx = (u16)opening_event->block_name;
                                    }
#endif
                                }
                            }
                            else
                            {
                                // TODO: record all frames in between and begin/end spans!
                            }

                            deallocate_open_debug_block(debug_state, &thread->first_open_code_block);
                        }
                        else
                        {
                            // TODO: record span that goes to the beginning of the frame series?
                        }
                    }
                } break;

                case eDebug_Type_Open_Data_Block:
                {
                    Open_Debug_Block *debug_block =
                        alloc_open_debug_block(debug_state, element, frame_idx, event, &thread->first_open_data_block);
                    store_event(debug_state, element, event);
                } break;

                case eDebug_Type_Close_Data_Block:
                {
                    if (thread->first_open_data_block)
                    {
                        store_event(debug_state, thread->first_open_data_block->element, event);

                        Open_Debug_Block *matching_block = thread->first_open_data_block;
                        Debug_Event *opening_event = matching_block->opening_event;
                        if (events_match(*opening_event, *event))
                        {
                            deallocate_open_debug_block(debug_state, &thread->first_open_data_block);
                        }
                    }
                } break;

                default:
                {
                    Debug_Element *storage_element = element;
                    if (thread->first_open_data_block)
                    {
                        storage_element = thread->first_open_data_block->element;
                    }
                    store_event(debug_state, storage_element, event);
                } break;
            }
        }
    }
}

internal void
debug_start(Debug_State *debug_state, Game_Assets *game_assets, u32 width, u32 height)
{
    TIMED_FUNCTION();

    if (!debug_state->init)
    {
        debug_state->frame_bar_lane_count = 0;
        debug_state->first_thread = 0;
        debug_state->first_free_thread = 0;
        debug_state->first_free_block = 0;

        debug_state->frame_count = 0;

        debug_state->oldest_frame = debug_state->most_recent_frame = debug_state->first_free_frame = 0;
        debug_state->collation_frame = 0;

        debug_state->high_priority_queue = g_debug_memory->high_priority_queue;
        debug_state->tree_sentinel.next = &debug_state->tree_sentinel;
        debug_state->tree_sentinel.prev = &debug_state->tree_sentinel;
        debug_state->tree_sentinel.group = 0;

        size_t total_memory_size = g_debug_memory->debug_storage_size - sizeof(Debug_State);
        init_arena(&debug_state->debug_arena,
                   total_memory_size,
                   debug_state + 1);
#if 1
        init_sub_arena(&debug_state->per_frame_arena, &debug_state->debug_arena, (total_memory_size / 2));
#else
        // this is the stress-testing case to make sure the memory recycling works.
        init_sub_arena(&debug_state->per_frame_arena, &debug_state->debug_arena, 128 * 1024);
#endif

        debug_state->root_group = create_variable_group(debug_state, 4, "Root");

        debug_state->game_assets = &((Transient_State *)g_debug_memory->transient_memory)->game_assets;

#if 0
        Debug_Variable_Definition_Context context = {};
        context.state = debug_state;
        context.arena = &debug_state->debug_arena;
        context.group_stack[0] = 0;

        debug_state->root_group = debug_begin_variable_group(&context, "Root");
        debug_begin_variable_group(&context, "Debugging");

        debug_create_variables(&context);
        debug_begin_variable_group(&context, "Profile");
        debug_begin_variable_group(&context, "By Thread");
        debug_add_variable(&context, eDebug_Type_Counter_Thread_List, "");
        debug_end_variable_group(&context);
        debug_begin_variable_group(&context, "By Function");
        debug_add_variable(&context, eDebug_Type_Counter_Thread_List, "");
        debug_end_variable_group(&context);
        debug_end_variable_group(&context);

        debug_add_variable(&context, "Test Bitmap", debug_state->game_assets->debug_bitmap);

        debug_end_variable_group(&context);
        debug_end_variable_group(&context);
        Assert(context.group_depth == 0);
#endif

        Camera *debug_overlay_camera = push_camera(&debug_state->debug_arena, eCamera_Type_Orthographic, (f32)width, (f32)height);
        debug_state->render_group = alloc_render_group(&debug_state->debug_arena, MB(16),
                                                       debug_overlay_camera);

        debug_state->font = &game_assets->debug_font;

        debug_state->paused = false;
        debug_state->scope_to_record = 0;

        debug_state->init = true;

        add_tree(debug_state, debug_state->root_group, _v2_(0.0f, (f32)height));
    }

    begin_render(debug_state->render_group);

    debug_state->left_edge = 0.0f;
    debug_state->left_edge = (f32)width;
    debug_state->at_y = 0.5f * height;
    debug_state->width = (f32)width;
    debug_state->height = (f32)height;

    debug_state->at_y = (f32)height;
    debug_state->left_edge = 0.0f;

}

internal void
debug_dump_struct(u32 member_count, Member_Definition *member_defs, void *struct_ptr, u32 indent_level = 0)
{
    for (u32 member_idx = 0;
         member_idx < member_count;
         ++member_idx)
    {
        char text_buffer_base[256];
        char *text_buffer = text_buffer_base;
        for (u32 indent = 0;
             indent < indent_level;
             ++indent)
        {
            *text_buffer++ = ' ';
            *text_buffer++ = ' ';
            *text_buffer++ = ' ';
            *text_buffer++ = ' ';
        }
        text_buffer[0] = 0;
        size_t text_buffer_left = (text_buffer_base + sizeof(text_buffer_base)) - text_buffer;

        Member_Definition *member = member_defs + member_idx;

        void *member_ptr = (((u8 *)struct_ptr) + member->offset);

        if (member_ptr)
        {
            switch (member->type)
            {
                case eMeta_Type_u32:
                {
                    _snprintf_s(text_buffer, text_buffer_left, text_buffer_left, "%s: %u", member->name, *(u32 *)member_ptr);
                } break;

                case eMeta_Type_f32:
                {
                    _snprintf_s(text_buffer, text_buffer_left, text_buffer_left, "%s: %f", member->name, *(f32 *)member_ptr);
                } break;

                case eMeta_Type_v2:
                {
                    _snprintf_s(text_buffer, text_buffer_left, text_buffer_left, "%s: (%f, %f)",
                                member->name,
                                ((v2 *)member_ptr)->x,
                                ((v2 *)member_ptr)->y);
                } break;

                case eMeta_Type_v3:
                {
                    _snprintf_s(text_buffer, text_buffer_left, text_buffer_left, "%s: (%f, %f, %f)",
                                member->name,
                                ((v3 *)member_ptr)->x,
                                ((v3 *)member_ptr)->y,
                                ((v3 *)member_ptr)->z);
                } break;

                // META_HANDLE_TYPE_DUMP(member_ptr, indent_level + 1);

            }
        }

        if (text_buffer[0])
        {
            debug_text_line(text_buffer);
        }
    }
};

internal void
debug_end(Debug_State *debug_state, Game_Input *input)
{
    TIMED_FUNCTION();

    Render_Group *render_group = debug_state->render_group;

    Debug_Event *hot_event = 0;

    v2 mouse_p = input->mouse.P;
    debug_draw_main_menu(debug_state, debug_state->menu_p, mouse_p);
    debug_interact(debug_state, input, mouse_p);

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

    if (debug_state->most_recent_frame)
    {
        char text_buffer[256];
        _snprintf_s(text_buffer, sizeof(text_buffer), 
                    "last frame time: %.02fms",
                    debug_state->most_recent_frame->wall_seconds_elapsed * 1000.0f);
        debug_text_line(text_buffer);

        _snprintf_s(text_buffer, sizeof(text_buffer), 
                    "per-frame arena used: %ukb",
                    (u32)(get_arena_size_remaining(&debug_state->per_frame_arena) / 1024));
        debug_text_line(text_buffer);
    }


    if (input->mouse.is_down[eMouse_Left] && 
        input->mouse.toggle[eMouse_Left])
    {
        if (hot_event)
        {
            debug_state->scope_to_record = hot_event->block_name;
        }
        else
        {
            debug_state->scope_to_record = 0;
        }
    }

    render_group_to_output_batch(debug_state->render_group, &g_debug_memory->render_batch);
    end_render(debug_state->render_group);

    // clear the UI state for the next frame.
    zero_struct(debug_state->next_hot_interaction);
}

internal Game_Assets *
debug_get_game_assets(Game_Memory *memory)
{
    Game_Assets *assets = 0;

    Transient_State *trans_state = (Transient_State *)memory->transient_memory;
    if (trans_state->init)
    {
        assets = &trans_state->game_assets;
    }

    return assets;
}

extern "C"
DEBUG_FRAME_END(debug_frame_end)
{
    g_debug_table->current_event_array_idx = !g_debug_table->current_event_array_idx;
    u64 event_array_idx_event_idx = atomic_exchange_u64(&g_debug_table->event_array_idx_event_idx,
                                                        (u64)g_debug_table->current_event_array_idx << 32);

    u32 event_array_idx = (event_array_idx_event_idx >> 32);
    Assert(event_array_idx <= 1);
    u32 event_count = (event_array_idx_event_idx & 0xffffffff);

    Debug_State *debug_state = (Debug_State *)memory->debug_storage;
    if (debug_state)
    {
        Game_Assets *game_assets = debug_get_game_assets(memory);

        debug_start(debug_state, game_assets, game_screen_buffer->width, game_screen_buffer->height);
        collate_debug_records(debug_state, event_count, g_debug_table->events[event_array_idx]);

        debug_end(debug_state, game_input);
    }

    return g_debug_table;
}
