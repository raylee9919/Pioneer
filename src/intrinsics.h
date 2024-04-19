 /* ―――――――――――――――――――――――――――――――――――◆――――――――――――――――――――――――――――――――――――
    $File: $
    $Date: $
    $Revision: $
    $Creator: Sung Woo Lee $
    $Notice: (C) Copyright 2024 by Sung Woo Lee. All Rights Reserved. $
    ―――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――― */

#include <intrin.h>

#if     __MSVC
inline u32
atomic_compare_exchange_u32(u32 *value, u32 _new, u32 expected) {
    u32 result = _InterlockedCompareExchange((long *)value, _new, expected);
    return result;
}

inline u32
atomic_exchange_u32(u32 *value, u32 _new) {
    u32 result = _InterlockedExchange((long *)value, _new);
    return result;
}

inline u64
atomic_exchange_u64(u64 *value, u64 _new) {
    u64 result = _InterlockedExchange64((long long *)value, _new);
    return result;
}

inline u32
atomic_add_u32(u32 *value, u32 addend) {
    // returns the original value.
    u32 result = _InterlockedExchangeAdd((long *)value, addend);
    return result;
}

inline u64
atomic_add_u64(u64 *value, u64 addend) {
    // returns the original value.
    u64 result = _InterlockedExchangeAdd64((long long *)value, addend);
    return result;
}
#elif   __LLVM

#endif

//
// @Trigonometry
//
inline r32
cos(r32 x) {
    r32 result = _mm_cvtss_f32(_mm_cos_ps(_mm_set1_ps(x)));
    return result;
}

inline r32
sin(r32 x) {
    r32 result = _mm_cvtss_f32(_mm_sin_ps(_mm_set1_ps(x)));
    return result;
}


inline r32
tan(r32 x) {
    r32 result = _mm_cvtss_f32(_mm_tan_ps(_mm_set1_ps(x)));
    return result;
}

//
// @Convert
//
inline r32
sqrt(r32 x) {
    r32 result = _mm_cvtss_f32(_mm_sqrt_ss(_mm_set_ss(x)));
    return result;
}

inline s32 
round_r32_to_s32(r32 x) {
    s32 result = _mm_cvtss_si32(_mm_set_ss(x));
    return result;
}

inline u32 
round_r32_to_u32(r32 x) {
    s32 result = (u32)_mm_cvtss_si32(_mm_set_ss(x));
    return result;
}

inline s32
FloorR32ToS32(r32 A) {
    s32 result = (s32)A;
    return result;
}

inline s32
CeilR32ToS32(r32 A) {
    s32 result = (s32)A + 1;
    return result;
}

//
// @
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
    for (u32 test = 0;
        test < 32;
        ++test) {
        if (value & (1 << test)) {
            result.index = test;
            result.found = true;
            break;
        }
    }
#endif
    
    return result;
}
