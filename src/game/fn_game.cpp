#include "fn_game.h"

enum fn_input_action_type
{
    MOVE_FORWARD,
    MOVE_BACK,
    MOVE_RIGHT,
    MOVE_LEFT
};

struct fn_asset_bitmap_load_job
{
    game_assets* Assets;
    char* FileName;
    game_asset_id AssetId;
    task_with_memory* Task;
    fn_bitmap* Bitmap;
};

internal task_with_memory* fn_begin_task_with_memory(transient_state* transientState)
{
    task_with_memory* foundTask = 0;

    for (uint32 i = 0; i < array_length(transientState->Tasks); i++)
    {
        task_with_memory* task = transientState->Tasks + i;
        if (!task->BeingUsed)
        {
            foundTask = task;
            task->BeingUsed = true;
            task->MemoryFlush = fn_memory_temporary_begin(&task->Arena);
            break;
        }
    }

    return foundTask;
}

internal void fn_end_task_with_memory(task_with_memory* task)
{
    fn_memory_temporary_end(task->MemoryFlush);
    _WriteBarrier();
    task->BeingUsed = false;
}

internal void fn_assets_bitmap_load_job_callback(platform_job_queue* queue, void* data)
{
    fn_asset_bitmap_load_job* job = (fn_asset_bitmap_load_job*)data;

    *job->Bitmap = fn_assets_bitmap_load(job->Assets->PlatformAPI, job->FileName);

    // @TODO: fence
    job->Assets->Bitmaps[job->AssetId] = job->Bitmap;

    fn_end_task_with_memory(job->Task);
}

internal void fn_assets_bitmap_load_async(game_assets* assets, game_asset_id assetId)
{
    task_with_memory* task = fn_begin_task_with_memory(assets->TransientState);

    if (task)
    {
        fn_asset_bitmap_load_job* job = fn_memory_alloc_struct(&task->Arena, fn_asset_bitmap_load_job);

        job->Task = task;
        job->FileName = "C:/dev/FarNorthEngine/data/images/playerspritesheet.png";
        job->AssetId = assetId;
        job->Assets = assets;
        job->Bitmap = fn_memory_alloc_struct(&assets->Arena, fn_bitmap);

        assets->PlatformAPI->ScheduleJob(assets->TransientState->LowPriorityQueue, fn_assets_bitmap_load_job_callback, job);
    }
}

internal void fn_game_initialize(game_memory* memory, game_state* gameState)
{
    fn_memory_initialize_arena(
        &gameState->WorldArena,
        memory->PersistentStorageSize - sizeof(game_state),
        (uint8*)memory->PersistentStorage + sizeof(game_state)
    );

    /*fn_memory_initialize_sub_arena(&gameState->EntityStore.Arena, &gameState->WorldArena, megabytes(10));

    gameState->EntityStore.Entities = (fn_entity*)fn_memory_alloc(&gameState->EntityStore.Arena, sizeof(fn_entity) * MAX_ENTITIES);
    gameState->EntityStore.PositionComponents = (position2d_component*)fn_memory_alloc(&gameState->EntityStore.Arena, sizeof(position2d_component) * MAX_ENTITIES);
    gameState->EntityStore.VelocityComponents = (velocity2d_component*)fn_memory_alloc(&gameState->EntityStore.Arena, sizeof(velocity2d_component) * MAX_ENTITIES);
    gameState->EntityStore.SpeedComponents = (speed_component*)fn_memory_alloc(&gameState->EntityStore.Arena, sizeof(speed_component) * MAX_ENTITIES);*/

    gameState->GameWorld = fn_memory_alloc_struct(&gameState->WorldArena, fn_world);
    fn_world* gameWorld = gameState->GameWorld;

    gameWorld->Chunks = fn_memory_alloc_struct(&gameState->WorldArena, fn_world_chunk);
    fn_world_chunk* chunks = gameWorld->Chunks;

    gameWorld->Player = fn_memory_alloc_struct(&gameState->WorldArena, fn_player);
    fn_player* player = gameWorld->Player;

    f32 vertices[] = 
    {
         // Front
        -1.0, -1.0,  1.0,
         1.0, -1.0,  1.0,
         1.0,  1.0,  1.0,
        -1.0,  1.0,  1.0,
        // Back
        -1.0, -1.0, -1.0,
         1.0, -1.0, -1.0,
         1.0,  1.0, -1.0,
        -1.0,  1.0, -1.0
    };

    player->SpeedFactor = 70.0f;
    player->Position.x = 50.0f;
    player->Position.y = 50.0f;
}

internal void fn_game_process_input(game_memory* memory, game_state* gameState, game_input* input)
{
    fn_world* gameWorld = gameState->GameWorld;
    fn_player* player = gameWorld->Player;

    game_controller_input* gamepad = &input->Gamepads[0];
    game_keyboard_input* keyboard = &input->Keyboard;
    game_mouse_input* mouse = &input->Mouse;

    if (keyboard->KeyCode == FN_KEY_D)
    {
        if (keyboard->Pressed)
        {
            player->Velocity.x = 1.0f;
            memory->PlatformAPI->DebugLog("D PRESSED");
        }

        if (keyboard->Released)
        {
            player->Velocity.x = 0.0f;
            memory->PlatformAPI->DebugLog("D RELEASED");
        }
    }

    if (keyboard->KeyCode == FN_KEY_A)
    {
        if (keyboard->Pressed)
        {
            player->Velocity.x = -1.0f;
            memory->PlatformAPI->DebugLog("A PRESSED");
        }

        if (keyboard->Released)
        {
            player->Velocity.x = 0.0f;
            memory->PlatformAPI->DebugLog("A RELEASED");
        }
    }

    if (keyboard->KeyCode == FN_KEY_W)
    {
        if (keyboard->Pressed)
        {
            player->Velocity.y = -1.0f;
            memory->PlatformAPI->DebugLog("W PRESSED");
        }

        if (keyboard->Released)
        {
            player->Velocity.y = 0.0f;
            memory->PlatformAPI->DebugLog("W RELEASED");
        }
    }

    if (keyboard->KeyCode == FN_KEY_S)
    {
        if (keyboard->Pressed)
        {
            player->Velocity.y = 1.0f;
            memory->PlatformAPI->DebugLog("S PRESSED");
        }

        if (keyboard->Released)
        {
            player->Velocity.y = 0.0f;
            memory->PlatformAPI->DebugLog("S RELEASED");
        }
    }
}

internal void fn_game_tick(game_memory* memory, game_state* gameState)
{

}

internal void fn_game_update(game_memory* memory, game_state* gameState)
{
    //fn_entity_storage entityStore = gameState->EntityStore;

    //for (int i = 0; i < MAX_ENTITIES; i++)
    //{
    //    uint32 index = fn_entity_index(entityStore.Entities[i].EntityId);
    //    vec2f* positionData = &entityStore.PositionComponents[index].Position;
    //}


    fn_world* gameWorld = gameState->GameWorld;
    fn_player* player = gameWorld->Player;

    f32 dt = memory->DeltaTime;

    player->Position.x += player->Velocity.x * dt * player->SpeedFactor;
    player->Position.y += player->Velocity.y * dt * player->SpeedFactor;
}

internal void fn_game_render(game_memory* memory, game_state* gameState, game_offscreen_buffer* offScreenBuffer)
{
    fn_world* gameWorld = gameState->GameWorld;
    fn_player* player = gameWorld->Player;

    fn_renderer_clear_screen(offScreenBuffer, 0x00, 0x00, 0x00);
    //fn_renderer_draw_quad(offScreenBuffer, 500, 400, 80, 80, 0x00, 0xFF, 0xFF);

    fn_renderer_draw_triangle(offScreenBuffer, vec2i{ 500, 500 }, vec2i{ 700, 200 }, vec2i{ 900, 500 }, 0xFF, 0x00, 0x00);
    //fn_renderer_draw_triangle(offScreenBuffer, vec2i{ 500, 500 }, vec2i{ 700, 200 }, vec2i{ 900, 500 }, 0x00, 0xFF, 0x00);

    //fn_renderer_draw_line_dda(offScreenBuffer, vec2i{ 500, 500 }, vec2i{ 700, 200 }, 0xFF, 0x00, 0x00);
    //fn_renderer_draw_line_dda(offScreenBuffer, vec2i{ 900, 500 }, vec2i{ 700, 200 }, 0x00, 0x00, 0xFF);
    //fn_renderer_draw_line_dda(offScreenBuffer, vec2i{ 500, 500 }, vec2i{ 900, 500 }, 0x00, 0xFF, 0x00);

   /* transient_state* transientState = (transient_state*)memory->TransientStorage;
    fn_bitmap* bitmap = fn_assets_bitmap_get(&transientState->Assets, GAI_Player);
    if (bitmap)
        fn_renderer_draw_bitmap(offScreenBuffer, bitmap, (int32)player->Position.x + 200, (int32)player->Position.y);*/
}

internal void fn_game_output_sound(game_memory* memory, game_state* gameState, game_sound_output_buffer* soundBuffer)
{
    int16 toneVolume = 0;
    int32 toneHz = 256;
    int32 wavePeriod = soundBuffer->SamplesPerSecond / toneHz;

    int16* sampleOut = soundBuffer->Samples;

    for (int32 sampleIndex = 0; sampleIndex < soundBuffer->SampleCount; sampleIndex++)
    {
        f32 sineValue = sinf(gameState->tSine);
        int16 sampleValue = (int16)(sineValue * toneVolume);

        *sampleOut++ = sampleValue;
        *sampleOut++ = sampleValue;

        gameState->tSine += 2.0f * PI * 1.0f / (f32)wavePeriod;
    }
}

fn_api FN_GAME_RUN_FRAME(RunFrame)
{
    assert(sizeof(game_state) <= memory->PersistentStorageSize);

    game_state* gameState = (game_state*)memory->PersistentStorage;

    if (!memory->IsInitialized)
    {
        fn_game_initialize(memory, gameState);
        memory->IsInitialized = true;
    }

    assert(sizeof(transient_state) <= memory->TransientStorageSize);

    transient_state* transientState = (transient_state*)memory->TransientStorage;
    
    if (!transientState->IsInitialized)
    {
        fn_memory_initialize_arena(
            &transientState->TransientArena,
            memory->TransientStorageSize - sizeof(transient_state),
            (uint8*)memory->TransientStorage + sizeof(transient_state)
        );

        fn_memory_initialize_sub_arena(&transientState->Assets.Arena, &transientState->TransientArena, megabytes(64));

        for (uint32 i = 0; i < array_length(transientState->Tasks); i++)
        {
            task_with_memory* task = transientState->Tasks + i;
            task->BeingUsed = false;
            fn_memory_initialize_sub_arena(&task->Arena, &transientState->TransientArena, megabytes(1));
        }

        transientState->Assets.PlatformAPI = memory->PlatformAPI;
        transientState->Assets.TransientState = transientState;

        transientState->LowPriorityQueue = memory->LowPriorityQueue;
        transientState->HighPriorityQueue = memory->HighPriorityQueue;

        fn_assets_bitmap_load_async(&transientState->Assets, game_asset_id::GAI_Player);

        transientState->IsInitialized = true;
    }

    fn_game_process_input(memory, gameState, input);
    fn_game_update(memory, gameState);
    fn_game_render(memory, gameState, offScreenBuffer);
}

fn_api FN_GAME_TICK(Tick)
{
    assert(sizeof(game_state) <= memory->PersistentStorageSize);
    game_state* gameState = (game_state*)memory->PersistentStorage;
    fn_game_tick(memory, gameState);
}

fn_api FN_GAME_OUTPUT_SOUND(OutputSound)
{
    assert(sizeof(game_state) <= memory->PersistentStorageSize);
    game_state* gameState = (game_state*)memory->PersistentStorage;
    fn_game_output_sound(memory, gameState, soundBuffer);
}
