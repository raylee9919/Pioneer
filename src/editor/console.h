#ifndef CONSOLE_H
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Sung Woo Lee $
   $Notice: (C) Copyright 2024 by Sung Woo Lee. All Rights Reserved. $
   ======================================================================== */

#define CONSOLE_REMAIN_TIME_INIT 0.2f

struct Font;

struct Input_Buffer
{
    char        *buffer;
    u32         length;
    u32         at;
    v4          color;
    v2          offset;
};

struct Cursor
{
    v2          offset;
    v2          dim;
    v4          color1;
    v4          color2;
};

struct Console
{
    b32         initted;

    f32         remain_t;
    f32         current_y;
    b32         is_down;
    v2          half_dim;
    v4          bg_color;

    Cursor      cursor;
    Font        *font;

    Input_Buffer input_buffer;
};

#define CONSOLE_H
#endif
