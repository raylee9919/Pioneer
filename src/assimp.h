/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Sung Woo Lee $
   $Notice: (C) Copyright %s by Sung Woo Lee. All Rights Reserved. $
   ======================================================================== */

#include "types.h"

#define KB(X) (   X *  1024ll)
#define MB(X) (KB(X) * 1024ll)
#define GB(X) (MB(X) * 1024ll)
#define TB(X) (GB(X) * 1024ll)
#define max(A, B) (A > B ? A : B)
#define min(A, B) (A < B ? A : B)
#define assert(EXP) if (!(EXP)) { *(volatile int *)0 = 0; }
#define array_count(ARRAY) (sizeof(ARRAY) / sizeof(ARRAY[0]))

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
    struct
    {
        f32 r, g, b;
    };
    f32 e[3];
};

inline v3
_v3_(f32 x, f32 y, f32 z)
{
    v3 result;
    result.x = x;
    result.y = y;
    result.z = z;
    return result;
}

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


//
//
//
