/* ―――――――――――――――――――――――――――――――――――◆――――――――――――――――――――――――――――――――――――
   $File: $
   $Date: $
   $Revision: $
   $Creator: Sung Woo Lee $
   $Notice: (C) Copyright 2024 by Sung Woo Lee. All Rights Reserved. $
   ―――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――― */

/*
    STUDY: vertex attrib name stored in vao? multiple programs, no overlapping
            vertex attrib...
 */

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
#define GL_ELEMENT_ARRAY_BUFFER             0x8893
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
#define GL_DEBUG_OUTPUT                     0x92E0
#define GL_DEBUG_OUTPUT_SYNCHRONOUS         0x8242
#define GL_DEBUG_SEVERITY_HIGH              0x9146
#define GL_DEBUG_SEVERITY_MEDIUM            0x9147
#define GL_DEBUG_SEVERITY_LOW               0x9148
#define GL_CULL_FACE                        0x0B44
#define GL_NONE                             0
#define GL_FRONT_LEFT                       0x0400
#define GL_FRONT_RIGHT                      0x0401
#define GL_BACK_LEFT                        0x0402
#define GL_BACK_RIGHT                       0x0403
#define GL_FRONT                            0x0404
#define GL_BACK                             0x0405
#define GL_LEFT                             0x0406
#define GL_RIGHT                            0x0407
#define GL_FRONT_AND_BACK                   0x0408
#define GL_CW                               0x0900
#define GL_CCW                              0x0901
#define GL_BYTE                             0x1400
#define GL_UNSIGNED_BYTE                    0x1401
#define GL_SHORT                            0x1402
#define GL_UNSIGNED_SHORT                   0x1403
#define GL_INT                              0x1404
#define GL_UNSIGNED_INT                     0x1405
#define GL_FLOAT                            0x1406


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
typedef void        Type_glUniform3fv (GLint location, GLsizei count, const GLfloat *value);
typedef void        Type_glVertexAttribIPointer (GLuint index, GLint size, GLenum type, GLsizei stride, const void *pointer);
typedef void        Type_glUniform4fv (GLint location, GLsizei count, const GLfloat *value);
typedef void        Type_glVertexAttribDivisor (GLuint index, GLuint divisor);
typedef void        Type_glDrawElementsInstanced (GLenum mode, GLsizei count, GLenum type, const void *indices, GLsizei instancecount);
typedef void        Type_glUniform1f (GLint location, GLfloat v0);
typedef void        Type_glUniform1fv (GLint location, GLsizei count, const GLfloat *value);

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
GL_DECLARE_GLOBAL_FUNCTION(glUniform3fv);
GL_DECLARE_GLOBAL_FUNCTION(glVertexAttribIPointer);
GL_DECLARE_GLOBAL_FUNCTION(glUniform4fv);
GL_DECLARE_GLOBAL_FUNCTION(glVertexAttribDivisor);
GL_DECLARE_GLOBAL_FUNCTION(glDrawElementsInstanced);
GL_DECLARE_GLOBAL_FUNCTION(glUniform1f);
GL_DECLARE_GLOBAL_FUNCTION(glUniform1fv);


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
gl_parse_version()
{
    gl_info.version = (char *)glGetString(GL_VERSION);

    char *at = gl_info.version;
    for (s32 i = 0; i < 2;) 
    {
        char c = *at++;
        if (c >= '0' && c <= '9') 
        {
            if (i++ == 0) {
                gl_info.major = (c - '0');
            } 
            else {
                gl_info.minor = (c - '0');
            }
        }
    }
}

internal void
gl_debug_callback(GLenum source, GLenum type, GLuint id,
                  GLenum severity, GLsizei length, const GLchar *message,
                  const void *userParam)
{
    char *error = (char *)message;
    switch (severity) 
    {
        case GL_DEBUG_SEVERITY_LOW: 
        {

        } break;
        case GL_DEBUG_SEVERITY_MEDIUM: 
        {
            Assert(0);
        } break;
        case GL_DEBUG_SEVERITY_HIGH: 
        {
            Assert(0);
        } break;
    }
}

internal void
gl_get_info()
{
    gl_info.vendor                   = (char *)glGetString(GL_VENDOR);
    gl_info.renderer                 = (char *)glGetString(GL_RENDERER);
    gl_info.extensions               = (char *)glGetString(GL_EXTENSIONS);
    gl_parse_version();
    if (gl_info.modern) 
    {
        gl_info.shading_language_version = (char *)glGetString(GL_SHADING_LANGUAGE_VERSION);
    } 
    else 
    {
        gl_info.shading_language_version = "N/A";
    }

#define X(ext) #ext,
    char *ext_str_table[] = {
        GL_EXTENSION_LIST
    };
#undef X
    size_t ext_str_len[GL_EXT_COUNT];
    for (s32 idx = 0;
         idx < array_count(ext_str_table);
         ++idx) 
    {
        ext_str_len[idx] = str_len(ext_str_table[idx]);
    }


    if (!gl_info.modern) 
    {
        for (char *tk_begin = gl_info.extensions;
             *tk_begin;) 
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
                     idx < array_count(ext_str_table);
                     ++idx) 
                {
                    char *ext_str = ext_str_table[idx];
                    size_t p_len = ext_str_len[idx];
                    if (t_len == p_len) 
                    {
                        b32 match = str_match(tk_begin, ext_str, t_len);
                        if (match) 
                        {
                            gl_info.has_ext[idx] = true;
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
    }

}


internal GLuint
gl_create_program(const char *header,
                  const char *vsrc,
                  const char *fsrc)
{
    GLuint program = 0;

    if (glCreateShader) 
    {
        GLuint vshader = glCreateShader(GL_VERTEX_SHADER);
        const GLchar *vunit[] = {
            header,
            vsrc
        };
        glShaderSource(vshader, array_count(vunit), (const GLchar **)vunit, 0);
        glCompileShader(vshader);

        GLuint fshader = glCreateShader(GL_FRAGMENT_SHADER);
        const GLchar *funit[] = {
            header,
            fsrc
        };
        glShaderSource(fshader, array_count(funit), (const GLchar **)funit, 0);
        glCompileShader(fshader);

        program = glCreateProgram();
        glAttachShader(program, vshader);
        glAttachShader(program, fshader);
        glLinkProgram(program);

        glValidateProgram(program);
        GLint linked = false;
        glGetProgramiv(program, GL_LINK_STATUS, &linked);
        if (!linked) 
        {
            GLsizei stub;

            GLchar vlog[1024];
            glGetShaderInfoLog(vshader, sizeof(vlog), &stub, vlog);

            GLchar flog[1024];
            glGetShaderInfoLog(fshader, sizeof(flog), &stub, flog);

            GLchar plog[1024];
            glGetProgramInfoLog(program, sizeof(plog), &stub, plog);

            Assert(!"compile/link error.");
        }
    } 
    else 
    {
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
                 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, bitmap->memory);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

internal void
gl_bind_texture(Bitmap *bitmap)
{
    if (!bitmap) 
    {
        bitmap = &gl.white_bitmap;
    } 
    else 
    {

    }

    if (bitmap->handle) 
    {
        glBindTexture(GL_TEXTURE_2D, bitmap->handle);
    } 
    else 
    {
        gl_alloc_texture(bitmap);
        glBindTexture(GL_TEXTURE_2D, bitmap->handle);
    }
}


// @batch
internal void
gl_render_batch(Render_Batch *batch, u32 win_w, u32 win_h)
{
    glViewport(0, 0, win_w, win_h);

    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_SCISSOR_TEST);

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.10f, 0.10f, 0.10f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glClearDepth(1.0f);
    glClear(GL_DEPTH_BUFFER_BIT);
    glDepthFunc(GL_LEQUAL);

    for (Render_Group *group = (Render_Group *)batch->base;
         (u8 *)group < (u8 *)batch->base + batch->used;
         ++group)
    {

        switch (group->type)
        {
            case eRender_Group_Skeletal_Mesh:
            {
                glBindBuffer(GL_ARRAY_BUFFER, gl.vbo);

                glEnable(GL_CULL_FACE);
                glCullFace(GL_BACK);
                glFrontFace(GL_CCW);

                Skeletal_Mesh_Program *program = &gl.skeletal_mesh_program;
                s32 pid = program->id;
                glUseProgram(pid);

                glUniformMatrix4fv(program->mvp, 1, GL_TRUE, &group->camera->projection.e[0][0]);
                glUniform3fv(program->cam_pos, 1, (GLfloat *)&group->camera->world_translation);

                glEnableVertexAttribArray(0);
                glEnableVertexAttribArray(1);
                glEnableVertexAttribArray(2);
                glEnableVertexAttribArray(3);
                glEnableVertexAttribArray(4);
                glEnableVertexAttribArray(5);

                glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(Asset_Vertex), (GLvoid *)(offset_of(Asset_Vertex, pos)));
                glVertexAttribPointer(1, 3, GL_FLOAT, false, sizeof(Asset_Vertex), (GLvoid *)(offset_of(Asset_Vertex, normal)));
                glVertexAttribPointer(2, 2, GL_FLOAT, false, sizeof(Asset_Vertex), (GLvoid *)(offset_of(Asset_Vertex, uv)));
                glVertexAttribPointer(3, 4, GL_FLOAT, true,  sizeof(Asset_Vertex), (GLvoid *)(offset_of(Asset_Vertex, color)));

                glVertexAttribIPointer(4, MAX_BONE_PER_VERTEX, GL_INT, sizeof(Asset_Vertex), (GLvoid *)(offset_of(Asset_Vertex, bone_ids)));
                glVertexAttribPointer(5, MAX_BONE_PER_VERTEX, GL_FLOAT, false, sizeof(Asset_Vertex), (GLvoid *)(offset_of(Asset_Vertex, bone_weights)));

                for (u8 *at = group->base;
                     at < group->base + group->used;)
                {
                    Render_Entity_Header *entity =(Render_Entity_Header *)at;
                    Render_Skeletal_Mesh *piece = (Render_Skeletal_Mesh *)entity;
                    at += sizeof(Render_Skeletal_Mesh);

                    Asset_Mesh *mesh    = piece->mesh;
                    Asset_Material *mat = piece->material;

                    glBufferData(GL_ARRAY_BUFFER,
                                 mesh->vertex_count * sizeof(Asset_Vertex),
                                 mesh->vertices,
                                 GL_DYNAMIC_DRAW);

                    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                                 mesh->index_count * sizeof(u32),
                                 mesh->indices,
                                 GL_DYNAMIC_DRAW);

                    glUniformMatrix4fv(program->world_transform, 1, true, &piece->world_transform.e[0][0]);
                    glUniformMatrix4fv(program->bone_transforms, MAX_BONE_PER_MESH, true, (GLfloat *)piece->animation_transforms);

                    glUniform3fv(program->color_ambient, 1, (GLfloat *)&mat->color_ambient);
                    glUniform3fv(program->color_diffuse, 1, (GLfloat *)&mat->color_diffuse);
                    glUniform3fv(program->color_specular, 1, (GLfloat *)&mat->color_specular);

                    glDrawElements(GL_TRIANGLES, mesh->index_count, GL_UNSIGNED_INT, (void *)0);

                }

                glDisableVertexAttribArray(0);
                glDisableVertexAttribArray(1);
                glDisableVertexAttribArray(2);
                glDisableVertexAttribArray(3);
                glDisableVertexAttribArray(4);
                glDisableVertexAttribArray(5);
            } break;

            case eRender_Group_Static_Mesh:
            {
                glBindBuffer(GL_ARRAY_BUFFER, gl.vbo);

                glEnable(GL_CULL_FACE);
                glCullFace(GL_BACK);
                glFrontFace(GL_CCW);

                Static_Mesh_Program *program = &gl.static_mesh_program;
                s32 pid = program->id;
                glUseProgram(pid);

                glUniformMatrix4fv(program->mvp, 1, GL_TRUE, &group->camera->projection.e[0][0]);
                glUniform3fv(program->cam_pos, 1, (GLfloat *)&group->camera->world_translation);

                glEnableVertexAttribArray(0);
                glEnableVertexAttribArray(1);
                glEnableVertexAttribArray(2);
                glEnableVertexAttribArray(3);

                glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(Asset_Vertex), (GLvoid *)(offset_of(Asset_Vertex, pos)));
                glVertexAttribPointer(1, 3, GL_FLOAT, false, sizeof(Asset_Vertex), (GLvoid *)(offset_of(Asset_Vertex, normal)));
                glVertexAttribPointer(2, 2, GL_FLOAT, false, sizeof(Asset_Vertex), (GLvoid *)(offset_of(Asset_Vertex, uv)));
                glVertexAttribPointer(3, 4, GL_FLOAT, true,  sizeof(Asset_Vertex), (GLvoid *)(offset_of(Asset_Vertex, color)));

                for (u8 *at = group->base;
                     at < group->base + group->used;)
                {
                    Render_Entity_Header *entity =(Render_Entity_Header *)at;
                    Render_Static_Mesh *piece = (Render_Static_Mesh *)entity;
                    at += sizeof(Render_Static_Mesh);

                    Asset_Mesh *mesh    = piece->mesh;
                    Asset_Material *mat = piece->material;

                    glBufferData(GL_ARRAY_BUFFER,
                                 mesh->vertex_count * sizeof(Asset_Vertex),
                                 mesh->vertices,
                                 GL_DYNAMIC_DRAW);

                    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                                 mesh->index_count * sizeof(u32),
                                 mesh->indices,
                                 GL_DYNAMIC_DRAW);

                    glUniformMatrix4fv(program->world_transform, 1, true, &piece->world_transform.e[0][0]);

                    glUniform3fv(program->color_ambient, 1, (GLfloat *)&mat->color_ambient);
                    glUniform3fv(program->color_diffuse, 1, (GLfloat *)&mat->color_diffuse);
                    glUniform3fv(program->color_specular, 1, (GLfloat *)&mat->color_specular);

                    glDrawElements(GL_TRIANGLES, mesh->index_count, GL_UNSIGNED_INT, (void *)0);

                }

                glDisableVertexAttribArray(0);
                glDisableVertexAttribArray(1);
                glDisableVertexAttribArray(2);
                glDisableVertexAttribArray(3);
            } break;

            case eRender_Group_Grass:
            {
#if 1
                glBindBuffer(GL_ARRAY_BUFFER, gl.vbo);

                glDisable(GL_CULL_FACE);

                Grass_Program *program = &gl.grass_program;
                s32 pid = program->id;
                glUseProgram(pid);

                glUniformMatrix4fv(program->mvp, 1, GL_TRUE, &group->camera->projection.e[0][0]);

                glEnableVertexAttribArray(0);
                glEnableVertexAttribArray(1);
                glEnableVertexAttribArray(2);
                glEnableVertexAttribArray(3);
                glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(Asset_Vertex), (GLvoid *)(offset_of(Asset_Vertex, pos)));
                glVertexAttribPointer(1, 3, GL_FLOAT, false, sizeof(Asset_Vertex), (GLvoid *)(offset_of(Asset_Vertex, normal)));
                glVertexAttribPointer(2, 2, GL_FLOAT, false, sizeof(Asset_Vertex), (GLvoid *)(offset_of(Asset_Vertex, uv)));
                glVertexAttribPointer(3, 4, GL_FLOAT, true,  sizeof(Asset_Vertex), (GLvoid *)(offset_of(Asset_Vertex, color)));

                Render_Grass *piece = (Render_Grass *)group->base;
                glUniform1f(program->time, piece->time);
                glUniform1f(program->grass_max_vertex_y, piece->grass_max_vertex_y);
                Asset_Mesh *mesh    = piece->mesh;
                glBufferData(GL_ARRAY_BUFFER,
                             mesh->vertex_count * sizeof(Asset_Vertex),
                             mesh->vertices,
                             GL_DYNAMIC_DRAW);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                             mesh->index_count * sizeof(u32),
                             mesh->indices,
                             GL_DYNAMIC_DRAW);

                glBindBuffer(GL_ARRAY_BUFFER, gl.grass_vbo);
                glEnableVertexAttribArray(6);
                glVertexAttribPointer(6, 3, GL_FLOAT, false, sizeof(v3), (GLvoid *)0);
                glVertexAttribDivisor(6, 1);
                glBufferData(GL_ARRAY_BUFFER, sizeof(v3) * piece->count, piece->translations, GL_DYNAMIC_DRAW);
                glBindBuffer(GL_ARRAY_BUFFER, gl.vbo);

                Bitmap *turbulence_map = piece->turbulence_map;
                if (!turbulence_map->handle)
                {
                    glGenTextures(1, &turbulence_map->handle);
                    glBindTexture(GL_TEXTURE_2D, turbulence_map->handle);
                    glTexImage2D(GL_TEXTURE_2D, 0, g_gl_texture_internal_format, turbulence_map->width, turbulence_map->height,
                                 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, (u8 *)turbulence_map->memory);

                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                }
                glBindTexture(GL_TEXTURE_2D, turbulence_map->handle);
                glDrawElementsInstanced(GL_TRIANGLES, mesh->index_count, GL_UNSIGNED_INT, (void *)0, piece->count);

                glDisableVertexAttribArray(0);
                glDisableVertexAttribArray(1);
                glDisableVertexAttribArray(2);
                glDisableVertexAttribArray(3);
                glDisableVertexAttribArray(6);

#endif
            } break;

            case eRender_Group_Text:
            {
                glUseProgram(gl.skeletal_mesh_program.id);
            } break;

            INVALID_DEFAULT_CASE
        }

        gl_bind_texture(0);
        glUseProgram(0);

    }
            
    batch->used = 0;
}


internal void
gl_init()
{
#if __DEBUG
    if (glDebugMessageCallbackARB) 
    {
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallbackARB(gl_debug_callback, 0);
    } 
    else 
    {
        Assert("!glDebugMessageCallbackARB not found.");
    }
#endif

    //
    // @shader
    //
    const char *header = 
#include "shader/shader.header"
    ;

    const char *skeletal_mesh_vshader = 
#include "shader/skeletal_mesh.vert"
    ;

    const char *static_mesh_vshader = 
#include "shader/static_mesh.vert"
    ;

    const char *grass_vshader = 
#include "shader/grass.vert"
    ;

    const char *mesh_fshader = 
#include "shader/mesh.frag"
    ;

    const char *grass_fshader = 
#include "shader/grass.frag"
    ;


    gl.skeletal_mesh_program.id = gl_create_program(header,
                                                    skeletal_mesh_vshader,
                                                    mesh_fshader);
    gl.skeletal_mesh_program.world_transform  = glGetUniformLocation(gl.skeletal_mesh_program.id, "world_transform");
    gl.skeletal_mesh_program.mvp              = glGetUniformLocation(gl.skeletal_mesh_program.id, "mvp");
    gl.skeletal_mesh_program.texture_sampler  = glGetUniformLocation(gl.skeletal_mesh_program.id, "texture_sampler");
    gl.skeletal_mesh_program.cam_pos          = glGetUniformLocation(gl.skeletal_mesh_program.id, "cam_pos");
    gl.skeletal_mesh_program.bone_transforms  = glGetUniformLocation(gl.skeletal_mesh_program.id, "bone_transforms");
    gl.skeletal_mesh_program.color_ambient    = glGetUniformLocation(gl.skeletal_mesh_program.id, "color_ambient");
    gl.skeletal_mesh_program.color_diffuse    = glGetUniformLocation(gl.skeletal_mesh_program.id, "color_diffuse");
    gl.skeletal_mesh_program.color_specular   = glGetUniformLocation(gl.skeletal_mesh_program.id, "color_specular");

    gl.static_mesh_program.id = gl_create_program(header,
                                                  static_mesh_vshader,
                                                  mesh_fshader);
    gl.static_mesh_program.world_transform  = glGetUniformLocation(gl.static_mesh_program.id, "world_transform");
    gl.static_mesh_program.mvp              = glGetUniformLocation(gl.static_mesh_program.id, "mvp");
    gl.static_mesh_program.texture_sampler  = glGetUniformLocation(gl.static_mesh_program.id, "texture_sampler");
    gl.static_mesh_program.cam_pos          = glGetUniformLocation(gl.static_mesh_program.id, "cam_pos");
    gl.static_mesh_program.color_ambient    = glGetUniformLocation(gl.static_mesh_program.id, "color_ambient");
    gl.static_mesh_program.color_diffuse    = glGetUniformLocation(gl.static_mesh_program.id, "color_diffuse");
    gl.static_mesh_program.color_specular   = glGetUniformLocation(gl.static_mesh_program.id, "color_specular");

    gl.grass_program.id = gl_create_program(header,
                                            grass_vshader,
                                            grass_fshader);
    gl.grass_program.mvp                = glGetUniformLocation(gl.grass_program.id, "mvp");
    gl.grass_program.time               = glGetUniformLocation(gl.grass_program.id, "time");
    gl.grass_program.grass_max_vertex_y = glGetUniformLocation(gl.grass_program.id, "grass_max_vertex_y");
    gl.grass_program.turbulence_map     = glGetUniformLocation(gl.grass_program.id, "turbulence_map");


    gl.white_bitmap.width   = 4;
    gl.white_bitmap.height  = 4;
    gl.white_bitmap.pitch   = 16;
    gl.white_bitmap.handle  = 0;
    gl.white_bitmap.size    = 64;
    gl.white_bitmap.memory  = &gl.white;
    for (u32 *at = (u32 *)gl.white;
         at <= &gl.white[3][3];
         ++at) 
    {
        *at = 0xffffffff;
    }
    gl_alloc_texture(&gl.white_bitmap);

    // dummy.
    glGenVertexArrays(1, &gl.vao);
    glBindVertexArray(gl.vao);

    glGenBuffers(1, &gl.vbo);

    glGenBuffers(1, &gl.vio);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gl.vio);

    glGenBuffers(1, &gl.grass_vbo);
}
