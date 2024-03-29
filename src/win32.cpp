/* ―――――――――――――――――――――――――――――――――――◆――――――――――――――――――――――――――――――――――――
   $File: $
   $Date: $
   $Revision: $
   $Creator: Sung Woo Lee $
   $Notice: (C) Copyright 2024 by Sung Woo Lee. All Rights Reserved. $
   ―――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――― */
#include <windows.h>
#include <Xinput.h>
#include <gl/GL.h>

#include <math.h>
#include <stdio.h>

#include "types.h"
#include "game.h"
#include "win32.h"

global_var bool g_running = true;
global_var Win32ScreenBuffer g_screen_buffer;
global_var LARGE_INTEGER g_counter_hz;
global_var b32 g_show_cursor;
global_var WINDOWPLACEMENT g_wpPrev = { sizeof(g_wpPrev) };

#define XINPUT_GET_STATE(NAME) DWORD NAME(DWORD, XINPUT_STATE *)
typedef XINPUT_GET_STATE(__XInputGetState);
XINPUT_GET_STATE(xinput_get_state_stub) { return 1; }
__XInputGetState *xinput_get_state = xinput_get_state_stub;

#define XINPUT_SET_STATE(name) DWORD name(DWORD, XINPUT_VIBRATION *)
typedef XINPUT_SET_STATE(__XInputSetState);
XINPUT_SET_STATE(xinput_set_state_stub) { return 1; }
__XInputSetState *xinput_set_state = xinput_set_state_stub;


internal void
Win32LoadXInput() {
    HMODULE xinput_module = LoadLibraryA("xinput1_4.dll");
    if(!xinput_module) { LoadLibraryA("Xinput9_1_0.dll"); }
    if(!xinput_module) { LoadLibraryA("xinput1_3.dll"); }

    if(xinput_module) {
        xinput_get_state = (__XInputGetState *)GetProcAddress(xinput_module, "XInputGetState");
        xinput_set_state = (__XInputSetState *)GetProcAddress(xinput_module, "XInputSetState");
    } else {
        xinput_get_state = xinput_get_state_stub;
        xinput_set_state = xinput_set_state_stub;
    }
}

internal void
Win32InitOpenGL(HWND window) {
    HDC windowDC = GetDC(window);

    PIXELFORMATDESCRIPTOR desiredPixelFormat = {};
    desiredPixelFormat.nSize = sizeof(desiredPixelFormat);
    desiredPixelFormat.nVersion = 1;
    desiredPixelFormat.dwFlags = PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW | PFD_DOUBLEBUFFER;
    desiredPixelFormat.iPixelType = PFD_TYPE_RGBA;
    desiredPixelFormat.cColorBits = 32;
    desiredPixelFormat.cAlphaBits = 8;
    desiredPixelFormat.iLayerType = PFD_MAIN_PLANE;

    int suggestedPixelFormatIndex = ChoosePixelFormat(windowDC, &desiredPixelFormat);
    PIXELFORMATDESCRIPTOR suggestedPixelFormat;
    DescribePixelFormat(windowDC, suggestedPixelFormatIndex, sizeof(suggestedPixelFormat), &suggestedPixelFormat);
    SetPixelFormat(windowDC, suggestedPixelFormatIndex, &suggestedPixelFormat);

    HGLRC openGLRC = wglCreateContext(windowDC);
    if (wglMakeCurrent(windowDC, openGLRC)) {

    } else {
        INVALID_CODE_PATH;
    }

    ReleaseDC(window, windowDC);
}

internal void 
Win32ToggleFullScreen(HWND window) {
    DWORD style = GetWindowLong(window, GWL_STYLE);
    if (style & WS_OVERLAPPEDWINDOW) {
        MONITORINFO mi = { sizeof(mi) };
        if (GetWindowPlacement(window, &g_wpPrev) &&
                GetMonitorInfo(MonitorFromWindow(window,
                        MONITOR_DEFAULTTOPRIMARY), &mi)) {
            SetWindowLong(window, GWL_STYLE,
                    style & ~WS_OVERLAPPEDWINDOW);
            SetWindowPos(window, HWND_TOP,
                    mi.rcMonitor.left, mi.rcMonitor.top,
                    mi.rcMonitor.right - mi.rcMonitor.left,
                    mi.rcMonitor.bottom - mi.rcMonitor.top,
                    SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
        }
    } else {
        SetWindowLong(window, GWL_STYLE,
                style | WS_OVERLAPPEDWINDOW);
        SetWindowPlacement(window, &g_wpPrev);
        SetWindowPos(window, 0, 0, 0, 0, 0,
                SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER |
                SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
    }
}

internal Win32WindowDimension 
Win32GetWindowDimension(HWND hwnd) {
    Win32WindowDimension result = {};
    RECT rect;
    GetWindowRect(hwnd, &rect);
    result.width = rect.right - rect.left;
    result.height = rect.bottom - rect.top;

    return result;
}

internal void
Win32ResizeDIBSection(Win32ScreenBuffer *screen_buffer, int width, int height) {
    if(screen_buffer->memory) {
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
Win32UpdateScreen(HDC hdc, int windowWidth, int windowHeight) {
    // NOTE: For debugging purpose
    StretchDIBits(hdc,
#if 0
            0, 0, g_screen_buffer.width, g_screen_buffer.height,
#else
            0, 0, windowWidth, windowHeight,
#endif
            0, 0, g_screen_buffer.width, g_screen_buffer.height,
            g_screen_buffer.memory, &g_screen_buffer.bitmap_info, DIB_RGB_COLORS, SRCCOPY);
}

internal inline LARGE_INTEGER
Win32GetClock() {
         LARGE_INTEGER result;
         QueryPerformanceCounter(&result);

         return result;
}

internal inline r32
Win32GetElapsedSec(LARGE_INTEGER begin, LARGE_INTEGER end) {
    r32 result = (r32)(end.QuadPart - begin.QuadPart)
    / (r32)g_counter_hz.QuadPart;

    return result;
}

internal inline r32
Win32GetElapsedMs(LARGE_INTEGER begin, LARGE_INTEGER end) {
    r32 result = Win32GetElapsedSec(begin, end) * 1000.0f;
    return result;
}

internal FILETIME
Win32GetFileTime(LPCSTR filename) {
    FILETIME result = {};
    WIN32_FIND_DATA find_data;
    FindFirstFileA(filename, &find_data);
    result = find_data.ftLastWriteTime;

    return result;
}

internal void
Win32ConcatNString(char *dst,
        char *src1, size_t len1,
        const char *src2, size_t len2) {
    char *at = dst;
    for (int i = 0; i < len1; i++) { *at++ = src1[i]; }
    for (int i = 0; i < len2; i++) { *at++ = src2[i]; }
    *at = '\0';
}

internal inline size_t
StrLen(const char *str) {
    size_t result = 0;
    while (str[result] != '\0') { ++result; }

    return result;
}

internal void
Win32XInputHandleDeadzone(XINPUT_STATE *state) {
#define XINPUT_DEAD_ZONE 2500
    r32 lx = state->Gamepad.sThumbLX;
    r32 ly = state->Gamepad.sThumbLY;
    if (sqrt(lx * lx + ly * ly) < XINPUT_DEAD_ZONE) {
        state->Gamepad.sThumbLX = 0;
        state->Gamepad.sThumbLY = 0;
    }
    r32 rx = state->Gamepad.sThumbRX;
    r32 ry = state->Gamepad.sThumbRY;
    if (sqrt(rx * rx + ry * ry) < XINPUT_DEAD_ZONE) {
        state->Gamepad.sThumbRX = 0;
        state->Gamepad.sThumbRY = 0;
    }
}

DEBUG_PLATFORM_FREE_MEMORY(DebugPlatformFreeMemory) {
    if (memory) {
        VirtualFree(memory, 0, MEM_RELEASE); 
    }
}

DEBUG_PLATFORM_READ_FILE(DebugPlatformReadEntireFile) {
    DebugReadFileResult result = {};

    HANDLE file = CreateFileA(filename, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
    if (file != INVALID_HANDLE_VALUE) {
        LARGE_INTEGER filesize;
        if (GetFileSizeEx(file, &filesize)) {
            uint32 filesize32 = (uint32)(filesize.QuadPart);
            result.contents = VirtualAlloc(0, filesize32, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
            if (result.contents) {
                DWORD BytesRead;
                if (ReadFile(file, result.contents, filesize32, &BytesRead, 0) &&
                        (filesize32 == BytesRead)) {
                    result.content_size = filesize32;
                } else {
                    DebugPlatformFreeMemory(result.contents);
                    result.contents = 0;
                }
            } else {

            }
        } else {

        }
        CloseHandle(file);
    }
    else {

    }

    return result;
}

DEBUG_PLATFORM_WRITE_FILE(DebugPlatformWriteEntireFile) {
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
Win32BeginRecordingInput(Win32State *win32_state) {
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
Win32RecordInput(Win32State *win32_state, GameInput *game_input) {
    DWORD bytes_written;
    WriteFile(win32_state->record_file, game_input, sizeof(*game_input),
            &bytes_written, 0);
}

internal void
Win32EndInputRecording(Win32State *win32_state) {
    CloseHandle(win32_state->record_file);
    win32_state->is_recording = 0;
}

internal void
Win32BeginInputPlayback(Win32State *win32_state) {
    win32_state->is_playing = 1;

    const char *filename = "inputNstate.rec";
    win32_state->record_file = CreateFileA(filename, GENERIC_READ,
            FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);

    DWORD bytes_read;
    ReadFile(win32_state->record_file, win32_state->game_memory,
            (DWORD)win32_state->game_mem_total_cap, &bytes_read, 0);
}

internal void
Win32EndInputPlayback(Win32State *win32_state) {
    CloseHandle(win32_state->record_file);
    win32_state->is_playing = 0;
}

internal void
Win32PlaybackInput(Win32State *win32_state, GameInput *game_input) {
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
Win32ProcessKeyboard(GameKey *game_key, b32 is_down) {
    if (is_down) { game_key->is_set = true; } 
    else { game_key->is_set = false; }
}

internal LRESULT 
Win32WindowCallback(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
    LRESULT result = 0;

    switch(msg) {
        case WM_ACTIVATEAPP : {

        } break;

        case WM_CLOSE: {
            g_running = false;
        } break;

        case WM_DESTROY: {
            g_running = false;
        } break;

        case WM_PAINT: {
            PAINTSTRUCT paint;
            HDC hdc = BeginPaint(hwnd, &paint);
            Assert(hdc != 0);
            Win32WindowDimension wd = Win32GetWindowDimension(hwnd);
            Win32UpdateScreen(hdc, wd.width, wd.height);
            ReleaseDC(hwnd, hdc);
            EndPaint(hwnd, &paint);
        } break;
        
        case WM_SIZE: {
        } break;

        case WM_SETCURSOR: {
            if (g_show_cursor) {
                result = DefWindowProcA(hwnd, msg, wparam, lparam);
            } else {
                SetCursor(0);
            }
        } break;

        default: {
            result = DefWindowProcA(hwnd, msg, wparam, lparam);
        } break;
    }

    return result;
}

///////////////////////////////////////////////////////////////////////////////
//
// Multi-Threading
// 

struct PlatformWorkQueueEntry {
    PlatformWorkQueueCallback *Callback;
    void *Data;
};

struct PlatformWorkQueue {
    u32 volatile CompletionGoal;
    u32 volatile CompletionCount;
    
    u32 volatile NextEntryToWrite;
    u32 volatile NextEntryToRead;
    HANDLE SemaphoreHandle;

    PlatformWorkQueueEntry Entries[256];
};

internal void
Win32AddEntry(PlatformWorkQueue *Queue, PlatformWorkQueueCallback *Callback, void *Data) {
    // TODO: Switch to InterlockedCompareExchange eventually
    // so that any thread can add?
    u32 NewNextEntryToWrite = (Queue->NextEntryToWrite + 1) % ArrayCount(Queue->Entries);
    Assert(NewNextEntryToWrite != Queue->NextEntryToRead);
    PlatformWorkQueueEntry *Entry = Queue->Entries + Queue->NextEntryToWrite;
    Entry->Callback = Callback;    Entry->Data = Data;
    ++Queue->CompletionGoal;
    _WriteBarrier();
    Queue->NextEntryToWrite = NewNextEntryToWrite;
    ReleaseSemaphore(Queue->SemaphoreHandle, 1, 0);
}
internal bool32
Win32DoNextWorkQueueEntry(PlatformWorkQueue *Queue) {
    b32 shouldSleep = false;

    u32 OriginalNextEntryToRead = Queue->NextEntryToRead;
    u32 NewNextEntryToRead = (OriginalNextEntryToRead + 1) % ArrayCount(Queue->Entries);
    if (OriginalNextEntryToRead != Queue->NextEntryToWrite)
    {
        u32 Index = InterlockedCompareExchange((LONG volatile *)&Queue->NextEntryToRead,
                                                  NewNextEntryToRead,
                                                  OriginalNextEntryToRead);
        if(Index == OriginalNextEntryToRead)
        {        
             PlatformWorkQueueEntry Entry = Queue->Entries[Index];
            Entry.Callback(Queue, Entry.Data);
            InterlockedIncrement((LONG volatile *)&Queue->CompletionCount);
        }
    } else {
        shouldSleep = true;
    }

    return shouldSleep;
}

internal void
Win32CompleteAllWork(PlatformWorkQueue *Queue) {
    while(Queue->CompletionGoal != Queue->CompletionCount) {
        Win32DoNextWorkQueueEntry(Queue);
    }

    Queue->CompletionGoal = 0;
    Queue->CompletionCount = 0;
}

DWORD WINAPI
ThreadProc(LPVOID lpParameter) {
    PlatformWorkQueue *Queue = (PlatformWorkQueue *)lpParameter;

    for(;;) {
        if(Win32DoNextWorkQueueEntry(Queue)) {
            WaitForSingleObjectEx(Queue->SemaphoreHandle, INFINITE, FALSE);
        }
    }
}

internal void
Win32MakeQueue(PlatformWorkQueue *Queue, u32 ThreadCount) {
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
        ++ThreadIndex) {
        DWORD ThreadID;
        HANDLE ThreadHandle = CreateThread(0, 0, ThreadProc, Queue, 0, &ThreadID);
        CloseHandle(ThreadHandle);
    }
}

int WINAPI
WinMain(HINSTANCE hinst, HINSTANCE deprecated, LPSTR cmd, int show_cmd) {
    //
    // Multi-Threading
    //
    PlatformWorkQueue highPriorityQueue = {};
    Win32MakeQueue(&highPriorityQueue, 6);
    
    PlatformWorkQueue lowPriorityQueue = {};
    Win32MakeQueue(&lowPriorityQueue, 2);

    QueryPerformanceFrequency(&g_counter_hz);
    timeBeginPeriod(1);

#if __DEBUG
    g_show_cursor = true;
#endif

    WNDCLASSA wnd_class = {};
    wnd_class.style             = CS_HREDRAW | CS_VREDRAW;
    wnd_class.lpfnWndProc       = Win32WindowCallback;
    wnd_class.hInstance         = hinst;
    wnd_class.hCursor           = LoadCursorA(0, IDC_ARROW);
    wnd_class.lpszClassName     = "GameWindowClass";
    RegisterClassA(&wnd_class);

    // Win32ResizeDIBSection(&g_screen_buffer, 960, 540);
    Win32ResizeDIBSection(&g_screen_buffer, 1920, 1080);

    HWND hwnd = CreateWindowExA(
            0, wnd_class.lpszClassName, "Winter Serenity",
            WS_OVERLAPPEDWINDOW | WS_VISIBLE,
            CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
            0, 0, hinst, 0);

    if (hwnd) {
        Win32ToggleFullScreen(hwnd);
    }

    Win32LoadXInput();

    int monitor_hz = GetDeviceCaps(GetDC(hwnd), VREFRESH);
    int desired_hz = monitor_hz / 2;
    r32 desired_mspf = 1000.0f / (r32)desired_hz;


#if __DEBUG
    LPVOID base_address = (LPVOID)TB(2);
#else
    LPVOID base_address = 0;
#endif
    GameMemory game_memory = {};
    Win32State win32_state = {};
    game_memory.permanent_memory_capacity = MB(64);
    game_memory.transient_memory_capacity = GB(1);
    game_memory.highPriorityQueue = &highPriorityQueue;
    game_memory.lowPriorityQueue = &lowPriorityQueue;
    game_memory.platform.platform_add_entry = Win32AddEntry;
    game_memory.platform.platform_complete_all_work = Win32CompleteAllWork;
    game_memory.platform.debug_platform_read_file = DebugPlatformReadEntireFile;
    game_memory.platform.debug_platform_write_file = DebugPlatformWriteEntireFile;
    game_memory.platform.debug_platform_free_memory = DebugPlatformFreeMemory;
    uint64 total_capacity = game_memory.permanent_memory_capacity + game_memory.transient_memory_capacity;
    win32_state.game_memory = VirtualAlloc(base_address, (size_t)total_capacity,
                    MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    win32_state.game_mem_total_cap = total_capacity;
    game_memory.permanent_memory = win32_state.game_memory;
    game_memory.transient_memory = ((uint8 *)(game_memory.permanent_memory) + game_memory.permanent_memory_capacity);

    GameState *game_state = (GameState *)game_memory.permanent_memory;

    GameScreenBuffer gameScreenBuffer = {};
    gameScreenBuffer.memory = g_screen_buffer.memory;
    gameScreenBuffer.width = g_screen_buffer.width;
    gameScreenBuffer.height = g_screen_buffer.height;
    gameScreenBuffer.bpp = g_screen_buffer.bpp;
    gameScreenBuffer.pitch = g_screen_buffer.bpp * g_screen_buffer.width;

    HMODULE xinput_dll = LoadLibraryA(TEXT("xinput.dll"));
    if (!xinput_dll) { 
        // TODO: Diagnostic 
    }

    HMODULE game_dll = {};
    const char *game_dll_filename = "game.dll";
    const char *game_dll_load_filename = "game_load.dll";

    char exe_path_buf[MAX_PATH];
    DWORD exe_path_length = GetModuleFileNameA(0, exe_path_buf, MAX_PATH);
    DWORD last_backslash_idx = 0;
    for(DWORD idx = 0; idx < exe_path_length; idx++) {
        if(exe_path_buf[idx] == '\\') { last_backslash_idx = idx; }
    }

    char game_dll_abs_path[MAX_PATH];
    char game_dll_load_abs_path[MAX_PATH];
    Win32ConcatNString(game_dll_abs_path, exe_path_buf, last_backslash_idx + 1, game_dll_filename, StrLen(game_dll_filename));
    Win32ConcatNString(game_dll_load_abs_path, exe_path_buf, last_backslash_idx + 1, game_dll_load_filename, StrLen(game_dll_load_filename));

    FILETIME game_dll_time_last = {};
    FILETIME game_dll_time = {};

    GameMain_ *game_main = 0;
    GameInput game_input = {};

    //
    // Loop ------------------------------------------------------
    //
    while(g_running) {
        LARGE_INTEGER counter_begin = Win32GetClock();

        game_dll_time = Win32GetFileTime(game_dll_abs_path);
        if (CompareFileTime(&game_dll_time_last, &game_dll_time) != 0) {
            if (game_dll) {
                FreeLibrary(game_dll); 
                game_dll = 0;
            }
            CopyFileA(game_dll_abs_path, game_dll_load_abs_path, FALSE);
            game_dll = LoadLibraryA(game_dll_load_filename);
            if (game_dll) { 
                game_main = (GameMain_*)GetProcAddress(game_dll, "GameMain");
            } else {
                // TODO: Diagnostic 
            }
            game_dll_time_last = game_dll_time;
        }

        MSG msg;
        while(PeekMessageA(&msg, hwnd, 0, 0, PM_REMOVE)) {
            switch(msg.message) {
                case WM_QUIT: {
                    g_running = false;
                } break;
                case WM_SYSKEYDOWN:
                case WM_SYSKEYUP:
                case WM_KEYDOWN:
                case WM_KEYUP: {
                    u64 vkCode = msg.wParam;
                    b32 isDown = ((msg.lParam & (1 << 31)) == 0);
                    b32 wasDown = ((msg.lParam & (1 << 30)) != 0);
                    if (wasDown != isDown) {
                        switch (vkCode) {
                            case 'W': {
                                Win32ProcessKeyboard(&game_input.move_up, isDown);
                            } break;
                            case 'A': {
                                Win32ProcessKeyboard(&game_input.move_left, isDown);
                            } break;
                            case 'S': {
                                Win32ProcessKeyboard(&game_input.move_down, isDown);
                            } break;
                            case 'D': {
                                Win32ProcessKeyboard(&game_input.move_right, isDown);
                            } break;
#ifdef __DEBUG
                            // tilde key.
                            case VK_OEM_3: {
                                Win32ProcessKeyboard(&game_input.toggle_debug, isDown);
                            } break;

                            case 'L': {
                                if (isDown) {
                                    if (!win32_state.is_recording) {
                                        Win32BeginRecordingInput(&win32_state);
                                    } else {
                                        Win32EndInputRecording(&win32_state);
                                        Win32BeginInputPlayback(&win32_state);
                                    }
                                }
                            } break;
#endif
                        }

                        if (isDown) {
                            bool32 altWasDown = (msg.lParam & (1 << 29));
                            if ((vkCode == VK_F4) && altWasDown) {
                                g_running = false;
                            }
                            if ((vkCode == VK_RETURN) && altWasDown) {
                                if (msg.hwnd) {
                                    Win32ToggleFullScreen(msg.hwnd);
                                }
                            }
                        }

                    }
                } break;
            }
            TranslateMessage(&msg);
            DispatchMessageA(&msg);
        }

         DWORD result;    
         for(DWORD idx = 0;
                 idx < XUSER_MAX_COUNT;
                 idx++) {
             XINPUT_STATE state;
             ZeroMemory(&state, sizeof(XINPUT_STATE));
             result = xinput_get_state(idx, &state);
             Win32XInputHandleDeadzone(&state);
             if (result == ERROR_SUCCESS) {

             } else {
                 // TODO: Diagnostic
             }
         }

         if (win32_state.is_recording) {
             Win32RecordInput(&win32_state, &game_input);
         }
         if (win32_state.is_playing) {
             Win32PlaybackInput(&win32_state, &game_input);
         }

         game_input.dt_per_frame = desired_mspf / 1000.0f;

         if (game_main) {
             game_main(&game_memory, game_state, &game_input, &gameScreenBuffer);
         }

         Win32WindowDimension wd = Win32GetWindowDimension(hwnd);
         HDC dc = GetDC(hwnd);
         Assert(dc != 0);
         Win32UpdateScreen(dc, wd.width, wd.height);
         ReleaseDC(hwnd, dc);

         LARGE_INTEGER counter_end = Win32GetClock();
         r32 actual_mspf = Win32GetElapsedMs(counter_begin, counter_end);

         if(actual_mspf < desired_mspf) {
             DWORD ms_to_sleep = (DWORD)(desired_mspf - actual_mspf);
             Sleep(ms_to_sleep);
             actual_mspf = Win32GetElapsedMs(counter_begin, Win32GetClock());
         } else {
             // TODO: Missed framerate handling
         }
#if 0
         r32 fps = 1000.0f / actual_mspf;
         char profile[256];
         sprintf_s(profile, "elapsed_ms: %02f, fps: %02f\n", actual_mspf, fps);
         OutputDebugStringA(profile);
#endif

    }

    return 0;
}
