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


typedef BOOL        WGL_Swap_Interval(int interval);
typedef GLuint      GL_Create_Shader(GLenum shaderType);
typedef void        GL_Shader_Source(GLuint shader, GLsizei count, const GLchar **string, const GLint *length);
typedef void        GL_Compile_Shader(GLuint shader);
typedef GLuint      GL_Create_Program(void);
typedef void        GL_Attach_Shader(GLuint program, GLuint shader);
typedef void        GL_Link_Program(GLuint program);
typedef void        GL_Get_Programiv(GLuint program, GLenum pname, GLint *params);
typedef void        GL_Get_Shader_Info_Log(GLuint shader, GLsizei maxLength, GLsizei *length, GLchar *infoLog);
typedef void        GL_Validate_Program(GLuint program);
typedef void        GL_Get_Program_Info_Log(GLuint program, GLsizei maxLength, GLsizei *length, GLchar *infoLog);
typedef void        GL_Gen_Buffers(GLsizei n, GLuint *buffers);
typedef void        GL_Bind_Buffer(GLenum target, GLuint buffer);
typedef void        GL_Uniform_Matrix4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
typedef GLint       GL_Get_Uniform_Location(GLuint program, const GLchar *name);
typedef void        GL_Use_Program(GLuint program);
typedef void        GL_Uniform1i (GLint location, GLint v0);

global_var WGL_Swap_Interval        *wglSwapIntervalEXT;
global_var GL_Create_Shader         *glCreateShader;
global_var GL_Shader_Source         *glShaderSource;
global_var GL_Compile_Shader        *glCompileShader;
global_var GL_Create_Program        *glCreateProgram;
global_var GL_Attach_Shader         *glAttachShader;
global_var GL_Link_Program          *glLinkProgram;
global_var GL_Get_Programiv         *glGetProgramiv;
global_var GL_Get_Shader_Info_Log   *glGetShaderInfoLog;
global_var GL_Validate_Program      *glValidateProgram;
global_var GL_Get_Program_Info_Log  *glGetProgramInfoLog;
global_var GL_Gen_Buffers           *glGenBuffers;
global_var GL_Bind_Buffer           *glBindBuffer;
global_var GL_Uniform_Matrix4fv     *glUniformMatrix4fv;
global_var GL_Get_Uniform_Location  *glGetUniformLocation;
global_var GL_Use_Program           *glUseProgram;
global_var GL_Uniform1i             *glUniform1i;


global_var GL gl;


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
         ++idx) {
        ext_str_len[idx] = str_len(ext_str_table[idx]);
    }


    for (char *tk_begin = result.extensions;
         *tk_begin;
        ) {
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
                        result.has_ext[idx] = true;
                        break;
                    }

                } else {
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
    glVertex3f(V[0].x, V[0].y, V[0].z);

    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(V[1].x, V[1].y, V[1].z);

    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(V[3].x, V[3].y, V[3].z);

    // bottom triangle.
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(V[0].x, V[0].y, V[0].z);

    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(V[2].x, V[2].y, V[2].z);

    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(V[3].x, V[3].y, V[3].z);

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

#if 1
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
    //
    // Shader
    //
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
                vec4 input_vertex = gl_Vertex;
                input_vertex.w = 1.0f;
                gl_Position = transform * input_vertex;

                frag_uv = gl_MultiTexCoord0.xy;
                frag_color = gl_Color;
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
    gl.texture_sampler_id    = glGetUniformLocation(gl.program, "texture_sampler");
}

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

    glEnable(GL_TEXTURE_2D);




    for (Render_Group *group = (Render_Group *)batch->base;
         (u8 *)group < (u8 *)batch->base + batch->used;
         ++group) {

        gl_load_projection(&group->camera);

        for (Sort_Entry *entry = (Sort_Entry *)group->sort_entry_begin;
             (u8 *)entry < group->base + group->capacity;
             ++entry) {

            Render_Entity_Header *entity =(Render_Entity_Header *)entry->render_entity;

            switch (entity->type) {
                case eRenderEntityBitmap: {
                    RenderEntityBitmap *piece = (RenderEntityBitmap *)entity;
                    local_persist s32 handle_idx = 1;
                    Bitmap *bitmap = piece->bitmap;
                    if (bitmap->handle) {
                        glBindTexture(GL_TEXTURE_2D, bitmap->handle);
                    } else {
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

                    glUseProgram(gl.program);
                    m4x4 projection = group->camera.projection;
                    glUniformMatrix4fv(gl.transform_id, 1, GL_TRUE, &projection.e[0][0]);
                    glUniform1i(gl.texture_sampler_id, 0);

                    v3 vertices[] = {
                        piece->origin,
                        piece->origin + piece->axis_x,
                        piece->origin + piece->axis_y,
                        piece->origin + piece->axis_x + piece->axis_y
                    };

                    gl_draw_bitmap(vertices, bitmap, piece->color);

                    glUseProgram(0);
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
