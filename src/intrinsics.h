 /* ―――――――――――――――――――――――――――――――――――◆――――――――――――――――――――――――――――――――――――
    $File: $
    $Date: $
    $Revision: $
    $Creator: Sung Woo Lee $
    $Notice: (C) Copyright 2024 by Sung Woo Lee. All Rights Reserved. $
    ―――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――― */

#include <intrin.h>
#include <math.h>

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

inline s32 
RoundR32ToS32(r32 value) {
    s32 result = (s32)(value + 0.5);
    return result;
}

inline u32 
RoundR32ToU32(r32 value) {
    u32 result = (u32)(value + 0.5);
    return result;
}

internal r32
cos(r32 x) {
    r32 result = cosf(x);
    return result;
}

internal r32
sin(r32 x) {
    r32 result = sinf(x);
    return result;
}

internal r32
sqrt(r32 x) {
    r32 result = sqrtf(x);
    return result;
}

internal s32
FloorR32ToS32(r32 A) {
    s32 result = (s32)A;
    return result;
}

internal s32
CeilR32ToS32(r32 A) {
    s32 result = (s32)A + 1;
    return result;
}

struct Bit_Scan_Result {
    b32 found;
    u32 index;
};

inline Bit_Scan_Result
find_least_significant_set_bit(u32 value) {
    Bit_Scan_Result result = {};

#if COMPILER_MSVC
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
