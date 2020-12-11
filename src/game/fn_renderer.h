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

struct fn_framebuffer
{
    uint32 FrameBufferId;
    uint32 ColorBufferId;
};

struct fn_vertex
{
    vec3f Position;
    vec3f Normal;
    vec2f TextureCoords;
    vec3f Tangent;
};

struct fn_mesh_quad
{
    uint32 vaoId;
    uint32 vboId;
};

struct fn_mesh_plane
{
    uint32 vaoId;
    uint32 vboId;
    uint32 iboId;

    uint32 VerticesCount;
    vec3f* Positions;
    vec3f* Normals;
    vec2f* TextureCoords;

    uint32 IndicesCount;
    uint32* Indices;
};

struct fn_mesh_cube
{
    uint32 vaoId;
    uint32 vboId;
};

struct fn_mesh_data
{
    uint32 VerticesCount;
    fn_vertex* Vertices;

    uint32 IndicesCount;
    uint32* Indices;
};

struct fn_mesh
{
    uint32 vaoId;
    uint32 vboId;
    uint32 iboId;
    fn_mesh_data Data;
};

enum fn_shader_type
{
    ShaderType_Standard,
    ShaderType_PostProcess,
    ShaderType_Skybox,
    ShaderType_Count
};

struct fn_shader_data
{
    platform_file_result VertexShader;
    platform_file_result FragmentShader;
};

struct fn_shader
{
    uint32 ShaderProgramId;
    fn_shader_data Data;
};

enum fn_texture_type
{
    TextureType_Diffuse,
    TextureType_Specular,
    TextureType_Normal,
    TextureType_Count
};

struct fn_texture_params
{
    bool GenerateMipMaps;
    bool sRGB;
};

struct fn_texture_data
{
    int32 Width;
    int32 Height;
    int32 NrChannels;
    uint8* Data;
    fn_texture_type Type;
    fn_texture_params Params;
};

struct fn_texture
{
    uint32 Id;
    fn_texture_type Type;
};

struct fn_material
{
    fn_texture DiffuseMap;
    fn_texture SpecularMap;
    fn_texture NormalMap;
    float Shininess;
};

struct fn_directional_light
{
    vec3f Color;
    quaternion Rotation;
    f32 Intensity;
};

struct fn_render_state
{
    fn_directional_light DirectionalLight;
    fn_mesh_quad PostProcessQuad;
    fn_shader PostProcessShader;
    fn_framebuffer SceneFramebuffer;
    fn_mesh_cube Skybox;
    fn_shader SkyboxShader;
    fn_texture SkyboxCubemapTexture;
};

internal uint32 fn_opengl_mesh_create(const fn_vertex* vertices, uint32 vertexCount, const uint32* indices, uint32 indicesCount);
internal uint32 fn_opengl_texture_create(uint8* data, int32 width, int32 height, fn_texture_type type);
internal uint32 fn_opengl_shader_create(const char* vertexShaderCode, const char* fragmentShaderCode);