#include "opengl_renderer.h"

internal void opengl_initalize()
{
    glewInit();

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
}

internal uint32 opengl_create_vertex_buffer(const float* vertices, uint32 vertexCount, const uint16* indices, uint32 indicesCount)
{
    uint32 vaoId;
    glGenVertexArrays(1, &vaoId);
    glBindVertexArray(vaoId);
    
    uint32 vboId;
    glGenBuffers(1, &vboId);

    uint32 iboId;
    glGenBuffers(1, &iboId);

    glBindBuffer(GL_ARRAY_BUFFER, vboId);
 
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertexCount, vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboId);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint16) * indicesCount, indices, GL_STATIC_DRAW);
    
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

internal void opengl_load_matrix(const fn_shader* shader, const char* uniformName, const mat4* matrix)
{
    uint32 uniformLocation = glGetUniformLocation(shader->Id, uniformName);
    glUniformMatrix4fv(uniformLocation, 1, GL_TRUE, matrix->Data);
}

internal fn_texture opengl_create_texture(const char* imageFileName)
{
    fn_texture texture = {};

    glGenTextures(1, &texture.Id);

    glBindTexture(GL_TEXTURE_2D, texture.Id);

    int width;
    int height;
    int channels;

    stbi_set_flip_vertically_on_load(true);
    uint8* imageData = stbi_load(imageFileName, &width, &height, &channels, 0);
    
    if (imageData)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, imageData);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    }

    stbi_image_free(imageData);

    return texture;
}

internal void opengl_render_frame(fn_camera* camera, fn_entity* entities, uint32 count)
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for (uint32 i = 0; i < count; i++)
    {
        fn_entity* entityToRender = &entities[i];
        fn_material* mat = &entityToRender->Material;
        uint32 shaderId = mat->Shader.Id;

        glUseProgram(shaderId);
        
        mat4 localToWorldMatrix = fn_math_mat4_local_to_world(
            entityToRender->Transform.Position, 
            entityToRender->Transform.Rotation, 
            entityToRender->Transform.Scale
        );

        mat4 cameraViewMatrix = fn_math_mat4_camera_view(camera->Position, camera->Rotation);
        mat4 projectionMatrix = camera->ProjectionMatrix;

        opengl_load_matrix(&mat->Shader, "localToWorldMatrix", &localToWorldMatrix);
        opengl_load_matrix(&mat->Shader, "cameraViewMatrix", &cameraViewMatrix);
        opengl_load_matrix(&mat->Shader, "projectionMatrix", &projectionMatrix);

        glBindVertexArray(entityToRender->Mesh.Id);
        int size;
        glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
        glDrawElements(GL_TRIANGLES, size / sizeof(uint16), GL_UNSIGNED_SHORT, 0);

        glBindVertexArray(0);
        glUseProgram(0);
    }
}