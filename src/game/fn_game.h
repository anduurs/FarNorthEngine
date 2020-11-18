#pragma once

#include "../platform/fn_platform.h"
#include "fn_memory.h"
#include "../math/fn_math.h"
#include "fn_game_input.h"

struct game_offscreen_buffer
{
    int32 Width;
    int32 Height;
    int32 Pitch;
    int32 BytesPerPixel;
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

    uint64 PersistentStorageSize;
    void* PersistentStorage;

    uint64 TemporaryStorageSize;
    void* TemporaryStorage;

    uint64 TransientStorageSize;
    void* TransientStorage;

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

struct player
{
    vec2 Position;
    vec2 Velocity;
};

struct fn_world_chunk
{
    uint16 ChunkX;
    uint16 ChunkY;
    uint8 ChunkSize;

    uint8* TileIds;
};

struct fn_world
{
    uint16 ChunkDimension;
    fn_world_chunk* Chunks;
    player* Player;
};

struct game_state
{
    float tSine;

    memory_arena WorldArena;
    fn_world* GameWorld;
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

#define FN_GAME_RENDER(name) void name(game_memory* memory, game_offscreen_buffer* offScreenBuffer)
typedef FN_GAME_RENDER(game_render);
FN_GAME_RENDER(fn_game_render_stub){}

#define FN_GAME_OUTPUT_SOUND(name) void name(game_memory* memory, game_sound_output_buffer* soundBuffer)
typedef FN_GAME_OUTPUT_SOUND(game_output_sound);
FN_GAME_OUTPUT_SOUND(fn_game_output_sound_stub){}
