#pragma once

#include "../platform/fn_platform.h"
#include "../math/fn_math.h"
#include "fn_game_input.h"
#include "fn_memory.cpp"
#include "fn_entity.cpp"
#include "fn_world.cpp"
#include "fn_renderer.cpp"

struct task_with_memory
{
    bool BeingUsed;
    memory_arena Arena;
    temporary_memory MemoryFlush;
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

inline fn_bitmap* fn_assets_bitmap_get(game_assets* assets, game_asset_id id)
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

    task_with_memory Tasks[4];

    platform_job_queue* HighPriorityQueue;
    platform_job_queue* LowPriorityQueue;

    game_assets Assets;
};
