#ifndef DEBUG_H
#define DEBUG_H
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Sung Woo Lee $
   $Notice: (C) Copyright 2024 by Sung Woo Lee. All Rights Reserved. $
   ======================================================================== */

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
    u32     lane_idx;
    f32     min_t;
    f32     max_t;
};

#define MAX_REGIONS_PER_FRAME 256
struct Debug_Frame
{
    u64 begin_clock;
    u64 end_clock;
    u32 region_count;
    Debug_Frame_Region *regions;
};

struct Open_Debug_Block
{
    u32 starting_frame_idx;
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

    Memory_Arena collate_arena;
    Temporary_Memory collate_tmp;

    u32 frame_bar_lane_count;
    u32 frame_count;
    f32 frame_bar_scale;

    Debug_Frame *frames;
    Debug_Thread *first_thread;
    Open_Debug_Block *first_free_block;
};

struct Render_Group;

internal void debug_reset(u32 width, u32 height);
internal void debug_overlay(Game_Memory *game_memory, f32 width, f32 height, Game_Assets *game_assets);

#endif
