#ifndef DEBUG_H
#define DEBUG_H
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Sung Woo Lee $
   $Notice: (C) Copyright 2024 by Sung Woo Lee. All Rights Reserved. $
   ======================================================================== */

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

#define MAX_REGIONS_PER_FRAME 2 * 4096
struct Debug_Frame
{
    u64 begin_clock;
    u64 end_clock;
    f32 wall_seconds_elapsed;

    u32 region_count;
    Debug_Frame_Region *regions;
};

struct Open_Debug_Block
{
    u32 starting_frame_idx;
    Debug_Record *src;
    Debug_Event *opening_event;
    Open_Debug_Block *parent;

    Open_Debug_Block *next_free;
};

struct Debug_Thread
{
    u32 id;
    u32 lane_idx;
    Open_Debug_Block *first_open_block;
    Debug_Thread *next;
};

struct Debug_State
{
    b32 init;

    b32 is_debug_mode;
    f32 debug_toggle_delay;

    Platform_Work_Queue *high_priority_queue;

    Memory_Arena debug_arena;
    Render_Group *render_group;

    f32 left_edge;
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

    Rect2 profile_rect;

    Debug_Frame *frames;
    Debug_Thread *first_thread;
    Open_Debug_Block *first_free_block;
};

internal void debug_start(u32 width, u32 height);
internal void debug_end(Game_Input *input, Game_Assets *game_assets);
internal void refresh_collation(Debug_State *debug_state);

#endif
