#pragma once

#include "../../game/fn_game.h"

#include <windows.h>
#include <stdio.h>
#include <xinput.h>
#include <dsound.h>

struct win32_game_code
{
    HMODULE GameCodeDLL;
    FILETIME LastDLLWriteTime;

    bool IsValid;

    game_init* Init;
    game_process_input* ProcessInput;
    game_tick* Tick;
    game_render* Render;
    game_output_sound* OutputSound;
};

struct win32_state
{
    uint64 TotalGameMemorySize;
    void* GameMemoryBlock;

    HANDLE RecordingHandle;
    int32 InputRecordingIndex;

    HANDLE PlaybackHandle;
    int32 InputPlayingIndex;
};

struct win32_offscreen_buffer
{
    BITMAPINFO Info; 
    int32 Width;
    int32 Height;
    int32 Pitch;
    int32 BytesPerPixel;
    void* Data;
};

struct win32_window_dimension
{
    int32 Width;
    int32 Height;
};

global_variable volatile bool GlobalApplicationRunning;
global_variable win32_offscreen_buffer GlobalBackBuffer;
global_variable uint64 GlobalPerfCountFrequency;