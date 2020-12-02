#include <xinput.h>

#define XINPUT_GET_STATE(name) DWORD WINAPI name(DWORD dwUserIndex, XINPUT_STATE* pState)
typedef XINPUT_GET_STATE(xinput_get_state);
XINPUT_GET_STATE(XInputGetStateStub)
{
    return ERROR_DEVICE_NOT_CONNECTED;
}
global xinput_get_state* XInputGetState_ = XInputGetStateStub;
#define XInputGetState XInputGetState_

#define XINPUT_SET_STATE(name) DWORD WINAPI name(DWORD dwUserIndex, XINPUT_VIBRATION* pVibration)
typedef XINPUT_SET_STATE(xinput_set_state);
XINPUT_SET_STATE(XInputSetStateStub)
{
    return ERROR_DEVICE_NOT_CONNECTED;
}
global xinput_set_state* XInputSetState_ = XInputSetStateStub;
#define XInputSetState XInputSetState_

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

internal void win32_input_process_messages(HWND window, win32_state* win32State, game_input* input)
{
    MSG message;

    while (PeekMessage(&message, 0, 0, 0, PM_REMOVE))
    {
        switch (message.message)
        {
        case WM_QUIT:
        {
            GlobalApplicationRunning = false;
        } break;

        case WM_MOUSEMOVE:
        {
            f32 lastMousePosX = input->Mouse.MouseCursorX;
            f32 lastMousePosY = input->Mouse.MouseCursorY;

            POINT currentMousePosition;
            GetCursorPos(&currentMousePosition);
            ScreenToClient(window, &currentMousePosition);

            auto dim = win32_window_get_dimension(window);

            SetCursorPos(dim.Width/2, dim.Height/2);

            input->Mouse.MouseCursorX = (f32)currentMousePosition.x;
            input->Mouse.MouseCursorY = (f32)currentMousePosition.y;
            //char printBuffer[256];
            //sprintf_s(printBuffer, "%f %f \n", (float)currentMousePosition.x, (float)currentMousePosition.y);
            //OutputDebugStringA(printBuffer);
        } break;

        case WM_SYSKEYDOWN:
        case WM_KEYDOWN:
        case WM_SYSKEYUP:
        case WM_KEYUP:
        {
            uint32 vkCode = (uint32)message.wParam;

            bool wasDown = ((message.lParam & (1 << 30)) != 0);
            bool isDown = ((message.lParam & (1 << 31)) == 0);

            if (isDown)
            {
                bool altKeyWasDown = message.lParam & (1 << 29);

                if (vkCode == VK_F4 && altKeyWasDown)
                {
                    GlobalApplicationRunning = false;
                }

                if (vkCode == VK_RETURN && altKeyWasDown)
                {
                    win32_window_toggle_fullscreen(message.hwnd);
                }
            }

            if (wasDown != isDown)
            {
                input->Keyboard.Pressed = isDown;
                input->Keyboard.Released = !isDown;
                input->Keyboard.KeyCode = vkCode;

                if (vkCode == VK_ESCAPE)
                {
                    GlobalApplicationRunning = false;
                }
                else if (vkCode == 'L')
                {
                    if (isDown)
                    {
                        if (win32State->InputRecordingIndex == 0)
                        {
                            OutputDebugStringA("Begin recording input \n");
                            win32_begin_recording_input(win32State, 1);
                        }
                        else
                        {
                            OutputDebugStringA("End recording input \n");
                            win32_end_recording_input(win32State);
                            win32_begin_play_back_input(win32State, 1);
                        }
                    }
                }
            }
        } break;

        default:
        {
            TranslateMessage(&message);
            DispatchMessageA(&message);
        } break;
        }
    }
}