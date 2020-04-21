#pragma once

#include "../common/fn_common.h"
#include "../math/fn_math.h"
#include "fn_game_input.h"

// Services that the platform layer provides to the game
struct platform_file_result
{
    uint32 FileSize;
    void* Data;
};

// NOTE(Anders): make async versions of read and write file. Look up I/O completion ports for windows platform layer

#define FN_PLATFORM_FILE_FREE(name) void name(void* data)
typedef FN_PLATFORM_FILE_FREE(platform_file_free);

#define FN_PLATFORM_FILE_WRITE(name) bool name(const char* fileName, uint32 size, void* data)
typedef FN_PLATFORM_FILE_WRITE(platform_file_write);

#define FN_PLATFORM_FILE_READ(name) platform_file_result name(const char* fileName)
typedef FN_PLATFORM_FILE_READ(platform_file_read);

#define FN_PLATFORM_DEBUG_LOG(name) void name(const char* message)
typedef FN_PLATFORM_DEBUG_LOG(platform_debug_log);


struct game_offscreen_buffer
{
    int32 Width;
    int32 Height;
    int32 Pitch;
    void* Data;
};

struct game_sound_output_buffer
{
    int32 SamplesPerSecond;
    int32 SampleCount;
    int16* Samples;
};

struct game_memory
{
    bool IsInitialized;

    uint64 PermanentStorageSize;
    void* PermanentStorage;

    uint64 TransientStorageSize;
    void* TransientStorage;

    platform_file_write* PlatformWriteFile;
    platform_file_read* PlatformReadFile;
    platform_file_free* PlatformFreeFile;
    platform_debug_log* PlatformDebugLog;
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

};

struct game_input
{
    game_controller_input Gamepads[4];
    game_keyboard_input Keyboard;
    game_mouse_input Mouse;
};

struct game_state
{
    float PlayerX;
    float PlayerY;

    float PlayerVelocityX;
    float PlayerVelocityY;

    float tSine;
};

// Services that the game provides to the platform layer
#define FN_GAME_INIT(name) void name(game_memory* memory)
typedef FN_GAME_INIT(game_init);
FN_GAME_INIT(fn_game_init_stub){}

#define FN_GAME_PROCESS_INPUT(name) void name(game_memory* memory, game_input* input)
typedef FN_GAME_PROCESS_INPUT(game_process_input);
FN_GAME_PROCESS_INPUT(fn_game_process_input_stub){}

#define FN_GAME_TICK(name) void name(game_memory* memory, float dt)
typedef FN_GAME_TICK(game_tick);
FN_GAME_TICK(fn_game_tick_stub){}

#define FN_GAME_RENDER(name) void name(game_memory* memory, game_offscreen_buffer* buffer)
typedef FN_GAME_RENDER(game_render);
FN_GAME_RENDER(fn_game_render_stub){}

#define FN_GAME_OUTPUT_SOUND(name) void name(game_memory* memory, game_sound_output_buffer* soundBuffer)
typedef FN_GAME_OUTPUT_SOUND(game_output_sound);
FN_GAME_OUTPUT_SOUND(fn_game_output_sound_stub){}
