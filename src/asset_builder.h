#ifndef ASSET_BUILDER_H
 /* ―――――――――――――――――――――――――――――――――――◆――――――――――――――――――――――――――――――――――――
    $File: $
    $Date: $
    $Revision: $
    $Creator: Sung Woo Lee $
    $Notice: (C) Copyright 2024 by Sung Woo Lee. All Rights Reserved. $
    ―――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――― */

#pragma pack(push, 1)
struct Bitmap_Info_Header {
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
    void *memory;
};

struct Package_Header {
    u32 magic;
    u32 file_version;
    u32 flags;

    u64 table_of_contents_offset;
};

struct Package {
    Package_Header header;
};

#pragma pack(pop)

#define ASSET_BUILDER_H
#endif
