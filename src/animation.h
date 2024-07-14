#ifndef ANIMATION_H
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Sung Woo Lee $
   $Notice: (C) Copyright 2024 by Sung Woo Lee. All Rights Reserved. $
   ======================================================================== */

struct Animation_Hash_Slot
{
    u32 id;
    u32 idx;
    Animation_Hash_Slot *next;
};
struct Animation_Hash_Entry
{
    Animation_Hash_Slot *first;
};
struct Animation_Hash_Table
{
    u32 entry_count;
    Animation_Hash_Entry *entries;
};

//
//
//

struct dt_v3_Pair
{
    f32     dt;
    v3      vec;
};
struct dt_qt_Pair
{
    f32     dt;
    qt      q;
};
struct Sample
{
    s32         id;

    u32         translation_count;
    u32         rotation_count;
    u32         scaling_count;

    dt_v3_Pair  *translations;
    dt_qt_Pair  *rotations;
    dt_v3_Pair  *scalings;
};
struct Animation
{
    char        *name;

    f32         duration;

    u32         sample_count;
    Sample      *samples;

    //

    Animation_Hash_Table hash_table;
};

    
#define ANIMATION_H
#endif
