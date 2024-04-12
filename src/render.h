 /* ―――――――――――――――――――――――――――――――――――◆――――――――――――――――――――――――――――――――――――
    $File: $
    $Date: $
    $Revision: $
    $Creator: Sung Woo Lee $
    $Notice: (C) Copyright 2024 by Sung Woo Lee. All Rights Reserved. $
    ―――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――― */

enum RenderType {
    RenderType_RenderEntityBitmap,
    RenderType_RenderEntityRect,
    RenderType_Render_Text,
};

struct Render_Entity_Header {
    RenderType type;
    u32 size;
    v3 base;
};

//
// Render Entities
//

struct Game_Assets;

struct Render_Text {
    Render_Entity_Header header;
    const char *str;
    Game_Assets *game_assets;
    r32 scale;
    v4 color;
};

struct RenderEntityBitmap {
    Render_Entity_Header header;
    v3 base;
    v3 origin;
    v3 axis_x;
    v3 axis_y;
    Bitmap *bitmap;
    v4 color;
};

struct RenderEntityRect {
    Render_Entity_Header header;
    v2 min;
    v2 max;
    v4 color;
};


struct Sort_Entry {
    v3 base;
    void *render_entity;
};

struct Camera {
    b32     orthographic;
    m4x4    transform;
    r32     focal_length;
    v2      screen_dim;
    v3      pos;
};

struct Render_Group {
    size_t capacity;
    size_t used;
    u8 *base;
    u8 *sort_entry_begin;
    Camera camera;
};

