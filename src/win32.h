 /* ―――――――――――――――――――――――――――――――――――◆――――――――――――――――――――――――――――――――――――
    $File: $
    $Date: $
    $Revision: $
    $Creator: Sung Woo Lee $
    $Notice: (C) Copyright 2024 by Sung Woo Lee. All Rights Reserved. $
    ―――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――― */

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
    b32 is_recording;
    b32 is_playing;
    void *game_memory;
    u64 game_mem_total_cap;
};
