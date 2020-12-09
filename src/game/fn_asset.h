#pragma once 

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "fn_renderer.h"

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
    AssetId_Skybox,
    AssetId_Count
};

struct game_assets
{
    struct transient_state* TransientState;
    memory_arena Arena;
    fn_bitmap* Bitmaps[AssetId_Count];
    fn_texture* Textures[AssetId_Count];
    fn_material* Materials[AssetId_Count];
    fn_mesh* Meshes[AssetId_Count];
    fn_shader* Shaders[AssetId_Count];
    platform_api* PlatformAPI;
};