#pragma once

#include "../common/fn_common.h"

struct game_offscreen_buffer
{
    int32 Width;
    int32 Height;
    int32 Pitch; // Pitch = Width * bytesPerPixel (bytesPerPixel = 4)
    void* Data;
};

struct game_sound_output_buffer
{
    int32 SamplesPerSecond;
    int32 SampleCount;
    int16* Samples;
};

struct platform_job_queue;
struct platform_api;

struct game_memory
{
    bool IsInitialized;

    uint64 PersistentStorageSize;
    void* PersistentStorage;

    uint64 TransientStorageSize;
    void* TransientStorage;

    platform_api* PlatformAPI;

    platform_job_queue* HighPriorityQueue;
    platform_job_queue* LowPriorityQueue;

    float DeltaTime;
    float FixedDeltaTime;

    uint32 WindowWidth;
    uint32 WindowHeight;
};

struct game_button_state
{
    int32 HalfTransitionCount;
    bool EndedDown;
};

struct game_controller_input
{
    bool IsAnalog;
    bool IsConnected;

    float LStickAverageX;
    float LStickAverageY;

    float RStickAverageX;
    float RStickAverageY;

    game_button_state Start;
    game_button_state Back;

    game_button_state Dpad_Up;
    game_button_state Dpad_Down;
    game_button_state Dpad_Right;
    game_button_state Dpad_Left;

    game_button_state LStickUp;
    game_button_state LStickDown;
    game_button_state LStickRight;
    game_button_state LStickLeft;

    game_button_state RStickUp;
    game_button_state RStickDown;
    game_button_state RStickRight;
    game_button_state RStickLeft;

    game_button_state ButtonUp;
    game_button_state ButtonDown;
    game_button_state ButtonRight;
    game_button_state ButtonLeft;

    game_button_state LeftShoulder;
    game_button_state RightShoulder;

    game_button_state LeftTrigger;
    game_button_state RightTrigger;
};

struct game_keyboard_input
{
    uint32 KeyCode;
    bool Pressed;
    bool Released;
};

struct game_mouse_input
{
    float MouseCursorX;
    float MouseCursorY;

    bool LeftMouseButtonPressed;
    bool RightMouseButtonPressed;
};

struct game_input
{
    game_controller_input Gamepads[4];
    game_keyboard_input Keyboard;
    game_mouse_input Mouse;
};

// GAME API
// Services that the game provides to the platform layer

#define FN_GAME_API extern "C" __declspec(dllexport)

#define FN_GAME_TICK(name) void name(game_memory* memory)
typedef FN_GAME_TICK(game_tick);
FN_GAME_TICK(fn_game_tick_stub) {}

#define FN_GAME_RUN_FRAME(name) void name(game_memory* memory, game_input* input, game_offscreen_buffer* offScreenBuffer)
typedef FN_GAME_RUN_FRAME(game_run_frame);
FN_GAME_RUN_FRAME(fn_game_run_frame_stub) {}

#define FN_GAME_OUTPUT_SOUND(name) void name(game_memory* memory, game_sound_output_buffer* soundBuffer)
typedef FN_GAME_OUTPUT_SOUND(game_output_sound);
FN_GAME_OUTPUT_SOUND(fn_game_output_sound_stub) {}

struct game_api
{
    game_tick* Tick;
    game_run_frame* RunFrame;
    game_output_sound* OutputSound;
};

// PLATFORM API
// Services that the platform layer provides to the game

enum platform_rendering_context
{
    SOFTWARE_RENDERING = 0,
    OPENGL = 1,
    VULKAN = 2,
    DIRECTX11 = 3,
    DIRECTX12 = 4
};

struct platform_file_result
{
    uint32 FileSize;
    void* Data;
};

// @TODO(Anders): make async versions of read and write file. Look up I/O completion ports for windows platform layer

#define FN_PLATFORM_FILE_FREE(name) void name(void* data)
typedef FN_PLATFORM_FILE_FREE(platform_file_free);

#define FN_PLATFORM_FILE_WRITE(name) bool name(const char* fileName, uint32 size, void* data)
typedef FN_PLATFORM_FILE_WRITE(platform_file_write);

#define FN_PLATFORM_FILE_READ(name) platform_file_result name(const char* fileName)
typedef FN_PLATFORM_FILE_READ(platform_file_read);

#define FN_PLATFORM_DEBUG_LOG(name) void name(const char* message)
typedef FN_PLATFORM_DEBUG_LOG(platform_debug_log);

#define FN_PLATFORM_JOB_QUEUE_CALLBACK(name) void name(platform_job_queue* queue, void* data)
typedef FN_PLATFORM_JOB_QUEUE_CALLBACK(platform_job_queue_callback);

#define FN_PLATFORM_SCHEDULE_JOB(name) void name(platform_job_queue* queue, platform_job_queue_callback* callback, void* data)
typedef FN_PLATFORM_SCHEDULE_JOB(platform_schedule_job);

#define FN_PLATFORM_COMPLETE_ALL_JOBS(name) void name(platform_job_queue* queue)
typedef FN_PLATFORM_COMPLETE_ALL_JOBS(platform_complete_all_jobs);

struct platform_api
{
    platform_file_write* WriteFile;
    platform_file_read* ReadFile;
    platform_file_free* FreeFile;
    platform_debug_log* DebugLog;
    platform_schedule_job* ScheduleJob;
    platform_complete_all_jobs* CompleteAllJobs;
};