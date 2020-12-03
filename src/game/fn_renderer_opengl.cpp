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

    // describe the vertex buffer data layout
    // X - Y - Z - U - V
    //Stride = 4byte + 4byte + 4byte + 4byte + 4byte -> 5 * sizeof(f32)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(f32), (void*)0);
    glEnableVertexAttribArray(0);

    //glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(f32), (void*)(3 * sizeof(f32)));
    //glEnableVertexAttribArray(1);
    
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
    // @TODO(Anders E): log errors

    glShaderSource(fragmentShaderId, 1, &fragmentShaderCode, nullptr);
    glCompileShader(fragmentShaderId);

    glGetShaderiv(fragmentShaderId, GL_COMPILE_STATUS, &success);

    // @TODO(Anders E): log errors

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
        // @TODO(Anders E): log errors
    }

    glDeleteShader(vertexShaderId);
    glDeleteShader(fragmentShaderId);

    return shaderProgramId;
}

internal void fn_opengl_shader_load_mat4(const fn_shader* shader, const char* uniformName, const mat4* value)
{
    // @TODO(Anders E): Cache the uniformlocation in a hash table to avoid calling glGetUniformLocation on every load call. Store in shader maybe?
    uint32 uniformLocation = glGetUniformLocation(shader->Id, uniformName);
    glUniformMatrix4fv(uniformLocation, 1, GL_TRUE, value->Data);
}

internal void fn_opengl_shader_load_vec2f(const fn_shader* shader, const char* uniformName, vec2f value)
{
    // @TODO(Anders E): Cache the uniformlocation in a hash table to avoid calling glGetUniformLocation on every load call. Store in shader maybe?
    uint32 uniformLocation = glGetUniformLocation(shader->Id, uniformName);
    glUniform2f(uniformLocation, value.x, value.y);
}

internal void fn_opengl_shader_load_vec3f(const fn_shader* shader, const char* uniformName, vec3f value)
{
    // @TODO(Anders E): Cache the uniformlocation in a hash table to avoid calling glGetUniformLocation on every load call. Store in shader maybe?
    uint32 uniformLocation = glGetUniformLocation(shader->Id, uniformName);
    glUniform3f(uniformLocation, value.x, value.y, value.z);
}

internal void fn_opengl_shader_load_f32(const fn_shader* shader, const char* uniformName, f32 value)
{
    // @TODO(Anders E): Cache the uniformlocation in a hash table to avoid calling glGetUniformLocation on every load call. Store in shader maybe?
    uint32 uniformLocation = glGetUniformLocation(shader->Id, uniformName);
    glUniform1f(uniformLocation, value);
}

internal void fn_opengl_shader_load_int32(const fn_shader* shader, const char* uniformName, int32 value)
{
    // @TODO(Anders E): Cache the uniformlocation in a hash table to avoid calling glGetUniformLocation on every load call. Store in shader maybe?
    uint32 uniformLocation = glGetUniformLocation(shader->Id, uniformName);
    glUniform1i(uniformLocation, value);
}

internal void fn_opengl_shader_load_int32(const fn_shader* shader, const char* uniformName, bool value)
{
    // @TODO(Anders E): Cache the uniformlocation in a hash table to avoid calling glGetUniformLocation on every load call. Store in shader maybe?
    uint32 uniformLocation = glGetUniformLocation(shader->Id, uniformName);
    glUniform1i(uniformLocation, (int32)value);
}