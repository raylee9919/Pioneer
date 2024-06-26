/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Sung Woo Lee $
   $Notice: (C) Copyright 2024 by Sung Woo Lee. All Rights Reserved. $
   ======================================================================== */

struct Debug_Variable_Definition_Context
{
    Debug_State *state;
    Memory_Arena *arena;

    Debug_Variable_Reference *group;
};

internal Debug_Variable *
debug_add_unreferenced_variable(Debug_State *state, Debug_Variable_Type type, char *name)
{
    Debug_Variable *var = push_struct(&state->debug_arena, Debug_Variable);
    var->type = type;
    var->name = (char *)push_copy(&state->debug_arena, name, string_length(name) + 1);

    return var;
}

internal Debug_Variable_Reference *
debug_add_variable_reference(Debug_State *state, Debug_Variable_Reference *group_ref, Debug_Variable *var)
{
    Debug_Variable_Reference *ref = push_struct(&state->debug_arena, Debug_Variable_Reference);
    ref->var = var;
    ref->next = 0;

    ref->parent = group_ref;
    Debug_Variable *group = (ref->parent) ? ref->parent->var : 0;

    if (group)
    {
        if (group->group.last_child)
        {
            group->group.last_child = group->group.last_child->next = ref;
        }
        else
        {
            group->group.last_child = group->group.first_child = ref;
        }
    }

    return ref;
}

internal Debug_Variable_Reference *
debug_add_variable_reference(Debug_Variable_Definition_Context *context, Debug_Variable *var)
{
    Debug_Variable_Reference *ref = debug_add_variable_reference(context->state, context->group, var);

    return ref;
}

internal Debug_Variable_Reference *
debug_add_variable(Debug_Variable_Definition_Context *context, Debug_Variable_Type type, char *name)
{
    Debug_Variable *var = debug_add_unreferenced_variable(context->state, type, name);
    Debug_Variable_Reference *ref = debug_add_variable_reference(context, var);

    return ref;
}

internal Debug_Variable *
debug_add_root_group_internal(Debug_State *state, char *name)
{
    Debug_Variable *group = debug_add_unreferenced_variable(state, eDebug_Variable_Type_Group, name);
    group->group.expanded = true;
    group->group.first_child = group->group.last_child = 0;

    return group;
}

internal Debug_Variable_Reference *
debug_add_root_group(Debug_State *state, char *name)
{
    Debug_Variable_Reference *group_ref =
        debug_add_variable_reference(state, 0, debug_add_root_group_internal(state, name));

    return group_ref;
}

internal Debug_Variable_Reference *
debug_begin_variable_group(Debug_Variable_Definition_Context *context, char *name)
{
    Debug_Variable_Reference *group =
        debug_add_variable_reference(context, debug_add_root_group_internal(context->state, name));

    group->var->group.expanded = false;

    context->group = group;

    return group;
}

internal Debug_Variable_Reference *
debug_add_variable(Debug_Variable_Definition_Context *context, char *name, b32 value)
{
    Debug_Variable_Reference *ref = debug_add_variable(context, eDebug_Variable_Type_b32, name);
    ref->var->bool32 = value;

    return ref;
}

internal Debug_Variable_Reference *
debug_add_variable(Debug_Variable_Definition_Context *context, char *name, f32 value)
{
    Debug_Variable_Reference *ref = debug_add_variable(context, eDebug_Variable_Type_f32, name);
    ref->var->float32 = value;

    return ref;
}

internal void
debug_end_variable_group(Debug_Variable_Definition_Context *context)
{
    Assert(context->group);

    context->group = context->group->parent;
}


internal void
debug_create_variables(Debug_Variable_Definition_Context *context)
{
    Debug_Variable_Reference *use_debug_cam_ref = 0;
#define DEBUG_VARIABLE_LISTING(name) debug_add_variable(context, #name, DEBUG_UI_##name)

    debug_begin_variable_group(context, "Renderer");
    {
        DEBUG_VARIABLE_LISTING(COLOR);

        debug_begin_variable_group(context, "Draw");
        {
            DEBUG_VARIABLE_LISTING(DRAW_GRASS);
            DEBUG_VARIABLE_LISTING(DRAW_STAR);
        }
        debug_end_variable_group(context);
    }
    debug_end_variable_group(context);

    debug_begin_variable_group(context, "Camera");
    {
        use_debug_cam_ref = DEBUG_VARIABLE_LISTING(USE_DEBUG_CAMERA);
    }
    debug_end_variable_group(context);
    DEBUG_VARIABLE_LISTING(XBOT_ACCEL_CONSTANT);

    debug_add_variable_reference(context, use_debug_cam_ref->var);

#undef DEBUG_VARIABLE_LISTING
}
