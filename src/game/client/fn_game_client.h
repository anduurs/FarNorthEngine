#pragma once

#include "../../engine/fn_engine.h"

struct game_button_state
{
    int32 HalfTransitionCount;
    bool EndedDown;
};

struct game_input_controller
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

struct game_input_keyboard
{
    uint32 KeyCode;
    bool Pressed;
    bool Released;
};

struct game_input_mouse
{
    float MouseCursorX;
    float MouseCursorY;

    bool LeftMouseButtonPressed;
    bool RightMouseButtonPressed;
};

struct game_input
{
    game_input_controller Gamepads[4];
    game_input_keyboard Keyboard;
    game_input_mouse Mouse;
};

struct game_sound_output_buffer
{
    int32 SamplesPerSecond;
    int32 SampleCount;
    int16* Samples;
};

// Services that the game provides to the platform layer
#define FN_GAME_CLIENT_INIT(name) void name(game_memory* memory)
typedef FN_GAME_CLIENT_INIT(game_client_init);
FN_GAME_CLIENT_INIT(fn_game_client_init_stub) {}

#define FN_GAME_CLIENT_UPDATE(name) void name(game_memory* memory, game_input* input, float secondsElapsed)
typedef FN_GAME_CLIENT_UPDATE(game_client_update);
FN_GAME_CLIENT_UPDATE(fn_game_client_update_) {}

#define FN_GAME_CLIENT_OUTPUT_SOUND(name) void name(game_memory* memory, game_sound_output_buffer* soundBuffer)
typedef FN_GAME_CLIENT_OUTPUT_SOUND(game_client_output_sound);
FN_GAME_CLIENT_OUTPUT_SOUND(fn_game_client_output_sound_stub) {}

#define FN_GAME_CLIENT_RENDER(name) void name(game_memory* memory)
typedef FN_GAME_CLIENT_RENDER(game_client_render);
FN_GAME_CLIENT_RENDER(fn_game_client_render_stub) {}

