#define GLEW_STATIC
#include "../../dependencies/GLEW/include/GL/glew.h"
#include "../../dependencies/GLEW/include/GL/wglew.h"

internal void fn_opengl_initalize()
{
    glewInit();

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
}
