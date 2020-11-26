#pragma once

#include "../platform/fn_platform.h"
#include "../math/fn_math.h"
#include "fn_game_input.h"
#include "fn_memory.cpp"
#include "fn_assets.cpp"
#include "fn_entity.cpp"
#include "fn_world.cpp"
#include "fn_renderer.cpp"

struct game_state
{
    float tSine;

    memory_arena WorldArena;
    fn_world* GameWorld;

    fn_entity_storage EntityStore;
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
