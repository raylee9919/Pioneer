/* ―――――――――――――――――――――――――――――――――――◆――――――――――――――――――――――――――――――――――――
   $File: $
   $Date: $
   $Revision: $
   $Creator: Sung Woo Lee $
   $Notice: (C) Copyright 2024 by Sung Woo Lee. All Rights Reserved. $
   ―――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――― */

union v2
{
    struct
    {
        f32 x, y;
    };
    f32 e[2];
};

union v3
{
    struct
    {
        f32 x, y, z;
    };
    f32 e[3];
};

union v4
{
    struct
    {
        f32 w, x, y, z;
    };
    struct
    {
        f32 r, g, b, a;
    };
    f32 e[4];
};

struct qt
{
    f32 w, x, y, z;
};

inline v4
_v4_(f32 r, f32 g, f32 b, f32 a)
{
    v4 result;
    result.r = r;
    result.g = g;
    result.b = b;
    result.a = a;
    return result;
}

struct m4x4 
{
    f32 e[4][4];
};

struct Memory_Arena
{
    size_t  size;
    size_t  used;
    void    *base;
};

#define push_struct(arena, type)         (type *)push_size(arena, sizeof(type))
#define push_array(arena, type, count)   (type *)push_size(arena, count * sizeof(type))
static void *
push_size(Memory_Arena *arena, size_t size)
{
    assert((arena->used + size) <= arena->size);
    void *result = (u8 *)arena->base + arena->used;
    arena->used += size;
    return result;
}

    

