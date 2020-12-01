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

internal uint32 fn_opengl_mesh_create(const float* vertices, uint32 vertexCount, const uint16* indices, uint32 indicesCount)
{
    uint32 vaoId;
    glGenVertexArrays(1, &vaoId);
    glBindVertexArray(vaoId);

    uint32 vboId;
    glGenBuffers(1, &vboId);

    uint32 iboId;
    glGenBuffers(1, &iboId);

    glBindBuffer(GL_ARRAY_BUFFER, vboId);

    glBufferData(GL_ARRAY_BUFFER, sizeof(f32) * vertexCount, vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboId);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint16) * indicesCount, indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(f32), (void*)0);
    glEnableVertexAttribArray(0);

    return vaoId;
}

internal uint32 fn_opengl_texture_create(uint8* data, int32 width, int32 height)
{
    uint32 id;

    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    return id;
}

internal uint32 fn_opengl_shader_create(const char* vertexShaderCode, const char* fragmentShaderCode)
{
    uint32 vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
    uint32 fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(vertexShaderId, 1, &vertexShaderCode, nullptr);
    glCompileShader(vertexShaderId);

    int success;

    glGetShaderiv(vertexShaderId, GL_COMPILE_STATUS, &success);

    glShaderSource(fragmentShaderId, 1, &fragmentShaderCode, nullptr);
    glCompileShader(fragmentShaderId);

    glGetShaderiv(fragmentShaderId, GL_COMPILE_STATUS, &success);

    uint32 shaderProgramId = glCreateProgram();

    glAttachShader(shaderProgramId, vertexShaderId);
    glAttachShader(shaderProgramId, fragmentShaderId);
    glLinkProgram(shaderProgramId);

    int success3;
    char infoLog3[512];
    glGetProgramiv(shaderProgramId, GL_LINK_STATUS, &success3);
    if (!success3)
    {
        glGetProgramInfoLog(shaderProgramId, 512, NULL, infoLog3);
    }

    glDeleteShader(vertexShaderId);
    glDeleteShader(fragmentShaderId);

    return shaderProgramId;
}

internal void fn_opengl_shader_mat4_load(const fn_shader* shader, const char* uniformName, const mat4* matrix)
{
    uint32 uniformLocation = glGetUniformLocation(shader->Id, uniformName);
    glUniformMatrix4fv(uniformLocation, 1, GL_TRUE, matrix->Data);
}