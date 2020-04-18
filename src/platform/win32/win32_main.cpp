#include "win32_main.h"
#include "win32_input.cpp"
#include "win32_audio.cpp"

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

internal void win32_process_messages()
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
                    if (vkCode == 'W')
                    {

                    }
                    else if(vkCode == 'S')
                    {

                    }
                    else if(vkCode == 'D')
                    {

                    }
                    else if(vkCode == 'A')
                    {

                    }
                    else if(vkCode == VK_UP)
                    {

                    }
                    else if(vkCode == VK_DOWN)
                    {

                    }
                    else if(vkCode == VK_RIGHT)
                    {

                    }
                    else if(vkCode == VK_LEFT)
                    {

                    }
                    else if(vkCode == VK_SPACE)
                    {

                    }
                    else if(vkCode == VK_ESCAPE)
                    {

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

internal win32_game_code win32_load_game_code()
{
    win32_game_code result = {};

    const char* sourceDLLName = "game.dll";
    const char* tempDLLName = "game_temp.dll";

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
    file_result result = {};

    HANDLE fileHandle = CreateFile(fileName, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);

    if (fileHandle != INVALID_HANDLE_VALUE)
    {
        LARGE_INTEGER fileSize;
        if (GetFileSizeEx(fileHandle, &fileSize))
        {
            assert(fileSize.QuadPart <= 0xFFFFFFFF);

            uint32 size = (uint32)fileSize.QuadPart;

            void* fileData = VirtualAlloc(0, size, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);

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

int32 CALLBACK WinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPSTR commandLine,
    _In_ int32 showCode)
{
    WNDCLASSEX windowClass = {};
    windowClass.cbSize = sizeof(WNDCLASSEX);
    windowClass.style = CS_HREDRAW | CS_VREDRAW;
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

            game_memory gameMemory = {};

            gameMemory.PermanentStorageSize = Megabytes(64);
            gameMemory.TransientStorageSize = Gigabytes((uint64)4);

            uint64 totalSize = gameMemory.PermanentStorageSize + gameMemory.TransientStorageSize;
            gameMemory.PermanentStorage = VirtualAlloc(0, totalSize, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
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

            win32_game_code game = win32_load_game_code();
            game.Init(&gameMemory);

            uint32 gameCodeLoadCounter = 0;
            
            while (GlobalApplicationRunning)
            {
                if (gameCodeLoadCounter++ > 120)
                {
                    win32_unload_game_code(&game);
                    game = win32_load_game_code();
                    gameCodeLoadCounter = 0;
                }

                win32_process_messages();
                win32_input_poll_gamepad(oldInput, newInput);

                game.ProcessInput(newInput);

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