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

    Debug_Variable *group;
};

internal Debug_Variable *
debug_add_variable(Debug_Variable_Definition_Context *context, Debug_Variable_Type type, char *name)
{
    Debug_Variable *var = push_struct(context->arena, Debug_Variable);
    var->type = type;
    var->name = (char *)push_copy(context->arena, name, string_length(name) + 1);
    var->next = 0;

    Debug_Variable *group = context->group;
    var->parent = group;

    if (group)
    {
        if (group->group.last_child)
        {
            group->group.last_child = group->group.last_child->next = var;
        }
        else
        {
            group->group.last_child = group->group.first_child = var;
        }
    }

    return var;
}

internal Debug_Variable *
debug_begin_variable_group(Debug_Variable_Definition_Context *context, char *name)
{
    Debug_Variable *group = debug_add_variable(context, eDebug_Variable_Type_Group, name);
    group->group.expanded = false;
    group->group.first_child = group->group.last_child = 0;

    context->group = group;

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

internal void
debug_end_variable_group(Debug_Variable_Definition_Context *context)
{
    Assert(context->group);

    context->group = context->group->parent;
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
    DEBUG_VARIABLE_LISTING(XBOT_ACCEL_CONSTANT);

#undef DEBUG_VARIABLE_LISTING
}
