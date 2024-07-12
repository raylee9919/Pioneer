#ifndef ANIMATION_H
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Sung Woo Lee $
   $Notice: (C) Copyright 2024 by Sung Woo Lee. All Rights Reserved. $
   ======================================================================== */

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
struct Animation_Node
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
    char           *name;

    f32            duration;
    u32            node_count;

    Animation_Node  *nodes;
};

    
#define ANIMATION_H
#endif
