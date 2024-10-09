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

#define OCTREE_LEVEL        10       // For debug buffer, LEVEL 10 won't work. Too big.
#define VOXEL_HALF_SIDE     50 
#define VV                  0


#if __DEVELOPER
  #define GL(func) func; Assert(glGetError() == GL_NO_ERROR);
#else
  #define GL(func)
#endif

#define GET_UNIFORM_LOCATION(program, name)\
    gl.program.name = GL(glGetUniformLocation(gl.program.id, #name));


typedef char    GLchar;
typedef size_t  GLsizeiptr;
typedef size_t  GLintptr;
typedef void (APIENTRY  *GLDEBUGPROCARB)(GLenum source,GLenum type,GLuint id,GLenum severity,GLsizei length,const GLchar *message,const void *userParam);

#define GL_SRGB8_ALPHA8                     0x8C43
#define GL_FRAMEBUFFER_SRGB                 0x8DB9            
#define GL_SHADING_LANGUAGE_VERSION         0x8B8C
#define GL_VERTEX_SHADER                    0x8B31
#define GL_GEOMETRY_SHADER                  0x8DD9
#define GL_FRAGMENT_SHADER                  0x8B30
#define GL_COMPUTE_SHADER                   0x91B9
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
#define GL_R8UI                             0x8232
#define GL_RGBA8UI                          0x8D7C
#define GL_R32UI                            0x8236
#define GL_RED_INTEGER                      0x8D94
#define GL_COLOR_ATTACHMENT0                0x8CE0
#define GL_COLOR_ATTACHMENT1                0x8CE1
#define GL_COLOR_ATTACHMENT2                0x8CE2
#define GL_RGBA16F                          0x881A
#define GL_MAX_COLOR_ATTACHMENTS            0x8CDF
#define GL_TEXTURE0                         0x84C0
#define GL_TEXTURE1                         0x84C1
#define GL_TEXTURE2                         0x84C2
#define GL_TEXTURE3                         0x84C3
#define GL_RENDERBUFFER                     0x8D41
#define GL_ATOMIC_COUNTER_BUFFER            0x92C0
#define GL_DYNAMIC_STORAGE_BIT              0x0100
#define GL_MAP_READ_BIT                     0x0001
#define GL_MAP_WRITE_BIT                    0x0002
#define GL_SHADER_STORAGE_BUFFER            0x90D2
#define GL_UNSIGNED_INT_10_10_10_2          0x8036
#define GL_RGB10_A2UI                       0x906F
#define GL_RGBA8                            0x8058
#define GL_TEXTURE_BUFFER                   0x8C2A
#define GL_MAX_COMPUTE_WORK_GROUP_COUNT     0x91BE
#define GL_MAX_COMPUTE_WORK_GROUP_SIZE      0x91BF
#define GL_MAP_PERSISTENT_BIT               0x0040
#define GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT  0x00000001
#define GL_ELEMENT_ARRAY_BARRIER_BIT        0x00000002
#define GL_UNIFORM_BARRIER_BIT              0x00000004
#define GL_TEXTURE_FETCH_BARRIER_BIT        0x00000008
#define GL_SHADER_IMAGE_ACCESS_BARRIER_BIT  0x00000020
#define GL_COMMAND_BARRIER_BIT              0x00000040
#define GL_PIXEL_BUFFER_BARRIER_BIT         0x00000080
#define GL_TEXTURE_UPDATE_BARRIER_BIT       0x00000100
#define GL_BUFFER_UPDATE_BARRIER_BIT        0x00000200
#define GL_FRAMEBUFFER_BARRIER_BIT          0x00000400
#define GL_TRANSFORM_FEEDBACK_BARRIER_BIT   0x00000800
#define GL_ATOMIC_COUNTER_BARRIER_BIT       0x00001000
#define GL_ALL_BARRIER_BITS                 0xFFFFFFFF



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
typedef void        Type_glDrawBuffers (GLsizei n, const GLenum *bufs);
typedef void        Type_glActiveTexture (GLenum texture);
typedef void        Type_glBindRenderbuffer (GLenum target, GLuint renderbuffer);
typedef void        Type_glRenderbufferStorage (GLenum target, GLenum internalformat, GLsizei width, GLsizei height);
typedef void        Type_glFramebufferRenderbuffer (GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer);
typedef void        Type_glGenRenderbuffers (GLsizei n, GLuint *renderbuffers);
typedef void        Type_glBufferSubData (GLenum target, GLintptr offset, GLsizeiptr size, const void *data);
typedef void        Type_glBufferStorage (GLenum target, GLsizeiptr size, const void *data, GLbitfield flags);
typedef void        Type_glBindBufferBase (GLenum target, GLuint index, GLuint buffer);
typedef void        Type_glGetBufferSubData (GLenum target, GLintptr offset, GLsizeiptr size, void *data);
typedef void        Type_glTexBuffer (GLenum target, GLenum internalformat, GLuint buffer);
typedef void        Type_glUniform1ui (GLint location, GLuint v0);
typedef void        Type_glDispatchCompute (GLuint num_groups_x, GLuint num_groups_y, GLuint num_groups_z);
typedef void        Type_glMemoryBarrier (GLbitfield barriers);
typedef void *      Type_glMapBufferRange (GLenum target, GLintptr offset, GLsizeiptr length, GLbitfield access);
typedef GLboolean   Type_glUnmapBuffer (GLenum target);
typedef void        Type_glGetIntegeri_v (GLenum target, GLuint index, GLint *data);
typedef void        Type_glDeleteBuffers (GLsizei n, const GLuint *buffers);
typedef void        Type_glClearNamedBufferData (GLuint buffer, GLenum internalformat, GLenum format, GLenum type, const void *data);


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
GL_DECLARE_GLOBAL_FUNCTION(glDrawBuffers);
GL_DECLARE_GLOBAL_FUNCTION(glActiveTexture);
GL_DECLARE_GLOBAL_FUNCTION(glBindRenderbuffer);
GL_DECLARE_GLOBAL_FUNCTION(glRenderbufferStorage);
GL_DECLARE_GLOBAL_FUNCTION(glFramebufferRenderbuffer);
GL_DECLARE_GLOBAL_FUNCTION(glGenRenderbuffers);
GL_DECLARE_GLOBAL_FUNCTION(glBufferSubData);
GL_DECLARE_GLOBAL_FUNCTION(glBufferStorage);
GL_DECLARE_GLOBAL_FUNCTION(glBindBufferBase);
GL_DECLARE_GLOBAL_FUNCTION(glGetBufferSubData);
GL_DECLARE_GLOBAL_FUNCTION(glTexBuffer);
GL_DECLARE_GLOBAL_FUNCTION(glUniform1ui);
GL_DECLARE_GLOBAL_FUNCTION(glDispatchCompute);
GL_DECLARE_GLOBAL_FUNCTION(glMemoryBarrier);
GL_DECLARE_GLOBAL_FUNCTION(glMapBufferRange);
GL_DECLARE_GLOBAL_FUNCTION(glUnmapBuffer);
GL_DECLARE_GLOBAL_FUNCTION(glGetIntegeri_v);
GL_DECLARE_GLOBAL_FUNCTION(glDeleteBuffers);
GL_DECLARE_GLOBAL_FUNCTION(glClearNamedBufferData);


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
    gl_info.vendor     = (char *)GL(glGetString(GL_VENDOR));
    gl_info.renderer   = (char *)GL(glGetString(GL_RENDERER));
    gl_info.extensions = (char *)GL(glGetString(GL_EXTENSIONS));
    Assert(gl_info.extensions);
    gl_parse_version();
    if (gl_info.modern) 
    {
        gl_info.shading_language_version = (char *)GL(glGetString(GL_SHADING_LANGUAGE_VERSION));
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
gl_create_compute_program(const char *header,
                          const char *csrc)
{
    GLuint program = 0;

    if (glCreateShader) 
    {
        GLuint cshader = glCreateShader(GL_COMPUTE_SHADER);
        const GLchar *cunit[] = {
            header,
            csrc
        };
        glShaderSource(cshader, array_count(cunit), (const GLchar **)cunit, 0);
        glCompileShader(cshader);

        program = glCreateProgram();
        glAttachShader(program, cshader);
        glLinkProgram(program);

        glValidateProgram(program);
        GLint linked = false;
        glGetProgramiv(program, GL_LINK_STATUS, &linked);
        if (!linked) 
        {
            GLsizei stub;

            GLchar clog[1024];
            glGetProgramInfoLog(cshader, sizeof(clog), &stub, clog);

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
        GLuint vshader = GL(glCreateShader(GL_VERTEX_SHADER));
        const GLchar *vunit[] = {
            header,
            vsrc
        };
        GL(glShaderSource(vshader, array_count(vunit), (const GLchar **)vunit, 0));
        GL(glCompileShader(vshader));

        GLuint gshader = GL(glCreateShader(GL_GEOMETRY_SHADER));
        const GLchar *gunit[] = {
            header,
            gsrc
        };
        GL(glShaderSource(gshader, array_count(gunit), (const GLchar **)gunit, 0));
        GL(glCompileShader(gshader));

        GLuint fshader = GL(glCreateShader(GL_FRAGMENT_SHADER));
        const GLchar *funit[] = {
            header,
            fsrc
        };
        GL(glShaderSource(fshader, array_count(funit), (const GLchar **)funit, 0));
        GL(glCompileShader(fshader));

        program = GL(glCreateProgram());
        GL(glAttachShader(program, vshader));
        GL(glAttachShader(program, gshader));
        GL(glAttachShader(program, fshader));
        GL(glLinkProgram(program));

        GL(glValidateProgram(program));
        GLint linked = false;
        GL(glGetProgramiv(program, GL_LINK_STATUS, &linked));
        if (!linked) 
        {
            GLsizei stub;

            GLchar vlog[1024];
            GL(glGetShaderInfoLog(vshader, sizeof(vlog), &stub, vlog));

            GLchar glog[1024];
            GL(glGetShaderInfoLog(gshader, sizeof(glog), &stub, glog));

            GLchar flog[1024];
            GL(glGetShaderInfoLog(fshader, sizeof(flog), &stub, flog));

            GLchar plog[1024];
            GL(glGetProgramInfoLog(program, sizeof(plog), &stub, plog));

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
    GL(glGenTextures(1, &bitmap->handle));
    GL(glBindTexture(GL_TEXTURE_2D, bitmap->handle));
    GL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, bitmap->width, bitmap->height,
                 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, bitmap->memory));

    GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
}

internal void
gl_bind_texture(Bitmap *bitmap)
{
    if (!bitmap) 
    {
        bitmap = &gl.white_bitmap;
    } 

    if (bitmap->handle) 
    {
        GL(glBindTexture(GL_TEXTURE_2D, bitmap->handle));
    } 
    else 
    {
        gl_alloc_texture(bitmap);
        GL(glBindTexture(GL_TEXTURE_2D, bitmap->handle));
    }
}

internal void
gl_bind_atomic_counter(s32 id, s32 binding_point)
{
    GL(glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, id));
    GL(glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, binding_point, id));
}

internal void
gl_gen_linear_buffer(u32 *buf, u32 *tex, GLenum format, size_t size)
{
    if (*buf)
    {
        GL(glDeleteBuffers(1, buf));
    }
    GL(glGenBuffers(1, buf));
    GL(glBindBuffer(GL_TEXTURE_BUFFER, *buf));
    GL(glBufferData(GL_TEXTURE_BUFFER, size, 0, GL_STATIC_DRAW));

    if (*tex)
    {
        GL(glDeleteTextures(1, tex));
    }
    GL(glGenTextures(1, tex));
    GL(glBindTexture(GL_TEXTURE_BUFFER, *tex));
    GL(glTexBuffer(GL_TEXTURE_BUFFER, format, *buf));

    GL(glBindBuffer(GL_TEXTURE_BUFFER, 0));
}

internal void
gl_reallocate_screen_dependent_buffers(u32 width, u32 height)
{
    //
    // G-Buffer
    //
    G_Buffer *gb = &gl.gbuffer;
    s32 max_attachment;
    GL(glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &max_attachment));
    Assert(max_attachment >= 3);

    GL(glGenFramebuffers(1, &gb->id));
    GL(glBindFramebuffer(GL_FRAMEBUFFER, gb->id));

    // Depth-Buffer
    u32 gdepth_buffer;
    GL(glGenRenderbuffers(1, &gdepth_buffer));
    GL(glBindRenderbuffer(GL_RENDERBUFFER, gdepth_buffer));
    GL(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height));
    GL(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, gdepth_buffer));

    // Position
    //   if there's an entity, alpha value is set to value other than 0.
    GL(glGenTextures(1, &gb->Pid));
    GL(glBindTexture(GL_TEXTURE_2D, gb->Pid));
    GL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, 0));
    GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
    GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
    GL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gb->Pid, 0));

    // Normal
    GL(glGenTextures(1, &gb->Nid));
    GL(glBindTexture(GL_TEXTURE_2D, gb->Nid));
    GL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL));
    GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
    GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
    GL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gb->Nid, 0);) 

        // Color (Albedo + Specular)
    GL(glGenTextures(1, &gb->Cid));
    GL(glBindTexture(GL_TEXTURE_2D, gb->Cid));
    GL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL));
    GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
    GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
    GL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gb->Cid, 0));


    u32 attachments[3] = { 
        GL_COLOR_ATTACHMENT0,
        GL_COLOR_ATTACHMENT1,
        GL_COLOR_ATTACHMENT2,
    };
    GL(glDrawBuffers(array_count(attachments), attachments));
    GL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}

internal void
gl_voxelize_scene(s32 write, Render_Batch *batch, m4x4 voxelize_clip_P, v3 light_P, v3 light_color, f32 light_strength)
{
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

                    // Use voxelization program.
                    Voxelization_Program *program = &gl.voxelization_program;
                    s32 pid = program->id;
                    GL(glUseProgram(pid));

                    // @TEMPORARY...?
                    Camera *camera = group->camera;

                    GL(glUniformMatrix4fv(program->world_transform, 1, true, &piece->world_transform.e[0][0]));
                    GL(glUniformMatrix4fv(program->V, 1, GL_TRUE, &camera->V.e[0][0]));
                    GL(glUniformMatrix4fv(program->voxel_P, 1, GL_TRUE, &voxelize_clip_P.e[0][0]));
                    GL(glUniform1i(program->is_skeletal, piece->animation_transforms ? 1 : 0));
                    GL(glUniform3fv(program->ambient, 1, (GLfloat *)&mat->color_ambient));
                    GL(glUniform3fv(program->diffuse, 1, (GLfloat *)&mat->color_diffuse));
                    GL(glUniform3fv(program->specular, 1, (GLfloat *)&mat->color_specular));

                    GL(glUniform1ui(program->octree_level, OCTREE_LEVEL));
                    GL(glUniform1ui(program->octree_resolution, gl.octree_resolution));
                    GL(glUniform1ui(program->write, write));

                    GL(glUniform3fv(program->DEBUG_light_P, 1, (GLfloat *)&light_P));
                    GL(glUniform3fv(program->DEBUG_light_color, 1, (GLfloat *)&light_color));
                    GL(glUniform1f(program->DEBUG_light_strength, light_strength));

                    GL(glEnableVertexAttribArray(0));
                    GL(glEnableVertexAttribArray(1));
                    GL(glEnableVertexAttribArray(2));
                    GL(glEnableVertexAttribArray(3));
                    GL(glEnableVertexAttribArray(4));
                    GL(glEnableVertexAttribArray(5));

                    GL(glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(Vertex), (GLvoid *)(offset_of(Vertex, pos))));
                    GL(glVertexAttribPointer(1, 3, GL_FLOAT, false, sizeof(Vertex), (GLvoid *)(offset_of(Vertex, normal))));
                    GL(glVertexAttribPointer(2, 2, GL_FLOAT, false, sizeof(Vertex), (GLvoid *)(offset_of(Vertex, uv))));
                    GL(glVertexAttribPointer(3, 4, GL_FLOAT, true,  sizeof(Vertex), (GLvoid *)(offset_of(Vertex, color))));
                    GL(glVertexAttribIPointer(4, MAX_BONE_PER_VERTEX, GL_INT, sizeof(Vertex), (GLvoid *)(offset_of(Vertex, node_ids))));
                    GL(glVertexAttribPointer(5, MAX_BONE_PER_VERTEX, GL_FLOAT, false, sizeof(Vertex), (GLvoid *)(offset_of(Vertex, node_weights))));

                    GL(glBufferData(GL_ARRAY_BUFFER,
                                 mesh->vertex_count * sizeof(Vertex),
                                 mesh->vertices,
                                 GL_DYNAMIC_DRAW));

                    GL(glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                                 mesh->index_count * sizeof(u32),
                                 mesh->indices,
                                 GL_DYNAMIC_DRAW));

                    if (piece->animation_transforms)
                        GL(glUniformMatrix4fv(program->bone_transforms, MAX_BONE_PER_MESH, true, (GLfloat *)piece->animation_transforms));

                    GL(glDrawElements(GL_TRIANGLES, mesh->index_count, GL_UNSIGNED_INT, (void *)0));
                    glMemoryBarrier(GL_ATOMIC_COUNTER_BARRIER_BIT|GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

                    GL(glDisableVertexAttribArray(0));
                    GL(glDisableVertexAttribArray(1));
                    GL(glDisableVertexAttribArray(2));
                    GL(glDisableVertexAttribArray(3));
                    GL(glDisableVertexAttribArray(4));
                    GL(glDisableVertexAttribArray(5));
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
}

//
// Main Render Batch Process.
//
internal void
gl_render_batch(Render_Batch *batch, u32 win_w, u32 win_h)
{
    TIMED_FUNCTION();

    f32 time = batch->time;
    G_Buffer *gb = &gl.gbuffer;

    //
    // Reallocate buffers dependent to changed window width & height.
    //
    if (batch->width != win_w ||
        batch->height != win_h)
    {
        batch->width = win_w;
        batch->height = win_h;

        gl_reallocate_screen_dependent_buffers(win_w, win_h);
    }


    u32 res = gl.octree_resolution;

#if 0
    f32 DEBUG_R = 1;
    f32 DEBUG_T = time;
    v3 light_P = v3{DEBUG_R*cos(DEBUG_T), 2, DEBUG_R*sin(DEBUG_T)};
    v3 light_color = v3{1, 1, 1};
    f32 light_strength = 1.0f;
#else
    v3 light_P = v3{0, 3, 0};
    v3 light_color = v3{1, 1, 1};
    f32 light_strength = 1.0f;
#endif

    //
    // Voxelization Pass
    //
    f32 x = 1.0f / VOXEL_HALF_SIDE;
    m4x4 voxelize_clip_P = m4x4{{
        { x,  0,  0,  0},
        { 0,  x,  0,  0},
        { 0,  0, -x , 0},
        { 0,  0,  0,  1}
    }};
    f32 voxel_in_meter = ((2.0f*VOXEL_HALF_SIDE) / (gl.octree_resolution));

    // Settings
    GL(glViewport(0, 0, gl.octree_resolution, gl.octree_resolution));
    GL(glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE));
    GL(glDisable(GL_CULL_FACE));
    GL(glDisable(GL_DEPTH_TEST));
    GL(glDisable(GL_BLEND));
    GL(glDisable(GL_SCISSOR_TEST));
    GL(glDisable(GL_MULTISAMPLE));

    GL(glBindBuffer(GL_ARRAY_BUFFER, gl.vbo));

    // Prepare atomic fragment-counter.
    GL(glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, gl.fragment_counter));
    GL(glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 0, gl.fragment_counter));

    gl_voxelize_scene(0, batch, voxelize_clip_P, light_P, light_color, light_strength);

    GL(glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, gl.fragment_counter));
    u32 *mapped_fragment_count = (u32 *)glMapBufferRange(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(GLuint), GL_MAP_READ_BIT|GL_MAP_WRITE_BIT);
    u32 fragment_count = mapped_fragment_count[0];
    *mapped_fragment_count = 0;
    GL(glUnmapBuffer(GL_ATOMIC_COUNTER_BUFFER));
    GL(glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0));

    if (gl.max_fragment_count < fragment_count)
    {
        gl.max_fragment_count = fragment_count;
        gl_gen_linear_buffer(&gl.flist_P, &gl.flist_P_texture, GL_R32UI, sizeof(u32) * fragment_count);
        gl_gen_linear_buffer(&gl.flist_diffuse, &gl.flist_diffuse_texture, GL_RGBA8, sizeof(u32) * fragment_count);
    }

    GL(glBindImageTexture(0, gl.flist_P_texture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGB10_A2UI));
    GL(glBindImageTexture(1, gl.flist_diffuse_texture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA8));

    gl_voxelize_scene(1, batch, voxelize_clip_P, light_P, light_color, light_strength);

    GL(glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, gl.fragment_counter));
    mapped_fragment_count = (u32 *)glMapBufferRange(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(GLuint), GL_MAP_READ_BIT|GL_MAP_WRITE_BIT);
    u32 fragment_count2 = mapped_fragment_count[0];
    Assert(fragment_count == fragment_count2);
    *mapped_fragment_count = 0;
    GL(glUnmapBuffer(GL_ATOMIC_COUNTER_BUFFER)); // @TODO: Is unmapping necessary?
    GL(glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0));
    glMemoryBarrier(GL_ATOMIC_COUNTER_BARRIER_BIT);


    //
    // Build Octree
    //
#if 1
    // Reset variables
    s32 wx, wy, gx, gy;
    u32 one = 1;
    u32 start = 0;
    u32 level_start[OCTREE_LEVEL + 1];
    u32 level_end[OCTREE_LEVEL + 1];
    level_start[0] = 0;
    level_end[0] = 1;

    // Reset alloc counter to 1.
    GL(glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, gl.alloc_count));
    GL(glBufferSubData(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(u32), &one));
    GL(glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 0, gl.alloc_count));
    
    // 'Level' to be allocated.
    for (u32 level = 1;
         level <= OCTREE_LEVEL;
         ++level)
    {
        //
        // 1. Flag
        //
        Flag_Program *fp = &gl.flag_program;
        GL(glUseProgram(fp->id));

        GL(glBindImageTexture(0, gl.flist_P_texture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGB10_A2UI));
        GL(glBindImageTexture(1, gl.octree_nodes_texture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32UI));
        GL(glUniform1ui(fp->current_level, level));
        GL(glUniform1ui(fp->octree_level, OCTREE_LEVEL));
        GL(glUniform1ui(fp->octree_resolution, gl.octree_resolution));
        GL(glUniform1ui(fp->fragment_count, fragment_count));

        wx = 1024;
        wy = (fragment_count + wx - 1) / wx;
        gx = 128;
        gy = ((wy + 7) >> 3);
        Assert((gx*gy*8*8) >= (s32)fragment_count);
        Assert(gx <= gl.max_compute_work_group_count[0] &&
               gy <= gl.max_compute_work_group_count[1]);
        GL(glDispatchCompute(gx, gy, 1));
        GL(glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT));


        //
        // 2. Alloc
        //
        Alloc_Program *ap = &gl.alloc_program;
        glUseProgram(ap->id);

        u32 prev_alloc_count;
        GL(glGetBufferSubData(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(u32), &prev_alloc_count));
        u32 node_count = (prev_alloc_count - (start >> 3));
        u32 alloc_size = (prev_alloc_count << 3);

        GL(glUniform1ui(ap->alloc_size, alloc_size));
        GL(glUniform1ui(ap->start, start));
        GL(glBindImageTexture(0, gl.octree_nodes_texture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32UI));

        wx = 1024;
        wy = ((node_count << 3) + wx - 1) / wx;
        gx = 128;
        gy = ((wy + 7) >> 3);
        Assert((gx*gy*8*8) >= (s32)(node_count<<3));
        Assert(gx <= gl.max_compute_work_group_count[0] &&
               gy <= gl.max_compute_work_group_count[1]);
        GL(glDispatchCompute(gx, gy, 1));
        GL(glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT|GL_ATOMIC_COUNTER_BARRIER_BIT));

        // Update
        start = (prev_alloc_count << 3);
        GL(glGetBufferSubData(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(u32), &prev_alloc_count));
        node_count = (prev_alloc_count - (start >> 3));
        alloc_size = (prev_alloc_count << 3);
        level_start[level] = start;
        level_end[level] = alloc_size;

        //
        // 3. Init
        //
        Init_Program *ip = &gl.init_program;
        GL(glUseProgram(ip->id));

        GL(glBindImageTexture(0, gl.octree_nodes_texture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32UI));
        GL(glUniform1ui(ip->start, start));
        GL(glUniform1ui(ip->alloc_size, alloc_size));

        wx = 1024;
        wy = ((node_count << 3) + wx - 1) / wx;
        gx = 128;
        gy = ((wy + 7) >> 3);
        Assert(gx * gy * 8 * 8 >= (s32)(node_count << 3));
        Assert(gx <= gl.max_compute_work_group_count[0] &&
               gy <= gl.max_compute_work_group_count[1]);
        GL(glDispatchCompute(gx, gy, 1));
        GL(glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT|GL_ATOMIC_COUNTER_BARRIER_BIT));
    }

    // get alloced node #.
    u32 node_count;
    GL(glGetBufferSubData(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(u32), &node_count));
    GL(glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0));

    // flag all leaves for mip-mapping.
    u32 work_size;
  #if 0
    Flag_Leaf_Program *fp = &gl.flag_leaf_program;
    GL(glUseProgram(fp->id));

    GL(glBindImageTexture(0, gl.octree_nodes_texture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32UI));
    GL(glUniform1ui(fp->leaf_start, level_start[OCTREE_LEVEL]));
    GL(glUniform1ui(fp->leaf_end, level_end[OCTREE_LEVEL]));

    work_size = (level_end[OCTREE_LEVEL] - level_start[OCTREE_LEVEL]);
    wx = 1024;
    wy = (work_size + wx - 1) / wx;
    gx = 128;
    gy = ((wy + 7) >> 3);
    Assert((gx*gy*8*8) >= (s32)work_size);
    Assert(gx <= gl.max_compute_work_group_count[0] &&
           gy <= gl.max_compute_work_group_count[1]);
    GL(glDispatchCompute(gx, gy, 1));
    GL(glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT));
  #endif

    u32 zero = 0;
    //
    // Creating (probably) memory-saved Octree Attribute Buffer.
    //
    u32 buffer_size = (sizeof(u32) * (node_count << 3));
    gl_gen_linear_buffer(&gl.octree_diffuse, &gl.octree_diffuse_texture, GL_R32UI, buffer_size);
    GL(glClearNamedBufferData(gl.octree_diffuse, GL_R32UI, GL_RED_INTEGER, GL_UNSIGNED_INT, &zero));

    Octree_Program *op = &gl.octree_program;
    GL(glUseProgram(op->id));

    GL(glBindImageTexture(0, gl.flist_P_texture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGB10_A2UI));
    GL(glBindImageTexture(1, gl.flist_diffuse_texture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA8));
    GL(glBindImageTexture(2, gl.octree_nodes_texture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32UI));
    GL(glBindImageTexture(3, gl.octree_diffuse_texture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32UI));

    GL(glUniform1ui(op->octree_level, OCTREE_LEVEL));
    GL(glUniform1ui(op->octree_resolution, gl.octree_resolution));
    GL(glUniform1ui(op->fragment_count, fragment_count));

    wx = 1024;
    wy = (fragment_count + wx - 1) / wx;
    gx = 128;
    gy = ((wy + 7) >> 3);
    Assert((gx*gy*8*8) >= (s32)fragment_count);
    Assert(gx <= gl.max_compute_work_group_count[0] &&
           gy <= gl.max_compute_work_group_count[1]);
    GL(glDispatchCompute(gx, gy, 1));
    GL(glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT));
#endif

    //
    // build octree mipmap from leaf.
    //
#if 1
    Mipmap_Program *mp = &gl.mipmap_program;
    GL(glUseProgram(mp->id));

    GL(glBindImageTexture(0, gl.octree_nodes_texture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32UI));
    GL(glBindImageTexture(1, gl.octree_diffuse_texture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32UI));

    for (u32 i = 1; i <= OCTREE_LEVEL; ++i)
    {
        u32 level = (OCTREE_LEVEL - i);

        GL(glUniform1ui(mp->level_start, level_start[level]));
        GL(glUniform1ui(mp->level_end, level_end[level]));

        work_size = (level_end[level] - level_start[level]);
        wx = 1024;
        wy = (work_size + wx - 1) / wx;
        gx = 128;
        gy = ((wy + 7) >> 3);
        Assert((gx*gy*8*8) >= (s32)work_size);
        Assert(gx <= gl.max_compute_work_group_count[0] &&
               gy <= gl.max_compute_work_group_count[1]);
        GL(glDispatchCompute(gx, gy, 1));
        GL(glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT));
    }
#endif




    
    //
    // G-Buffer Pass (Deferred Rendering)
    //
    GL(glBindFramebuffer(GL_FRAMEBUFFER, gb->id));
    GL(glBindBuffer(GL_ARRAY_BUFFER, gl.vbo));

    // Settings
    GL(glViewport(0, 0, win_w, win_h));
    GL(glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE));
    GL(glDisable(GL_BLEND));
    GL(glEnable(GL_SCISSOR_TEST));
    GL(glEnable(GL_DEPTH_TEST));
    GL(glClearColor(0, 0, 0, 0));
    GL(glClear(GL_COLOR_BUFFER_BIT));
    GL(glClearDepth(1));
    GL(glClear(GL_DEPTH_BUFFER_BIT));
    GL(glDepthFunc(GL_LEQUAL));
    GL(glDisable(GL_MULTISAMPLE));
    GL(glEnable(GL_CULL_FACE));
    GL(glCullFace(GL_BACK));
    GL(glFrontFace(GL_CCW));

    //

    v3 cam_P;
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
                    Render_Mesh *piece = (Render_Mesh *)entity;
                    Mesh *mesh            = piece->mesh;
                    Material *mat         = piece->material;

                    G_Buffer_Program *program = &gl.gbuffer_program;
                    s32 pid = program->id;
                    glUseProgram(pid);

                    Camera *camera = group->camera;
                    cam_P = camera->world_translation;

                    GL(glUniformMatrix4fv(program->VP, 1, GL_TRUE, &group->camera->VP.e[0][0]));
                    GL(glUniform1i(program->is_skeletal, piece->animation_transforms ? 1 : 0));
                    GL(glUniformMatrix4fv(program->world_transform, 1, true, &piece->world_transform.e[0][0]));
                    GL(glUniform3fv(program->ambient, 1, (GLfloat *)&mat->color_ambient));
                    GL(glUniform3fv(program->diffuse, 1, (GLfloat *)&mat->color_diffuse));
                    GL(glUniform3fv(program->specular, 1, (GLfloat *)&mat->color_specular));

                    GL(glEnableVertexAttribArray(0));
                    GL(glEnableVertexAttribArray(1));
                    GL(glEnableVertexAttribArray(2));
                    GL(glEnableVertexAttribArray(3));
                    GL(glEnableVertexAttribArray(4));
                    GL(glEnableVertexAttribArray(5));

                    GL(glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(Vertex), (GLvoid *)(offset_of(Vertex, pos))));
                    GL(glVertexAttribPointer(1, 3, GL_FLOAT, false, sizeof(Vertex), (GLvoid *)(offset_of(Vertex, normal))));
                    GL(glVertexAttribPointer(2, 2, GL_FLOAT, false, sizeof(Vertex), (GLvoid *)(offset_of(Vertex, uv))));
                    GL(glVertexAttribPointer(3, 4, GL_FLOAT, true,  sizeof(Vertex), (GLvoid *)(offset_of(Vertex, color))));
                    GL(glVertexAttribIPointer(4, MAX_BONE_PER_VERTEX, GL_INT, sizeof(Vertex), (GLvoid *)(offset_of(Vertex, node_ids))));
                    GL(glVertexAttribPointer(5, MAX_BONE_PER_VERTEX, GL_FLOAT, false, sizeof(Vertex), (GLvoid *)(offset_of(Vertex, node_weights))));

                    GL(glBufferData(GL_ARRAY_BUFFER,
                                 mesh->vertex_count * sizeof(Vertex),
                                 mesh->vertices,
                                 GL_DYNAMIC_DRAW));

                    GL(glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                                 mesh->index_count * sizeof(u32),
                                 mesh->indices,
                                 GL_DYNAMIC_DRAW));

                    if (piece->animation_transforms)
                        GL(glUniformMatrix4fv(program->bone_transforms, MAX_BONE_PER_MESH, true, (GLfloat *)piece->animation_transforms));

                    GL(glDrawElements(GL_TRIANGLES, mesh->index_count, GL_UNSIGNED_INT, (void *)0));
                    GL(glMemoryBarrier(GL_FRAMEBUFFER_BARRIER_BIT));

                    GL(glDisableVertexAttribArray(0));
                    GL(glDisableVertexAttribArray(1));
                    GL(glDisableVertexAttribArray(2));
                    GL(glDisableVertexAttribArray(3));
                    GL(glDisableVertexAttribArray(4));
                    GL(glDisableVertexAttribArray(5));
                } break;
            }
        }
    }





    //
    // Draw
    //
#if 1
    GL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
    GL(glViewport(0, 0, win_w, win_h));

    GL(glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE));

    GL(glEnable(GL_BLEND));
    GL(glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA));

    GL(glEnable(GL_SCISSOR_TEST));

    GL(glEnable(GL_DEPTH_TEST));
    GL(glClear(GL_COLOR_BUFFER_BIT));
    GL(glClearDepth(1.0f));
    GL(glClear(GL_DEPTH_BUFFER_BIT));
    GL(glDepthFunc(GL_LEQUAL));

    GL(glDisable(GL_MULTISAMPLE));

    GL(glEnable(GL_CULL_FACE));
    GL(glCullFace(GL_BACK));
    GL(glFrontFace(GL_CCW));

  #if 0
    int DEBUG_DRAW_MODE = VV;

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
                        case VV:
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
                            glUniform1ui(program->octree_level, OCTREE_LEVEL);
                            glUniform1ui(program->octree_resolution, gl.octree_resolution);
                            glUniform1ui(program->DEBUG_level, batch->DEBUG_voxel_level);

                            glBindImageTexture(0, gl.octree_nodes_texture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32UI);
                            glBindImageTexture(1, gl.octree_diffuse_texture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32UI);

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
  #else
    GL(glBindBuffer(GL_ARRAY_BUFFER, gl.vbo));

    Defer_Program *dp = &gl.defer_program;
    s32 dpid = dp->id;
    GL(glUseProgram(dpid));

    // G-Buffer
    GL(glActiveTexture(GL_TEXTURE1));
    GL(glBindTexture(GL_TEXTURE_2D, gb->Pid));
    GL(glActiveTexture(GL_TEXTURE2));
    GL(glBindTexture(GL_TEXTURE_2D, gb->Nid));
    GL(glActiveTexture(GL_TEXTURE3));
    GL(glBindTexture(GL_TEXTURE_2D, gb->Cid));
    GL(glActiveTexture(GL_TEXTURE0));

    GL(glBindImageTexture(0, gl.octree_nodes_texture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32UI));
    GL(glBindImageTexture(1, gl.octree_diffuse_texture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32UI));

    GL(glUniformMatrix4fv(dp->voxel_P, 1, GL_TRUE, &voxelize_clip_P.e[0][0]));
    GL(glUniform3fv(dp->cam_P, 1, (GLfloat *)&cam_P));
    GL(glUniform3fv(dp->DEBUG_light_P, 1, (GLfloat *)&light_P));
    GL(glUniform3fv(dp->DEBUG_light_color, 1, (GLfloat *)&light_color));
    GL(glUniform1f(dp->DEBUG_light_strength, light_strength));
    GL(glUniform1f(dp->voxel_in_meter, voxel_in_meter));
    GL(glUniform1ui(dp->octree_level, OCTREE_LEVEL));
    GL(glUniform1ui(dp->octree_resolution, gl.octree_resolution));

    // Draw full quad on the screen.
    f32 vertices[] = { // P, UV
         1.0f, -1.0f, 0.0f,      1.0f,  0.0f,
         1.0f,  1.0f, 0.0f,      1.0f,  1.0f,
        -1.0f, -1.0f, 0.0f,      0.0f,  0.0f,
        -1.0f,  1.0f, 0.0f,      0.0f,  1.0f,
    };
    
    GL(glEnableVertexAttribArray(0));
    GL(glEnableVertexAttribArray(2));

    GL(glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(f32) * 5, (GLvoid *)(0)));
    GL(glVertexAttribPointer(2, 2, GL_FLOAT, false, sizeof(f32) * 5, (GLvoid *)(sizeof(f32) * 3)));

    GL(glBufferData(GL_ARRAY_BUFFER,
                    sizeof(f32) * array_count(vertices),
                    vertices,
                    GL_DYNAMIC_DRAW));
    GL(glDrawArrays(GL_TRIANGLE_STRIP, 0, 4));
    GL(glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT));

    GL(glDisableVertexAttribArray(0));
    GL(glDisableVertexAttribArray(2));


    // Draw else... overlay..
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
  #endif
#endif
            
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
        GL(glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS));
        GL(glDebugMessageCallbackARB(gl_debug_callback, 0));
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
    const char *voxelization_gs = 
        #include "shader/voxelization.gs"
    const char *voxelization_fs = 
        #include "shader/voxelization.fs"

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

    const char *gbuffer_vs = 
        #include "shader/gbuffer.vs"
    const char *gbuffer_fs = 
        #include "shader/gbuffer.fs"

    const char *flag_cs = 
        #include "shader/flag.comp"
    const char *alloc_cs = 
        #include "shader/alloc.comp"
    const char *init_cs = 
        #include "shader/init.comp"
    const char *octree_cs = 
        #include "shader/octree.comp"
    const char *flag_leaf_cs = 
        #include "shader/flag_leaf.comp"
    const char *mipmap_cs = 
        #include "shader/mipmap.comp"

    const char *defer_vs = 
        #include "shader/defer.vs"
    const char *defer_fs = 
        #include "shader/defer.fs"




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
    GET_UNIFORM_LOCATION(mesh_program, octree_resolution);
    GET_UNIFORM_LOCATION(mesh_program, DEBUG_light_P);
    GET_UNIFORM_LOCATION(mesh_program, DEBUG_light_color);
    GET_UNIFORM_LOCATION(mesh_program, DEBUG_light_strength);

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
    GET_UNIFORM_LOCATION(voxelization_program, voxel_P);
    GET_UNIFORM_LOCATION(voxelization_program, is_skeletal);
    GET_UNIFORM_LOCATION(voxelization_program, bone_transforms);
    GET_UNIFORM_LOCATION(voxelization_program, voxel_map);
    GET_UNIFORM_LOCATION(voxelization_program, octree_level);
    GET_UNIFORM_LOCATION(voxelization_program, octree_resolution);
    GET_UNIFORM_LOCATION(voxelization_program, write);
    GET_UNIFORM_LOCATION(voxelization_program, ambient);
    GET_UNIFORM_LOCATION(voxelization_program, diffuse);
    GET_UNIFORM_LOCATION(voxelization_program, specular);
    GET_UNIFORM_LOCATION(voxelization_program, DEBUG_light_P);
    GET_UNIFORM_LOCATION(voxelization_program, DEBUG_light_color);
    GET_UNIFORM_LOCATION(voxelization_program, DEBUG_light_strength);

    gl.voxel_program.id = gl_create_program(header, voxel_vs, voxel_fs);
    GET_UNIFORM_LOCATION(voxel_program, world_transform);
    GET_UNIFORM_LOCATION(voxel_program, V);
    GET_UNIFORM_LOCATION(voxel_program, persp_P);
    GET_UNIFORM_LOCATION(voxel_program, ortho_P);
    GET_UNIFORM_LOCATION(voxel_program, is_skeletal);
    GET_UNIFORM_LOCATION(voxel_program, bone_transforms);
    GET_UNIFORM_LOCATION(voxel_program, octree_level);
    GET_UNIFORM_LOCATION(voxel_program, octree_resolution);
    GET_UNIFORM_LOCATION(voxel_program, DEBUG_level);

    gl.gbuffer_program.id = gl_create_program(header, gbuffer_vs, gbuffer_fs);
    GET_UNIFORM_LOCATION(gbuffer_program, world_transform);
    GET_UNIFORM_LOCATION(gbuffer_program, VP);
    GET_UNIFORM_LOCATION(gbuffer_program, is_skeletal);
    GET_UNIFORM_LOCATION(gbuffer_program, bone_transforms);
    GET_UNIFORM_LOCATION(gbuffer_program, ambient);
    GET_UNIFORM_LOCATION(gbuffer_program, diffuse);
    GET_UNIFORM_LOCATION(gbuffer_program, specular);

    gl.flag_program.id = gl_create_compute_program(header, flag_cs);
    GET_UNIFORM_LOCATION(flag_program, current_level);
    GET_UNIFORM_LOCATION(flag_program, octree_level);
    GET_UNIFORM_LOCATION(flag_program, octree_resolution);
    GET_UNIFORM_LOCATION(flag_program, fragment_count);

    gl.alloc_program.id = gl_create_compute_program(header, alloc_cs);
    GET_UNIFORM_LOCATION(alloc_program, start);
    GET_UNIFORM_LOCATION(alloc_program, alloc_size);

    gl.init_program.id = gl_create_compute_program(header, init_cs);
    GET_UNIFORM_LOCATION(init_program, start);
    GET_UNIFORM_LOCATION(init_program, alloc_size);

    gl.octree_program.id = gl_create_compute_program(header, octree_cs);
    GET_UNIFORM_LOCATION(octree_program, octree_level);
    GET_UNIFORM_LOCATION(octree_program, octree_resolution);
    GET_UNIFORM_LOCATION(octree_program, fragment_count);

    gl.flag_leaf_program.id = gl_create_compute_program(header, flag_leaf_cs);
    GET_UNIFORM_LOCATION(flag_leaf_program, leaf_start);
    GET_UNIFORM_LOCATION(flag_leaf_program, leaf_end);

    gl.mipmap_program.id = gl_create_compute_program(header, mipmap_cs);
    GET_UNIFORM_LOCATION(mipmap_program, level_start);
    GET_UNIFORM_LOCATION(mipmap_program, level_end);

    gl.defer_program.id = gl_create_program(header, defer_vs, defer_fs);
    GET_UNIFORM_LOCATION(defer_program, gP);
    GET_UNIFORM_LOCATION(defer_program, gN);
    GET_UNIFORM_LOCATION(defer_program, gC);
    GET_UNIFORM_LOCATION(defer_program, voxel_P);
    GET_UNIFORM_LOCATION(defer_program, voxel_in_meter);
    GET_UNIFORM_LOCATION(defer_program, octree_level);
    GET_UNIFORM_LOCATION(defer_program, octree_resolution);
    GET_UNIFORM_LOCATION(defer_program, cam_P);
    GET_UNIFORM_LOCATION(defer_program, DEBUG_light_P);
    GET_UNIFORM_LOCATION(defer_program, DEBUG_light_color);
    GET_UNIFORM_LOCATION(defer_program, DEBUG_light_strength);



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


    // Experiment
    GL(glGenBuffers(1, &gl.grass_vbo));
    GL(glGenBuffers(1, &gl.star_vbo));

    //
    // Sparse Voxel Octree
    //
    gl.octree_resolution = (1 << OCTREE_LEVEL);
    GL(glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &gl.max_compute_work_group_count[0]));
    GL(glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &gl.max_compute_work_group_count[1]));
    GL(glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &gl.max_compute_work_group_count[2]));

    // Atomic Fragment List Counter
    u32 zero = 0;
    GL(glGenBuffers(1, &gl.fragment_counter));
    GL(glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, gl.fragment_counter));
    GL(glBufferStorage(GL_ATOMIC_COUNTER_BUFFER, sizeof(u32), &zero, GL_MAP_READ_BIT|GL_MAP_WRITE_BIT));
    GL(glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0));

    // Atomic Node Alloc Counter
    GL(glGenBuffers(1, &gl.alloc_count));
    GL(glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, gl.alloc_count));
    GL(glBufferData(GL_ATOMIC_COUNTER_BUFFER, sizeof(u32), &zero, GL_DYNAMIC_DRAW));
    GL(glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0));

    // Fragment Lists
    u32 max_cap = 8;
    for (u32 level = 1, tmp = 8;
         level <= OCTREE_LEVEL;
         ++level, tmp <<= 3)
    {
        max_cap += tmp;
    }
    gl_gen_linear_buffer(&gl.octree_nodes, &gl.octree_nodes_texture, GL_R32UI, sizeof(u32) * max_cap);


    //
    // Dummy
    //
    GL(glGenVertexArrays(1, &gl.vao));
    GL(glBindVertexArray(gl.vao));

    GL(glGenBuffers(1, &gl.vbo));

    GL(glGenBuffers(1, &gl.vio));
    GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gl.vio));
    
    //DEBUG
    u32 res = gl.octree_resolution;
}
