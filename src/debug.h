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
    char                    *file_name;
    char                    *block_name;
    u32                     line_number;
    u32                     reserved;
    Debug_Counter_Snapshot  snapshots[120];
};

struct Debug_State
{
    u32                     snapshot_idx;
    u32                     counter_count;
    Debug_Counter_State     counter_states[512];
};

struct Render_Group;
internal void
debug_overlay(Game_Memory *game_memory, Render_Group *render_group, Game_Assets *game_assets, f32 *cen_y);

#endif
