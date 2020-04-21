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
    uint32 gameMaxGamepadCount = array_length(newInput->Gamepads);

    if (xinputMaxGamepadCount > gameMaxGamepadCount)
    {
        xinputMaxGamepadCount = gameMaxGamepadCount;
    }

    for (DWORD gamepadIndex = 0; gamepadIndex < xinputMaxGamepadCount; gamepadIndex++)
    {
        game_controller_input* oldGamepad = &oldInput->Gamepads[gamepadIndex];
        game_controller_input* newGamepad = &newInput->Gamepads[gamepadIndex];

        XINPUT_STATE gamepadState;
        if (XInputGetState(gamepadIndex, &gamepadState) == ERROR_SUCCESS)
        {
            XINPUT_GAMEPAD* gamepad = &gamepadState.Gamepad;

            newGamepad->IsAnalog = true;
            newGamepad->IsConnected = true;

            // LEFT STICK
            if (gamepad->sThumbLX < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
            {
                newGamepad->LStickAverageX = (float)gamepad->sThumbLX / 32768.0f;
            }
            else if (gamepad->sThumbLX > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
            {
                newGamepad->LStickAverageX = (float)gamepad->sThumbLX / 32767.0f;
            }

            if (gamepad->sThumbLY < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
            {
                newGamepad->LStickAverageY = (float)gamepad->sThumbLY / 32768.0f;
            }
            else if (gamepad->sThumbLY > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
            {
                newGamepad->LStickAverageY = (float)gamepad->sThumbLY / 32767.0f;
            }

            // RIGHT STICK
            if (gamepad->sThumbRX < -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE)
            {
                newGamepad->RStickAverageX = (float)gamepad->sThumbRX / 32768.0f;
            }
            else if (gamepad->sThumbRX > XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE)
            {
                newGamepad->RStickAverageX = (float)gamepad->sThumbRX / 32767.0f;
            }

            if (gamepad->sThumbRY < -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE)
            {
                newGamepad->RStickAverageY = (float)gamepad->sThumbRY / 32768.0f;
            }
            else if (gamepad->sThumbRY > XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE)
            {
                newGamepad->RStickAverageY = (float)gamepad->sThumbRY / 32767.0f;
            }

            // TODO(Anders): LEFT TRIGGER

            // TODO(Anders): RIGHT TRIGGER

            float stickThreshold = 0.5f;

            win32_input_process_digital_button(&oldGamepad->LStickRight, &newGamepad->LStickRight, 1, newGamepad->LStickAverageX < -stickThreshold ? 1 : 0);
            win32_input_process_digital_button(&oldGamepad->LStickLeft, &newGamepad->LStickLeft, 1, newGamepad->LStickAverageX > stickThreshold ? 1 : 0);
            win32_input_process_digital_button(&oldGamepad->LStickUp, &newGamepad->LStickUp, 1, newGamepad->LStickAverageY > stickThreshold ? 1 : 0);
            win32_input_process_digital_button(&oldGamepad->LStickDown, &newGamepad->LStickDown, 1, newGamepad->LStickAverageY < -stickThreshold ? 1 : 0);

            win32_input_process_digital_button(&oldGamepad->RStickRight, &newGamepad->RStickRight, 1, newGamepad->RStickAverageX < -stickThreshold ? 1 : 0);
            win32_input_process_digital_button(&oldGamepad->RStickLeft, &newGamepad->RStickLeft, 1, newGamepad->RStickAverageX > stickThreshold ? 1 : 0);
            win32_input_process_digital_button(&oldGamepad->RStickUp, &newGamepad->RStickUp, 1, newGamepad->RStickAverageY > stickThreshold ? 1 : 0);
            win32_input_process_digital_button(&oldGamepad->RStickDown, &newGamepad->RStickDown, 1, newGamepad->RStickAverageY < -stickThreshold ? 1 : 0);
            
            win32_input_process_digital_button(&oldGamepad->ButtonDown, &newGamepad->ButtonDown, XINPUT_GAMEPAD_A, gamepad->wButtons);
            win32_input_process_digital_button(&oldGamepad->ButtonUp, &newGamepad->ButtonUp, XINPUT_GAMEPAD_Y, gamepad->wButtons);
            win32_input_process_digital_button(&oldGamepad->ButtonRight, &newGamepad->ButtonRight, XINPUT_GAMEPAD_B, gamepad->wButtons);
            win32_input_process_digital_button(&oldGamepad->ButtonLeft, &newGamepad->ButtonLeft, XINPUT_GAMEPAD_X, gamepad->wButtons);

            win32_input_process_digital_button(&oldGamepad->Dpad_Down, &newGamepad->Dpad_Down, XINPUT_GAMEPAD_DPAD_DOWN, gamepad->wButtons);
            win32_input_process_digital_button(&oldGamepad->Dpad_Up, &newGamepad->Dpad_Up, XINPUT_GAMEPAD_DPAD_UP, gamepad->wButtons);
            win32_input_process_digital_button(&oldGamepad->Dpad_Right, &newGamepad->Dpad_Right, XINPUT_GAMEPAD_DPAD_RIGHT, gamepad->wButtons);
            win32_input_process_digital_button(&oldGamepad->Dpad_Left, &newGamepad->Dpad_Left, XINPUT_GAMEPAD_DPAD_LEFT, gamepad->wButtons);

            win32_input_process_digital_button(&oldGamepad->Start, &newGamepad->Start, XINPUT_GAMEPAD_START, gamepad->wButtons);
            win32_input_process_digital_button(&oldGamepad->Back, &newGamepad->Back, XINPUT_GAMEPAD_BACK, gamepad->wButtons);

            win32_input_process_digital_button(&oldGamepad->LeftShoulder, &newGamepad->LeftShoulder, XINPUT_GAMEPAD_LEFT_SHOULDER, gamepad->wButtons);
            win32_input_process_digital_button(&oldGamepad->RightShoulder, &newGamepad->RightShoulder, XINPUT_GAMEPAD_RIGHT_SHOULDER, gamepad->wButtons);
        }
        else
        {
            newGamepad->IsConnected = false;
        }
    }
}