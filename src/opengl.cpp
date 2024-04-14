/* ―――――――――――――――――――――――――――――――――――◆――――――――――――――――――――――――――――――――――――
   $File: $
   $Date: $
   $Revision: $
   $Creator: Sung Woo Lee $
   $Notice: (C) Copyright 2024 by Sung Woo Lee. All Rights Reserved. $
   ―――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――― */


#include "render_group.h"


#define GL_SRGB8_ALPHA8                     0x8C43
#define GL_FRAMEBUFFER_SRGB                 0x8DB9            
#define GL_SHADING_LANGUAGE_VERSION         0x8B8C


typedef BOOL Wgl_Swap_Interval(int interval);
Wgl_Swap_Interval *wgl_swap_interval;


//
// Forward Declaration
//
internal inline size_t
str_len(const char *str);

internal void *
str_find(char *text, size_t t_len, char *pattern, size_t p_len);

internal inline b32
is_whitespace(char c);

internal b32
str_match(char *text, char *pattern, size_t len);

//
// Struct
//
#define GL_EXTENSION_LIST           \
    X(GL_EXT_texture_sRGB)          \
    X(GL_EXT_framebuffer_sRGB)

#define X(ext) ext,
enum GL_Extensions {
    GL_EXTENSION_LIST
    GL_EXT_COUNT
};
#undef X

struct GL_Info {
    char *vendor;
    char *renderer;
    char *version;
    char *shading_language_version;
    char *extensions;

    b32 has_ext[GL_EXT_COUNT];
};


//
// Global
//
global_var GLint g_gl_texture_internal_format = GL_RGBA8;


internal GL_Info
gl_get_info() {
    GL_Info result = {};

    result.vendor                   = (char *)glGetString(GL_VENDOR);
    result.renderer                 = (char *)glGetString(GL_RENDERER);
    // TODO: availabe from gl2.
    result.shading_language_version = "N/A";
    result.version                  = (char *)glGetString(GL_VERSION);
    result.extensions               = (char *)glGetString(GL_EXTENSIONS);

#define X(ext) #ext,
    char *ext_str_table[] = {
        GL_EXTENSION_LIST
    };
#undef X
    size_t ext_str_len[GL_EXT_COUNT];
    for (s32 idx = 0;
            idx < ArrayCount(ext_str_table);
            ++idx)
    {
        ext_str_len[idx] = str_len(ext_str_table[idx]);
    }


    for (char *tk_begin = result.extensions;
            *tk_begin;
        )
    {
        if (is_whitespace(*tk_begin))
        {
            ++tk_begin;
        }
        else 
        {
            char *tk_end = tk_begin;
            while (!is_whitespace(*(tk_end + 1)))
            {
                ++tk_end;
            }

            size_t t_len = (tk_end - tk_begin + 1);

            for (u32 idx = 0;
                    idx < ArrayCount(ext_str_table);
                    ++idx)
            {
                char *ext_str = ext_str_table[idx];
                size_t p_len = ext_str_len[idx];
                if (t_len == p_len)
                {
                    b32 match = str_match(tk_begin, ext_str, t_len);
                    if (match)
                    {
                        result.has_ext[idx] = true;
                        break;
                    }

                } 
                else
                {
                    continue;
                }
            }



            tk_begin = tk_end + 1;
        }
    }



    

    return result;
}

internal void
gl_draw_rect(v2 min, v2 max, v4 color) {
    glColor4f(color.r, color.g, color.b, color.a);
    glBegin(GL_TRIANGLES);

    glVertex2f(min.x, min.y);
    glVertex2f(max.x, min.y);
    glVertex2f(max.x, max.y);

    glVertex2f(min.x, min.y);
    glVertex2f(min.x, max.y);
    glVertex2f(max.x, max.y);

    glEnd();
}

internal void
gl_draw_bitmap(v3 V[4], Bitmap *bitmap, v4 color) {
    glBegin(GL_TRIANGLES);

    glColor4f(color.r, color.g, color.b, color.a);
    
    // upper triangle.
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(V[0].x,
               V[0].y,
               V[0].z);

    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(V[1].x,
               V[1].y,
               V[1].z);

    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(V[3].x,
               V[3].y,
               V[3].z);

    // bottom triangle.
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(V[0].x,
               V[0].y,
               V[0].z);

    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(V[2].x,
               V[2].y,
               V[2].z);

    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(V[3].x,
               V[3].y,
               V[3].z);

    glEnd();
}

internal void
gl_draw_cube(v3 V[8]) {
    glBegin(GL_TRIANGLES);
    
    v4 tc = v4{0.5f, 0.5f, 0.5f, 1.0f};
    v4 bc = v4{0.05f, 0.05f, 0.05f, 1.0f};

    glColor4f(tc.r, tc.g, tc.b, tc.a);
    glVertex3f(V[0].x, V[0].y, V[0].z);
    glVertex3f(V[1].x, V[1].y, V[1].z);
    glVertex3f(V[2].x, V[2].y, V[2].z);

    glVertex3f(V[0].x, V[0].y, V[0].z);
    glVertex3f(V[2].x, V[2].y, V[2].z);
    glVertex3f(V[3].x, V[3].y, V[3].z);

    glColor4f(bc.r, bc.g, bc.b, bc.a);
    glVertex3f(V[4].x, V[4].y, V[4].z);
    glVertex3f(V[5].x, V[5].y, V[5].z);
    glVertex3f(V[6].x, V[6].y, V[6].z);

    glVertex3f(V[4].x, V[4].y, V[4].z);
    glVertex3f(V[6].x, V[6].y, V[6].z);
    glVertex3f(V[7].x, V[7].y, V[7].z);


    glColor4f(bc.r, bc.g, bc.b, bc.a);
    glVertex3f(V[4].x, V[4].y, V[4].z);
    glVertex3f(V[5].x, V[5].y, V[5].z);
    glColor4f(tc.r, tc.g, tc.b, tc.a);
    glVertex3f(V[1].x, V[1].y, V[1].z);

    glColor4f(bc.r, bc.g, bc.b, bc.a);
    glVertex3f(V[4].x, V[4].y, V[4].z);
    glColor4f(tc.r, tc.g, tc.b, tc.a);
    glVertex3f(V[1].x, V[1].y, V[1].z);
    glVertex3f(V[0].x, V[0].y, V[0].z);

    glColor4f(bc.r, bc.g, bc.b, bc.a);
    glVertex3f(V[5].x, V[5].y, V[5].z);
    glVertex3f(V[7].x, V[7].y, V[7].z);
    glColor4f(tc.r, tc.g, tc.b, tc.a);
    glVertex3f(V[3].x, V[3].y, V[3].z);

    glColor4f(bc.r, bc.g, bc.b, bc.a);
    glVertex3f(V[5].x, V[5].y, V[5].z);
    glColor4f(tc.r, tc.g, tc.b, tc.a);
    glVertex3f(V[3].x, V[3].y, V[3].z);
    glVertex3f(V[1].x, V[1].y, V[1].z);

    glColor4f(bc.r, bc.g, bc.b, bc.a);
    glVertex3f(V[7].x, V[7].y, V[7].z);
    glVertex3f(V[6].x, V[6].y, V[6].z);
    glColor4f(tc.r, tc.g, tc.b, tc.a);
    glVertex3f(V[3].x, V[3].y, V[3].z);

    glColor4f(bc.r, bc.g, bc.b, bc.a);
    glVertex3f(V[7].x, V[7].y, V[7].z);
    glColor4f(tc.r, tc.g, tc.b, tc.a);
    glVertex3f(V[2].x, V[2].y, V[2].z);
    glVertex3f(V[3].x, V[3].y, V[3].z);

    glColor4f(bc.r, bc.g, bc.b, bc.a);
    glVertex3f(V[6].x, V[6].y, V[6].z);
    glVertex3f(V[4].x, V[4].y, V[4].z);
    glColor4f(tc.r, tc.g, tc.b, tc.a);
    glVertex3f(V[0].x, V[0].y, V[0].z);

    glColor4f(bc.r, bc.g, bc.b, bc.a);
    glVertex3f(V[6].x, V[6].y, V[6].z);
    glColor4f(tc.r, tc.g, tc.b, tc.a);
    glVertex3f(V[0].x, V[0].y, V[0].z);
    glVertex3f(V[2].x, V[2].y, V[2].z);
    

    glEnd();
}

inline void
gl_load_matrix(m4x4 M) {
    M = transpose(M);
    glLoadMatrixf(&M.e[0][0]);
}

internal void
gl_load_projection(Camera *camera) {
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glMatrixMode(GL_PROJECTION);

    if (camera->orthographic) {
        gl_load_matrix(camera->projection);

    } else {
        r32 f = camera->focal_length;
        r32 a = camera->width_over_height * f;

        r32 N = 0.1f;
        r32 F = 100.0f;
        r32 b = (N + F) / (N - F);
        r32 c = (2 * N * F) / (N - F);

        m4x4 proj = {{
            { f,  0,  0,  0},
            { 0,  a,  0,  0},
            { 0,  0,  b,  c},
            { 0,  0, -1,  0}
        }};

        // in terms of procedure, it goes backward.
        proj = proj * camera->projection;

        gl_load_matrix(proj);
    }

}

internal void
gl_render_batch(HDC hdc, Render_Batch *batch, u32 win_w, u32 win_h) {
    glViewport(0, 0, win_w, win_h);


    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_SCISSOR_TEST);

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.2f, 0.3f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glClearDepth(1.0f);
    glClear(GL_DEPTH_BUFFER_BIT);
    glDepthFunc(GL_LEQUAL);

    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GREATER, 0.0f);

    glEnable(GL_TEXTURE_2D);


    for (Render_Group *group = (Render_Group *)batch->base;
         (u8 *)group < (u8 *)batch->base + batch->used;
         ++group)
    {

        gl_load_projection(&group->camera);

        for (Sort_Entry *entry = (Sort_Entry *)group->sort_entry_begin;
             (u8 *)entry < group->base + group->capacity;
             ++entry)
        {

            Render_Entity_Header *entity =(Render_Entity_Header *)entry->render_entity;

            switch (entity->type) {
                case eRenderEntityBitmap: {
                    RenderEntityBitmap *piece = (RenderEntityBitmap *)entity;
                    local_persist s32 handle_idx = 1;
                    Bitmap *bitmap = piece->bitmap;
                    if (bitmap->handle) {
                        glBindTexture(GL_TEXTURE_2D, bitmap->handle);
                    }
                    else {
                        bitmap->handle = handle_idx++;
                        glBindTexture(GL_TEXTURE_2D, bitmap->handle);
                        glTexImage2D(GL_TEXTURE_2D, 0, g_gl_texture_internal_format, bitmap->width, bitmap->height,
                                     0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, (u8 *)bitmap->memory + bitmap->pitch * (bitmap->height - 1));

                        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
                        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
                        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
                    }

                    v3 vertices[] = {
                        piece->origin,
                        piece->origin + piece->axis_x,
                        piece->origin + piece->axis_y,
                        piece->origin + piece->axis_x + piece->axis_y
                    };

#if 0
                    // get translation
                    v3 C = get_column(group->camera.transform, 3);
                    for (int idx = 0;
                         idx < ArrayCount(vertices);
                         ++idx) 
                    {
                        v3 V = vertices[idx];
                        r32 inv_z = 1.0f / (C.z - V.z);
                        vertices[idx].x *= inv_z;
                        vertices[idx].y *= inv_z;
                    }
#endif

                    gl_draw_bitmap(vertices, bitmap, piece->color);
                } break;

                case eRender_Text: {
                    Render_Text *piece = (Render_Text *)entity;
                } break;

                case eRenderEntityRect: {
                    RenderEntityRect *piece = (RenderEntityRect *)entity;
                    glDisable(GL_TEXTURE_2D);
                    gl_draw_rect(piece->min, piece->max, piece->color);
                    glEnable(GL_TEXTURE_2D);
                } break;

                case eRender_Cube: {
                    glDisable(GL_TEXTURE_2D);
                    Render_Cube *piece = (Render_Cube *)entity;
                    v3 B = piece->base;
                    r32 H = piece->height;
                    r32 R = piece->radius;
                    r32 min_x = B.x - R;
                    r32 max_x = B.x + R;
                    r32 min_y = B.y - R;
                    r32 max_y = B.y + R;
                    r32 min_z = B.z - H;
                    r32 max_z = B.z;
                    v3 vertices[8] = {
                        v3{min_x, min_y, max_z},
                        v3{max_x, min_y, max_z},
                        v3{max_x, max_y, max_z},
                        v3{min_x, max_y, max_z},
                        v3{min_x, min_y, min_z},
                        v3{max_x, min_y, min_z},
                        v3{max_x, max_y, min_z},
                        v3{min_x, max_y, min_z},
                    };
                    gl_draw_cube(vertices);
                    glEnable(GL_TEXTURE_2D);
                } break;

                INVALID_DEFAULT_CASE
            }

        }
    }
            
    SwapBuffers(hdc);
    batch->used = 0;
}
    

