#define STB_IMAGE_IMPLEMENTATION
#include "../../dependencies/stb_image/stb_image.h"

struct fn_bitmap
{
    uint8* Data;
    int32 Width;
    int32 Height;
};

enum game_asset_id
{
    GAI_Player,
    GAI_Count
};

struct game_assets
{
    struct transient_state* TransientState;
    memory_arena Arena;
    fn_bitmap* Bitmaps[GAI_Count];
    platform_api* PlatformAPI;
};

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

inline fn_bitmap* fn_assets_bitmap_get(game_assets* assets, game_asset_id id)
{
    fn_bitmap* bitmap = assets->Bitmaps[id];
    return bitmap;
}