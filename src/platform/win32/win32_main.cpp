#include "win32_main.h"

#include "win32_opengl.cpp"
#include "win32_window.cpp"
#include "win32_time.cpp"
#include "win32_file.cpp"
#include "win32_input_recording.cpp"
#include "win32_input.cpp"
#include "win32_audio.cpp"
#include "win32_thread.cpp"

internal void win32_debug_log(const char* message)
{
    // @TODO(Anders E): Make a better way of debug logging
    OutputDebugStringA(message);
    OutputDebugStringA("\n");
}

internal inline FILETIME win32_get_last_write_time(const char* fileName)
{
    FILETIME lastWriteTime = {};
    WIN32_FILE_ATTRIBUTE_DATA data;

    if (GetFileAttributesEx(fileName, GetFileExInfoStandard, &data)) 
    {
        lastWriteTime = data.ftLastWriteTime;
    }

    return lastWriteTime;
}

internal win32_game_code win32_load_game_code(const char* sourceDllPath, const char* tempDLLPath, const char* lockPath)
{
    win32_game_code result = {};
    game_api gameApi = {};

    WIN32_FILE_ATTRIBUTE_DATA ignored;
    if (!GetFileAttributesEx(lockPath, GetFileExInfoStandard, &ignored)) 
    {
        result.LastDLLWriteTime = win32_get_last_write_time(sourceDllPath);

        int32 copyCounter = 0;

        while (true)
        {
            copyCounter++;
            assert(copyCounter <= 10);
            BOOL success = CopyFile(sourceDllPath, tempDLLPath, false);
            if (success) break;

            DWORD lastErrorCode = GetLastError();
            if (lastErrorCode == ERROR_FILE_NOT_FOUND) break;
        }

        result.GameCodeDLL = LoadLibraryA(tempDLLPath);

        if (result.GameCodeDLL)
        {
            gameApi.Tick = (game_tick*)GetProcAddress(result.GameCodeDLL, "Tick");
            gameApi.RunFrame = (game_run_frame*)GetProcAddress(result.GameCodeDLL, "RunFrame");
            gameApi.OutputSound = (game_output_sound*)GetProcAddress(result.GameCodeDLL, "OutputSound");

            result.IsValid = gameApi.Tick && gameApi.RunFrame && gameApi.OutputSound;
        }
    }

    if (!result.IsValid)
    {
        win32_debug_log("FAILED TO LOAD GAME DLL!");
        gameApi.Tick = fn_game_tick_stub;
        gameApi.RunFrame = fn_game_run_frame_stub;
        gameApi.OutputSound = fn_game_output_sound_stub;
    }

    result.GameAPI = gameApi;

    return result;
}

internal void win32_unload_game_code(win32_game_code* gameCode)
{
    if (gameCode->GameCodeDLL)
    {
        FreeLibrary(gameCode->GameCodeDLL);
        gameCode->GameCodeDLL = NULL;
    }

    gameCode->IsValid = false;
    gameCode->GameAPI.Tick = fn_game_tick_stub;
    gameCode->GameAPI.RunFrame = fn_game_run_frame_stub;
    gameCode->GameAPI.OutputSound = fn_game_output_sound_stub;
}

internal void win32_get_exe_path(win32_state* state)
{
    DWORD fileNameSize = GetModuleFileNameA(NULL, state->ExePath, sizeof(state->ExePath));
    char* onePastLastSlash = state->ExePath;

    for (char* scan = state->ExePath + fileNameSize;; --scan)
    {
        if (*scan == '\\') 
        {
            onePastLastSlash = scan + 1;
            break;
        }
    }

    state->ExeDirLength = (int)(onePastLastSlash - state->ExePath);
}

int32 WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR commandLine, int showCode)
{
    LARGE_INTEGER perfCountFrequencyResult;
    QueryPerformanceFrequency(&perfCountFrequencyResult);

    GlobalPerfCountFrequency = perfCountFrequencyResult.QuadPart;

    UINT desiredSchedulerMS = 1;
    bool sleepIsGranular = (timeBeginPeriod(desiredSchedulerMS) == TIMERR_NOERROR);

    int32 windowWidth = 1280;
    int32 windowHeight = 720;

    win32_window_info windowInfo = win32_window_create(
        hInstance, 
        "Far North Engine", 
        windowWidth,
        windowHeight
    );

    if (windowInfo.IsValid)
    {
        win32_window_init_offscreen_buffer(&GlobalBackBuffer, windowWidth, windowHeight);

#if !DEBUG_BUILD
        win32_window_toggle_fullscreen(windowInfo.WindowHandle);
#endif

        HDC deviceContext = windowInfo.DeviceContext;
        HWND window = windowInfo.WindowHandle;

        HGLRC openglContext = win32_opengl_context_create(deviceContext, 0, 0, windowWidth, windowHeight);

        game_memory gameMemory = {};

        gameMemory.PersistentStorageSize = megabytes(128);
        gameMemory.TransientStorageSize = gigabytes(1);

        uint64 totalSize = gameMemory.PersistentStorageSize + gameMemory.TransientStorageSize;

        win32_state win32State = {};

        win32State.TotalGameMemorySize = totalSize;
        win32State.GameMemoryBlock = VirtualAlloc(0, totalSize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

        gameMemory.PersistentStorage = win32State.GameMemoryBlock;
        gameMemory.TransientStorage = (uint64*)gameMemory.PersistentStorage + gameMemory.PersistentStorageSize;

        win32_sound_output_buffer soundOutput = {};

        soundOutput.SamplesPerSecond = 48000;
        soundOutput.BytesPerSample = sizeof(int16) * 2;
        soundOutput.ToneHz = 256;
        soundOutput.ToneVolume = 1000;
        soundOutput.WavePeriod = soundOutput.SamplesPerSecond / soundOutput.ToneHz;
        soundOutput.BufferSize = soundOutput.SamplesPerSecond * soundOutput.BytesPerSample;
        soundOutput.LatencySampleCount = soundOutput.SamplesPerSecond / 15;

        int16* samples = (int16*)VirtualAlloc(0, soundOutput.BufferSize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

        if (GlobalBackBuffer.Data &&
            gameMemory.PersistentStorage &&
            gameMemory.TransientStorage &&
            samples)
        {
            GlobalApplicationRunning = true;

            uint32 availableCPUCores = win32_thread_get_number_of_cores();

            uint32 lowPriorityQueueThreadCount = 2;
            uint32 highPriorityQueueThreadCount = availableCPUCores - lowPriorityQueueThreadCount;

            platform_job_queue highPriorityQueue = {};
            win32_thread_create_job_queue(&highPriorityQueue, highPriorityQueueThreadCount);

            platform_job_queue lowPriorityQueue = {};
            win32_thread_create_job_queue(&lowPriorityQueue, lowPriorityQueueThreadCount);

            platform_api platformAPI = {};
            gameMemory.PlatformAPI = &platformAPI;

            gameMemory.PlatformAPI->WriteFile = win32_file_write;
            gameMemory.PlatformAPI->ReadFile = win32_file_read;
            gameMemory.PlatformAPI->FreeFile = win32_file_free;
            gameMemory.PlatformAPI->DebugLog = win32_debug_log;
            gameMemory.PlatformAPI->ScheduleJob = win32_thread_schedule_job;
            gameMemory.PlatformAPI->CompleteAllJobs = win32_thread_complete_all_jobs;

            gameMemory.HighPriorityQueue = &highPriorityQueue;
            gameMemory.LowPriorityQueue = &lowPriorityQueue;

            gameMemory.WindowWidth = windowWidth;
            gameMemory.WindowHeight = windowHeight;

            win32_input_load_xinput();
            win32_audio_init_dsound(window, soundOutput.SamplesPerSecond, soundOutput.BufferSize);
            win32_audio_clear_sound_buffer(&soundOutput);

            GlobalSoundBuffer->Play(0, 0, DSBPLAY_LOOPING);

            game_input input[2] = {};
            game_input* oldInput = &input[0];
            game_input* newInput = &input[1];

            win32_get_exe_path(&win32State);

            char gameDLLName[] = "game.dll";
            char gameDLLPath[MAX_PATH];
            win32_build_path_in_exe_dir(&win32State, gameDLLName, sizeof(gameDLLName) - 1, gameDLLPath, sizeof(gameDLLPath));

            char gameTempDLLName[] = "game_temp.dll";
            char gameTempDLLPath[MAX_PATH];
            win32_build_path_in_exe_dir(&win32State, gameTempDLLName, sizeof(gameTempDLLName) - 1, gameTempDLLPath, sizeof(gameTempDLLPath));

            char pdbLockName[] = "pdb.lock";
            char pdbLockPath[MAX_PATH];
            win32_build_path_in_exe_dir(&win32State, pdbLockName, sizeof(pdbLockName) - 1, pdbLockPath, sizeof(pdbLockPath));

            FILETIME DLLWriteTime = win32_get_last_write_time(gameDLLPath);
            win32_game_code game = win32_load_game_code(gameDLLPath, gameTempDLLPath, pdbLockPath);
            game_api gameAPI = game.GameAPI;

            uint64 lastTickCounter = win32_time_get_counter();
            uint64 lastFrameCounter = win32_time_get_counter();
            uint64 flipCounter = win32_time_get_counter();
            uint64 lastCycleCounter = __rdtsc();

            int32 monitorRefreshRate = 60;
            HDC refreshDC = GetDC(window);
            int32 win32RefreshRate = GetDeviceCaps(refreshDC, VREFRESH);
            ReleaseDC(window, refreshDC);

            if (win32RefreshRate >= 59)
                monitorRefreshRate = 60;

            f32 targetFrameRate = (f32)monitorRefreshRate;
            f32 targetSecondsPerFrame = 1.0f / targetFrameRate;

            f32 targetTickRate = targetFrameRate;
            f32 targetSecondsPerTick = 1.0f / targetTickRate;
            f32 accumulator = 0.0f;

            gameMemory.FixedDeltaTime = targetSecondsPerTick;

            while (GlobalApplicationRunning)
            {
                FILETIME newDLLWriteTime = win32_get_last_write_time(gameDLLPath);

                if (CompareFileTime(&newDLLWriteTime, &game.LastDLLWriteTime) != 0)
                {
                    win32_unload_game_code(&game);
                    game = win32_load_game_code(gameDLLPath, gameTempDLLPath, pdbLockPath);
                }

                win32_input_process_messages(window, &win32State, newInput);
                win32_input_poll_gamepad(oldInput, newInput);

                POINT currentMousePosition;
                GetCursorPos(&currentMousePosition);

                auto center = win32_window_get_center(window);

                input->Mouse.MouseCursorX = (f32)currentMousePosition.x;
                input->Mouse.MouseCursorY = (f32)currentMousePosition.y;
                input->Mouse.MouseDeltaX = (f32)currentMousePosition.x - (f32)center.CenterX;
                input->Mouse.MouseDeltaY = (f32)currentMousePosition.y - (f32)center.CenterY;

                if (win32State.InputRecordingIndex)
                {
                    win32_record_input(&win32State, newInput);
                }

                if (win32State.InputPlayingIndex)
                {
                    win32_play_back_input(&win32State, newInput);
                }

                uint64 newTickCounter = win32_time_get_counter();
                f32 passedTime = win32_time_get_seconds_elapsed(lastTickCounter, newTickCounter);
                lastTickCounter = newTickCounter;

                accumulator += passedTime;

                while (accumulator >= targetSecondsPerTick)
                {
                    gameAPI.Tick(&gameMemory, newInput);
                    
                    accumulator -= targetSecondsPerTick;
                }

                game_offscreen_buffer offScreenBuffer = {};

                offScreenBuffer.Width = GlobalBackBuffer.Width;
                offScreenBuffer.Height = GlobalBackBuffer.Height;
                offScreenBuffer.Pitch = GlobalBackBuffer.Pitch;
                offScreenBuffer.Data = GlobalBackBuffer.Data;

                gameAPI.RunFrame(&gameMemory, newInput, &offScreenBuffer);

                win32_window_reset_mouse_cursor(window, center);

                if (!win32State.InputPlayingIndex)
                {
                    newInput->Keyboard.KeyCode = 0;
                    newInput->Keyboard.Pressed = false;
                    newInput->Keyboard.Released = false;
                }

                DWORD playCursor;
                DWORD writeCursor;
                DWORD byteToLock = 0;
                DWORD bytesToWrite = 0;

                bool soundIsValid = false;

                if (SUCCEEDED(GlobalSoundBuffer->GetCurrentPosition(&playCursor, &writeCursor)))
                {
                    byteToLock = (soundOutput.RunningSampleIndex * soundOutput.BytesPerSample) % soundOutput.BufferSize;

                    if (byteToLock > playCursor)
                    {
                        bytesToWrite = soundOutput.BufferSize - byteToLock;
                        bytesToWrite += playCursor;
                    }
                    else
                    {
                        bytesToWrite = playCursor - byteToLock;
                    }

                    soundIsValid = true;
                }

                game_sound_output_buffer soundBuffer = {};
                soundBuffer.SamplesPerSecond = soundOutput.SamplesPerSecond;
                soundBuffer.SampleCount = bytesToWrite / soundOutput.BytesPerSample;
                soundBuffer.Samples = samples;

                gameAPI.OutputSound(&gameMemory, &soundBuffer);

                if (soundIsValid)
                {
                    win32_audio_fill_sound_buffer(&soundOutput, byteToLock, bytesToWrite, &soundBuffer);
                }

                win32_opengl_swap_buffers(deviceContext);

                float secondsElapsedForFrame = win32_time_get_seconds_elapsed(lastFrameCounter, win32_time_get_counter());

                gameMemory.DeltaTime = secondsElapsedForFrame;
                gameMemory.FixedDeltaTime = targetSecondsPerTick;

                if (secondsElapsedForFrame < targetSecondsPerFrame)
                {
                    while (secondsElapsedForFrame < targetSecondsPerFrame)
                    {
                        if (sleepIsGranular)
                        {
                            DWORD sleepMS = (DWORD)(1000.0f * (targetSecondsPerFrame - secondsElapsedForFrame));
                            if (sleepMS > 0)
                            {
                                Sleep(sleepMS);
                            }
                        }

                        secondsElapsedForFrame = win32_time_get_seconds_elapsed(lastFrameCounter, win32_time_get_counter());
                    }
                }

                uint64 endCounter = win32_time_get_counter();

                /*win32_window_dimension dimension = win32_window_get_dimension(window);

                win32_window_blit_to_screen(
                    deviceContext,
                    &GlobalBackBuffer,
                    dimension.Width,
                    dimension.Height,
                    0, 0,
                    dimension.Width,
                    dimension.Height
                );*/

                flipCounter = win32_time_get_counter();

                fn_swap(oldInput, newInput);

                uint64 currentCycleCounter = __rdtsc();
#if 1
                int64 counterElapsed = endCounter - lastFrameCounter;
                uint64 cyclesElapsed = currentCycleCounter - lastCycleCounter;

                float MSPerFrame = 1000.0f * (float)counterElapsed / (float)perfCountFrequencyResult.QuadPart;
                float FPS = (float)perfCountFrequencyResult.QuadPart / (float)counterElapsed;
                float MCPF = (float)cyclesElapsed / (1000.0f * 1000.0f);

                char OutputBuffer[256];
                sprintf_s(OutputBuffer, sizeof(OutputBuffer), "%.2f ms, %.2f fps\n", MSPerFrame, FPS);
                SetWindowTextA(window, OutputBuffer);
                //OutputDebugStringA(OutputBuffer);
#endif
                lastFrameCounter = endCounter;
                lastCycleCounter = currentCycleCounter;
            }

            win32_opengl_context_destroy(deviceContext, openglContext);
        }
    }
    
    return 0;
}