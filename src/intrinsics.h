/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Sung Woo Lee $
   $Notice: (C) Copyright %s by Sung Woo Lee. All Rights Reserved. $
   ======================================================================== */

#include <intrin.h>


inline u32
get_thread_id()
{
    u8 *thread_local_storage = (u8 *)__readgsqword(0x30);
    u32 thread_id = *(u32 *)(thread_local_storage + 0x48);
    return thread_id;
}

//
// @trigonometry
//
inline f32
cos(f32 x) 
{
    f32 result = _mm_cvtss_f32(_mm_cos_ps(_mm_set1_ps(x)));
    return result;
}

inline f32
acos(f32 x) 
{
    f32 result = _mm_cvtss_f32(_mm_acos_ps(_mm_set1_ps(x)));
    return result;
}

inline f32
sin(f32 x) 
{
    f32 result = _mm_cvtss_f32(_mm_sin_ps(_mm_set1_ps(x)));
    return result;
}

inline f32
tan(f32 x) 
{
    f32 result = _mm_cvtss_f32(_mm_tan_ps(_mm_set1_ps(x)));
    return result;
}

//
// @convert
//
inline f32
sqrt(f32 x) {
    f32 result = _mm_cvtss_f32(_mm_sqrt_ss(_mm_set_ss(x)));
    return result;
}

inline s32 
round_f32_to_s32(f32 x) {
    s32 result = _mm_cvtss_si32(_mm_set_ss(x));
    return result;
}

inline u32 
round_f32_to_u32(f32 x) {
    s32 result = (u32)_mm_cvtss_si32(_mm_set_ss(x));
    return result;
}

inline s32
floor_f32_to_s32(f32 A) {
    s32 result = (s32)A;
    return result;
}

inline s32
ceil_f32_to_s32(f32 A) {
    s32 result = (s32)A + 1;
    return result;
}

//
// @misc.
//
struct Bit_Scan_Result {
    b32 found;
    u32 index;
};

inline Bit_Scan_Result
find_least_significant_set_bit(u32 value) {
    Bit_Scan_Result result = {};

#if __MSVC
    result.found = _BitScanForward((unsigned long *)&result.index, value);
#else
    for (u32 test = 0; test < 32; ++test) {
        if (value & (1 << test)) {
            result.index = test;
            result.found = true;
            break;
        }
    }
#endif

    return result;
}
