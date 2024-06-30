/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Sung Woo Lee $
   $Notice: (C) Copyright %s by Sung Woo Lee. All Rights Reserved. $
   ======================================================================== */
/*
 * TODO:
 * mouse half transition counts
 */
#include <windows.h>
#include <Xinput.h>
#include <gl/GL.h>

#include <math.h>
#include <stdio.h>

#include "types.h"
#include "game.h"
#include "win32.h"

global_var bool                 g_running = true;
global_var Win32ScreenBuffer    g_screen_buffer;
global_var f64                  g_counter_hz;
global_var b32                  g_show_cursor;
global_var WINDOWPLACEMENT      g_wpPrev = { sizeof(g_wpPrev) };

#include "opengl.cpp"

#define XINPUT_GET_STATE(Name) DWORD Name(DWORD, XINPUT_STATE *)
typedef XINPUT_GET_STATE(XInput_Get_State);
XINPUT_GET_STATE(xinput_get_state_stub) { return 1; }
XInput_Get_State *xinput_get_state = xinput_get_state_stub;

#define XINPUT_SET_STATE(name) DWORD name(DWORD, XINPUT_VIBRATION *)
typedef XINPUT_SET_STATE(XInput_Set_State);
XINPUT_SET_STATE(xinput_set_state_stub) { return 1; }
XInput_Set_State *xinput_set_state = xinput_set_state_stub;

internal void
win32_load_xinput() 
{
    HMODULE xinput_module = LoadLibraryA("xinput1_4.dll");
    if (!xinput_module) 
    {
        LoadLibraryA("Xinput9_1_0.dll"); 
    }
    if (!xinput_module) 
    {
        LoadLibraryA("xinput1_3.dll"); 
    }

    if(xinput_module) 
    {
        xinput_get_state = (XInput_Get_State *)GetProcAddress(xinput_module, "XInputGetState");
        xinput_set_state = (XInput_Set_State *)GetProcAddress(xinput_module, "XInputSetState");
    } 
    else 
    {
        xinput_get_state = xinput_get_state_stub;
        xinput_set_state = xinput_set_state_stub;
    }
}

internal inline size_t
str_len(const char *str) 
{
    size_t result = 0;
    while (str[result] != '\0') 
    {
        ++result; 
    }
    return result;
}

inline b32
is_whitespace(char c) 
{
    b32 result = ( (c == ' ')  ||
                   (c == '\t') ||
                   (c == '\v') ||
                   (c == '\n') ||
                   (c == '\f') ||
                   (c == '\r') ||
                   (c == 0) );
    return result;
}

internal b32
str_match(char *text, char *pattern, size_t len) 
{
    b32 found = true;
    if (len > 0) 
    {
        for (size_t i = 0; i < len; ++i) 
        {
            if (text[i] != pattern[i]) 
            {
                found = false;
                break;
            }
        }
    } 
    else 
    {
        found = false;
    }
    return found;
}

internal void *
str_find(char *text, size_t t_len, char *pattern, size_t p_len) 
{
    void *result = 0;

    if (t_len >= p_len) 
    {
        u8 *t = (u8 *)text;
        u8 *p = (u8 *)pattern;
        for (u8 *t_base = t;
             t_base < t + t_len;
             ++t_base) 
        {
            u8 *t_at = t_base;
            for (u8 *p_at = p;
                 p_at < p + p_len && *p_at == *t_at;
                 ++p_at, ++t_at) 
            {
                if (p_at == p + p_len - 1) 
                {
                    result = t_base;
                    break;
                }
            }
            if (result) 
            {
                break;
            }
        }
    }

    return result;
}

internal void
win32_set_pixel_format(HDC dc) 
{
    PIXELFORMATDESCRIPTOR desiredPixelFormat = {};
    desiredPixelFormat.nSize            = sizeof(desiredPixelFormat);
    desiredPixelFormat.nVersion         = 1;
    desiredPixelFormat.dwFlags          = PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW | PFD_DOUBLEBUFFER;
    desiredPixelFormat.iPixelType       = PFD_TYPE_RGBA;
    desiredPixelFormat.cColorBits       = 32;
    desiredPixelFormat.cAlphaBits       = 8;
    desiredPixelFormat.cDepthBits       = 24;
    desiredPixelFormat.iLayerType       = PFD_MAIN_PLANE;

    int suggestedPixelFormatIndex = ChoosePixelFormat(dc, &desiredPixelFormat);
    PIXELFORMATDESCRIPTOR suggestedPixelFormat;
    DescribePixelFormat(dc, suggestedPixelFormatIndex, sizeof(suggestedPixelFormat), &suggestedPixelFormat);

    if (wglChoosePixelFormatARB)
    {
        // TODO: robust msaa init.
        Assert(gl_info.has_ext[GL_ARB_multisample]);

        s32 suggested_pixel_format_idx;
        u32 format_count = 1;
        u32 res;
        f32 attribf[] = { 0, 0 };
        s32 attribi[] = { 
            WGL_SUPPORT_OPENGL_ARB,             GL_TRUE,
            WGL_DRAW_TO_WINDOW_ARB,             GL_TRUE,
            WGL_DOUBLE_BUFFER_ARB,              GL_TRUE,
            WGL_ACCELERATION_ARB,               WGL_FULL_ACCELERATION_ARB,
            WGL_PIXEL_TYPE_ARB,                 WGL_TYPE_RGBA_ARB,
            WGL_COLOR_BITS_ARB,                 32,
            WGL_RED_BITS_ARB,                   8,
            WGL_GREEN_BITS_ARB,                 8,
            WGL_BLUE_BITS_ARB,                  8,
            WGL_ALPHA_BITS_ARB,                 8,
            WGL_DEPTH_BITS_ARB,                 24,
            WGL_STENCIL_BITS_ARB,               0,

            WGL_FRAMEBUFFER_SRGB_CAPABLE_ARB,   GL_TRUE,

            WGL_SAMPLE_BUFFERS_ARB, GL_TRUE,
            WGL_SAMPLES_ARB,        8,
            0, 0 };


        Assert(wglChoosePixelFormatARB(dc, attribi, attribf,
                                       format_count, &suggested_pixel_format_idx,
                                       &res));
        SetPixelFormat(dc, suggested_pixel_format_idx, &suggestedPixelFormat);
    }
    else
    {
        SetPixelFormat(dc, suggestedPixelFormatIndex, &suggestedPixelFormat);
    }
}

internal void
win32_load_gl_extensions() 
{
    WNDCLASSA wclass = {};
    wclass.lpfnWndProc     = DefWindowProcA;
    wclass.hInstance       = GetModuleHandleA(0);
    wclass.lpszClassName   = "WGL_Loader";

    if (RegisterClassA(&wclass)) 
    {
        HWND win = CreateWindowExA(0,
                                   wclass.lpszClassName,
                                   "WGL_Loader",
                                   0,
                                   CW_USEDEFAULT,
                                   CW_USEDEFAULT,
                                   CW_USEDEFAULT,
                                   CW_USEDEFAULT,
                                   0,
                                   0,
                                   wclass.hInstance,
                                   0);
        HDC dc = GetDC(win);
        win32_set_pixel_format(dc);

        HGLRC glrc = wglCreateContext(dc);
        if (wglMakeCurrent(dc, glrc)) 
        {
            WGL_GET_PROC_ADDRESS(wglCreateContextAttribsARB);
            WGL_GET_PROC_ADDRESS(wglSwapIntervalEXT);
            WGL_GET_PROC_ADDRESS(wglChoosePixelFormatARB);

            // @wgl_get_proc
            WGL_GET_PROC_ADDRESS(glCreateShader);
            WGL_GET_PROC_ADDRESS(glShaderSource);
            WGL_GET_PROC_ADDRESS(glCompileShader);
            WGL_GET_PROC_ADDRESS(glCreateProgram);
            WGL_GET_PROC_ADDRESS(glAttachShader);
            WGL_GET_PROC_ADDRESS(glLinkProgram);
            WGL_GET_PROC_ADDRESS(glGetProgramiv);
            WGL_GET_PROC_ADDRESS(glGetShaderInfoLog);
            WGL_GET_PROC_ADDRESS(glValidateProgram);
            WGL_GET_PROC_ADDRESS(glGetProgramInfoLog);
            WGL_GET_PROC_ADDRESS(glGenBuffers);
            WGL_GET_PROC_ADDRESS(glBindBuffer);
            WGL_GET_PROC_ADDRESS(glUniformMatrix4fv);
            WGL_GET_PROC_ADDRESS(glGetUniformLocation);
            WGL_GET_PROC_ADDRESS(glUseProgram);
            WGL_GET_PROC_ADDRESS(glUniform1i);
            WGL_GET_PROC_ADDRESS(glBufferData);
            WGL_GET_PROC_ADDRESS(glVertexAttribPointer);
            WGL_GET_PROC_ADDRESS(glGetAttribLocation);
            WGL_GET_PROC_ADDRESS(glEnableVertexAttribArray);
            WGL_GET_PROC_ADDRESS(glGenVertexArrays);
            WGL_GET_PROC_ADDRESS(glBindVertexArray);
            WGL_GET_PROC_ADDRESS(glBindAttribLocation);
            WGL_GET_PROC_ADDRESS(glDebugMessageCallbackARB);
            WGL_GET_PROC_ADDRESS(glDisableVertexAttribArray);
            WGL_GET_PROC_ADDRESS(glUniform3fv);
            WGL_GET_PROC_ADDRESS(glVertexAttribIPointer);
            WGL_GET_PROC_ADDRESS(glUniform4fv);
            WGL_GET_PROC_ADDRESS(glVertexAttribDivisor);
            WGL_GET_PROC_ADDRESS(glDrawElementsInstanced);
            WGL_GET_PROC_ADDRESS(glUniform1f);
            WGL_GET_PROC_ADDRESS(glUniform1fv);

            gl_init_info();

            wglMakeCurrent(0, 0);
        }

        wglDeleteContext(glrc);
        ReleaseDC(win, dc);
        DestroyWindow(win);
    }
}


internal void
win32_init_opengl(HDC dc) 
{
    win32_load_gl_extensions();
    win32_set_pixel_format(dc);

    // create context.
    gl_info.modern = true;

const int context_attrib_list[] = 
{
    WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
    WGL_CONTEXT_MINOR_VERSION_ARB, 3,
    WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB
#if __INTERNAL
        | WGL_CONTEXT_DEBUG_BIT_ARB
#endif
        ,
    //WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB,
    WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
    0,
};

    HGLRC glrc = 0;
    if (wglCreateContextAttribsARB) 
    {
        glrc = wglCreateContextAttribsARB(dc, 0, context_attrib_list);
    }
    if (!glrc) 
    {
        glrc = wglCreateContext(dc);
        gl_info.modern = false;
    }

    // associate with the thread.
    if (wglMakeCurrent(dc, glrc)) 
    {

        // v-sync.
        if (wglSwapIntervalEXT) 
        {
            wglSwapIntervalEXT(1);
        }

        // sRGB.
        if (gl_info.has_ext[GL_EXT_texture_sRGB]) 
        {
            gl_info.texture_internal_format = GL_SRGB8_ALPHA8;
        }
        else
        {
            gl_info.texture_internal_format = GL_RGBA8;
        }

        if (gl_info.has_ext[GL_EXT_framebuffer_sRGB])
        {
            glEnable(GL_FRAMEBUFFER_SRGB);
            gl_info.is_framebuffer_srgb = 1;
        }
        else
        {
            gl_info.is_framebuffer_srgb = 0;
        }

        gl_init();

    }
}

internal void 
win32_toggle_fullscreen(HWND window) 
{
    DWORD style = GetWindowLong(window, GWL_STYLE);
    if (style & WS_OVERLAPPEDWINDOW) 
    {
        MONITORINFO mi = { sizeof(mi) };
        if (GetWindowPlacement(window, &g_wpPrev) &&
            GetMonitorInfo(MonitorFromWindow(window, MONITOR_DEFAULTTOPRIMARY), &mi)) 
        {
            SetWindowLong(window, GWL_STYLE,
                          style & ~WS_OVERLAPPEDWINDOW);
            SetWindowPos(window, HWND_TOP,
                         mi.rcMonitor.left, mi.rcMonitor.top,
                         mi.rcMonitor.right - mi.rcMonitor.left,
                         mi.rcMonitor.bottom - mi.rcMonitor.top,
                         SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
        }
    } 
    else 
    {
        SetWindowLong(window, GWL_STYLE,
                      style | WS_OVERLAPPEDWINDOW);
        SetWindowPlacement(window, &g_wpPrev);
        SetWindowPos(window, 0, 0, 0, 0, 0,
                     SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER |
                     SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
    }
}

internal Win32_Window_Dimension 
win32_get_window_dimension(HWND hwnd) 
{
    Win32_Window_Dimension result = {};
    RECT rect;
    GetWindowRect(hwnd, &rect);
    result.width = rect.right - rect.left;
    result.height = rect.bottom - rect.top;

    return result;
}

internal void
win32_resize_dib_section(Win32ScreenBuffer *screen_buffer, int width, int height) 
{
    if (screen_buffer->memory) 
    {
        VirtualFree(g_screen_buffer.memory, 0, MEM_RELEASE);
    }

    screen_buffer->width = width;
    screen_buffer->height = height;
    screen_buffer->bpp = 4;

    BITMAPINFOHEADER *header = &screen_buffer->bitmap_info.bmiHeader;
    header->biWidth = width;
    header->biHeight = height;
    header->biSize = sizeof(screen_buffer->bitmap_info.bmiHeader);
    header->biWidth = width;
    header->biHeight = -height;
    header->biPlanes = 1;
    header->biBitCount = 32;
    header->biCompression = BI_RGB;

    screen_buffer->memory = VirtualAlloc(0, width * height * screen_buffer->bpp,
                                         MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
}

internal void 
win32_update_screen(HDC hdc, int windowWidth, int windowHeight) 
{
    // NOTE: For debugging purpose
#if 0
    StretchDIBits(hdc,
#if 0
                  0, 0, g_screen_buffer.width, g_screen_buffer.height,
#else
                  0, 0, windowWidth, windowHeight,
#endif
                  0, 0, g_screen_buffer.width, g_screen_buffer.height,
                  g_screen_buffer.memory, &g_screen_buffer.bitmap_info, DIB_RGB_COLORS, SRCCOPY);
#endif
}

inline LARGE_INTEGER
win32_get_wall_clock() 
{
    LARGE_INTEGER result;
    QueryPerformanceCounter(&result);

    return result;
}

inline f32
win32_get_wall_clock_seconds()
{
    f32 result = (f32)((f64)win32_get_wall_clock().QuadPart / (f64)g_counter_hz);
    return result;
}

inline f32
win32_get_seconds_elapsed(LARGE_INTEGER begin, LARGE_INTEGER end) 
{
    f32 result = (f32)(end.QuadPart - begin.QuadPart) / (f32)g_counter_hz;
    return result;
}

inline f32
win32_get_elapsed_ms(LARGE_INTEGER begin, LARGE_INTEGER end) 
{
    f32 result = win32_get_seconds_elapsed(begin, end) * 1000.0f;
    return result;
}

internal FILETIME
win32_get_file_time(LPCSTR filename) 
{
    FILETIME result = {};
    WIN32_FIND_DATA find_data;
    FindFirstFileA(filename, &find_data);
    result = find_data.ftLastWriteTime;

    return result;
}

internal void
win32_concat_str(char *dst,
                 char *src1, size_t len1,
                 const char *src2, size_t len2) 
{
    char *at = dst;
    for (int i = 0; i < len1; i++) { *at++ = src1[i]; }
    for (int i = 0; i < len2; i++) { *at++ = src2[i]; }
    *at = '\0';
}


internal void
win32_xinput_handle_deadzone(XINPUT_STATE *state) 
{
#define XINPUT_DEAD_ZONE 2500
    f32 lx = state->Gamepad.sThumbLX;
    f32 ly = state->Gamepad.sThumbLY;
    if (sqrt(lx * lx + ly * ly) < XINPUT_DEAD_ZONE) {
        state->Gamepad.sThumbLX = 0;
        state->Gamepad.sThumbLY = 0;
    }
    f32 rx = state->Gamepad.sThumbRX;
    f32 ry = state->Gamepad.sThumbRY;
    if (sqrt(rx * rx + ry * ry) < XINPUT_DEAD_ZONE) {
        state->Gamepad.sThumbRX = 0;
        state->Gamepad.sThumbRY = 0;
    }
}

DEBUG_PLATFORM_FREE_MEMORY(DebugPlatformFreeMemory) 
{
    if (memory) {
        VirtualFree(memory, 0, MEM_RELEASE); 
    }
}

PLATFORM_READ_ENTIRE_FILE(DebugPlatformReadEntireFile) 
{
    Entire_File result = {};

    HANDLE file = CreateFileA(filename, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
    if (file != INVALID_HANDLE_VALUE) 
    {
        LARGE_INTEGER filesize;
        if (GetFileSizeEx(file, &filesize)) 
        {
            uint32 filesize32 = (uint32)(filesize.QuadPart);
            result.contents = VirtualAlloc(0, filesize32, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
            if (result.contents) 
            {
                DWORD BytesRead;
                if (ReadFile(file, result.contents, filesize32, &BytesRead, 0) &&
                    (filesize32 == BytesRead)) 
                {
                    result.content_size = filesize32;
                } 
                else 
                {
                    DebugPlatformFreeMemory(result.contents);
                    result.contents = 0;
                }
            } 
            else 
            {

            }
        } 
        else 
        {

        }
        CloseHandle(file);
    }
    else 
    {

    }

    return result;
}

DEBUG_PLATFORM_WRITE_FILE(DebugPlatformWriteEntireFile) 
{
    bool32 result = false;

    HANDLE file = CreateFileA(filename, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, 0, 0);
    if(file != INVALID_HANDLE_VALUE) {
        DWORD bytes_written;
        if(WriteFile(file, contents, size, &bytes_written, 0) &&
           bytes_written == size) {
            result = 1;
        } else {

        }

        CloseHandle(file);
    } else {

    }

    return result;
}

internal void
Win32BeginRecordingInput(Win32State *win32_state) 
{
    win32_state->is_recording = 1;
    const char *filename = "inputNstate.rec";
    win32_state->record_file = CreateFileA(filename, GENERIC_WRITE,
                                           0, 0, CREATE_ALWAYS, 0, 0);
    DWORD bytes_written;
    DWORD bytes_to_write = (DWORD)win32_state->game_mem_total_cap;
    Assert(bytes_to_write == win32_state->game_mem_total_cap);
    WriteFile(win32_state->record_file, win32_state->game_memory, 
              (DWORD)win32_state->game_mem_total_cap, &bytes_written, 0);
}

internal void
Win32RecordInput(Win32State *win32_state, Game_Input *game_input) 
{
    DWORD bytes_written;
    WriteFile(win32_state->record_file, game_input, sizeof(*game_input),
              &bytes_written, 0);
}

internal void
Win32EndInputRecording(Win32State *win32_state) 
{
    CloseHandle(win32_state->record_file);
    win32_state->is_recording = 0;
}

internal void
Win32BeginInputPlayback(Win32State *win32_state) 
{
    win32_state->is_playing = 1;

    const char *filename = "inputNstate.rec";
    win32_state->record_file = CreateFileA(filename, GENERIC_READ,
                                           FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);

    DWORD bytes_read;
    ReadFile(win32_state->record_file, win32_state->game_memory,
             (DWORD)win32_state->game_mem_total_cap, &bytes_read, 0);
}

internal void
Win32EndInputPlayback(Win32State *win32_state) 
{
    CloseHandle(win32_state->record_file);
    win32_state->is_playing = 0;
}

internal void
Win32PlaybackInput(Win32State *win32_state, Game_Input *game_input) 
{
    DWORD bytes_read;
    if (ReadFile(win32_state->record_file, game_input,
                 sizeof(*game_input), &bytes_read, 0)) {
        if (bytes_read == 0) {
            Win32EndInputPlayback(win32_state);
            Win32BeginInputPlayback(win32_state);
        }
    }
}

internal void
win32_process_keyboard(Game_Key *game_key, b32 is_down) 
{
    if (is_down) { game_key->is_set = true; } 
    else { game_key->is_set = false; }
}

internal void
win32_get_mouse_pos_to_game_coord(HWND hwnd, Win32_Window_Dimension wd, Game_Input *game_input) 
{
    f32 aspect_h_over_w = safe_ratio((f32)wd.height, (f32)wd.width);
    POINT mouse_p;
    GetCursorPos(&mouse_p);
    ScreenToClient(hwnd, &mouse_p);
#if 0
    game_input->mouse.P.x = safe_ratio( 2.0f * (f32)mouse_p.x, (f32)wd.width) - 1.0f;
    game_input->mouse.P.y = (f32)mouse_p.y; //safe_ratio(-2.0f * (f32)mouse_p.y * aspect_h_over_w, (f32)wd.height) + aspect_h_over_w;
#endif
    game_input->mouse.P.x = (f32)mouse_p.x;
    game_input->mouse.P.y = (f32)(wd.height - mouse_p.y - 1);
}

internal void
win32_process_mouse_click(s32 vk, Mouse_Input *mouse) 
{
    b32 is_down = GetKeyState(vk) & (1 << 15);
    u32 E = 0;

    switch(vk) {
        case VK_LBUTTON: {
            E = eMouse_Left;
        } break;
        case VK_MBUTTON: {
            E = eMouse_Middle;
        } break;
        case VK_RBUTTON: {
            E = eMouse_Right;
        } break;

        INVALID_DEFAULT_CASE
    }

    if (is_down) {
        if (!mouse->is_down[E]) {
            mouse->toggle[E] = true;
            mouse->click_p[E] = mouse->P;
        } else {
            mouse->toggle[E] = false;
        }
    } else {
        if (mouse->is_down[E]) {
            mouse->toggle[E] = true;
        } else {
            mouse->toggle[E] = false;
        }
    }
    mouse->is_down[E] = is_down;
}

internal LRESULT 
win32_window_callback(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) 
{
    LRESULT result = 0;

    switch(msg) 
    {
        case WM_ACTIVATEAPP : 
        {

        } break;

        case WM_CLOSE: 
        {
            g_running = false;
        } break;

        case WM_DESTROY: 
        {
            g_running = false;
        } break;

        case WM_PAINT: 
        {
            PAINTSTRUCT paint;
            HDC hdc = BeginPaint(hwnd, &paint);
            Assert(hdc != 0);
            Win32_Window_Dimension wd = win32_get_window_dimension(hwnd);
            win32_update_screen(hdc, wd.width, wd.height);
            ReleaseDC(hwnd, hdc);
            EndPaint(hwnd, &paint);
        } break;

        case WM_SIZE: 
        {
        } break;

        case WM_SETCURSOR: 
        {
            if (g_show_cursor) {
                result = DefWindowProcA(hwnd, msg, wparam, lparam);
            } else {
                SetCursor(0);
            }
        } break;

        default: 
        {
            result = DefWindowProcA(hwnd, msg, wparam, lparam);
        } break;
    }

    return result;
}

internal void
win32_init_render_batch(Render_Batch *batch, size_t size) 
{
    batch->base = VirtualAlloc(0, size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    batch->size = size;
    batch->used = 0;
}

///////////////////////////////////////////////////////////////////////////////
//
// Multi-Threading
// 

struct Platform_Work_QueueEntry 
{
    Platform_Work_QueueCallback *Callback;
    void *Data;
};

struct Platform_Work_Queue 
{
    u32 volatile CompletionGoal;
    u32 volatile CompletionCount;

    u32 volatile NextEntryToWrite;
    u32 volatile NextEntryToRead;
    HANDLE SemaphoreHandle;

    Platform_Work_QueueEntry Entries[256];
};

internal void
Win32AddEntry(Platform_Work_Queue *Queue, Platform_Work_QueueCallback *Callback, void *Data) 
{
    // TODO: Switch to InterlockedCompareExchange eventually
    // so that any thread can add?
    u32 NewNextEntryToWrite = (Queue->NextEntryToWrite + 1) % array_count(Queue->Entries);
    Assert(NewNextEntryToWrite != Queue->NextEntryToRead);
    Platform_Work_QueueEntry *Entry = Queue->Entries + Queue->NextEntryToWrite;
    Entry->Callback = Callback;    Entry->Data = Data;
    ++Queue->CompletionGoal;
    _WriteBarrier();
    Queue->NextEntryToWrite = NewNextEntryToWrite;
    ReleaseSemaphore(Queue->SemaphoreHandle, 1, 0);
}

internal bool32
Win32DoNextWorkQueueEntry(Platform_Work_Queue *Queue) 
{
    b32 shouldSleep = false;

    u32 OriginalNextEntryToRead = Queue->NextEntryToRead;
    u32 NewNextEntryToRead = (OriginalNextEntryToRead + 1) % array_count(Queue->Entries);
    if (OriginalNextEntryToRead != Queue->NextEntryToWrite)
    {
        u32 Index = InterlockedCompareExchange((LONG volatile *)&Queue->NextEntryToRead,
                                               NewNextEntryToRead,
                                               OriginalNextEntryToRead);
        if(Index == OriginalNextEntryToRead)
        {        
            Platform_Work_QueueEntry Entry = Queue->Entries[Index];
            Entry.Callback(Queue, Entry.Data);
            InterlockedIncrement((LONG volatile *)&Queue->CompletionCount);
        }
    } else {
        shouldSleep = true;
    }

    return shouldSleep;
}

internal void
win32_complete_all_work(Platform_Work_Queue *Queue) 
{
    while(Queue->CompletionGoal != Queue->CompletionCount) 
    {
        Win32DoNextWorkQueueEntry(Queue);
    }

    Queue->CompletionGoal = 0;
    Queue->CompletionCount = 0;
}

DWORD WINAPI
ThreadProc(LPVOID lpParameter) 
{
    Platform_Work_Queue *Queue = (Platform_Work_Queue *)lpParameter;

    for(;;) 
    {
        if (Win32DoNextWorkQueueEntry(Queue)) 
        {
            WaitForSingleObjectEx(Queue->SemaphoreHandle, INFINITE, FALSE);
        }
    }
}

internal void
win32_make_queue(Platform_Work_Queue *Queue, u32 ThreadCount) 
{
    Queue->CompletionGoal = 0;
    Queue->CompletionCount = 0;

    Queue->NextEntryToWrite = 0;
    Queue->NextEntryToRead = 0;

    u32 InitialCount = 0;
    Queue->SemaphoreHandle = CreateSemaphoreEx(0,
                                               InitialCount,
                                               ThreadCount,
                                               0, 0, SEMAPHORE_ALL_ACCESS);
    for(u32 ThreadIndex = 0;
        ThreadIndex < ThreadCount;
        ++ThreadIndex) 
    {
        DWORD ThreadID;
        HANDLE ThreadHandle = CreateThread(0, 0, ThreadProc, Queue, 0, &ThreadID);
        CloseHandle(ThreadHandle);
    }
}

DEBUG_PLATFORM_EXECUTE_SYSTEM_COMMAND(win32_execute_system_command)
{
    Debug_Executing_Process result = {};

    STARTUPINFO startup_info = {};
    startup_info.cb = sizeof(startup_info);
#if 1
    startup_info.dwFlags = STARTF_USESHOWWINDOW;
    startup_info.wShowWindow = SW_HIDE;
#endif

    PROCESS_INFORMATION process_info = {};
    if (CreateProcessA(command,
                       command_line,
                       0, 
                       0,
                       FALSE,
                       0,
                       0,
                       path,
                       &startup_info,
                       &process_info
                      ))
    {
        // Assert(sizeof(result.os_handle) >= sizeof(process_info.hProcess));
        *(HANDLE *)&result.os_handle = process_info.hProcess;
    }
    else
    {
        DWORD error_code = GetLastError();
        *(HANDLE *)&result.os_handle = INVALID_HANDLE_VALUE;
    }

    return result;
}

DEBUG_PLATFORM_GET_PROCESS_STATE(win32_get_process_state)
{
    Debug_Process_State result = {};

    HANDLE hProcess = *(HANDLE *)&process.os_handle;
    if (hProcess != INVALID_HANDLE_VALUE)
    {
        result.started_successfully = true;

        if (WaitForSingleObject(hProcess, 0) == WAIT_OBJECT_0)
        {
            DWORD return_code;
            GetExitCodeProcess(hProcess, &return_code);
            result.return_code = return_code;
            CloseHandle(hProcess);
        }
        else
        {
            result.is_running = true;
        }
    }

    return result;
}

#if __INTERNAL
global_var Debug_Table g_debug_table_;
Debug_Table *g_debug_table = &g_debug_table_;
#endif

int WINAPI
WinMain(HINSTANCE hinst, HINSTANCE deprecated, LPSTR cmd, int show_cmd) 
{
    //
    // Multi-Threading
    //
    Platform_Work_Queue high_priority_queue = {};
    win32_make_queue(&high_priority_queue, 6);

    Platform_Work_Queue low_priority_queue = {};
    win32_make_queue(&low_priority_queue, 2);

    LARGE_INTEGER g_counter_hz_large_integer;
    QueryPerformanceFrequency(&g_counter_hz_large_integer);
    g_counter_hz = (f64)g_counter_hz_large_integer.QuadPart;
    timeBeginPeriod(1);

#if __INTERNAL
    g_show_cursor = true;
#endif

    WNDCLASSA wnd_class = {};
    wnd_class.style             = CS_HREDRAW | CS_VREDRAW;
    wnd_class.lpfnWndProc       = win32_window_callback;
    wnd_class.hInstance         = hinst;
    wnd_class.hCursor           = LoadCursorA(0, IDC_ARROW);
    wnd_class.lpszClassName     = "GameWindowClass";
    RegisterClassA(&wnd_class);

    HWND hwnd = CreateWindowExA(0, wnd_class.lpszClassName, "Game",
                                WS_OVERLAPPEDWINDOW | WS_VISIBLE,
#if 1
                                CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
#else
                                0, 0, 1920, 1080,
#endif
                                0, 0, hinst, 0);
    Assert(hwnd);

    win32_toggle_fullscreen(hwnd);

    Win32_Window_Dimension dim = win32_get_window_dimension(hwnd);
    win32_resize_dib_section(&g_screen_buffer, dim.width, dim.height);

    win32_init_opengl(GetDC(hwnd));

    win32_load_xinput();

    s32 monitor_hz = GetDeviceCaps(GetDC(hwnd), VREFRESH);
    s32 desired_hz = monitor_hz / 2;
    f32 desired_mspf = 1000.0f / (f32)desired_hz;


#if __INTERNAL
    LPVOID base_address = (LPVOID)TB(2);
#else
    LPVOID base_address = 0;
#endif
    Game_Memory game_memory = {};
    Win32State win32_state = {};
    game_memory.permanent_memory_size   = MB(256);
    game_memory.transient_memory_size   = GB(1);
    game_memory.debug_storage_size      = MB(64);

    u64 total_capacity = (game_memory.permanent_memory_size +
                          game_memory.transient_memory_size + 
                          game_memory.debug_storage_size);
    win32_state.game_memory = VirtualAlloc(base_address, (size_t)total_capacity,
                                           MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    win32_state.game_mem_total_cap = total_capacity;
    game_memory.permanent_memory = win32_state.game_memory;
    game_memory.transient_memory = ((u8 *)(game_memory.permanent_memory) + game_memory.permanent_memory_size);
    game_memory.debug_storage = ((u8 *)(game_memory.transient_memory) + game_memory.transient_memory_size);

    game_memory.high_priority_queue = &high_priority_queue;
    game_memory.low_priority_queue = &low_priority_queue;
    game_memory.platform.platform_add_entry = Win32AddEntry;
    game_memory.platform.platform_complete_all_work = win32_complete_all_work;
    game_memory.platform.debug_platform_read_file = DebugPlatformReadEntireFile;
#if __INTERNAL
    game_memory.platform.debug_platform_write_file = DebugPlatformWriteEntireFile;
    game_memory.platform.debug_platform_free_memory = DebugPlatformFreeMemory;
    game_memory.platform.debug_platform_execute_system_command = win32_execute_system_command;
    game_memory.platform.debug_platform_get_process_state = win32_get_process_state;
#endif

    win32_init_render_batch(&game_memory.render_batch, KB(4));

    Game_State *game_state = (Game_State *)game_memory.permanent_memory;

    Game_Screen_Buffer game_screen_buffer = {};
    game_screen_buffer.memory = g_screen_buffer.memory;
    game_screen_buffer.width = g_screen_buffer.width;
    game_screen_buffer.height = g_screen_buffer.height;
    game_screen_buffer.bpp = g_screen_buffer.bpp;
    game_screen_buffer.pitch = g_screen_buffer.bpp * g_screen_buffer.width;

    HMODULE xinput_dll = LoadLibraryA(TEXT("xinput.dll"));
    if (!xinput_dll) 
    {
        // TODO: diagnostic.
    }

    const char *game_dll_filename = "game.dll";
    const char *game_dll_load_filename = "game_load.dll";

    char exe_path_buf[MAX_PATH];
    DWORD exe_path_length = GetModuleFileNameA(0, exe_path_buf, MAX_PATH);
    DWORD last_backslash_idx = 0;
    for(DWORD idx = 0; idx < exe_path_length; idx++) 
    {
        if(exe_path_buf[idx] == '\\') { last_backslash_idx = idx; }
    }

    char game_dll_abs_path[MAX_PATH];
    char game_dll_load_abs_path[MAX_PATH];
    win32_concat_str(game_dll_abs_path, exe_path_buf, last_backslash_idx + 1, game_dll_filename, str_len(game_dll_filename));
    win32_concat_str(game_dll_load_abs_path, exe_path_buf, last_backslash_idx + 1, game_dll_load_filename, str_len(game_dll_load_filename));

    FILETIME game_dll_time_last = {};
    FILETIME game_dll_time = {};

    Win32_Game game = {};
    Game_Input game_input = {};

    //
    // Loop
    //
    if (game_memory.permanent_memory && 
        game_memory.transient_memory)
    {
        while(g_running) 
        {
            LARGE_INTEGER last_counter = win32_get_wall_clock();

            BEGIN_BLOCK(win32_executable_ready);
            game_memory.executable_reloaded = false;
            game_dll_time = win32_get_file_time(game_dll_abs_path);
            if (CompareFileTime(&game_dll_time_last, &game_dll_time) != 0) 
            {
                win32_complete_all_work(&high_priority_queue);
                win32_complete_all_work(&low_priority_queue);

#if __INTERNAL
                g_debug_table = &g_debug_table_;
#endif

                if (game.dll) 
                {
                    FreeLibrary(game.dll); 
                    game.dll = 0;
                }
                CopyFileA(game_dll_abs_path, game_dll_load_abs_path, FALSE);
                game.dll = LoadLibraryA(game_dll_load_filename);
                if (game.dll) 
                { 
                    game.game_update          = (Game_Update *)GetProcAddress(game.dll, "game_update");
                    game.debug_frame_end    = (Debug_Frame_End *)GetProcAddress(game.dll, "debug_frame_end");

                    game.is_valid           = (game.game_update &&
                                               1);
                    if (!game.is_valid)
                    {
                        game.game_update = 0;
                    }
                } 
                else 
                {
                    // TODO: Diagnostic 
                }
                game_dll_time_last = game_dll_time;
                game_memory.executable_reloaded = true;
            }
            END_BLOCK(win32_executable_ready);

            BEGIN_BLOCK(win32_process_input);
            game_input.mouse.wheel_delta = 0;
            MSG msg;
            while (PeekMessageA(&msg, hwnd, 0, 0, PM_REMOVE)) 
            {
                switch(msg.message) 
                {
                    case WM_QUIT: 
                    {
                        g_running = false;
                    } break;
                    case WM_SYSKEYDOWN:
                    case WM_SYSKEYUP:
                    case WM_KEYDOWN:
                    case WM_KEYUP: 
                    {
                        u64 vk_code  = msg.wParam;
                        b32 is_down  = ((msg.lParam & (1 << 31)) == 0);
                        b32 was_down = ((msg.lParam & (1 << 30)) != 0);
                        if (was_down != is_down) 
                        {
                            switch (vk_code) 
                            {
                                // TODO: compressable I guess?
                                case 'Q': 
                                {
                                    win32_process_keyboard(&game_input.Q, is_down);
                                } break;
                                case 'E': 
                                {
                                    win32_process_keyboard(&game_input.E, is_down);
                                } break;
                                case 'W': 
                                {
                                    win32_process_keyboard(&game_input.W, is_down);
                                } break;
                                case 'A': 
                                {
                                    win32_process_keyboard(&game_input.A, is_down);
                                } break;
                                case 'S': 
                                {
                                    win32_process_keyboard(&game_input.S, is_down);
                                } break;
                                case 'D': 
                                {
                                    win32_process_keyboard(&game_input.D, is_down);
                                } break;
                                case VK_MENU: // Alt
                                {
                                    win32_process_keyboard(&game_input.alt, is_down);
                                } break;
#if __INTERNAL
                                case VK_OEM_3: // tilde
                                {
                                    win32_process_keyboard(&game_input.tilde, is_down);
                                } break;
#endif

#if __INTERNAL
                                case 'L': 
                                {
                                    if (is_down) 
                                    {
                                        if (!win32_state.is_recording) 
                                        {
                                            Win32BeginRecordingInput(&win32_state);
                                        } 
                                        else 
                                        {
                                            Win32EndInputRecording(&win32_state);
                                            Win32BeginInputPlayback(&win32_state);
                                        }
                                    }
                                } break;
#endif
                            }

                            if (is_down) 
                            {
                                bool32 altWasDown = (msg.lParam & (1 << 29));
                                if ((vk_code == VK_F4) && altWasDown) 
                                {
                                    g_running = false;
                                }
                                if ((vk_code == VK_RETURN) && altWasDown) 
                                {
                                    if (msg.hwnd) 
                                    {
                                        win32_toggle_fullscreen(msg.hwnd);
                                    }
                                }
                            }

                        }
                    } break;
                    case WM_MOUSEWHEEL: 
                    {
                        s16 z_delta = (GET_WHEEL_DELTA_WPARAM(msg.wParam) / WHEEL_DELTA);
                        game_input.mouse.wheel_delta = z_delta;
                    } break;
                }
                TranslateMessage(&msg);
                DispatchMessageA(&msg);
            }

            Win32_Window_Dimension wd = win32_get_window_dimension(hwnd);
            win32_resize_dib_section(&g_screen_buffer, dim.width, dim.height);

            Mouse_Input *mouse = &game_input.mouse;
            // TODO: we don't need to calculate aspect ratio every frame!
            win32_get_mouse_pos_to_game_coord(hwnd, wd, &game_input);

            // get mouse info via function call, alternative to WM.
            win32_process_mouse_click(VK_LBUTTON, mouse);
            win32_process_mouse_click(VK_MBUTTON, mouse);
            win32_process_mouse_click(VK_RBUTTON, mouse);

            DWORD result;    
            for(DWORD idx = 0;
                idx < XUSER_MAX_COUNT;
                idx++) 
            {
                XINPUT_STATE state;
                ZeroMemory(&state, sizeof(XINPUT_STATE));
                result = xinput_get_state(idx, &state);
                win32_xinput_handle_deadzone(&state);
                if (result == ERROR_SUCCESS) 
                {

                } 
                else 
                {
                    // TODO: Diagnostic
                }
            }
            END_BLOCK(win32_process_input);

            BEGIN_BLOCK(win32_record_input);
            if (win32_state.is_recording) 
            {
                Win32RecordInput(&win32_state, &game_input);
            }
            if (win32_state.is_playing) 
            {
                Win32PlaybackInput(&win32_state, &game_input);
            }
            END_BLOCK(win32_record_input);

#if 0
            game_input.dt_per_frame = (desired_mspf / 1000.0f);
#endif

            BEGIN_BLOCK(win32_game_update);
            if (game.game_update) 
            {
                game.game_update(&game_memory, game_state, &game_input, &game_screen_buffer);
            }
            END_BLOCK(win32_game_update);

            BEGIN_BLOCK(win32_render);
            HDC dc = GetDC(hwnd);
            gl_render_batch(&game_memory.render_batch, wd.width, wd.height);
            SwapBuffers(dc);
            // win32_update_screen(dc, wd.width, wd.height);
            ReleaseDC(hwnd, dc);
            END_BLOCK(win32_render);

            f32 actual_mspf = win32_get_elapsed_ms(last_counter, win32_get_wall_clock());
            game_input.dt_per_frame = (actual_mspf / 1000.0f);

            //
            //
            //

#if __INTERNAL
            BEGIN_BLOCK(win32_debug_collation);
            if (game.debug_frame_end)
            {
                g_debug_table = game.debug_frame_end(&game_memory, &game_screen_buffer, &game_input);
            }
            g_debug_table_.event_array_idx_event_idx = 0;
            END_BLOCK(win32_debug_collation);
#endif

            //
            //
            //

            // TODO: leave this off until we have vblank support?
#if 0
            BEGIN_BLOCK(win32_framerate_wait);
            LARGE_INTEGER counter_end = win32_get_wall_clock();
            f32 actual_mspf = win32_get_elapsed_ms(last_counter, counter_end);

            if (actual_mspf < desired_mspf) 
            {
                DWORD ms_to_sleep = (DWORD)(desired_mspf - actual_mspf);
                if (ms_to_sleep > 0)
                {
                    Sleep(ms_to_sleep);
                }
                actual_mspf = win32_get_elapsed_ms(last_counter, win32_get_wall_clock());
            } 
            else 
            {
                // TODO: Missed framerate handling
            }
            END_BLOCK(win32_framerate_wait);
#endif

            LARGE_INTEGER end_counter = win32_get_wall_clock();
            FRAME_MARKER(win32_get_seconds_elapsed(last_counter, end_counter));
            last_counter = end_counter;

#if __INTERNAL
            if (g_debug_table)
            {
                // TODO: move this to a global variable so that
                // there can be timers below this one?
                g_debug_table->record_count[TRANSLATION_UNIT_IDX] = __COUNTER__;
            }
#endif
        }
    }

    return 0;
}
