/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Sung Woo Lee $
   $Notice: (C) Copyright %s by Sung Woo Lee. All Rights Reserved. $
   ======================================================================== */

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
#define GL_CLAMP_TO_BORDER                  0x812D
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
#define GL_MULTISAMPLE                      0x809D
#define GL_SAMPLE_ALPHA_TO_COVERAGE         0x809E
#define GL_SAMPLE_ALPHA_TO_ONE              0x809F
#define GL_SAMPLE_COVERAGE                  0x80A0
#define GL_SAMPLE_BUFFERS                   0x80A8
#define GL_SAMPLES                          0x80A9
#define GL_SAMPLE_COVERAGE_VALUE            0x80AA
#define GL_SAMPLE_COVERAGE_INVERT           0x80AB
#define GL_MAX_SAMPLES                      0x8D57
#define GL_FRAMEBUFFER                      0x8D40
#define GL_DEPTH_ATTACHMENT                 0x8D00
#define GL_TEXTURE_3D                       0x806F
#define GL_TEXTURE_WRAP_R                   0x8072
#define GL_READ_ONLY                        0x88B8
#define GL_WRITE_ONLY                       0x88B9
#define GL_READ_WRITE                       0x88BA
#define GL_GEOMETRY_SHADER                  0x8DD9
#define GL_R8UI                             0x8232
#define GL_RGBA8UI                          0x8D7C
#define GL_R32UI                            0x8236
#define GL_RED_INTEGER                      0x8D94

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
typedef void        Type_glGenFramebuffers (GLsizei n, GLuint *framebuffers);
typedef void        Type_glBindFramebuffer (GLenum target, GLuint framebuffer);
typedef void        Type_glFramebufferTexture2D (GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
typedef void        Type_glTexStorage3D (GLenum target, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth);
typedef void        Type_glTexSubImage3D (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void *pixels);
typedef void        Type_glGenerateMipmap (GLenum target);
typedef void        Type_glBindImageTexture (GLuint unit, GLuint texture, GLint level, GLboolean layered, GLint layer, GLenum access, GLenum format);
typedef void        Type_glClearTexImage (GLuint texture, GLint level, GLenum format, GLenum type, const void *data);


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
GL_DECLARE_GLOBAL_FUNCTION(glGenFramebuffers);
GL_DECLARE_GLOBAL_FUNCTION(glBindFramebuffer);
GL_DECLARE_GLOBAL_FUNCTION(glFramebufferTexture2D);
GL_DECLARE_GLOBAL_FUNCTION(glTexStorage3D);
GL_DECLARE_GLOBAL_FUNCTION(glTexSubImage3D);
GL_DECLARE_GLOBAL_FUNCTION(glGenerateMipmap);
GL_DECLARE_GLOBAL_FUNCTION(glBindImageTexture);
GL_DECLARE_GLOBAL_FUNCTION(glClearTexImage);


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
            if (i++ == 0) 
            {
                gl_info.major = (c - '0');
            } 
            else 
            {
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
            // Assert(0);
        } break;
        case GL_DEBUG_SEVERITY_HIGH: 
        {
            Assert(0);
        } break;
    }
}

internal void
gl_init_info()
{
    gl_info.vendor     = (char *)glGetString(GL_VENDOR);
    gl_info.renderer   = (char *)glGetString(GL_RENDERER);
    gl_info.extensions = (char *)glGetString(GL_EXTENSIONS);
    Assert(gl_info.extensions);
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

internal GLuint
gl_create_program(const char *header,
                  const char *vsrc,
                  const char *gsrc,
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

        GLuint gshader = glCreateShader(GL_GEOMETRY_SHADER);
        const GLchar *gunit[] = {
            header,
            gsrc
        };
        glShaderSource(gshader, array_count(gunit), (const GLchar **)gunit, 0);
        glCompileShader(gshader);

        GLuint fshader = glCreateShader(GL_FRAGMENT_SHADER);
        const GLchar *funit[] = {
            header,
            fsrc
        };
        glShaderSource(fshader, array_count(funit), (const GLchar **)funit, 0);
        glCompileShader(fshader);

        program = glCreateProgram();
        glAttachShader(program, vshader);
        glAttachShader(program, gshader);
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

            GLchar glog[1024];
            glGetShaderInfoLog(gshader, sizeof(glog), &stub, glog);

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
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, bitmap->width, bitmap->height,
                 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, bitmap->memory);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

#define VOXEL_SIZE 512 
internal void
gl_alloc_voxel_map(Voxel_Map *vm, GLenum internal_format, GLenum subimage_format, size_t data_size)
{
    vm->voxel_size = VOXEL_SIZE;
    vm->data = VirtualAlloc(0, VOXEL_SIZE*VOXEL_SIZE*VOXEL_SIZE*data_size,
                            MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

    glGenTextures(1, &vm->id); // generate name.
    glBindTexture(GL_TEXTURE_3D, vm->id); // bind name to target.
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    s32 levels  = 7;
    glTexStorage3D(GL_TEXTURE_3D, levels, internal_format, VOXEL_SIZE, VOXEL_SIZE, VOXEL_SIZE); // alloc for each level.
    glTexSubImage3D(GL_TEXTURE_3D, 0, 0, 0, 0, VOXEL_SIZE, VOXEL_SIZE, VOXEL_SIZE, subimage_format, GL_UNSIGNED_BYTE, vm->data); // assign image data
    glGenerateMipmap(GL_TEXTURE_3D);

    glBindTexture(GL_TEXTURE_3D, 0);
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

static f32 DEBUG_time = 0.0f;

internal void
gl_render_batch(Render_Batch *batch, u32 win_w, u32 win_h)
{
    TIMED_FUNCTION();

    //
    // Voxelization
    //
    Voxel_Map *vm = &gl.voxel_map;
    Voxel_Map *am = &gl.albedo_map;
    Voxel_Map *nm = &gl.normal_map;
    f32 side_in_meter = 50.0f;
    f32 x = 2.0f / side_in_meter;
    m4x4 voxelize_clip_P = m4x4{{
        { x,  0,  0,  0},
        { 0,  x,  0,  0},
        { 0,  0,  x , 0},
        { 0,  0,  0,  1}
    }};

    //glClearTexImage(vm->id, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glClearTexImage(am->id, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    //glClearTexImage(nm->id, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

    // Settings
    glViewport(0, 0, VOXEL_SIZE, VOXEL_SIZE);
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
    glDisable(GL_SCISSOR_TEST);

    for (Render_Group *group = (Render_Group *)batch->base;
         (u8 *)group < (u8 *)batch->base + batch->used;
         ++group)
    {
        for (u8 *at = group->base;
             at < group->base + group->used;
            at += ((Render_Entity_Header *)at)->size)
        {
            Render_Entity_Header *entity = (Render_Entity_Header *)at;
            switch (entity->type)
            {
                case eRender_Mesh:
                {
#if 1
                    Render_Mesh *piece = (Render_Mesh *)entity;
                    Mesh *mesh            = piece->mesh;
                    Material *mat         = piece->material;

                    glBindBuffer(GL_ARRAY_BUFFER, gl.vbo);

                    // Use voxelization program.
                    Voxelization_Program *program = &gl.voxelization_program;
                    s32 pid = program->id;
                    glUseProgram(pid);

                    // Bind to image unit.
                    glBindTexture(GL_TEXTURE_3D, vm->id);
                    glBindImageTexture(0, vm->id, 0, GL_TRUE, 0, GL_READ_WRITE, GL_RGBA8);
                    glBindImageTexture(1, am->id, 0, GL_TRUE, 0, GL_READ_WRITE, GL_RGBA8UI);
                    glBindImageTexture(2, nm->id, 0, GL_TRUE, 0, GL_READ_WRITE, GL_RGBA8UI);

                    glUniformMatrix4fv(program->world_transform, 1, true, &piece->world_transform.e[0][0]);

                    // @TEMPORARY...?
                    Camera *camera = group->camera;

                    f32 w = camera->width;
                    f32 h = camera->height;
                    f32 f = camera->focal_length;
                    f32 N = camera->N;
                    f32 F = camera->F;

                    m4x4 VP = voxelize_clip_P * camera->V;

                    glUniformMatrix4fv(program->V, 1, GL_TRUE, &camera->V.e[0][0]);
                    glUniformMatrix4fv(program->P, 1, GL_TRUE, &voxelize_clip_P.e[0][0]);
                    glUniformMatrix4fv(program->VP, 1, GL_TRUE, &VP.e[0][0]);
                    glUniform1i(program->is_skeletal, piece->animation_transforms ? 1 : 0);
                    glUniform1f(program->time, DEBUG_time);
                    DEBUG_time += (pi32/120000.0f);
                    glUniform3fv(program->ambient, 1, (GLfloat *)&mat->color_ambient);
                    glUniform3fv(program->diffuse, 1, (GLfloat *)&mat->color_diffuse);
                    glUniform3fv(program->specular, 1, (GLfloat *)&mat->color_specular);

                    glEnableVertexAttribArray(0);
                    glEnableVertexAttribArray(1);
                    glEnableVertexAttribArray(2);
                    glEnableVertexAttribArray(3);
                    glEnableVertexAttribArray(4);
                    glEnableVertexAttribArray(5);

                    glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(Vertex), (GLvoid *)(offset_of(Vertex, pos)));
                    glVertexAttribPointer(1, 3, GL_FLOAT, false, sizeof(Vertex), (GLvoid *)(offset_of(Vertex, normal)));
                    glVertexAttribPointer(2, 2, GL_FLOAT, false, sizeof(Vertex), (GLvoid *)(offset_of(Vertex, uv)));
                    glVertexAttribPointer(3, 4, GL_FLOAT, true,  sizeof(Vertex), (GLvoid *)(offset_of(Vertex, color)));
                    glVertexAttribIPointer(4, MAX_BONE_PER_VERTEX, GL_INT, sizeof(Vertex), (GLvoid *)(offset_of(Vertex, node_ids)));
                    glVertexAttribPointer(5, MAX_BONE_PER_VERTEX, GL_FLOAT, false, sizeof(Vertex), (GLvoid *)(offset_of(Vertex, node_weights)));

                    glBufferData(GL_ARRAY_BUFFER,
                                 mesh->vertex_count * sizeof(Vertex),
                                 mesh->vertices,
                                 GL_DYNAMIC_DRAW);

                    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                                 mesh->index_count * sizeof(u32),
                                 mesh->indices,
                                 GL_DYNAMIC_DRAW);

                    if (piece->animation_transforms)
                        glUniformMatrix4fv(program->bone_transforms, MAX_BONE_PER_MESH, true, (GLfloat *)piece->animation_transforms);

                    glDrawElements(GL_TRIANGLES, mesh->index_count, GL_UNSIGNED_INT, (void *)0);

                    glDisableVertexAttribArray(0);
                    glDisableVertexAttribArray(1);
                    glDisableVertexAttribArray(2);
                    glDisableVertexAttribArray(3);
                    glDisableVertexAttribArray(4);
                    glDisableVertexAttribArray(5);
#endif
                } break;

                case eRender_Grass:
                {
                } break;

                case eRender_Star:
                {
                } break;

                case eRender_Bitmap:
                {
                } break;

                INVALID_DEFAULT_CASE;
            }
        }
    }


    //
    // DRAW
    //
    glViewport(0, 0, win_w, win_h);

    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_SCISSOR_TEST);

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.03f, 0.02f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glClearDepth(1.0f);
    glClear(GL_DEPTH_BUFFER_BIT);
    glDepthFunc(GL_LEQUAL);

    glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE);
    glEnable(GL_SAMPLE_ALPHA_TO_ONE);
    glEnable(GL_MULTISAMPLE);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    // int n;
    // glGetIntegerv(GL_MAX_SAMPLES, &n);
    
    int DEBUG_DRAW_MODE = 0;

    for (Render_Group *group = (Render_Group *)batch->base;
         (u8 *)group < (u8 *)batch->base + batch->used;
         ++group)
    {
        for (u8 *at = group->base;
             at < group->base + group->used;
             at += ((Render_Entity_Header *)at)->size)
        {
            Render_Entity_Header *entity = (Render_Entity_Header *)at;
            switch (entity->type)
            {
                case eRender_Mesh:
                {
                    switch (DEBUG_DRAW_MODE)
                    {
                        case 0:
                        {
                            Render_Mesh *piece = (Render_Mesh *)entity;
                            Mesh *mesh            = piece->mesh;
                            Material *mat         = piece->material;

                            glBindBuffer(GL_ARRAY_BUFFER, gl.vbo);

                            Mesh_Program *program = &gl.mesh_program;
                            s32 pid = program->id;
                            glUseProgram(pid);

                            Camera *camera = group->camera;

                            m4x4 voxel_VP = voxelize_clip_P *camera->V;

                            glUniformMatrix4fv(program->voxel_VP, 1, GL_TRUE, &voxel_VP.e[0][0]);
                            glUniformMatrix4fv(program->persp_VP, 1, GL_TRUE, &group->camera->VP.e[0][0]);
                            glUniform3fv(program->cam_pos, 1, (GLfloat *)&group->camera->world_translation);
                            glUniform1i(program->is_skeletal, piece->animation_transforms ? 1 : 0);

                            glBindImageTexture(0, vm->id, 0, GL_TRUE, 0, GL_READ_WRITE, GL_RGBA8);
                            glBindImageTexture(1, am->id, 0, GL_TRUE, 0, GL_READ_WRITE, GL_RGBA8UI);
                            glBindImageTexture(2, nm->id, 0, GL_TRUE, 0, GL_READ_WRITE, GL_RGBA8UI);

                            glEnableVertexAttribArray(0);
                            glEnableVertexAttribArray(1);
                            glEnableVertexAttribArray(2);
                            glEnableVertexAttribArray(3);
                            glEnableVertexAttribArray(4);
                            glEnableVertexAttribArray(5);

                            glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(Vertex), (GLvoid *)(offset_of(Vertex, pos)));
                            glVertexAttribPointer(1, 3, GL_FLOAT, false, sizeof(Vertex), (GLvoid *)(offset_of(Vertex, normal)));
                            glVertexAttribPointer(2, 2, GL_FLOAT, false, sizeof(Vertex), (GLvoid *)(offset_of(Vertex, uv)));
                            glVertexAttribPointer(3, 4, GL_FLOAT, true,  sizeof(Vertex), (GLvoid *)(offset_of(Vertex, color)));
                            glVertexAttribIPointer(4, MAX_BONE_PER_VERTEX, GL_INT, sizeof(Vertex), (GLvoid *)(offset_of(Vertex, node_ids)));
                            glVertexAttribPointer(5, MAX_BONE_PER_VERTEX, GL_FLOAT, false, sizeof(Vertex), (GLvoid *)(offset_of(Vertex, node_weights)));

                            glBufferData(GL_ARRAY_BUFFER,
                                         mesh->vertex_count * sizeof(Vertex),
                                         mesh->vertices,
                                         GL_DYNAMIC_DRAW);

                            glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                                         mesh->index_count * sizeof(u32),
                                         mesh->indices,
                                         GL_DYNAMIC_DRAW);

                            glUniformMatrix4fv(program->world_transform, 1, true, &piece->world_transform.e[0][0]);
                            if (piece->animation_transforms)
                                glUniformMatrix4fv(program->bone_transforms, MAX_BONE_PER_MESH, true, (GLfloat *)piece->animation_transforms);
                            glUniform3fv(program->color_ambient, 1, (GLfloat *)&mat->color_ambient);
                            glUniform3fv(program->color_diffuse, 1, (GLfloat *)&mat->color_diffuse);
                            glUniform3fv(program->color_specular, 1, (GLfloat *)&mat->color_specular);

                            glUniform3fv(program->light_pos, 1, (GLfloat *)&piece->light_pos);

                            glDrawElements(GL_TRIANGLES, mesh->index_count, GL_UNSIGNED_INT, (void *)0);

                            glDisableVertexAttribArray(0);
                            glDisableVertexAttribArray(1);
                            glDisableVertexAttribArray(2);
                            glDisableVertexAttribArray(3);
                            glDisableVertexAttribArray(4);
                            glDisableVertexAttribArray(5);
                        } break;

                        case 1:
                        {
                            Render_Mesh *piece = (Render_Mesh *)entity;
                            Mesh *mesh            = piece->mesh;
                            Material *mat         = piece->material;

                            glBindBuffer(GL_ARRAY_BUFFER, gl.vbo);

                            Voxel_Program *program = &gl.voxel_program;
                            s32 pid = program->id;
                            glUseProgram(pid);

                            Camera *camera = group->camera;

                            glUniformMatrix4fv(program->world_transform, 1, true, &piece->world_transform.e[0][0]);
                            glUniformMatrix4fv(program->V, 1, GL_TRUE, &camera->V.e[0][0]);
                            glUniformMatrix4fv(program->persp_P, 1, GL_TRUE, &camera->P.e[0][0]);
                            glUniformMatrix4fv(program->ortho_P, 1, GL_TRUE, &voxelize_clip_P.e[0][0]);
                            glUniform1i(program->is_skeletal, piece->animation_transforms ? 1 : 0);
                            if (piece->animation_transforms)
                                glUniformMatrix4fv(program->bone_transforms, MAX_BONE_PER_MESH, true, (GLfloat *)piece->animation_transforms);

                            glBindTexture(GL_TEXTURE_3D, vm->id);
                            glUniform1i(program->voxel_map, 0);
                            glBindImageTexture(0, vm->id, 0, GL_TRUE, 0, GL_READ_WRITE, GL_RGBA8);
                            glBindImageTexture(1, am->id, 0, GL_TRUE, 0, GL_READ_WRITE, GL_RGBA8UI);
                            glBindImageTexture(2, nm->id, 0, GL_TRUE, 0, GL_READ_WRITE, GL_RGBA8UI);

                            glEnableVertexAttribArray(0);
                            glEnableVertexAttribArray(1);
                            glEnableVertexAttribArray(2);
                            glEnableVertexAttribArray(3);
                            glEnableVertexAttribArray(4);
                            glEnableVertexAttribArray(5);

                            glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(Vertex), (GLvoid *)(offset_of(Vertex, pos)));
                            glVertexAttribPointer(1, 3, GL_FLOAT, false, sizeof(Vertex), (GLvoid *)(offset_of(Vertex, normal)));
                            glVertexAttribPointer(2, 2, GL_FLOAT, false, sizeof(Vertex), (GLvoid *)(offset_of(Vertex, uv)));
                            glVertexAttribPointer(3, 4, GL_FLOAT, true,  sizeof(Vertex), (GLvoid *)(offset_of(Vertex, color)));
                            glVertexAttribIPointer(4, MAX_BONE_PER_VERTEX, GL_INT, sizeof(Vertex), (GLvoid *)(offset_of(Vertex, node_ids)));
                            glVertexAttribPointer(5, MAX_BONE_PER_VERTEX, GL_FLOAT, false, sizeof(Vertex), (GLvoid *)(offset_of(Vertex, node_weights)));

                            glBufferData(GL_ARRAY_BUFFER,
                                         mesh->vertex_count * sizeof(Vertex),
                                         mesh->vertices,
                                         GL_DYNAMIC_DRAW);

                            glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                                         mesh->index_count * sizeof(u32),
                                         mesh->indices,
                                         GL_DYNAMIC_DRAW);


                            glDrawElements(GL_TRIANGLES, mesh->index_count, GL_UNSIGNED_INT, (void *)0);

                            glDisableVertexAttribArray(0);
                            glDisableVertexAttribArray(1);
                            glDisableVertexAttribArray(2);
                            glDisableVertexAttribArray(3);
                            glDisableVertexAttribArray(4);
                            glDisableVertexAttribArray(5);
                        } break;

                        case 2:
                        {
                            Render_Mesh *piece = (Render_Mesh *)entity;
                            Mesh *mesh            = piece->mesh;
                            Material *mat         = piece->material;

                            glBindBuffer(GL_ARRAY_BUFFER, gl.vbo);

                            Clip_Program *program = &gl.clip_program;
                            s32 pid = program->id;
                            glUseProgram(pid);

                            // @TEMPORARY...?
                            Camera *camera = group->camera;

                            glUniformMatrix4fv(program->world_transform, 1, true, &piece->world_transform.e[0][0]);
                            glUniformMatrix4fv(program->V, 1, true, &camera->V.e[0][0]);
                            glUniformMatrix4fv(program->ortho_P, 1, true, &voxelize_clip_P.e[0][0]);
                            glUniformMatrix4fv(program->persp_P, 1, true, &camera->P.e[0][0]);
                            glUniform1i(program->is_skeletal, piece->animation_transforms ? 1 : 0);
                            if (piece->animation_transforms)
                                glUniformMatrix4fv(program->bone_transforms, MAX_BONE_PER_MESH, true, (GLfloat *)piece->animation_transforms);

                            glEnableVertexAttribArray(0);
                            glEnableVertexAttribArray(1);
                            glEnableVertexAttribArray(2);
                            glEnableVertexAttribArray(3);
                            glEnableVertexAttribArray(4);
                            glEnableVertexAttribArray(5);

                            glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(Vertex), (GLvoid *)(offset_of(Vertex, pos)));
                            glVertexAttribPointer(1, 3, GL_FLOAT, false, sizeof(Vertex), (GLvoid *)(offset_of(Vertex, normal)));
                            glVertexAttribPointer(2, 2, GL_FLOAT, false, sizeof(Vertex), (GLvoid *)(offset_of(Vertex, uv)));
                            glVertexAttribPointer(3, 4, GL_FLOAT, true,  sizeof(Vertex), (GLvoid *)(offset_of(Vertex, color)));
                            glVertexAttribIPointer(4, MAX_BONE_PER_VERTEX, GL_INT, sizeof(Vertex), (GLvoid *)(offset_of(Vertex, node_ids)));
                            glVertexAttribPointer(5, MAX_BONE_PER_VERTEX, GL_FLOAT, false, sizeof(Vertex), (GLvoid *)(offset_of(Vertex, node_weights)));

                            glBufferData(GL_ARRAY_BUFFER,
                                         mesh->vertex_count * sizeof(Vertex),
                                         mesh->vertices,
                                         GL_DYNAMIC_DRAW);

                            glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                                         mesh->index_count * sizeof(u32),
                                         mesh->indices,
                                         GL_DYNAMIC_DRAW);


                            glDrawElements(GL_TRIANGLES, mesh->index_count, GL_UNSIGNED_INT, (void *)0);

                            glDisableVertexAttribArray(0);
                            glDisableVertexAttribArray(1);
                            glDisableVertexAttribArray(2);
                            glDisableVertexAttribArray(3);
                            glDisableVertexAttribArray(4);
                            glDisableVertexAttribArray(5);
                        } break;

                        INVALID_DEFAULT_CASE;
                    }
                } break;

                case eRender_Grass:
                {
#if 1
                    Render_Grass *piece = (Render_Grass *)entity;

                    glBindBuffer(GL_ARRAY_BUFFER, gl.vbo);

                    glDisable(GL_CULL_FACE);

                    Grass_Program *program = &gl.grass_program;
                    s32 pid = program->id;
                    glUseProgram(pid);

                    glUniformMatrix4fv(program->mvp, 1, GL_TRUE, &group->camera->VP.e[0][0]);

                    glEnableVertexAttribArray(0);
                    glEnableVertexAttribArray(1);
                    glEnableVertexAttribArray(2);
                    glEnableVertexAttribArray(3);
                    glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(Vertex), (GLvoid *)(offset_of(Vertex, pos)));
                    glVertexAttribPointer(1, 3, GL_FLOAT, false, sizeof(Vertex), (GLvoid *)(offset_of(Vertex, normal)));
                    glVertexAttribPointer(2, 2, GL_FLOAT, false, sizeof(Vertex), (GLvoid *)(offset_of(Vertex, uv)));
                    glVertexAttribPointer(3, 4, GL_FLOAT, true,  sizeof(Vertex), (GLvoid *)(offset_of(Vertex, color)));

                    glUniform1f(program->time, piece->time);
                    glUniform1f(program->grass_max_vertex_y, piece->grass_max_vertex_y);
                    Mesh *mesh    = piece->mesh;
                    glBufferData(GL_ARRAY_BUFFER,
                                 mesh->vertex_count * sizeof(Vertex),
                                 mesh->vertices,
                                 GL_DYNAMIC_DRAW);
                    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                                 mesh->index_count * sizeof(u32),
                                 mesh->indices,
                                 GL_DYNAMIC_DRAW);

                    glBindBuffer(GL_ARRAY_BUFFER, gl.grass_vbo);
                    glEnableVertexAttribArray(6);
                    glEnableVertexAttribArray(7);
                    glEnableVertexAttribArray(8);
                    glEnableVertexAttribArray(9);
                    glVertexAttribPointer(6, 4, GL_FLOAT, false, 4 * sizeof(v4), (GLvoid *)0);
                    glVertexAttribPointer(7, 4, GL_FLOAT, false, 4 * sizeof(v4), (GLvoid *)(sizeof(v4)));
                    glVertexAttribPointer(8, 4, GL_FLOAT, false, 4 * sizeof(v4), (GLvoid *)(2 * sizeof(v4)));
                    glVertexAttribPointer(9, 4, GL_FLOAT, false, 4 * sizeof(v4), (GLvoid *)(3 * sizeof(v4)));
                    glVertexAttribDivisor(6, 1);
                    glVertexAttribDivisor(7, 1);
                    glVertexAttribDivisor(8, 1);
                    glVertexAttribDivisor(9, 1);
                    glBufferData(GL_ARRAY_BUFFER, sizeof(m4x4) * piece->count, piece->world_transforms, GL_STATIC_DRAW);
                    glBindBuffer(GL_ARRAY_BUFFER, gl.vbo);

                    Bitmap *turbulence_map = piece->turbulence_map;
                    if (!turbulence_map->handle)
                    {
                        glGenTextures(1, &turbulence_map->handle);
                        glBindTexture(GL_TEXTURE_2D, turbulence_map->handle);
                        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, turbulence_map->width, turbulence_map->height,
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
                    glDisableVertexAttribArray(7);
                    glDisableVertexAttribArray(8);
                    glDisableVertexAttribArray(9);
#endif
                } break;

                case eRender_Star:
                {
                    Render_Star *piece = (Render_Star *)entity;

                    Mesh *mesh = piece->mesh;

                    glBindBuffer(GL_ARRAY_BUFFER, gl.vbo);

                    Star_Program *program = &gl.star_program;
                    s32 pid = program->id;
                    glUseProgram(pid);

                    glUniformMatrix4fv(program->mvp, 1, GL_TRUE, &group->camera->VP.e[0][0]);
                    glUniform1f(program->time, piece->time);

                    glEnableVertexAttribArray(0);
                    glEnableVertexAttribArray(1);
                    glEnableVertexAttribArray(2);
                    glEnableVertexAttribArray(3);
                    glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(Vertex), (GLvoid *)(offset_of(Vertex, pos)));
                    glVertexAttribPointer(1, 3, GL_FLOAT, false, sizeof(Vertex), (GLvoid *)(offset_of(Vertex, normal)));
                    glVertexAttribPointer(2, 2, GL_FLOAT, false, sizeof(Vertex), (GLvoid *)(offset_of(Vertex, uv)));
                    glVertexAttribPointer(3, 4, GL_FLOAT, true,  sizeof(Vertex), (GLvoid *)(offset_of(Vertex, color)));

                    glBufferData(GL_ARRAY_BUFFER,
                                 mesh->vertex_count * sizeof(Vertex),
                                 mesh->vertices,
                                 GL_DYNAMIC_DRAW);
                    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                                 mesh->index_count * sizeof(u32),
                                 mesh->indices,
                                 GL_DYNAMIC_DRAW);

                    glBindBuffer(GL_ARRAY_BUFFER, gl.star_vbo);
                    glEnableVertexAttribArray(6);
                    glEnableVertexAttribArray(7);
                    glEnableVertexAttribArray(8);
                    glEnableVertexAttribArray(9);
                    glVertexAttribPointer(6, 4, GL_FLOAT, false, 4 * sizeof(v4), (GLvoid *)0);
                    glVertexAttribPointer(7, 4, GL_FLOAT, false, 4 * sizeof(v4), (GLvoid *)(sizeof(v4)));
                    glVertexAttribPointer(8, 4, GL_FLOAT, false, 4 * sizeof(v4), (GLvoid *)(2 * sizeof(v4)));
                    glVertexAttribPointer(9, 4, GL_FLOAT, false, 4 * sizeof(v4), (GLvoid *)(3 * sizeof(v4)));
                    glVertexAttribDivisor(6, 1);
                    glVertexAttribDivisor(7, 1);
                    glVertexAttribDivisor(8, 1);
                    glVertexAttribDivisor(9, 1);
                    glBufferData(GL_ARRAY_BUFFER, sizeof(m4x4) * piece->count, piece->world_transforms, GL_STATIC_DRAW);
                    glBindBuffer(GL_ARRAY_BUFFER, gl.vbo);

                    glDrawElementsInstanced(GL_TRIANGLES, mesh->index_count, GL_UNSIGNED_INT, (void *)0, piece->count);

                    glDisableVertexAttribArray(0);
                    glDisableVertexAttribArray(1);
                    glDisableVertexAttribArray(2);
                    glDisableVertexAttribArray(3);
                    glDisableVertexAttribArray(6);
                    glDisableVertexAttribArray(7);
                    glDisableVertexAttribArray(8);
                    glDisableVertexAttribArray(9);

                } break;

                case eRender_Bitmap:
                {
                    Render_Bitmap *piece  = (Render_Bitmap *)entity;

                    glBindBuffer(GL_ARRAY_BUFFER, gl.vbo);

                    Sprite_Program *program = &gl.sprite_program;
                    s32 pid = program->id;
                    glUseProgram(pid);

                    glUniformMatrix4fv(program->mvp, 1, GL_TRUE, &group->camera->VP.e[0][0]);
                    glUniform4fv(program->color, 1, &piece->color.r);

                    glEnableVertexAttribArray(0);
                    glEnableVertexAttribArray(2);

                    glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(Textured_Vertex), (GLvoid *)offset_of(Textured_Vertex, pos));
                    glVertexAttribPointer(2, 2, GL_FLOAT, false, sizeof(Textured_Vertex), (GLvoid *)offset_of(Textured_Vertex, uv));

                    gl_bind_texture(piece->bitmap);

                    v3 min = piece->min;
                    v3 max = piece->max;

                    Textured_Vertex v[4];
                    v[0].pos = _v3_(max.x, min.y, 0);
                    v[0].uv  = _v2_(1, 0);

                    v[1].pos = max;
                    v[1].uv  = _v2_(1, 1);

                    v[2].pos = min;
                    v[2].uv  = _v2_(0, 0);

                    v[3].pos = _v3_(min.x, max.y, 0);
                    v[3].uv  = _v2_(0, 1);

                    glBufferData(GL_ARRAY_BUFFER,
                                 array_count(v) * sizeof(*v), v,
                                 GL_STATIC_DRAW);
                    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

                    glDisableVertexAttribArray(0);
                    glDisableVertexAttribArray(2);
                } break;
            }
        }
    }
            
    batch->used = 0;
    gl_bind_texture(0);
    glUseProgram(0);
}


internal void
gl_init()
{
#if __DEVELOPER
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

    const char *mesh_vshader = 
    #include "shader/mesh.vs"
    ;

    const char *mesh_fshader = 
    #include "shader/mesh.fs"
    ;

    const char *grass_vshader = 
    #include "shader/grass.vert"
    ;

    const char *grass_fshader = 
    #include "shader/grass.frag"
    ;

    const char *sprite_vshader = 
    #include "shader/sprite.vert"
    ;
    const char *sprite_fshader = 
    #include "shader/sprite.frag"
    ;

    const char *star_vshader = 
    #include "shader/star.vert"
    ;
    const char *star_fshader = 
    #include "shader/star.frag"
    ;

    const char *voxelization_vs = 
    #include "shader/voxelization.vs"
    ;
    const char *voxelization_gs = 
    #include "shader/voxelization.gs"
    ;
    const char *voxelization_fs = 
    #include "shader/voxelization.fs"
    ;

    const char *voxel_vs = 
    #include "shader/voxel_visual.vs"
    ;
    const char *voxel_fs = 
    #include "shader/voxel_visual.fs"
    ;

    const char *clip_vs = 
    #include "shader/clip_coord.vs"
    ;
    const char *clip_fs = 
    #include "shader/clip_coord.fs"
    ;



#define GET_UNIFORM_LOCATION(program, name) gl.program.name = glGetUniformLocation(gl.program.id, #name);
    gl.mesh_program.id = gl_create_program(header, mesh_vshader, mesh_fshader);
    GET_UNIFORM_LOCATION(mesh_program, world_transform);
    GET_UNIFORM_LOCATION(mesh_program, voxel_VP);
    GET_UNIFORM_LOCATION(mesh_program, persp_VP);
    GET_UNIFORM_LOCATION(mesh_program, is_skeletal);
    GET_UNIFORM_LOCATION(mesh_program, cam_pos);
    GET_UNIFORM_LOCATION(mesh_program, bone_transforms);
    GET_UNIFORM_LOCATION(mesh_program, color_ambient);
    GET_UNIFORM_LOCATION(mesh_program, color_diffuse);
    GET_UNIFORM_LOCATION(mesh_program, color_specular);
    GET_UNIFORM_LOCATION(mesh_program, light_pos);

    gl.grass_program.id = gl_create_program(header, grass_vshader, grass_fshader);
    GET_UNIFORM_LOCATION(grass_program, mvp);
    GET_UNIFORM_LOCATION(grass_program, time);
    GET_UNIFORM_LOCATION(grass_program, grass_max_vertex_y);
    GET_UNIFORM_LOCATION(grass_program, turbulence_map);

    gl.sprite_program.id = gl_create_program(header, sprite_vshader, sprite_fshader);
    GET_UNIFORM_LOCATION(sprite_program, mvp);
    GET_UNIFORM_LOCATION(sprite_program, color);
    GET_UNIFORM_LOCATION(sprite_program, texture);

    gl.star_program.id = gl_create_program(header, star_vshader, star_fshader);
    GET_UNIFORM_LOCATION(star_program, mvp);
    GET_UNIFORM_LOCATION(star_program, time);

    gl.voxelization_program.id = gl_create_program(header, voxelization_vs, voxelization_gs, voxelization_fs);
    GET_UNIFORM_LOCATION(voxelization_program, world_transform);
    GET_UNIFORM_LOCATION(voxelization_program, V);
    GET_UNIFORM_LOCATION(voxelization_program, VP);
    GET_UNIFORM_LOCATION(voxelization_program, is_skeletal);
    GET_UNIFORM_LOCATION(voxelization_program, bone_transforms);
    GET_UNIFORM_LOCATION(voxelization_program, voxel_map);
    GET_UNIFORM_LOCATION(voxelization_program, time);
    GET_UNIFORM_LOCATION(voxelization_program, ambient);
    GET_UNIFORM_LOCATION(voxelization_program, diffuse);
    GET_UNIFORM_LOCATION(voxelization_program, specular);

    gl.voxel_program.id = gl_create_program(header, voxel_vs, voxel_fs);
    GET_UNIFORM_LOCATION(voxel_program, world_transform);
    GET_UNIFORM_LOCATION(voxel_program, V);
    GET_UNIFORM_LOCATION(voxel_program, persp_P);
    GET_UNIFORM_LOCATION(voxel_program, ortho_P);
    GET_UNIFORM_LOCATION(voxel_program, is_skeletal);
    GET_UNIFORM_LOCATION(voxel_program, bone_transforms);
    GET_UNIFORM_LOCATION(voxel_program, voxel_map);

    gl.clip_program.id = gl_create_program(header, clip_vs, clip_fs);
    GET_UNIFORM_LOCATION(clip_program, world_transform);
    GET_UNIFORM_LOCATION(clip_program, V);
    GET_UNIFORM_LOCATION(clip_program, persp_P);
    GET_UNIFORM_LOCATION(clip_program, ortho_P);
    GET_UNIFORM_LOCATION(clip_program, is_skeletal);
    GET_UNIFORM_LOCATION(clip_program, bone_transforms);



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

    
    //
    // Radiance Voxel-Map
    //
#if 1
    gl_alloc_voxel_map(&gl.voxel_map, GL_RGBA8, GL_RGBA, 4);
    gl_alloc_voxel_map(&gl.albedo_map, GL_RGBA8, GL_RGBA, 4);
    gl_alloc_voxel_map(&gl.normal_map, GL_RGBA8, GL_RGBA, 4);
#endif
    


    //
    // Dummy
    //
    glGenVertexArrays(1, &gl.vao);
    glBindVertexArray(gl.vao);

    glGenBuffers(1, &gl.vbo);

    glGenBuffers(1, &gl.vio);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gl.vio);

    glGenBuffers(1, &gl.grass_vbo);

    glGenBuffers(1, &gl.star_vbo);
}
