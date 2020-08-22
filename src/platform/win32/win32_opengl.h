#pragma once

#define GLEW_STATIC
#include "../../../dependencies/GLEW/include/GL/glew.h"
#include "../../../dependencies/GLEW/include/GL/wglew.h"

struct win32_opengl_context
{
    HDC DeviceContext;
    HGLRC OpenGLContext;
};