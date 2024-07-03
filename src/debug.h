#ifndef DEBUG_H
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Sung Woo Lee $
   $Notice: (C) Copyright 2024 by Sung Woo Lee. All Rights Reserved. $
   ======================================================================== */

struct Debug_Variable;

enum Debug_Variable_To_Text_Flag
{
    eDebug_Var_To_Text_Add_Debug_UI         = 0x1,
    eDebug_Var_To_Text_Add_Name             = 0x2,
    eDebug_Var_To_Text_Float_Suffix         = 0x4,
    eDebug_Var_To_Text_Line_Feed_End        = 0x8,
    eDebug_Var_To_Text_Null_Terminator      = 0x10,
    eDebug_Var_To_Text_Colon                = 0x20,
    eDebug_Var_To_Text_Pretty_Bools         = 0x40,
};

enum Debug_Variable_Type
{
    eDebug_Variable_Type_b32,
    eDebug_Variable_Type_s32,
    eDebug_Variable_Type_u32,
    eDebug_Variable_Type_f32,
    eDebug_Variable_Type_v2,
    eDebug_Variable_Type_v3,
    eDebug_Variable_Type_v4,

    eDebug_Variable_Type_Counter_Thread_List,
    eDebug_Variable_Type_Bitmap_Display,

    eDebug_Variable_Type_Var_Group,
};
inline b32
debug_should_be_written(Debug_Variable_Type type)
{
    b32 result = (type != eDebug_Variable_Type_Counter_Thread_List &&
                  type != eDebug_Variable_Type_Bitmap_Display);

    return result;
}

struct Debug_Tree;
struct Debug_Variable;

struct Debug_View_Inline_Block
{
    v2 dim;
};

struct Debug_View_Collapsible
{
    b32 expanded_always;
    b32 expanded_alt_view;
};

enum Debug_View_Type
{
    eDebug_View_Type_Unknown,
    eDebug_View_Type_Basic,
    eDebug_View_Type_Inline_Block,
    eDebug_View_Type_Collapsible,
};

struct Debug_ID
{
    void *value[2];
};

struct Debug_View
{
    Debug_ID id;
    Debug_View *next_in_hash;

    Debug_View_Type type;
    union
    {
        Debug_View_Inline_Block inline_block;
        Debug_View_Collapsible collapsible;
    };
};

struct Debug_Tree
{
    v2 ui_p;
    Debug_Variable *group;

    Debug_Tree *next;
    Debug_Tree *prev;
};

struct Debug_Profile_Settings
{
    int placeholder;
};

struct Debug_Bitmap_Display
{
    Bitmap *bitmap;
};

struct Debug_Variable_Link
{
    Debug_Variable_Link *next;
    Debug_Variable_Link *prev;
    Debug_Variable *var;
};

struct Debug_Variable
{
    Debug_Variable_Type type;
    char *name;

    union
    {
        b32 bool32;
        s32 int32;
        u32 uint32;
        f32 float32;
        v2 vector2;
        v3 vector3;
        v4 vector4;
        Debug_Profile_Settings profile;
        Debug_Bitmap_Display bitmap_display;
        Debug_Variable_Link var_group;
    };
};

struct Render_Group;

struct Debug_Counter_Snapshot
{
    u32     hit_count;
    u64     cycle_count;
};

struct Debug_Counter_State
{
    char    *file_name;
    char    *block_name;
    u32     line_number;
};

struct Debug_Frame_Region
{
    Debug_Record *record;
    u64 cycle_count;
    u16 lane_idx;
    u16 color_idx;
    f32 min_t;
    f32 max_t;
};

#define MAX_REGIONS_PER_FRAME (2 * 4096)
struct Debug_Frame
{
    u64 begin_clock;
    u64 end_clock;
    f32 wall_seconds_elapsed;

    Debug_Variable *root_group;

    u32 region_count;
    Debug_Frame_Region *regions;
};

struct Open_Debug_Block
{
    u32 starting_frame_idx;
    Debug_Record *src;
    Debug_Event *opening_event;
    Open_Debug_Block *parent;

    // NOTE: only for data blocks? Proly!
    Debug_Variable *group;

    Open_Debug_Block *next_free;
};

struct Debug_Thread
{
    u32 id;
    u32 lane_idx;
    Open_Debug_Block *first_open_code_block;
    Open_Debug_Block *first_open_data_block;
    Debug_Thread *next;
};

enum Debug_Interaction_Type
{
    eDebug_Interaction_None,

    eDebug_Interaction_NOP,

    eDebug_Interaction_Auto_Modify_Variable,

    eDebug_Interaction_Toggle_Value,
    eDebug_Interaction_Drag_Value,
    eDebug_Interaction_Tear_Value,

    eDebug_Interaction_Resize,
    eDebug_Interaction_Move,
};
struct Debug_Interaction
{
    Debug_ID id;
    Debug_Interaction_Type type;
    union
    {
        void *generic;
        Debug_Variable *var;
        Debug_Tree *tree;
        v2 *p;
    };
};

struct Debug_State
{
    b32 init;

    Platform_Work_Queue *high_priority_queue;

    Memory_Arena debug_arena;
    
    Render_Group *render_group;

    Game_Assets *game_assets;

    b32 compiling;
    Debug_Executing_Process compiler;

    v2 menu_p;
    b32 menu_active;

    Debug_Variable *root_group;
    Debug_View *view_hash[4096];
    Debug_Tree tree_sentinel;

    v2 last_mouse_p;
    Debug_Interaction interaction;
    Debug_Interaction hot_interaction;
    Debug_Interaction next_hot_interaction;

    f32 left_edge;
    f32 right_edge;
    f32 at_y;

    f32 width;
    f32 height;

    Debug_Record *scope_to_record;

    Memory_Arena collate_arena;
    Temporary_Memory collate_tmp;

    u32 collation_array_idx;
    Debug_Frame *collation_frame;
    u32 frame_bar_lane_count;
    u32 frame_count;
    f32 frame_bar_scale;
    b32 paused;

    Debug_Frame *frames;
    Debug_Thread *first_thread;
    Open_Debug_Block *first_free_block;
};

#define DEBUG_H
#endif
