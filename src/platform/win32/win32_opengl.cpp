#include "win32_opengl.h"

internal HGLRC win32_opengl_context_create(HDC deviceContext, int32 viewportX, int32 viewportY, int32 viewportWidth, int32 viewportHeight)
{
    PIXELFORMATDESCRIPTOR pfd = {};

    pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;
    pfd.iPixelType = PFD_TYPE_RGBA; // The kind of framebuffer. RGBA or palette.
    pfd.cColorBits = 32; // Number of bits for the color buffer
    pfd.cDepthBits = 32; // Number of bits for the depth buffer
    pfd.cStencilBits = 8; // Number of bits for the stencil buffer
    pfd.iLayerType = PFD_MAIN_PLANE;

    int pixelFormat = ChoosePixelFormat(deviceContext, &pfd);
    if (pixelFormat == 0) 
    { 
        OutputDebugStringA("Failed to match a pixel format supported by a device context to the given pixel format specification \n");
        return nullptr; 
    }

    BOOL result = SetPixelFormat(deviceContext, pixelFormat, &pfd);
    if (!result)
    {
        OutputDebugStringA("Failed to set the pixel format for the specified device context \n");
        return nullptr;
    }

    HGLRC tempContext = wglCreateContext(deviceContext);
    wglMakeCurrent(deviceContext, tempContext);

    GLenum initalized = glewInit();
    if (initalized != GLEW_OK)
    {
        OutputDebugStringA("Failed to initialize GLEW \n");
        return nullptr;
    }

    int attribs[] =
    {
        WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
        WGL_CONTEXT_MINOR_VERSION_ARB, 6,
        WGL_CONTEXT_FLAGS_ARB, 0,
        0
    };

    HGLRC glRenderContext = nullptr;

    if (wglewIsSupported("WGL_ARB_create_context") == 1)
    {
        glRenderContext = wglCreateContextAttribsARB(deviceContext, 0, attribs);
        wglMakeCurrent(NULL, NULL);
        wglDeleteContext(tempContext);
        wglMakeCurrent(deviceContext, glRenderContext);
    }
    else
    {
        glRenderContext = tempContext;
    }

    OutputDebugStringA("OPENGL VERSION \n");
    OutputDebugStringA((char*)glGetString(GL_VERSION));
    OutputDebugStringA("\n");

    glViewport(viewportX, viewportY, viewportWidth, viewportHeight);

    return glRenderContext;
}

internal void win32_opengl_context_destroy(HDC deviceContext, HGLRC context)
{
    wglMakeCurrent(deviceContext, nullptr);
    wglDeleteContext(context);
}

internal void win32_opengl_swap_buffers(HDC deviceContext)
{
    SwapBuffers(deviceContext);
}



