 /* ―――――――――――――――――――――――――――――――――――◆――――――――――――――――――――――――――――――――――――
    $File: $
    $Date: $
    $Revision: $
    $Creator: Sung Woo Lee $
    $Notice: (C) Copyright 2024 by Sung Woo Lee. All Rights Reserved. $
    ―――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――― */

struct Win32ScreenBuffer 
{
    BITMAPINFO bitmap_info;
    void *memory;
    int width;
    int height;
    int bpp;
};

struct Win32WindowDimension 
{
    int width;
    int height;
};

struct Win32State 
{
    HANDLE record_file;
    b32 is_recording;
    b32 is_playing;
    void *game_memory;
    u64 game_mem_total_cap;
};


#define WGL_CONTEXT_MAJOR_VERSION_ARB               0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB               0x2092
#define WGL_CONTEXT_LAYER_PLANE_ARB                 0x2093
#define WGL_CONTEXT_FLAGS_ARB                       0x2094
#define WGL_CONTEXT_PROFILE_MASK_ARB                0x9126

#define WGL_CONTEXT_DEBUG_BIT_ARB                   0x0001
#define WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB      0x0002
#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB            0x00000001
#define WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB   0x00000002

#define ERROR_INVALID_VERSION_ARB                   0x2095
#define ERROR_INVALID_PROFILE_ARB                   0x2096

#define WGL_GET_PROC_ADDRESS(Name) Name = (Type_##Name *)wglGetProcAddress(#Name)

const int wgl_attrib_list[] = 
{
    WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
    WGL_CONTEXT_MINOR_VERSION_ARB, 0,
    WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB
#if __DEBUG
        | WGL_CONTEXT_DEBUG_BIT_ARB
#endif
        ,
    //WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB,
    WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
    0,
};


typedef HGLRC Type_wglCreateContextAttribsARB(HDC hDC, HGLRC hShareContext, const int *attribList);
global_var Type_wglCreateContextAttribsARB *wglCreateContextAttribsARB;
