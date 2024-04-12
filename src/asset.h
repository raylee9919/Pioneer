/* ―――――――――――――――――――――――――――――――――――◆――――――――――――――――――――――――――――――――――――
   $File: $
   $Date: $
   $Revision: $
   $Creator: Sung Woo Lee $
   $Notice: (C) Copyright 2024 by Sung Woo Lee. All Rights Reserved. $
   ―――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――― */

// let's discriminimate microsoft's bmp and "pure" bitmap.
#pragma pack(push, 1)
struct BMP_Info_Header {
    u16 filetype;
    u32 filesize;
    u16 reserved1;
    u16 reserved2;
    u32 bitmap_offset;
    u32 bitmap_info_header_size;
    s32 width;
    s32 height;
    s16 plane;
    u16 bpp; // bits
    u32 compression;
    u32 image_size;
    u32 h_resolution;
    u32 v_resolution;
    u32 plt_entry_cnt;
    u32 important;

    u32 r_mask;
    u32 g_mask;
    u32 b_mask;
};

struct Bitmap {
    s32 width;
    s32 height;
    s32 pitch;
    s32 handle;
    size_t size;
    void *memory;
};

enum Asset_ID {
    GAI_Tree,
    GAI_Particle,
    GAI_Golem,

    GAI_Count
};

enum Asset_State {
    Asset_State_Unloaded,
    Asset_State_Queued,
    Asset_State_Loaded
};

//
// Font
//
struct Asset_Font_Header {
    u32 kerning_pair_count;
    u32 vertical_advance;
};
struct Asset_Kerning {
    u32 first;
    u32 second;
    s32 value;
};
struct Asset_Glyph {
    u32 codepoint;
    s32 ascent;
    s32 A;
    s32 B;
    s32 C;
    Bitmap bitmap;
};

#pragma pack(pop)
