 /* ―――――――――――――――――――――――――――――――――――◆――――――――――――――――――――――――――――――――――――
    $File: $
    $Date: $
    $Revision: $
    $Creator: Sung Woo Lee $
    $Notice: (C) Copyright 2024 by Sung Woo Lee. All Rights Reserved. $
    ―――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――― */

enum Render_Type {
    eRender_Bitmap,
    eRender_Text,
    eRender_Cube
};

struct Render_Entity_Header {
    Render_Type type;
    u32         size;
    v3          base;
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

struct Render_Bitmap {
    Render_Entity_Header header;
    v3 V[4];
    Bitmap *bitmap;
    v4 color;
};

struct Render_Cube {
    Render_Entity_Header header;
    v3      base;
    r32     radius;
    r32     height;
    v4      color;
};


struct Sort_Entry {
    v3 base;
    void *render_entity;
};

struct Camera {
    b32     orthographic;
    r32     focal_length;
    union {
        r32     width_over_height;
        r32     aspect_ratio;
    };
    m4x4    projection;
};

struct Render_Group {
    size_t capacity;
    size_t used;
    u8 *base;
    u8 *sort_entry_begin;
    Camera camera;
};

