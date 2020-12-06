#pragma once

struct fn_camera
{
    f32 FoV;
    f32 zNear;
    f32 zFar;

    vec3f Position;
    quaternion Rotation;

    mat4 ProjectionMatrix;

    bool MoveForward;
    bool MoveBack;
    bool MoveRight;
    bool MoveLeft;

    float CurrentMouseX;
    float CurrentMouseY;
    float PreviousMouseX;
    float PreviousMouseY;
};

struct fn_bitmap
{
    uint8* Data;
    int32 Width;
    int32 Height;
};

struct fn_vertex
{
    vec3f Position;
    vec3f Normal;
    vec2f TextureCoords;
};

struct fn_mesh
{
    uint32 Id;

    uint32 VerticesCount;
    fn_vertex* Vertices;

    uint32 IndicesCount;
    uint32* Indices;
};

enum fn_shader_type
{
    ShaderType_Standard,
    ShaderType_Count
};

struct fn_shader
{
    uint32 Id;
    platform_file_result VertexShaderData;
    platform_file_result FragmentShaderData;
};

enum fn_texture_type
{
    TextureType_Diffuse,
    TextureType_Specular,
    TextureType_Normal,
    TextureType_Count
};

struct fn_texture
{
    uint32 Id;
    fn_texture_type Type;
};

internal uint32 fn_opengl_mesh_create(const fn_vertex* vertices, uint32 vertexCount, const uint32* indices, uint32 indicesCount);
internal uint32 fn_opengl_texture_create(uint8* data, int32 width, int32 height);
internal uint32 fn_opengl_shader_create(const char* vertexShaderCode, const char* fragmentShaderCode);