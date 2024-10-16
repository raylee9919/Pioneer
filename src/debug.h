#ifndef DEBUG_H
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Sung Woo Lee $
   $Notice: (C) Copyright 2024 by Sung Woo Lee. All Rights Reserved. $
   ======================================================================== */

#define DEBUG_MAX_VARIABLE_STACK_DEPTH 64

struct Debug_Tree;
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

struct Debug_Stored_Event
{
    union {
        Debug_Stored_Event *next;
        Debug_Stored_Event *next_free;
    };

    u32 frame_idx;
    Debug_Event event;
};

struct Debug_Element
{
    char *guid;
    Debug_Element *next_in_hash;

    Debug_Stored_Event *oldest_event;
    Debug_Stored_Event *most_recent_event;
};

struct Debug_Variable_Group;
struct Debug_Variable_Link
{
    Debug_Variable_Link *next;
    Debug_Variable_Link *prev;
    Debug_Variable_Group *children;
    Debug_Element *element;
};

struct Debug_Variable_Group
{
    u32 name_length;
    char *name;
    Debug_Variable_Link sentinel;
};

struct Debug_Tree
{
    v2 ui_p;
    Debug_Variable_Group *group;

    Debug_Tree *next;
    Debug_Tree *prev;
};

struct Debug_Variable
{
    Debug_Type type;
    char *name;
    Debug_Event event;
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
    Debug_Event *event;
    u64 cycle_count;
    u16 lane_idx;
    u16 color_idx;
    f32 min_t;
    f32 max_t;
};

#define MAX_REGIONS_PER_FRAME (2 * 4096)
struct Debug_Frame
{
    union {
        Debug_Frame *next;
        Debug_Frame *next_free;
    };

    u64 begin_clock;
    u64 end_clock;
    f32 wall_seconds_elapsed;

    f32 frame_bar_scale;

    u32 frame_idx;
};

struct Open_Debug_Block
{
    union {
        Open_Debug_Block *parent;
        Open_Debug_Block *next_free;
    };

    u32 starting_frame_idx;
    Debug_Event *opening_event;
    Debug_Element *element;

    // NOTE: only for data blocks? Probly!
    Debug_Variable_Group *group;

};

struct Debug_Thread
{
    union {
        Debug_Thread *next;
        Debug_Thread *next_free;
    };

    u32 id;
    u32 lane_idx;
    Open_Debug_Block *first_open_code_block;
    Open_Debug_Block *first_open_data_block;
    
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

    eDebug_Interaction_Select,

    eDebug_Interaction_Toggle_Expansion,
};
struct Debug_Interaction
{
    Debug_ID id;
    Debug_Interaction_Type type;
    union
    {
        void *generic;
        Debug_Event *event;
        Debug_Tree *tree;
        v2 *p;
    };
};

struct Debug_State
{
    b32 init;

    Platform_Work_Queue *high_priority_queue;

    Memory_Arena debug_arena;
    Memory_Arena per_frame_arena;
    
    Render_Group *render_group;

    Game_Assets *game_assets;
    Font *font;

    b32 compiling;
    Debug_Executing_Process compiler;

    v2 menu_p;
    b32 menu_active;

    u32 selected_id_count;
    Debug_ID selected_id[64];

    Debug_Element *element_hash[256];
    Debug_View *view_hash[4096];
    Debug_Variable_Group *root_group;
    Debug_Tree tree_sentinel;

    v2 last_mouse_p;
    Debug_Interaction interaction;
    Debug_Interaction hot_interaction;
    Debug_Interaction next_hot_interaction;
    b32 paused;

    f32 left_edge;
    f32 right_edge;
    f32 at_y;

    f32 width;
    f32 height;

    char *scope_to_record;

    u32 total_frame_count;
    u32 frame_count;
    Debug_Frame *oldest_frame;
    Debug_Frame *most_recent_frame;

    Debug_Frame *collation_frame;

    u32 frame_bar_lane_count;
    Debug_Thread *first_thread;
    Debug_Thread *first_free_thread;
    Open_Debug_Block *first_free_block;

    // per-frame store management.
    Debug_Stored_Event *first_free_stored_event;
    Debug_Frame *first_free_frame;

};

#define DEBUG_H
#endif
