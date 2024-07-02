/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Sung Woo Lee $
   $Notice: (C) Copyright 2024 by Sung Woo Lee. All Rights Reserved. $
   ======================================================================== */

#define DEBUG_MAX_VARIABLE_STACK_DEPTH 64

struct Debug_Variable_Definition_Context
{
    Debug_State *state;
    Memory_Arena *arena;

    u32 group_depth;
    Debug_Variable *group_stack[DEBUG_MAX_VARIABLE_STACK_DEPTH];
};

internal Debug_Variable *
debug_add_variable(Debug_State *state, Debug_Variable_Type type, char *name)
{
    Debug_Variable *var = push_struct(&state->debug_arena, Debug_Variable);
    var->type = type;
    var->name = (char *)push_copy(&state->debug_arena, name, string_length(name) + 1);

    return var;
}

internal Debug_Variable *
debug_add_root_group(Debug_State *debug_state, char *name)
{
    Debug_Variable *group = debug_add_variable(debug_state, eDebug_Variable_Type_Var_Group, name);
    DLIST_INIT(&group->var_group);

    return group;
}

internal void
debug_add_variable_to_group(Debug_State *state, Debug_Variable *group, Debug_Variable *add)
{
    Debug_Variable_Link *link = push_struct(&state->debug_arena, Debug_Variable_Link);
    DLIST_INSERT(&group->var_group, link);
    link->var = add;
}

internal void
debug_add_variable_to_default_group(Debug_Variable_Definition_Context *context, Debug_Variable *var)
{
    Debug_Variable *parent = context->group_stack[context->group_depth];
    if (parent)
    {
        debug_add_variable_to_group(context->state, parent, var);
    }
}

internal Debug_Variable *
debug_add_variable(Debug_Variable_Definition_Context *context, Debug_Variable_Type type, char *name)
{
    Debug_Variable *var = debug_add_variable(context->state, type, name);
    debug_add_variable_to_default_group(context, var);

    return var;
}

internal Debug_Variable *
debug_begin_variable_group(Debug_Variable_Definition_Context *context, char *name)
{
    Debug_Variable *group = debug_add_root_group(context->state, name);
    debug_add_variable_to_default_group(context, group);

    Assert(context->group_depth < (array_count(context->group_stack) - 1));
    context->group_stack[++context->group_depth] = group;

    return group;
}

internal Debug_Variable *
debug_add_variable(Debug_Variable_Definition_Context *context, char *name, b32 value)
{
    Debug_Variable *var = debug_add_variable(context, eDebug_Variable_Type_b32, name);
    var->bool32 = value;

    return var;
}

internal Debug_Variable *
debug_add_variable(Debug_Variable_Definition_Context *context, char *name, f32 value)
{
    Debug_Variable *var = debug_add_variable(context, eDebug_Variable_Type_f32, name);
    var->float32 = value;

    return var;
}

internal Debug_Variable *
debug_add_variable(Debug_Variable_Definition_Context *context, char *name, Bitmap *bitmap)
{
    Debug_Variable *var = debug_add_variable(context, eDebug_Variable_Type_Bitmap_Display, name);
    var->bitmap_display.bitmap = bitmap;

    return var;
}

internal void
debug_end_variable_group(Debug_Variable_Definition_Context *context)
{
    Assert(context->group_depth > 0);
    --context->group_depth;
}

internal void
debug_create_variables(Debug_Variable_Definition_Context *context)
{

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
        DEBUG_VARIABLE_LISTING(USE_DEBUG_CAMERA);
    }
    debug_end_variable_group(context);

    DEBUG_VARIABLE_LISTING(XBOT_ANIMATION_SPEED);
    DEBUG_VARIABLE_LISTING(XBOT_ACCEL_CONSTANT);

#undef DEBUG_VARIABLE_LISTING
}
