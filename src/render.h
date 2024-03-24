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
    RenderType_RenderEntityCoordinateSystem
};

struct RenderEntityHeader {
    RenderType type;
};

//
// Render Entities
//

struct RenderEntityClear {
    RenderEntityHeader header;
};

struct RenderEntityBmp {
    RenderEntityHeader header;
    vec2 origin;
    vec2 axisX;
    vec2 axisY;
    Bitmap *bmp;
    r32 alpha;
};

struct RenderEntityRect {
    RenderEntityHeader header;
    vec2 min;
    vec2 max;
    vec4 color;
};

struct RenderEntityCoordinateSystem {
    RenderEntityHeader header;
    vec2 origin;
    vec2 axisX;
    vec2 axisY;
    Bitmap *bmp;
};

struct RenderGroup {
    size_t capacity;
    size_t used;
    u8 *base;
};

#define RENDER_H
#endif
