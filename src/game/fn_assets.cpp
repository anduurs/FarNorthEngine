#define STB_IMAGE_IMPLEMENTATION
#include "../../dependencies/stb_image/stb_image.h"

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

struct fn_shader
{
    uint32 Id;
    char* VertexShaderCode;
    char* FragmentShaderCode;
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
    AssetType_Shader,
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

// @TODO: impl fn_texture fn_assets_texture_load(platform_api* platformAPI, char* fileName)
// @TODO: impl fn_mesh fn_assets_mesh_load(platform_api* platformAPI, char* fileName)
// @TODO: impl fn_shader fn_assets_shader_load(platform_api* platformAPI, char* fileName)

internal fn_bitmap fn_assets_bitmap_load(platform_api* platformAPI, char* fileName)
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