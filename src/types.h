/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Sung Woo Lee $
   $Notice: (C) Copyright %s by Sung Woo Lee. All Rights Reserved. $
   ======================================================================== */

#include <stdint.h>
#include <stddef.h>
#include <limits.h>
#include <float.h>

#define internal        static
#define global_var      static
#define local_persist   static

typedef int8_t int8;  
typedef int16_t int16; 
typedef int32_t int32; 
typedef int64_t int64; 

typedef int8_t s8;  
typedef int16_t s16; 
typedef int32_t s32; 
typedef int64_t s64; 

typedef int32 bool32;
typedef bool32 b32;

typedef uint8_t uint8;
typedef uint16_t uint16; 
typedef uint32_t uint32; 
typedef uint64_t uint64; 

typedef uint8_t u8;  
typedef uint16_t u16; 
typedef uint32_t u32; 
typedef uint64_t u64; 

typedef float f32; 
typedef double f64; 

#define F32_MIN FLT_MIN
#define F32_MAX FLT_MAX
