#define GLEW_STATIC
#include "../../dependencies/GLEW/include/GL/glew.h"
#include "../../dependencies/GLEW/include/GL/wglew.h"

#include "fn_renderer.h"

internal void fn_renderer_initalize()
{
    glewInit();

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    //glEnable(GL_FRAMEBUFFER_SRGB);
}

internal fn_mesh fn_renderer_mesh_create(fn_mesh_data* data, bool hasTangents = true)
{
    fn_mesh result = {};

    uint32 vaoId;
    glGenVertexArrays(1, &vaoId);
    glBindVertexArray(vaoId);

    uint32 vboId;
    glGenBuffers(1, &vboId);

    uint32 iboId;
    glGenBuffers(1, &iboId);

    glBindBuffer(GL_ARRAY_BUFFER, vboId);

    GLsizei vertexSize = hasTangents ? sizeof(fn_vertex) : sizeof(fn_vertex) - sizeof(vec3f);

    glBufferData(GL_ARRAY_BUFFER, vertexSize * (GLsizei)data->VerticesCount, data->Vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboId);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32) * data->IndicesCount, data->Indices, GL_STATIC_DRAW);

    // describe the vertex buffer data layout
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vertexSize, (void*)0);
    
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, vertexSize, (void*)offsetof(fn_vertex, Normal));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, vertexSize, (void*)offsetof(fn_vertex, TextureCoords));

    if (hasTangents)
    {
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, vertexSize, (void*)offsetof(fn_vertex, Tangent));
    }

    glBindVertexArray(0);
    
    result.vaoId = vaoId;
    result.vboId = vboId;
    result.iboId = iboId;
    result.Data = *data;

    return result;
}

internal fn_mesh_data fn_renderer_mesh_generate_plane(uint32 vertexCount, size_t size)
{
    fn_mesh_data result = {};



    return result;
}

internal fn_mesh_quad fn_renderer_mesh_create_quad()
{
    fn_mesh_quad result = {};

    f32 positions[] = 
    { 
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f 
    };

    uint32 vaoId, vboId;
    glGenVertexArrays(1, &vaoId);
    glBindVertexArray(vaoId);
    glGenBuffers(1, &vboId);
    glBindBuffer(GL_ARRAY_BUFFER, vboId);
    glBufferData(GL_ARRAY_BUFFER, sizeof(positions), &positions, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(f32), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(f32), (void*)(2 * sizeof(f32)));
    glBindVertexArray(0);
    result.vaoId = vaoId;
    result.vboId = vboId;
    return result;
}

internal fn_mesh_cube fn_renderer_mesh_create_cube()
{
    f32 skyboxVertices[] = {      
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };

    fn_mesh_cube result = {};

    uint32 vaoId, vboId;
    glGenVertexArrays(1, &vaoId);
    glBindVertexArray(vaoId);
    glGenBuffers(1, &vboId);
    glBindBuffer(GL_ARRAY_BUFFER, vboId);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(f32), (void*)0);
    glBindVertexArray(0);
    result.vaoId = vaoId;
    result.vboId = vboId;
    return result;
}

internal fn_texture fn_renderer_texture_create(const char* fileName, fn_texture_type type)
{
    fn_texture result = {};

    uint32 id;

    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);

    stbi_set_flip_vertically_on_load(true);
    int32 width;
    int32 height;
    int32 nrChannels;

    uint8* data = stbi_load(fileName, &width, &height, &nrChannels, 0);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

    stbi_image_free(data);

    // @TODO(Anders): fix gamma correction
    /*if (type == fn_texture_type::TextureType_Diffuse)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    else
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);*/

    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    result.Id = id;
    result.Type = type;

    return result;
}

internal fn_texture fn_renderer_texture_cubemap_create(const char** cubemaps, uint32 count)
{
    fn_texture result = {};
    uint32 id;

    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_CUBE_MAP, id);

    int32 w, h, comp;

    for (uint32 i = 0; i < count; i++)
    {
        const char* fileName = *cubemaps++;
        stbi_set_flip_vertically_on_load(false);
        uint8* data = stbi_load(fileName, &w, &h, &comp, 0);
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        stbi_image_free(data);
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    result.Id = id;

    return result;
}

internal fn_framebuffer fn_renderer_framebuffer_create(int32 width, int32 height)
{
    fn_framebuffer result = {};

    uint32 fboId;
    glGenFramebuffers(1, &fboId);
    
    glBindFramebuffer(GL_FRAMEBUFFER, fboId);

    uint32 textureId;
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // attach the texture to the framebuffer as a color buffer attachment
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureId, 0);

    uint32 rboId;
    glGenRenderbuffers(1, &rboId);
    glBindRenderbuffer(GL_RENDERBUFFER, rboId);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);

    // attach the render buffer object to the framebuffer as a depth and stencil buffer attachment (24 bits for depth and 8 bits for stencil)
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rboId);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        // @TODO(Anders E): error logging
    }

    result.FrameBufferId = fboId;
    result.ColorBufferId = textureId;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return result;
}

internal void fn_renderer_framebuffer_enable(const fn_framebuffer* fbo)
{
    glBindFramebuffer(GL_FRAMEBUFFER, fbo->FrameBufferId);
}

internal void fn_renderer_framebuffer_disable()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

internal fn_shader fn_renderer_shader_create(const fn_shader_data* shaderData)
{
    fn_shader result = {};

    uint32 vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
    uint32 fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);

    const char* vertexShaderCode = static_cast<const char*>(shaderData->VertexShader.Data);
    const char* fragmentShaderCode = static_cast<const char*>(shaderData->FragmentShader.Data);

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

    result.ShaderProgramId = shaderProgramId;
    result.Data = *shaderData;

    return result;
}

internal void fn_renderer_shader_enable(const fn_shader* shader)
{
    glUseProgram(shader->ShaderProgramId);
}

internal void fn_renderer_shader_disable()
{
    glUseProgram(0);
}

internal void fn_renderer_shader_load_mat4(const fn_shader* shader, const char* uniformName, const mat4* value)
{
    // @TODO(Anders E): Cache the uniformlocation in a hash table to avoid calling glGetUniformLocation on every load call. Store in shader maybe?
    uint32 uniformLocation = glGetUniformLocation(shader->ShaderProgramId, uniformName);
    glUniformMatrix4fv(uniformLocation, 1, GL_TRUE, value->Data);
}

internal void fn_renderer_shader_load_vec2f(const fn_shader* shader, const char* uniformName, vec2f value)
{
    // @TODO(Anders E): Cache the uniformlocation in a hash table to avoid calling glGetUniformLocation on every load call. Store in shader maybe?
    uint32 uniformLocation = glGetUniformLocation(shader->ShaderProgramId, uniformName);
    glUniform2f(uniformLocation, value.x, value.y);
}

internal void fn_renderer_shader_load_vec3f(const fn_shader* shader, const char* uniformName, vec3f value)
{
    // @TODO(Anders E): Cache the uniformlocation in a hash table to avoid calling glGetUniformLocation on every load call. Store in shader maybe?
    uint32 uniformLocation = glGetUniformLocation(shader->ShaderProgramId, uniformName);
    glUniform3f(uniformLocation, value.x, value.y, value.z);
}

internal void fn_renderer_shader_load_f32(const fn_shader* shader, const char* uniformName, f32 value)
{
    // @TODO(Anders E): Cache the uniformlocation in a hash table to avoid calling glGetUniformLocation on every load call. Store in shader maybe?
    uint32 uniformLocation = glGetUniformLocation(shader->ShaderProgramId, uniformName);
    glUniform1f(uniformLocation, value);
}

internal void fn_renderer_shader_load_int32(const fn_shader* shader, const char* uniformName, int32 value)
{
    // @TODO(Anders E): Cache the uniformlocation in a hash table to avoid calling glGetUniformLocation on every load call. Store in shader maybe?
    uint32 uniformLocation = glGetUniformLocation(shader->ShaderProgramId, uniformName);
    glUniform1i(uniformLocation, value);
}

internal void fn_renderer_shader_load_int32(const fn_shader* shader, const char* uniformName, bool value)
{
    // @TODO(Anders E): Cache the uniformlocation in a hash table to avoid calling glGetUniformLocation on every load call. Store in shader maybe?
    uint32 uniformLocation = glGetUniformLocation(shader->ShaderProgramId, uniformName);
    glUniform1i(uniformLocation, (int32)value);
}