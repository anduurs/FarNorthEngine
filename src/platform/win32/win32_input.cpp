#include "win32_input.h"

internal void win32_input_load_xinput()
{
    HMODULE XInputLibrary = LoadLibraryA("xinput1_4.dll");

    if (!XInputLibrary)
    {
        XInputLibrary = LoadLibraryA("xinput1_3.dll");
    }

    if (XInputLibrary)
    {
        XInputGetState = (xinput_get_state*)GetProcAddress(XInputLibrary, "XInputGetState");
        XInputSetState = (xinput_set_state*)GetProcAddress(XInputLibrary, "XInputSetState");
    }
}

internal void win32_input_process_digital_button(game_button_state* oldState, game_button_state* newState, DWORD buttonBit, DWORD xinputButtonState)
{
    newState->EndedDown = (xinputButtonState & buttonBit) == buttonBit;
    newState->HalfTransitionCount = (oldState->EndedDown != newState->EndedDown) ? 1 : 0;
}

internal void win32_input_poll_gamepad(game_input* oldInput, game_input* newInput)
{
    uint32 xinputMaxGamepadCount = XUSER_MAX_COUNT;
    uint32 gameMaxGamepadCount = ARRAY_LENGTH(newInput->Gamepads);

    if (xinputMaxGamepadCount > gameMaxGamepadCount)
    {
        xinputMaxGamepadCount = gameMaxGamepadCount;
    }

    for (DWORD gamepadIndex = 0; gamepadIndex < xinputMaxGamepadCount; gamepadIndex++)
    {
        game_gamepad_input* oldGamepad = &oldInput->Gamepads[gamepadIndex];
        game_gamepad_input* newGamepad = &newInput->Gamepads[gamepadIndex];

        XINPUT_STATE gamepadState;
        if (XInputGetState(gamepadIndex, &gamepadState) == ERROR_SUCCESS)
        {
            XINPUT_GAMEPAD* gamepad = &gamepadState.Gamepad;

            newGamepad->IsAnalog = true;

            float x;

            if (gamepad->sThumbLX < 0)
            {
                x = (float)gamepad->sThumbLX / 32768.0f;
            }
            else
            {
                x = (float)gamepad->sThumbLX / 32767.0f;
            }

            newGamepad->MinX = newGamepad->MaxX = newGamepad->EndX = x;

            float y;
            
            if (gamepad->sThumbLY < 0)
            {
                y = (float)gamepad->sThumbLY / 32768.0f;
            }
            else
            {
                y = (float)gamepad->sThumbLY / 32767.0f;
            }

            newGamepad->MinY = newGamepad->MaxY = newGamepad->EndY = y;
            
            win32_input_process_digital_button(&oldGamepad->Down, &newGamepad->Down, XINPUT_GAMEPAD_A, gamepad->wButtons);
            win32_input_process_digital_button(&oldGamepad->Up, &newGamepad->Up, XINPUT_GAMEPAD_Y, gamepad->wButtons);
            win32_input_process_digital_button(&oldGamepad->Right, &newGamepad->Right, XINPUT_GAMEPAD_B, gamepad->wButtons);
            win32_input_process_digital_button(&oldGamepad->Left, &newGamepad->Left, XINPUT_GAMEPAD_X, gamepad->wButtons);
            win32_input_process_digital_button(&oldGamepad->LeftShoulder, &newGamepad->LeftShoulder, XINPUT_GAMEPAD_LEFT_SHOULDER, gamepad->wButtons);
            win32_input_process_digital_button(&oldGamepad->RightShoulder, &newGamepad->RightShoulder, XINPUT_GAMEPAD_RIGHT_SHOULDER, gamepad->wButtons);
        }
    }
}