#include "win32_main.h"
#include "win32_input.cpp"
#include "win32_audio.cpp"

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
}

internal win32_window_dimension win32_window_get_dimension(HWND window)
{
    win32_window_dimension result;
    
    RECT clientRect;
    GetClientRect(window, &clientRect);
    
    result.Width = clientRect.right - clientRect.left;
    result.Height = clientRect.bottom - clientRect.top;
    
    return result;
}

internal void win32_window_init_offscreen_buffer(win32_offscreen_buffer* buffer, int32 width, int32 height)
{
    if (buffer->Data)
    {
        VirtualFree(buffer->Data, 0, MEM_RELEASE);
    }
    
    buffer->Width = width;
    buffer->Height = height;
    
    buffer->Info.bmiHeader.biSize = sizeof(buffer->Info.bmiHeader);
    buffer->Info.bmiHeader.biWidth = width;
    buffer->Info.bmiHeader.biHeight = -height; //top-down
    buffer->Info.bmiHeader.biPlanes = 1;
    buffer->Info.bmiHeader.biBitCount = 32;
    buffer->Info.bmiHeader.biCompression = BI_RGB;
    
    buffer->BytesPerPixel = 4;
    
    int32 bitmapMemorySize = width * height * buffer->BytesPerPixel;
    buffer->Data = VirtualAlloc(0, bitmapMemorySize, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
    
    buffer->Pitch = width * buffer->BytesPerPixel;
}

internal void win32_window_update(HDC deviceContext, win32_offscreen_buffer* buffer, 
                    int32 windowWidth, int32 windowHeight,
                    int32 x, int32 y, int32 width, int32 height)
{
    StretchDIBits(deviceContext, 
                  0, 0, windowWidth, windowHeight, 
                  0, 0, buffer->Width, buffer->Height,
                  buffer->Data, 
                  &buffer->Info,
                  DIB_RGB_COLORS, SRCCOPY);
}

internal LRESULT CALLBACK win32_window_callback(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
{
    LRESULT result = 0;
    
    switch (message)
    {
        case WM_SIZE:
        {
            OutputDebugStringA("WM_SIZE\n");
        } break;
        
        case WM_CLOSE:
        {
            OutputDebugStringA("WM_CLOSE\n");
            PostQuitMessage(0);
        } break;
        
        case WM_ACTIVATEAPP:
        {
            OutputDebugStringA("WM_ACTIVATEAPP\n");
        } break;
        
        case WM_DESTROY:
        {
            OutputDebugStringA("WM_DESTROY\n");
            PostQuitMessage(0);
        } break;
        
        case WM_PAINT:
        {
            PAINTSTRUCT paint;
            HDC deviceContext = BeginPaint(window, &paint);
            int32 x = paint.rcPaint.left;
            int32 y = paint.rcPaint.top;
            int32 width = paint.rcPaint.right - paint.rcPaint.left;
            int32 height = paint.rcPaint.bottom - paint.rcPaint.top;
            win32_window_dimension dimension = win32_window_get_dimension(window);
            win32_window_update(deviceContext, &GlobalBackBuffer, dimension.Width, dimension.Height, x, y, width, height);
            EndPaint(window, &paint);
        } break;

        default:
        {
            result = DefWindowProc(window, message, wParam, lParam);
        } break;
    }
    
    return result;
}

internal void win32_begin_recording_input(win32_state* win32State, int32 inputRecordingIndex)
{
    win32State->InputRecordingIndex = inputRecordingIndex;
    const char* fileName = "input_record.fni";
    win32State->RecordingHandle = CreateFileA(fileName, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, 0, 0);
    
    DWORD bytesToWrite = (DWORD)win32State->TotalGameMemorySize;
    assert(bytesToWrite == win32State->TotalGameMemorySize);
    DWORD bytesWritten;
    WriteFile(win32State->RecordingHandle, win32State->GameMemoryBlock, bytesToWrite, &bytesWritten, 0);
}

internal void win32_end_recording_input(win32_state* win32State)
{
    CloseHandle(win32State->RecordingHandle);
    win32State->InputRecordingIndex = 0;
}

internal void win32_record_input(win32_state* win32State, game_input* input)
{
    DWORD bytesWritten;
    WriteFile(win32State->RecordingHandle, input, sizeof(*input), &bytesWritten, 0);
}

internal void win32_begin_play_back_input(win32_state* win32State, int32 inputPlayingIndex)
{
    win32State->InputPlayingIndex = inputPlayingIndex;

    const char* fileName = "input_playback.fni";
    win32State->PlaybackHandle = CreateFileA(fileName, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
    DWORD bytesToRead = (DWORD)win32State->TotalGameMemorySize;
    assert(bytesToRead == win32State->TotalGameMemorySize);
    DWORD bytesRead;
    ReadFile(win32State->PlaybackHandle, win32State->GameMemoryBlock, bytesToRead, &bytesRead, 0);
}

internal void win32_end_play_back_input(win32_state* win32State)
{
    CloseHandle(win32State->PlaybackHandle);
    win32State->InputPlayingIndex = 0;
}

internal void win32_play_back_input(win32_state* win32State, game_input* input)
{
    DWORD bytesRead = 0;
    if (ReadFile(win32State->PlaybackHandle, input, sizeof(*input), &bytesRead, 0))
    {
        // there is still input
        if (bytesRead == 0)
        {
            // we hit the end of the stream, go back to the beginning
            int32 playingIndex = win32State->InputPlayingIndex;
            win32_end_play_back_input(win32State);
            win32_begin_play_back_input(win32State, playingIndex);
            ReadFile(win32State->PlaybackHandle, input, sizeof(*input), &bytesRead, 0);
        }
    }
}

internal void win32_process_messages(win32_state* win32State, game_input* input)
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

            case WM_SYSKEYDOWN:
            case WM_SYSKEYUP:
            case WM_KEYDOWN:
            case WM_KEYUP:
            {
                uint32 vkCode = (uint32)message.wParam;
                bool wasDown = ((message.lParam & (1 << 30)) != 0);
                bool isDown = ((message.lParam & (1 << 31)) == 0);

                if (wasDown != isDown)
                {
                    if(vkCode == VK_ESCAPE)
                    {
                        GlobalApplicationRunning = false;
                    }
                    else if (vkCode == 'L')
                    {
                        if (isDown)
                        {
                            if (win32State->InputRecordingIndex == 0)
                            {
                                win32_begin_recording_input(win32State, 1);
                            }
                            else
                            {
                                win32_end_recording_input(win32State);
                                win32_begin_play_back_input(win32State, 1);
                            }
                        }
                    }
                }

                bool altKeyWasDown = ((message.lParam & (1 << 29)) != 0);
                
                if ((vkCode == VK_F4) && altKeyWasDown)
                {
                    GlobalApplicationRunning = false;
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

inline FILETIME win32_get_last_write_time(const char* fileName)
{
    FILETIME lastWriteTime = {};

    WIN32_FIND_DATA findData;
    HANDLE fileHandle = FindFirstFileA(fileName, &findData);

    if (fileHandle != INVALID_HANDLE_VALUE)
    {
        lastWriteTime = findData.ftLastWriteTime;
        FindClose(fileHandle);
    }

    return lastWriteTime;
}

internal win32_game_code win32_load_game_code(const char* sourceDLLName, const char* tempDLLName, FILETIME lastDLLWriteTime)
{
    win32_game_code result = {};

    result.LastDLLWriteTime = lastDLLWriteTime;
    CopyFile(sourceDLLName, tempDLLName, FALSE);
    result.GameCodeDLL = LoadLibraryA(tempDLLName);

    if (result.GameCodeDLL)
    {
        result.Init = (game_init*)GetProcAddress(result.GameCodeDLL, "fn_game_init");
        result.ProcessInput = (game_process_input*)GetProcAddress(result.GameCodeDLL, "fn_game_process_input");
        result.Tick = (game_tick*)GetProcAddress(result.GameCodeDLL, "fn_game_tick");
        result.Render = (game_render*)GetProcAddress(result.GameCodeDLL, "fn_game_render");
        result.OutputSound = (game_output_sound*)GetProcAddress(result.GameCodeDLL, "fn_game_output_sound");

        result.IsValid = result.Init && result.ProcessInput && result.Tick && result.Render && result.OutputSound;
    }

    if (!result.IsValid)
    {
        result.Init = fn_game_init_stub;
        result.ProcessInput = fn_game_process_input_stub;
        result.Tick = fn_game_tick_stub;
        result.Render = fn_game_render_stub;
        result.OutputSound = fn_game_output_sound_stub;
    }

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
    gameCode->Init = fn_game_init_stub;
    gameCode->ProcessInput = fn_game_process_input_stub;
    gameCode->Tick = fn_game_tick_stub;
    gameCode->Render = fn_game_render_stub;
    gameCode->OutputSound = fn_game_output_sound_stub;
}

internal void concat_strings(
    size_t sourceACount, char* sourceA, 
    size_t sourceBCount, char* sourceB, 
    size_t destCount, char* dest)
{
    for (int i = 0; i < sourceACount; i++)
    {
        *dest++ = *sourceA++;
    }

    for (int i = 0; i < sourceBCount; i++)
    {
        *dest++ = *sourceB++;
    }

    *dest++ = 0;
}

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR commandLine, int showCode)
{
    char exeFileName[MAX_PATH];

    DWORD sizeOfFileName = GetModuleFileNameA(0, exeFileName, sizeof(exeFileName));
    char* onePastLastSlash = exeFileName;
    for (char* scan = exeFileName; *scan; scan++)
    {
        if (*scan == '\\')
        {
            onePastLastSlash = scan + 1;
        }
    }

    char sourceGameCodeDLLFileName[] = "game.dll";
    char sourceGameCodeDLLFullPath[MAX_PATH];

    concat_strings(onePastLastSlash - exeFileName, exeFileName, 
        sizeof(sourceGameCodeDLLFileName) - 1, sourceGameCodeDLLFileName, 
        sizeof(sourceGameCodeDLLFullPath), sourceGameCodeDLLFullPath);

    char tempGameCodeDLLFileName[] = "game_temp.dll";
    char tempGameCodeDLLFullPath[MAX_PATH];

    concat_strings(onePastLastSlash - exeFileName, exeFileName,
        sizeof(tempGameCodeDLLFileName) - 1, tempGameCodeDLLFileName,
        sizeof(tempGameCodeDLLFullPath), tempGameCodeDLLFullPath);

    WNDCLASSEX windowClass = {};
    windowClass.cbSize = sizeof(WNDCLASSEX);
    windowClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    windowClass.lpfnWndProc = win32_window_callback;
    windowClass.hInstance = hInstance;
    windowClass.lpszClassName = "FarNorthWindowClass";
    
    if (RegisterClassEx(&windowClass))
    {
        const char* applicationName = "Far North Engine v0.01";
        
        HWND window = CreateWindowEx(0,
                                     windowClass.lpszClassName,
                                     applicationName,
                                     WS_OVERLAPPEDWINDOW | WS_VISIBLE,
                                     CW_USEDEFAULT,
                                     CW_USEDEFAULT,
                                     CW_USEDEFAULT,
                                     CW_USEDEFAULT,
                                     0,
                                     0,
                                     hInstance,
                                     0);
        
        if (window)
        {
            HDC deviceContext = GetDC(window);

            win32_state win32State = {};

            game_memory gameMemory = {};

            gameMemory.PermanentStorageSize = Megabytes(64);
            gameMemory.TransientStorageSize = Gigabytes((uint64)4);

            uint64 totalSize = gameMemory.PermanentStorageSize + gameMemory.TransientStorageSize;
            win32State.TotalGameMemorySize = totalSize;
            win32State.GameMemoryBlock = VirtualAlloc(0, totalSize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
            gameMemory.PermanentStorage = win32State.GameMemoryBlock;
            gameMemory.TransientStorage = (uint64*)gameMemory.PermanentStorage + gameMemory.PermanentStorageSize;

            gameMemory.PlatformWriteFile = PlatformWriteFile;
            gameMemory.PlatformFreeFile = PlatformFreeFile;
            gameMemory.PlatformReadFile = PlatformReadFile;
            gameMemory.PlatformDebugLog = PlatformDebugLog;

            win32_input_load_xinput();
            win32_window_init_offscreen_buffer(&GlobalBackBuffer, 1280, 720);

            win32_sound_output_buffer soundOutput = {};

            soundOutput.SamplesPerSecond = 48000;
            soundOutput.BytesPerSample = sizeof(int16) * 2;
            soundOutput.ToneHz = 256;
            soundOutput.ToneVolume = 1000;
            soundOutput.WavePeriod = soundOutput.SamplesPerSecond / soundOutput.ToneHz;
            soundOutput.BufferSize = soundOutput.SamplesPerSecond * soundOutput.BytesPerSample;
            soundOutput.LatencySampleCount = soundOutput.SamplesPerSecond / 15;

            int16* samples = (int16*)VirtualAlloc(0, soundOutput.BufferSize, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);

            win32_audio_init_dsound(window, soundOutput.SamplesPerSecond, soundOutput.BufferSize);
            win32_audio_clear_sound_buffer(&soundOutput);

            GlobalSoundBuffer->Play(0, 0, DSBPLAY_LOOPING);
            
            GlobalApplicationRunning = true;

            game_input input[2] = {};
            game_input* oldInput = &input[0];
            game_input* newInput = &input[1];

            LARGE_INTEGER perfCountFrequencyResult;
            QueryPerformanceFrequency(&perfCountFrequencyResult);

            uint64 perfCountFrequency = perfCountFrequencyResult.QuadPart;

            LARGE_INTEGER lastCounter;
            QueryPerformanceCounter(&lastCounter);

            uint64 lastCycleCount = __rdtsc();

            FILETIME DLLWriteTime = win32_get_last_write_time(sourceGameCodeDLLFullPath);
            win32_game_code game = win32_load_game_code(sourceGameCodeDLLFullPath, tempGameCodeDLLFullPath, DLLWriteTime);
            game.Init(&gameMemory);
            
            while (GlobalApplicationRunning)
            {
                FILETIME newDLLWriteTime = win32_get_last_write_time(sourceGameCodeDLLFullPath);

                if (CompareFileTime(&newDLLWriteTime, &game.LastDLLWriteTime) != 0)
                {
                    win32_unload_game_code(&game);
                    game = win32_load_game_code(sourceGameCodeDLLFullPath, tempGameCodeDLLFullPath, DLLWriteTime);
                }

                win32_process_messages(&win32State, newInput);
                win32_input_poll_gamepad(oldInput, newInput);

                if (win32State.InputRecordingIndex)
                {
                    win32_record_input(&win32State, newInput);
                }

                if (win32State.InputPlayingIndex)
                {
                    win32_play_back_input(&win32State, newInput);
                }

                game.ProcessInput(&gameMemory, newInput);

                game.Tick(&gameMemory);

                game_offscreen_buffer buffer = {};
                buffer.Width = GlobalBackBuffer.Width;
                buffer.Height = GlobalBackBuffer.Height;
                buffer.Pitch = GlobalBackBuffer.Pitch;
                buffer.Data = GlobalBackBuffer.Data;

                game.Render(&gameMemory, &buffer);

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

                game.OutputSound(&gameMemory, &soundBuffer);

                if (soundIsValid)
                {
                    win32_audio_fill_sound_buffer(&soundOutput, byteToLock, bytesToWrite, &soundBuffer);
                }

                win32_window_dimension dimension = win32_window_get_dimension(window);
                win32_window_update(deviceContext, &GlobalBackBuffer, dimension.Width, 
                    dimension.Height, 0, 0, dimension.Width, dimension.Height);

                uint64 endCycleCount = __rdtsc();    

                LARGE_INTEGER endCounter;
                QueryPerformanceCounter(&endCounter);

                uint64 cyclesElapsed = endCycleCount - lastCycleCount;
                int64 counterElapsed = endCounter.QuadPart - lastCounter.QuadPart;
                
                float msPerFrame = (float)((1000.0f * (float)counterElapsed) / (float)perfCountFrequency);
                float fps = (float)perfCountFrequency / (float)counterElapsed;
                float megaCyclesPerFrame = (float)(cyclesElapsed / (1000.0f * 1000.0f));

                char printBuffer[256];
                sprintf_s(printBuffer, "%f ms/frame - %f FPS - %f MegaCycles/frame\n", msPerFrame, fps, megaCyclesPerFrame);
                OutputDebugStringA(printBuffer);

                lastCounter = endCounter;
                lastCycleCount = endCycleCount;

                game_input* temp = newInput;
                newInput = oldInput;
                oldInput = temp;
            }
        }
    }
    
    return 0;
}