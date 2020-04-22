#pragma once

#define GLEW_STATIC
#include "../../../dependencies/GLEW/include/GL/glew.h"
#include "../../../dependencies/GLEW/include/GL/wglew.h"

internal HGLRC win32_opengl_init(HDC deviceContext);
internal void win32_opengl_shutdown(HDC deviceContext, HGLRC context);