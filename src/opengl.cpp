/* ―――――――――――――――――――――――――――――――――――◆――――――――――――――――――――――――――――――――――――
   $File: $
   $Date: $
   $Revision: $
   $Creator: Sung Woo Lee $
   $Notice: (C) Copyright 2024 by Sung Woo Lee. All Rights Reserved. $
   ―――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――― */


#include "render_group.h"
#include "opengl.h"

typedef char    GLchar;
typedef size_t  GLsizeiptr;
typedef void (APIENTRY  *GLDEBUGPROCARB)(GLenum source,GLenum type,GLuint id,GLenum severity,GLsizei length,const GLchar *message,const void *userParam);

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
#define GL_STREAM_DRAW                      0x88E0
#define GL_STREAM_READ                      0x88E1
#define GL_STREAM_COPY                      0x88E2
#define GL_STATIC_DRAW                      0x88E4
#define GL_STATIC_READ                      0x88E5
#define GL_STATIC_COPY                      0x88E6
#define GL_DYNAMIC_DRAW                     0x88E8
#define GL_DYNAMIC_READ                     0x88E9
#define GL_DYNAMIC_COPY                     0x88EA
#define GL_DEBUG_OUTPUT_SYNCHRONOUS         0x8242


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
typedef void        Type_glBufferData(GLenum target, GLsizeiptr size, const void *data, GLenum usage);
typedef void        Type_glVertexAttribPointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void *pointer);
typedef GLint       Type_glGetAttribLocation (GLuint program, const GLchar *name);
typedef void        Type_glEnableVertexAttribArray (GLuint index);
typedef void        Type_glGenVertexArrays (GLsizei n, GLuint *arrays);
typedef void        Type_glBindVertexArray (GLuint array);
typedef void        Type_glBindAttribLocation (GLuint program, GLuint index, const GLchar *name);
typedef void        Type_glDebugMessageCallbackARB (GLDEBUGPROCARB callback, const void *userParam);
typedef void        Type_glDisableVertexAttribArray (GLuint index);

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
GL_DECLARE_GLOBAL_FUNCTION(glBufferData);
GL_DECLARE_GLOBAL_FUNCTION(glVertexAttribPointer);
GL_DECLARE_GLOBAL_FUNCTION(glGetAttribLocation);
GL_DECLARE_GLOBAL_FUNCTION(glEnableVertexAttribArray);
GL_DECLARE_GLOBAL_FUNCTION(glGenVertexArrays);
GL_DECLARE_GLOBAL_FUNCTION(glBindVertexArray);
GL_DECLARE_GLOBAL_FUNCTION(glBindAttribLocation);
GL_DECLARE_GLOBAL_FUNCTION(glDebugMessageCallbackARB);
GL_DECLARE_GLOBAL_FUNCTION(glDisableVertexAttribArray);


global_var GL gl;
global_var GL_Info gl_info;
global_var s32 g_texture_handle_idx = 1;

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
gl_debug_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam) {
    char *error = (char *)message;
    Assert(0);
}

internal void
gl_get_info() {
    gl_info.vendor                   = (char *)glGetString(GL_VENDOR);
    gl_info.renderer                 = (char *)glGetString(GL_RENDERER);
    gl_info.extensions               = (char *)glGetString(GL_EXTENSIONS);
    gl_parse_version();
    if (gl_info.modern) {
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


    if (!gl_info.modern) {
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
gl_alloc_texture(Bitmap *bitmap)
{
    glGenTextures(1, &bitmap->handle);
    glBindTexture(GL_TEXTURE_2D, bitmap->handle);
    glTexImage2D(GL_TEXTURE_2D, 0, g_gl_texture_internal_format, bitmap->width, bitmap->height,
                 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, (u8 *)bitmap->memory + bitmap->pitch * (bitmap->height - 1));

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

internal void
gl_bind_texture(Bitmap *bitmap)
{
    if (!bitmap) {
        bitmap = &gl.white_bitmap;
    } else {

    }

    if (bitmap->handle) {
        glBindTexture(GL_TEXTURE_2D, bitmap->handle);
    } else {
        gl_alloc_texture(bitmap);
        glBindTexture(GL_TEXTURE_2D, bitmap->handle);
    }
}

// @batch
// TODO: remove sort entries.
internal void
gl_render_batch(Render_Batch *batch, u32 win_w, u32 win_h)
{
    glViewport(0, 0, win_w, win_h);

    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_SCISSOR_TEST);

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.01f, 0.01f, 0.01f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glClearDepth(1.0f);
    glClear(GL_DEPTH_BUFFER_BIT);
    glDepthFunc(GL_LEQUAL);

    for (Render_Group *group = (Render_Group *)batch->base;
         (u8 *)group < (u8 *)batch->base + batch->used;
         ++group) {

#if 1
        glUseProgram(gl.program);
        glUniformMatrix4fv(gl.transform_id, 1, GL_TRUE, &group->camera.projection.e[0][0]);

        glBufferData(GL_ARRAY_BUFFER,
                     group->vertex_count * sizeof(Textured_Vertex),
                     group->vertices,
                     GL_STREAM_DRAW);

        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);
        glEnableVertexAttribArray(3);

        glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(Textured_Vertex), (GLvoid *)(offset_of(Textured_Vertex, p)));
        glVertexAttribPointer(1, 2, GL_FLOAT, false, sizeof(Textured_Vertex), (GLvoid *)(offset_of(Textured_Vertex, uv)));
        glVertexAttribPointer(2, 4, GL_FLOAT, true,  sizeof(Textured_Vertex), (GLvoid *)(offset_of(Textured_Vertex, color)));
        glVertexAttribPointer(3, 3, GL_FLOAT, false, sizeof(Textured_Vertex), (GLvoid *)(offset_of(Textured_Vertex, normal)));

        u32 vidx = 0;
        for (u8 *at = group->base;
             at < group->base + group->used;)
        {
            Render_Entity_Header *entity =(Render_Entity_Header *)at;

            switch (entity->type) {
                case eRender_Quad: {
                    Render_Quad *piece = (Render_Quad *)entity;
                    at += sizeof(Render_Quad);


                    gl_bind_texture(piece->bitmap);
                    glDrawArrays(GL_TRIANGLE_STRIP, vidx, 4);
                    vidx += 4;
                } break;

                case eRender_Text: {
                    Render_Text *piece = (Render_Text *)entity;
                    at += sizeof(Render_Text);
                } break;

                INVALID_DEFAULT_CASE
            }

        }


        glUseProgram(0);

        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(2);
        glDisableVertexAttribArray(3);

#endif
    }
            
    batch->used = 0;
}

internal void
gl_init()
{
#if 0
    if (glDebugMessageCallbackARB) {
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallbackARB(gl_debug_callback, 0);
    } else {
        Assert("!no extension");
    }
#endif

    //
    // @shader
    //
    const char *header = R"FOO(
            #version 330 core
            )FOO";

    const char *vshader = R"FOO(
            uniform mat4x4 transform;

            layout (location = 0) in vec3 vP;
            layout (location = 1) in vec2 vUV;
            layout (location = 2) in vec4 vC;
            layout (location = 3) in vec3 vN;

            smooth out vec3 fP;
            smooth out vec2 fUV;
            smooth out vec4 fC;

            void main()
            {
                fP = vP;
                fC = vC;
                fUV = vUV;


                gl_Position = transform * vec4(vP, 1.0f);
            }
            )FOO";

    const char *fshader = R"FOO(
            uniform sampler2D texture_sampler;

            smooth in vec3 fP;
            smooth in vec2 fUV;
            smooth in vec4 fC;

            out vec4 C;

            void main()
            {
                C = texture(texture_sampler, fUV) * fC;

                vec3 lightP = vec3(0.0f, 0.0f, 0.4f);
                float d = distance(lightP, fP);
                C.xyz *= (1.0f / (d * d));

                if (C.a == 0.0f) {
                    discard;
                }
            }
            )FOO";

    gl.program              = gl_create_program(header, vshader, fshader);
    gl.transform_id         = glGetUniformLocation(gl.program, "transform");
    gl.texture_sampler_id   = glGetUniformLocation(gl.program, "texture_sampler");

    gl.white_bitmap.width   = 4;
    gl.white_bitmap.height  = 4;
    gl.white_bitmap.pitch   = -16;
    gl.white_bitmap.handle  = 0;
    gl.white_bitmap.size    = 64;
    gl.white_bitmap.memory  = &gl.white[3][0];
    for (u32 *at = (u32 *)gl.white;
         at <= &gl.white[3][3];
         ++at) {
        *at = 0xffffffff;
    }
    gl_alloc_texture(&gl.white_bitmap);

    // dummy.
    glGenVertexArrays(1, &gl.vao);
    glBindVertexArray(gl.vao);

    glGenBuffers(1, &gl.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, gl.vbo);
}
