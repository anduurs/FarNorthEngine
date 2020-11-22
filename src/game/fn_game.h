#pragma once

#include "../platform/fn_platform.h"
#include "../math/fn_math.h"
#include "fn_game_input.h"
#include "fn_memory.cpp"
#include "fn_entity.cpp"
#include "fn_world.cpp"
#include "fn_renderer.cpp"

enum game_asset_id
{
    GAI_Player,
    GAI_Count
};

struct game_assets
{
    memory_arena AssetsArena;
    fn_bitmap* Bitmaps[GAI_Count];
};

inline fn_bitmap* fn_get_bitmap(game_assets* assets, game_asset_id id)
{
    fn_bitmap* bitmap = assets->Bitmaps[id];
    return bitmap;
}

struct game_state
{
    float tSine;

    memory_arena WorldArena;
    fn_world* GameWorld;
};

struct transient_state
{
    bool IsInitialized;
    memory_arena TransientArena;

    platform_job_queue* HighPriorityQueue;
    platform_job_queue* LowPriorityQueue;

    game_assets GameAssets;
};
