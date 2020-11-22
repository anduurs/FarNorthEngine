#include "win32_main.h"

#include "win32_window.cpp"
#include "win32_input_recording.cpp"
#include "win32_input.cpp"
#include "win32_audio.cpp"
#include "win32_thread.cpp"
#include "win32_time.cpp"

FN_PLATFORM_FILE_WRITE(PlatformWriteFile)
{
    bool result = false;
    HANDLE fileHandle = CreateFile(fileName, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, 0, 0);

    if (fileHandle != INVALID_HANDLE_VALUE)
    {
        DWORD bytesWritten;
        if (WriteFile(fileHandle, data, size, &bytesWritten, 0))
        {
            result = bytesWritten == size;
        }

        CloseHandle(fileHandle);
    }

    return result;
}

FN_PLATFORM_FILE_FREE(PlatformFreeFile)
{
    if (data)
    {
        VirtualFree(data, 0, MEM_RELEASE);
    }
}

FN_PLATFORM_FILE_READ(PlatformReadFile)
{
    platform_file_result result = {};

    HANDLE fileHandle = CreateFile(fileName, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);

    if (fileHandle != INVALID_HANDLE_VALUE)
    {
        LARGE_INTEGER fileSize;
        if (GetFileSizeEx(fileHandle, &fileSize))
        {
            assert(fileSize.QuadPart <= 0xFFFFFFFF);

            uint32 size = (uint32)fileSize.QuadPart;

            void* fileData = VirtualAlloc(0, size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

            if (fileData)
            {
                DWORD bytesRead;
                if (ReadFile(fileHandle, fileData, size, &bytesRead, 0) && size == (uint32)bytesRead)
                {
                    result.FileSize = size;
                    result.Data = fileData;
                }
                else
                {
                    PlatformFreeFile(fileData);
                    fileData = 0;
                }
            }
        }

        CloseHandle(fileHandle);
    }

    return result;
}

FN_PLATFORM_DEBUG_LOG(PlatformDebugLog)
{
    OutputDebugStringA(message);
    OutputDebugStringA("\n");
}

internal inline FILETIME win32_get_last_write_time(const char* fileName)
{
    FILETIME lastWriteTime = {};
    WIN32_FILE_ATTRIBUTE_DATA data;

    if (GetFileAttributesExA(fileName, GetFileExInfoStandard, &data)) 
    {
        lastWriteTime = data.ftLastWriteTime;
    }

    return lastWriteTime;
}

internal win32_game_code win32_load_game_code(const char* sourceDllPath, const char* tempDLLPath)
{
    win32_game_code result = {};

    result.LastDLLWriteTime = win32_get_last_write_time(sourceDllPath);
    CopyFile(sourceDllPath, tempDLLPath, FALSE);
    result.GameCodeDLL = LoadLibraryA(tempDLLPath);

    game_api gameApi = {};

    if (result.GameCodeDLL)
    {
        gameApi.Init = (game_init*)GetProcAddress(result.GameCodeDLL, "fn_game_init");
        gameApi.ProcessInput = (game_process_input*)GetProcAddress(result.GameCodeDLL, "fn_game_process_input");
        gameApi.Tick = (game_tick*)GetProcAddress(result.GameCodeDLL, "fn_game_tick");
        gameApi.Update = (game_tick*)GetProcAddress(result.GameCodeDLL, "fn_game_update");
        gameApi.Render = (game_render*)GetProcAddress(result.GameCodeDLL, "fn_game_render");
        gameApi.OutputSound = (game_output_sound*)GetProcAddress(result.GameCodeDLL, "fn_game_output_sound");

        result.IsValid = gameApi.Init && gameApi.ProcessInput && gameApi.Tick && gameApi.Update && gameApi.Render && gameApi.OutputSound;
    }

    if (!result.IsValid)
    {
        gameApi.Init = fn_game_init_stub;
        gameApi.ProcessInput = fn_game_process_input_stub;
        gameApi.Tick = fn_game_tick_stub;
        gameApi.Update = fn_game_update_stub;
        gameApi.Render = fn_game_render_stub;
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
    gameCode->GameAPI.Init = fn_game_init_stub;
    gameCode->GameAPI.ProcessInput = fn_game_process_input_stub;
    gameCode->GameAPI.Tick = fn_game_tick_stub;
    gameCode->GameAPI.Update = fn_game_update_stub;
    gameCode->GameAPI.Render = fn_game_render_stub;
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

internal FN_PLATFORM_JOB_QUEUE_CALLBACK(job_callback)
{
    char buffer[256];
    wsprintf(buffer, "Thread %u: %s\n", GetCurrentThreadId(), (char*)data);
    OutputDebugString(buffer);
}

int32 WINAPI WinMain
(
    HINSTANCE hInstance, 
    HINSTANCE hPrevInstance, 
    LPSTR commandLine, 
    int showCode
)
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
        windowHeight,
        platform_rendering_context::SOFTWARE_RENDERING
    );

    if (windowInfo.IsValid)
    {
        HDC deviceContext = windowInfo.DeviceContext;
        HWND window = windowInfo.WindowHandle;

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

            platform_job_queue queue = {};

            win32_thread_info* workerThreads = win32_thread_create_workers(&queue);

            win32_thread_schedule_job(&queue, job_callback, "Work A0");
            win32_thread_schedule_job(&queue, job_callback, "Work A1");
            win32_thread_schedule_job(&queue, job_callback, "Work A2");
            win32_thread_schedule_job(&queue, job_callback, "Work A3");
            win32_thread_schedule_job(&queue, job_callback, "Work A4");
            win32_thread_schedule_job(&queue, job_callback, "Work A5");
            win32_thread_schedule_job(&queue, job_callback, "Work A6");
            win32_thread_schedule_job(&queue, job_callback, "Work A7");
            win32_thread_schedule_job(&queue, job_callback, "Work A8");
            win32_thread_schedule_job(&queue, job_callback, "Work A9");

            Sleep(1000);

            win32_thread_schedule_job(&queue, job_callback, "Work B0");
            win32_thread_schedule_job(&queue, job_callback, "Work B1");
            win32_thread_schedule_job(&queue, job_callback, "Work B2");
            win32_thread_schedule_job(&queue, job_callback, "Work B3");
            win32_thread_schedule_job(&queue, job_callback, "Work B4");
            win32_thread_schedule_job(&queue, job_callback, "Work B5");
            win32_thread_schedule_job(&queue, job_callback, "Work B6");
            win32_thread_schedule_job(&queue, job_callback, "Work B7");
            win32_thread_schedule_job(&queue, job_callback, "Work B8");
            win32_thread_schedule_job(&queue, job_callback, "Work B9");

            win32_thread_complete_all_jobs(&queue);

            platform_api platformAPI = {};
            gameMemory.PlatformAPI = &platformAPI;

            gameMemory.PlatformAPI->WriteFile = PlatformWriteFile;
            gameMemory.PlatformAPI->FreeFile = PlatformFreeFile;
            gameMemory.PlatformAPI->ReadFile = PlatformReadFile;
            gameMemory.PlatformAPI->DebugLog = PlatformDebugLog;
            gameMemory.PlatformAPI->ScheduleJob = win32_thread_schedule_job;
            gameMemory.PlatformAPI->CompleteAllJobs = win32_thread_complete_all_jobs;

            gameMemory.HighPriorityQueue = &queue;
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

            FILETIME DLLWriteTime = win32_get_last_write_time(gameDLLPath);
            win32_game_code game = win32_load_game_code(gameDLLPath, gameTempDLLPath);
            game_api gameAPI = game.GameAPI;
            gameAPI.Init(&gameMemory);

            uint64 lastTickCounter = win32_time_get_perf_counter();
            uint64 lastFrameCounter = win32_time_get_perf_counter();
            uint64 flipCounter = win32_time_get_perf_counter();
            uint64 lastCycleCounter = __rdtsc();

            int monitorRefreshRate = 60;
            HDC refreshDC = GetDC(window);
            int win32RefreshRate = GetDeviceCaps(refreshDC, VREFRESH);
            ReleaseDC(window, refreshDC);

            if (win32RefreshRate > 1)
                monitorRefreshRate = win32RefreshRate;

            float targetFrameRate = (float)monitorRefreshRate;
            float targetSecondsPerFrame = 1.0f / targetFrameRate;

            float targetTickRate = targetFrameRate / 2.0f;
            float targetSecondsPerTick = 1.0f / targetTickRate;
            float accumulator = 0.0f;

            while (GlobalApplicationRunning)
            {
                gameMemory.DeltaTime = targetSecondsPerFrame;
                FILETIME newDLLWriteTime = win32_get_last_write_time(gameDLLPath);

                if (CompareFileTime(&newDLLWriteTime, &game.LastDLLWriteTime) != 0)
                {
                    win32_unload_game_code(&game);
                    game = win32_load_game_code(gameDLLPath, gameTempDLLPath);
                }

                win32_input_process_messages(window, &win32State, newInput);
                win32_input_poll_gamepad(oldInput, newInput);

                if (win32State.InputRecordingIndex)
                {
                    win32_record_input(&win32State, newInput);
                }

                if (win32State.InputPlayingIndex)
                {
                    win32_play_back_input(&win32State, newInput);
                }

                gameAPI.ProcessInput(&gameMemory, newInput);

                if (!win32State.InputPlayingIndex)
                {
                    newInput->Keyboard.KeyCode = 0;
                    newInput->Keyboard.Pressed = false;
                    newInput->Keyboard.Released = false;
                }

                uint64 newTickCounter = win32_time_get_perf_counter();
                float passedTime = win32_time_get_seconds_elapsed(lastTickCounter, newTickCounter);
                lastTickCounter = newTickCounter;

                accumulator += passedTime;

                while (accumulator >= targetSecondsPerTick)
                {
                    gameAPI.Tick(&gameMemory);
                    accumulator -= targetSecondsPerTick;
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

                gameAPI.Update(&gameMemory);

                game_offscreen_buffer offScreenBuffer = {};

                offScreenBuffer.Width = GlobalBackBuffer.Width;
                offScreenBuffer.Height = GlobalBackBuffer.Height;
                offScreenBuffer.Pitch = GlobalBackBuffer.Pitch;
                offScreenBuffer.Data = GlobalBackBuffer.Data;

                gameAPI.Render(&gameMemory, &offScreenBuffer);

                game_sound_output_buffer soundBuffer = {};
                soundBuffer.SamplesPerSecond = soundOutput.SamplesPerSecond;
                soundBuffer.SampleCount = bytesToWrite / soundOutput.BytesPerSample;
                soundBuffer.Samples = samples;

                gameAPI.OutputSound(&gameMemory, &soundBuffer);

                if (soundIsValid)
                {
                    win32_audio_fill_sound_buffer(&soundOutput, byteToLock, bytesToWrite, &soundBuffer);
                }

                float secondsElapsedForFrame = win32_time_get_seconds_elapsed(lastFrameCounter, win32_time_get_perf_counter());

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

                        secondsElapsedForFrame = win32_time_get_seconds_elapsed(lastFrameCounter, win32_time_get_perf_counter());
                    }
                }

                uint64 endCounter = win32_time_get_perf_counter();

                win32_window_dimension dimension = win32_window_get_dimension(window);

                win32_window_blit_to_screen(
                    deviceContext,
                    &GlobalBackBuffer,
                    dimension.Width,
                    dimension.Height,
                    0, 0,
                    dimension.Width,
                    dimension.Height
                );

                flipCounter = win32_time_get_perf_counter();

                game_input* tmp = oldInput;
                oldInput = newInput;
                newInput = tmp;

                uint64 currentCycleCounter = __rdtsc();
#if 0
                int64 counterElapsed = endCounter - lastFrameCounter;
                uint64 cyclesElapsed = currentCycleCounter - lastCycleCounter;

                float MSPerFrame = 1000.0f * (float)counterElapsed / (float)perfCountFrequencyResult.QuadPart;
                float FPS = (float)perfCountFrequencyResult.QuadPart / (float)counterElapsed;
                float MCPF = (float)cyclesElapsed / (1000.0f * 1000.0f);

                char OutputBuffer[256];
                sprintf_s(OutputBuffer, sizeof(OutputBuffer), "ms/f: %.2f,  fps: %.2f,  mc/f: %.2f\n", MSPerFrame, FPS, MCPF);
                OutputDebugStringA(OutputBuffer);
#endif
                lastFrameCounter = endCounter;
                lastCycleCounter = currentCycleCounter;
            }
        }
    }
    
    return 0;
}