#ifndef RENDER_H
 /* ―――――――――――――――――――――――――――――――――――◆――――――――――――――――――――――――――――――――――――
    $File: $
    $Date: $
    $Revision: $
    $Creator: Sung Woo Lee $
    $Notice: (C) Copyright 2024 by Sung Woo Lee. All Rights Reserved. $
    ―――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――― */

enum RenderType {
    RenderType_RenderEntityClear,
    RenderType_RenderEntityBmp,
    RenderType_RenderEntityRect,
    RenderType_Render_Text,
    RenderType_RenderEntityCoordinateSystem
};

struct RenderEntityHeader {
    RenderType type;
    u32 size;
    v3 base;
};

//
// Render Entities
//

struct RenderEntityClear {
    RenderEntityHeader header;
};

struct Game_Assets;

struct Render_Text {
    RenderEntityHeader header;
    const char *str;
    Game_Assets *game_assets;
    r32 scale;
    v4 color;
};

struct RenderEntityBmp {
    RenderEntityHeader header;
    v3 base;
    v2 origin;
    v2 axisX;
    v2 axisY;
    Bitmap *bmp;
    v4 color;
};

struct RenderEntityRect {
    RenderEntityHeader header;
    v2 min;
    v2 max;
    v4 color;
};

struct RenderEntityCoordinateSystem {
    RenderEntityHeader header;
    v2 origin;
    v2 axisX;
    v2 axisY;
    Bitmap *bmp;
};

struct Sort_Entry {
    v3 base;
    void *render_entity;
};

struct Render_Group {
    size_t capacity;
    size_t used;
    u8 *base;
    u8 *sort_entry_begin;
};

#define RENDER_H
#endif
