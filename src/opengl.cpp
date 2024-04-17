/* ―――――――――――――――――――――――――――――――――――◆――――――――――――――――――――――――――――――――――――
   $File: $
   $Date: $
   $Revision: $
   $Creator: Sung Woo Lee $
   $Notice: (C) Copyright 2024 by Sung Woo Lee. All Rights Reserved. $
   ―――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――― */


#include "render_group.h"
#include "opengl.h"

typedef char GLchar;

#define GL_SRGB8_ALPHA8                     0x8C43
#define GL_FRAMEBUFFER_SRGB                 0x8DB9            
#define GL_SHADING_LANGUAGE_VERSION         0x8B8C
#define GL_VERTEX_SHADER                    0x8B31
#define GL_FRAGMENT_SHADER                  0x8B30
#define GL_VALIDATE_STATUS                  0x8B83
#define GL_COMPILE_STATUS                   0x8B81
#define GL_LINK_STATUS                      0x8B82
#define GL_ARRAY_BUFFER                     0x8892
#define GL_MAJOR_VERSION                    0x821B
#define GL_MINOR_VERSION                    0x821C
#define GL_CLAMP_TO_EDGE                    0x812F


typedef BOOL        Type_wglSwapIntervalEXT(int interval);
typedef GLuint      Type_glCreateShader(GLenum shaderType);
typedef void        Type_glShaderSource(GLuint shader, GLsizei count, const GLchar **string, const GLint *length);
typedef void        Type_glCompileShader(GLuint shader);
typedef GLuint      Type_glCreateProgram(void);
typedef void        Type_glAttachShader(GLuint program, GLuint shader);
typedef void        Type_glLinkProgram(GLuint program);
typedef void        Type_glGetProgramiv(GLuint program, GLenum pname, GLint *params);
typedef void        Type_glGetShaderInfoLog(GLuint shader, GLsizei maxLength, GLsizei *length, GLchar *infoLog);
typedef void        Type_glValidateProgram(GLuint program);
typedef void        Type_glGetProgramInfoLog(GLuint program, GLsizei maxLength, GLsizei *length, GLchar *infoLog);
typedef void        Type_glGenBuffers(GLsizei n, GLuint *buffers);
typedef void        Type_glBindBuffer(GLenum target, GLuint buffer);
typedef void        Type_glUniformMatrix4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
typedef GLint       Type_glGetUniformLocation(GLuint program, const GLchar *name);
typedef void        Type_glUseProgram(GLuint program);
typedef void        Type_glUniform1i (GLint location, GLint v0);
typedef void        Type_glGetIntegerv (GLenum pname, GLint *data);

#define GL_DECLARE_GLOBAL_FUNCTION(Name) global_var Type_##Name *Name
GL_DECLARE_GLOBAL_FUNCTION(wglSwapIntervalEXT);
GL_DECLARE_GLOBAL_FUNCTION(glCreateShader);
GL_DECLARE_GLOBAL_FUNCTION(glShaderSource);
GL_DECLARE_GLOBAL_FUNCTION(glCompileShader);
GL_DECLARE_GLOBAL_FUNCTION(glCreateProgram);
GL_DECLARE_GLOBAL_FUNCTION(glAttachShader);
GL_DECLARE_GLOBAL_FUNCTION(glLinkProgram);
GL_DECLARE_GLOBAL_FUNCTION(glGetProgramiv);
GL_DECLARE_GLOBAL_FUNCTION(glGetShaderInfoLog);
GL_DECLARE_GLOBAL_FUNCTION(glValidateProgram);
GL_DECLARE_GLOBAL_FUNCTION(glGetProgramInfoLog);
GL_DECLARE_GLOBAL_FUNCTION(glGenBuffers);
GL_DECLARE_GLOBAL_FUNCTION(glBindBuffer);
GL_DECLARE_GLOBAL_FUNCTION(glUniformMatrix4fv);
GL_DECLARE_GLOBAL_FUNCTION(glGetUniformLocation);
GL_DECLARE_GLOBAL_FUNCTION(glUseProgram);
GL_DECLARE_GLOBAL_FUNCTION(glUniform1i);


global_var GL gl;
global_var GL_Info gl_info;
global_var s32 g_texture_handle_idx = 1;
global_var u32 g_white_bitmap[64][64];

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



global_var GLint g_gl_texture_internal_format = GL_RGBA8;

inline void
gl_parse_version() {
    gl_info.version = (char *)glGetString(GL_VERSION);

    char *at = gl_info.version;
    for(s32 i = 0; i < 2;) {
        char c = *at++;
        if (c >= '0' && c <= '9') {
            if (i++ == 0) {
                gl_info.major = (c - '0');
            } else {
                gl_info.minor = (c - '0');
            }
        }
    }
}

internal void
gl_get_info() {

    gl_info.vendor                   = (char *)glGetString(GL_VENDOR);
    gl_info.renderer                 = (char *)glGetString(GL_RENDERER);
    gl_info.extensions               = (char *)glGetString(GL_EXTENSIONS);
    gl_parse_version();
    if (gl_info.major >= 2) {
        gl_info.shading_language_version = (char *)glGetString(GL_SHADING_LANGUAGE_VERSION);
    } else {
        gl_info.shading_language_version = "N/A";
    }

#define X(ext) #ext,
    char *ext_str_table[] = {
        GL_EXTENSION_LIST
    };
#undef X
    size_t ext_str_len[GL_EXT_COUNT];
    for (s32 idx = 0;
         idx < ArrayCount(ext_str_table);
         ++idx) {
        ext_str_len[idx] = str_len(ext_str_table[idx]);
    }


    for (char *tk_begin = gl_info.extensions;
         *tk_begin;) {
        if (is_whitespace(*tk_begin)) {
            ++tk_begin;
        } else {
            char *tk_end = tk_begin;
            while (!is_whitespace(*(tk_end + 1))) {
                ++tk_end;
            }

            size_t t_len = (tk_end - tk_begin + 1);

            for (u32 idx = 0;
                 idx < ArrayCount(ext_str_table);
                 ++idx) {
                char *ext_str = ext_str_table[idx];
                size_t p_len = ext_str_len[idx];
                if (t_len == p_len) {
                    b32 match = str_match(tk_begin, ext_str, t_len);
                    if (match) {
                        gl_info.has_ext[idx] = true;
                        break;
                    }

                } else {
                    continue;
                }
            }

            tk_begin = tk_end + 1;
        }
    }

}

internal void
gl_draw_quad(v3 V[4], Bitmap *bitmap, v4 color) {
    if (bitmap) {
        glBegin(GL_TRIANGLES);
        glColor4f(color.r, color.g, color.b, color.a);

        // upper triangle.
        glTexCoord2f(0.0f, 0.0f);
        glVertex3f(V[0].x, V[0].y, V[0].z);

        glTexCoord2f(1.0f, 0.0f);
        glVertex3f(V[1].x, V[1].y, V[1].z);

        glTexCoord2f(1.0f, 1.0f);
        glVertex3f(V[2].x, V[2].y, V[2].z);

        // bottom triangle.
        glTexCoord2f(0.0f, 0.0f);
        glVertex3f(V[0].x, V[0].y, V[0].z);

        glTexCoord2f(1.0f, 1.0f);
        glVertex3f(V[2].x, V[2].y, V[2].z);

        glTexCoord2f(0.0f, 1.0f);
        glVertex3f(V[3].x, V[3].y, V[3].z);

        glEnd();
    } else {
        glBegin(GL_TRIANGLES);
        glColor4f(color.r, color.g, color.b, color.a);

        glVertex3f(V[0].x, V[0].y, V[0].z);
        glVertex3f(V[1].x, V[1].y, V[1].z);
        glVertex3f(V[2].x, V[2].y, V[2].z);

        glVertex3f(V[0].x, V[0].y, V[0].z);
        glVertex3f(V[2].x, V[2].y, V[2].z);
        glVertex3f(V[3].x, V[3].y, V[3].z);

        glEnd();
    }
}

internal void
gl_draw_cube(v3 V[8], v4 color) {
    glBegin(GL_TRIANGLES);
    glColor4f(color.r, color.g, color.b, color.a);

    v3 facet1[4] = {V[0], V[1], V[2], V[3]};
    v3 facet2[4] = {V[4], V[5], V[6], V[7]};
    v3 facet3[4] = {V[4], V[5], V[1], V[0]};
    v3 facet4[4] = {V[5], V[7], V[3], V[1]};
    v3 facet5[4] = {V[7], V[6], V[3], V[2]};
    v3 facet6[4] = {V[6], V[4], V[0], V[2]};

    gl_draw_quad(facet1, 0, color);
    gl_draw_quad(facet2, 0, color);
    gl_draw_quad(facet3, 0, color);
    gl_draw_quad(facet4, 0, color);
    gl_draw_quad(facet5, 0, color);
    gl_draw_quad(facet6, 0, color);

    glEnd();
}

#if 0
inline void
gl_load_projection(Camera *cam) {
    glMatrixMode(GL_PROJECTION);
    m4x4 M = transpose(cam->projection);
    glLoadMatrixf(&M.e[0][0]);
}
#endif

internal GLuint
gl_create_program(const char *header,
                  const char *vsrc,
                  const char *fsrc) {
    GLuint program = 0;

    if (glCreateShader) {
        GLuint vshader = glCreateShader(GL_VERTEX_SHADER);
        const GLchar *vunit[] = {
            header,
            vsrc
        };
        glShaderSource(vshader, ArrayCount(vunit), (const GLchar **)vunit, 0);
        glCompileShader(vshader);

        GLuint fshader = glCreateShader(GL_FRAGMENT_SHADER);
        const GLchar *funit[] = {
            header,
            fsrc
        };
        glShaderSource(fshader, ArrayCount(funit), (const GLchar **)funit, 0);
        glCompileShader(fshader);

        program = glCreateProgram();
        glAttachShader(program, vshader);
        glAttachShader(program, fshader);
        glLinkProgram(program);

        glValidateProgram(program);
        GLint linked = false;
        glGetProgramiv(program, GL_LINK_STATUS, &linked);
        if (!linked) {
            GLsizei stub;

            GLchar vlog[1024];
            glGetShaderInfoLog(vshader, sizeof(vlog), &stub, vlog);

            GLchar flog[1024];
            glGetShaderInfoLog(fshader, sizeof(flog), &stub, flog);

            GLchar plog[1024];
            glGetProgramInfoLog(program, sizeof(plog), &stub, plog);

            Assert(!"compile/link error.");
        }
    } else {
        // TODO: handling.
    }
    
    return program;
}

internal void
gl_init() {
    const char *header = R"FOO(
            #version 130
            )FOO";

    const char *vshader = R"FOO(
            uniform mat4x4          transform;
            in vec2                 in_uv;
            in vec4                 in_color;
            smooth out vec2         frag_uv;
            smooth out vec4         frag_color;

            void main() {
                vec3 light_pos = vec3(0.0f, 0.0f, 1.0f);
                float light_strength = 1.0f;
                float d = distance(light_pos, gl_Vertex.xyz);
                float light_result = light_strength / pow(d, 2);

                vec4 input_vertex = gl_Vertex;
                input_vertex.w = 1.0f;
                gl_Position = transform * input_vertex;

                frag_uv = gl_MultiTexCoord0.xy;
                frag_color = gl_Color;
                frag_color.xyz *= light_result;
            }
            )FOO";

    const char *fshader = R"FOO(
            uniform sampler2D       texture_sampler;
            out vec4                result_color;
            smooth in vec2          frag_uv;
            smooth in vec4          frag_color;
            void main() {
                vec4 texture_sample = texture(texture_sampler, frag_uv);
                result_color = frag_color * texture_sample;
            }
            )FOO";

    gl.program              = gl_create_program(header, vshader, fshader);
    gl.transform_id         = glGetUniformLocation(gl.program, "transform");
    gl.texture_sampler_id   = glGetUniformLocation(gl.program, "texture_sampler");

    gl.white_bitmap.width   = 64;
    gl.white_bitmap.height  = 64;
    gl.white_bitmap.pitch   = -64;
    gl.white_bitmap.handle  = 0;
    gl.white_bitmap.size    = 4 * 64 * 64;
    gl.white_bitmap.memory  = g_white_bitmap;

    u32 *at = &g_white_bitmap[0][0];
    for (u32 i = 0; i < 64 * 64; ++i) {
        *at++ = 0xffffffff;
    }
}

internal void
gl_bind_texture(Bitmap *bitmap) {
    if (bitmap) {
        if (bitmap->handle) {
            glBindTexture(GL_TEXTURE_2D, bitmap->handle);
        } else {
            bitmap->handle = g_texture_handle_idx++;
            glBindTexture(GL_TEXTURE_2D, bitmap->handle);
            glTexImage2D(GL_TEXTURE_2D, 0, g_gl_texture_internal_format, bitmap->width, bitmap->height,
                         0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, (u8 *)bitmap->memory + bitmap->pitch * (bitmap->height - 1));

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
        }
    }
}

// TODO: remove sort entries.
internal void
gl_render_batch(HDC hdc, Render_Batch *batch, u32 win_w, u32 win_h) {
    glViewport(0, 0, win_w, win_h);

    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_SCISSOR_TEST);

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glClearDepth(1.0f);
    glClear(GL_DEPTH_BUFFER_BIT);
    glDepthFunc(GL_LEQUAL);

    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GREATER, 0.0f);





    for (Render_Group *group = (Render_Group *)batch->base;
         (u8 *)group < (u8 *)batch->base + batch->used;
         ++group) {

        for (Sort_Entry *entry = (Sort_Entry *)group->sort_entry_begin;
             (u8 *)entry < group->base + group->capacity;
             ++entry) {

            Render_Entity_Header *entity =(Render_Entity_Header *)entry->render_entity;

            switch (entity->type) {
                case eRender_Bitmap: {
                    Render_Bitmap *piece = (Render_Bitmap *)entity;

                    gl_bind_texture(piece->bitmap);

                    glUseProgram(gl.program);
                    m4x4 projection = group->camera.projection;
                    glUniformMatrix4fv(gl.transform_id, 1, GL_TRUE, &projection.e[0][0]);
                    glUniform1i(gl.texture_sampler_id, 0);

                    gl_draw_quad(piece->V, piece->bitmap, piece->color);

                    glUseProgram(0);
                } break;

                case eRender_Text: {
                    Render_Text *piece = (Render_Text *)entity;
                } break;

                case eRender_Cube: {
                    Render_Cube *piece = (Render_Cube *)entity;

                    gl_bind_texture(&gl.white_bitmap);

                    glUseProgram(gl.program);
                    m4x4 projection = group->camera.projection;
                    glUniformMatrix4fv(gl.transform_id, 1, GL_TRUE, &projection.e[0][0]);
                    glUniform1i(gl.texture_sampler_id, 0);

                    v3  B = piece->base;
                    r32 H = piece->height;
                    r32 R = piece->radius;
#if 1
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
#endif
                    gl_draw_cube(vertices, piece->color);
                    glUseProgram(0);
                } break;

                INVALID_DEFAULT_CASE
            }

        }
    }
            
    SwapBuffers(hdc);
    batch->used = 0;
}
