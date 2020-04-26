#include "opengl_renderer.h"

internal uint32 opengl_create_vertex_buffer(const float* vertices, const uint32* indices)
{
    uint32 vaoId;
    glGenVertexArrays(1, &vaoId);
    glBindVertexArray(vaoId);
    
    uint32 vboId;
    glGenBuffers(1, &vboId);

    uint32 iboId;
    glGenBuffers(1, &iboId);

    glBindBuffer(GL_ARRAY_BUFFER, vboId);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboId);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    return vaoId;
}

internal uint32 opengl_create_shader_program(const char* vertexShaderCode, const char* fragmentShaderCode)
{
    uint32 vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
    uint32 fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(vertexShaderId, 1, &vertexShaderCode, nullptr);
    glCompileShader(vertexShaderId);

    int success;
    char infoLog[512];
    glGetShaderiv(vertexShaderId, GL_COMPILE_STATUS, &success);
    if (!success)
        OutputDebugStringA(infoLog);

    glShaderSource(fragmentShaderId, 1, &fragmentShaderCode, nullptr);
    glCompileShader(fragmentShaderId);

    int success2;
    char infoLog2[512];
    glGetShaderiv(fragmentShaderId, GL_COMPILE_STATUS, &success2);
    if (!success2)
        OutputDebugStringA(infoLog2);

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
        OutputDebugStringA(infoLog3);
    }

    glDeleteShader(vertexShaderId);
    glDeleteShader(fragmentShaderId);

    return shaderProgramId;
}

internal void opengl_render(uint32 shaderProgramId, uint32 vertexArrayObjectId)
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(shaderProgramId);
    glBindVertexArray(vertexArrayObjectId);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    glUseProgram(0);
}