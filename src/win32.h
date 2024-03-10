#ifndef WIN32_GAME_H
/* ========================================================================
$File: $
$Date: $
$Revision: $
$Creator: Sung Woo Lee $
$Notice: (C) Copyright 2024 by Sung Woo Lee. All Rights Reserved. $
======================================================================== */

struct Win32ScreenBuffer {
    BITMAPINFO bitmap_info;
    void *memory;
    int width;
    int height;
    int bpp;
};

struct Win32WindowDimension {
    int width;
    int height;
};

struct Win32State {
    HANDLE record_file;
    bool32 is_recording;
    bool32 is_playing;
    void *game_memory;
    uint64 game_mem_total_cap;
};

#define WIN32_GAME_H
#endif
