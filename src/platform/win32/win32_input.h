#pragma once

#include <xinput.h>

#define XINPUT_GET_STATE(name) DWORD WINAPI name(DWORD dwUserIndex, XINPUT_STATE* pState)
typedef XINPUT_GET_STATE(xinput_get_state);
XINPUT_GET_STATE(XInputGetStateStub)
{
    return ERROR_DEVICE_NOT_CONNECTED;
}
global_variable xinput_get_state* XInputGetState_ = XInputGetStateStub;
#define XInputGetState XInputGetState_

#define XINPUT_SET_STATE(name) DWORD WINAPI name(DWORD dwUserIndex, XINPUT_VIBRATION* pVibration)
typedef XINPUT_SET_STATE(xinput_set_state);
XINPUT_SET_STATE(XInputSetStateStub)
{
    return ERROR_DEVICE_NOT_CONNECTED;
}
global_variable xinput_set_state* XInputSetState_ = XInputSetStateStub;
#define XInputSetState XInputSetState_

internal void win32_input_load_xinput();
internal void win32_input_process_digital_button(game_button_state* oldState, game_button_state* newState, DWORD buttonBit, DWORD xinputButtonState);
internal void win32_input_poll_gamepad(game_input* oldInput, game_input* newInput);

