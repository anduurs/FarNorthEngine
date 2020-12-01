#define STB_IMAGE_IMPLEMENTATION
#include "../../dependencies/stb_image/stb_image.h"

internal uint32 fn_opengl_mesh_create(const float* vertices, uint32 vertexCount, const uint16* indices, uint32 indicesCount);
internal uint32 fn_opengl_texture_create(uint8* data, int32 width, int32 height);
internal uint32 fn_opengl_shader_create(const char* vertexShaderCode, const char* fragmentShaderCode);

struct fn_bitmap
{
    uint8* Data;
    int32 Width;
    int32 Height;
};

struct fn_mesh
{
    uint32 Id;
    uint32 VerticesCount;
    f32* Vertices;
    uint32 IndicesCount;
    uint16* Indices;
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
};

enum game_asset_type
{
    AssetType_Bitmap,
    AssetType_Texture,
    AssetType_Mesh,
    AssetType_Count
};

enum game_asset_id
{
    AssetId_Player,
    AssetId_Container,
    AssetId_Count
};

struct game_assets
{
    struct transient_state* TransientState;
    memory_arena Arena;
    fn_bitmap* Bitmaps[AssetId_Count];
    fn_texture* Textures[AssetId_Count];
    fn_mesh* Meshes[AssetId_Count];
    fn_shader* Shaders[AssetId_Count];
    platform_api* PlatformAPI;
};

internal fn_texture fn_assets_texture_load(const platform_api* platformAPI, const char* fileName)
{
    fn_texture result = {};

    platform_file_result file = platformAPI->ReadFile(fileName);

    int32 width;
    int32 height;
    int32 comp;

    stbi_set_flip_vertically_on_load(true);
    uint8* imageData = stbi_load_from_memory((uint8*)file.Data, (int)file.FileSize, &width, &height, &comp, 0);
    platformAPI->FreeFile(&file);

    result.Id = fn_opengl_texture_create(imageData, width, height);

    stbi_image_free(imageData);

    return result;
}

internal fn_mesh fn_assets_mesh_load(const platform_api* platformAPI, const char* fileName)
{
    fn_mesh result = {};

    // @TODO(Anders E): Import model file with some third party lib

    f32 vertices[] =
    {
        // front
        -1.0, -1.0,  1.0,
         1.0, -1.0,  1.0,
         1.0,  1.0,  1.0,
        -1.0,  1.0,  1.0,
        // back
        -1.0, -1.0, -1.0,
         1.0, -1.0, -1.0,
         1.0,  1.0, -1.0,
        -1.0,  1.0, -1.0
    };

    uint16 indices[] =
    {
        // front
        0, 1, 2,
        2, 3, 0,
        // right
        1, 5, 6,
        6, 2, 1,
        // back
        7, 6, 5,
        5, 4, 7,
        // left
        4, 0, 3,
        3, 7, 4,
        // bottom
        4, 5, 1,
        1, 0, 4,
        // top
        3, 2, 6,
        6, 7, 3
    };

    result.VerticesCount = array_length(vertices);
    result.IndicesCount = array_length(indices);

    // @TODO(Anders E): these needs to be allocated in a mem arena
    result.Vertices = vertices;
    result.Indices = indices;

    result.Id = fn_opengl_mesh_create(vertices, result.VerticesCount, indices, result.IndicesCount);

    return result;
}

internal fn_shader fn_assets_shader_load(const platform_api* platformAPI, const char* vertexShaderFileName, const char* fragmentShaderFileName)
{
    fn_shader result = {};

    platform_file_result vertexShaderFile = platformAPI->ReadFile(vertexShaderFileName);
    platform_file_result fragmentShaderFile = platformAPI->ReadFile(fragmentShaderFileName);

    result.VertexShaderData = vertexShaderFile;
    result.FragmentShaderData = fragmentShaderFile;

    result.Id = fn_opengl_shader_create((const char*)vertexShaderFile.Data, (const char*)fragmentShaderFile.Data);

    return result;
}

internal fn_bitmap fn_assets_bitmap_load(const platform_api* platformAPI, const char* fileName)
{
    fn_bitmap result = {};

    platform_file_result file = platformAPI->ReadFile(fileName);

    int32 width;
    int32 height;
    int32 comp;

    uint8* imageData = stbi_load_from_memory((uint8*)file.Data, (int)file.FileSize, &width, &height, &comp, 4);
    platformAPI->FreeFile(&file);

    result.Data = imageData;
    result.Width = width;
    result.Height = height;

    return result;
}

internal inline fn_bitmap* fn_assets_bitmap_get(game_assets* assets, game_asset_id id)
{
    fn_bitmap* bitmap = assets->Bitmaps[id];
    return bitmap;
}

internal inline fn_mesh* fn_assets_mesh_get(game_assets* assets, game_asset_id id)
{
    fn_mesh* mesh = assets->Meshes[id];
    return mesh;
}

internal inline fn_texture* fn_assets_texture_get(game_assets* assets, game_asset_id id)
{
    fn_texture* texture = assets->Textures[id];
    return texture;
}

internal inline fn_shader* fn_assets_shader_get(game_assets* assets, game_asset_id id)
{
    fn_shader* shader = assets->Shaders[id];
    return shader;
}