/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Sung Woo Lee $
   $Notice: (C) Copyright 2024 by Sung Woo Lee. All Rights Reserved. $
   ======================================================================== */

#include "console.h"

internal void
init_console(Console *console, Font *font, f32 screen_height, f32 screen_width, Memory_Arena *arena)
{
    Assert(!console->initted);

    console->half_dim = v2{600.f, 250.0f};
    console->remain_t = 0.0f;
    console->is_down = false;
    console->current_y = screen_height + console->half_dim.y;
    console->bg_color = v4{0.02f, 0.02f, 0.02f, 0.9f};
    console->font = font;

    Input_Buffer *ib = &console->input_buffer;
    u32 len = 64;
    ib->length = len;
    ib->buffer = push_array(arena, char, len + 1);
    ib->buffer[len] = 0;
    ib->at = 0;
    ib->color = v4{1.0f, 1.0f, 0.7f, 1.0f};
    ib->offset = v2{5.0f, 10.0f};

    Cursor *cursor = &console->cursor;
    // @TODO: We need max ascend and descend.
    cursor->offset = ib->offset;
    cursor->offset.y -= font->descent;
    cursor->dim = v2{font->max_char_width, font->v_advance};
    cursor->color1 = v4{0.1f, 0.1f, 0.1f, 1.0f};
    cursor->color2 = v4{1.0f, 1.0f, 1.0f, 1.0f};

    console->initted = true;
}
