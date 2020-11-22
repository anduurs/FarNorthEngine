#pragma once

#include "../fn_platform.h"

#include <windows.h>
#include <stdio.h>

struct win32_game_code
{
    HMODULE GameCodeDLL;
    FILETIME LastDLLWriteTime;

    bool IsValid;
    game_api GameAPI;
};

global_variable bool volatile GlobalApplicationRunning;
global_variable uint64 GlobalPerfCountFrequency;