struct win32_window_info
{
    bool IsValid;
    HWND WindowHandle;
    HDC DeviceContext;
};

struct win32_offscreen_buffer
{
    BITMAPINFO Info;
    int32 Width;
    int32 Height;
    int32 Pitch;
    int32 BytesPerPixel;
    void* Data;
};

struct win32_window_dimension
{
    int32 Width;
    int32 Height;
};

struct win32_window_center_pos
{
    RECT WindowRect;
    int32 CenterX;
    int32 CenterY;
};

global win32_offscreen_buffer GlobalBackBuffer;

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
    buffer->Data = VirtualAlloc(0, bitmapMemorySize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

    buffer->Pitch = width * buffer->BytesPerPixel;
}

internal void win32_window_blit_to_screen(HDC deviceContext, win32_offscreen_buffer* buffer,
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

global WINDOWPLACEMENT GlobalWindowPlacement = { sizeof(GlobalWindowPlacement) };
internal void win32_window_toggle_fullscreen(HWND window)
{
    DWORD windowStyle = GetWindowLong(window, GWL_STYLE);

    if (windowStyle & WS_OVERLAPPEDWINDOW) {
        MONITORINFO monitorInfo = { sizeof(monitorInfo) };

        if (GetWindowPlacement(window, &GlobalWindowPlacement)
            && GetMonitorInfo(MonitorFromWindow(window, MONITOR_DEFAULTTOPRIMARY),
                &monitorInfo)) {
            SetWindowLong(window, GWL_STYLE, windowStyle & ~WS_OVERLAPPEDWINDOW);
            SetWindowPos(window,
                HWND_TOP,
                monitorInfo.rcMonitor.left,
                monitorInfo.rcMonitor.top,
                monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left,
                monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top,
                SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
        }
    }
    else {
        SetWindowLong(window, GWL_STYLE, windowStyle | WS_OVERLAPPEDWINDOW);
        SetWindowPlacement(window, &GlobalWindowPlacement);
        SetWindowPos(window, NULL, 0, 0, 0, 0,
            SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
    }
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

    //case WM_PAINT:
    //{
    //    PAINTSTRUCT paint;
    //    HDC deviceContext = BeginPaint(window, &paint);
    //    int32 x = paint.rcPaint.left;
    //    int32 y = paint.rcPaint.top;
    //    int32 width = paint.rcPaint.right - paint.rcPaint.left;
    //    int32 height = paint.rcPaint.bottom - paint.rcPaint.top;
    //    win32_window_dimension dimension = win32_window_get_dimension(window);
    //    win32_window_blit_to_screen(deviceContext, &GlobalBackBuffer, dimension.Width, dimension.Height, x, y, width, height);
    //    EndPaint(window, &paint);
    //} break;

    default:
    {
        result = DefWindowProc(window, message, wParam, lParam);
    } break;
    }

    return result;
}

internal void win32_window_center(HWND window, DWORD style, DWORD exStyle)
{
    int32 screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int32 screenHeight = GetSystemMetrics(SM_CYSCREEN);    
    RECT clientRect;
    GetClientRect(window, &clientRect);    
    AdjustWindowRectEx(&clientRect, style, FALSE, exStyle);    
    int32 clientWidth = clientRect.right - clientRect.left;
    int32 clientHeight = clientRect.bottom - clientRect.top;    
    SetWindowPos(window, NULL,
        screenWidth / 2 - clientWidth / 2,
        screenHeight / 2 - clientHeight / 2,
        clientWidth, clientHeight, 0);
}

internal win32_window_center_pos win32_window_get_center(HWND window)
{
    win32_window_center_pos result = {};

    RECT windowRect;

    int32 screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int32 screenHeight = GetSystemMetrics(SM_CYSCREEN);

    GetWindowRect(window, &windowRect);

    if (windowRect.left < 0)
        windowRect.left = 0;

    if (windowRect.top < 0)
        windowRect.top = 0;

    if (windowRect.right >= screenWidth)
        windowRect.right = screenWidth - 1;

    if (windowRect.bottom >= screenHeight - 1)
        windowRect.bottom = screenHeight - 1;

    result.CenterX = (windowRect.right + windowRect.left) / 2;
    result.CenterY = (windowRect.top + windowRect.bottom) / 2;

    result.WindowRect = windowRect;

    return result;
}

internal void win32_window_reset_mouse_cursor(HWND window, win32_window_center_pos center)
{
    SetCursorPos(center.CenterX, center.CenterY);
    SetCapture(window);
    ClipCursor(&center.WindowRect);

    while (ShowCursor(FALSE) >= 0);
}

internal void win32_window_deactivate_mouse()
{
    ClipCursor(NULL);
    ReleaseCapture();

    while (ShowCursor(TRUE) < 0);
}

internal win32_window_info win32_window_create
(
    HINSTANCE hInstance, 
    const char* applicationName, 
    int32 width, 
    int32 height
)
{
    WNDCLASSEX windowClass = {};
    windowClass.cbSize = sizeof(WNDCLASSEX);
    windowClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    windowClass.lpfnWndProc = win32_window_callback;
    windowClass.hInstance = hInstance;
    windowClass.lpszClassName = applicationName;

    win32_window_info result = {};
    result.IsValid = false;

    if (RegisterClassEx(&windowClass))
    {
        HWND window = CreateWindowEx(
            0,
            windowClass.lpszClassName,
            applicationName,
            WS_OVERLAPPEDWINDOW | WS_VISIBLE,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            width,
            height,
            0,
            0,
            hInstance,
            0
        );

        if (window)
        {
            win32_window_center(window, windowClass.style, 0);
            auto center = win32_window_get_center(window);
            win32_window_reset_mouse_cursor(window, center);
            
            HDC deviceContext = GetDC(window);

            if (deviceContext)
            {
                result.WindowHandle = window;
                result.DeviceContext = deviceContext;
                result.IsValid = true;
            }
        }
    }

    return result;
}