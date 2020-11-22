#include "fn_game.h"

enum fn_input_action_type
{
    MOVE_FORWARD,
    MOVE_BACK,
    MOVE_RIGHT,
    MOVE_LEFT
};


FN_GAME_API FN_GAME_INIT(fn_game_init)
{
    assert(sizeof(game_state) <= memory->PersistentStorageSize);

    game_state* gameState = (game_state*)memory->PersistentStorage;

    fn_memory_initialize(
        &gameState->WorldArena, 
        memory->PersistentStorageSize - sizeof(game_state), 
        (uint8*)memory->PersistentStorage + sizeof(game_state)
    );

    gameState->GameWorld = fn_memory_reserve_struct(&gameState->WorldArena, fn_world);
    fn_world* gameWorld = gameState->GameWorld;

    gameWorld->Chunks = fn_memory_reserve_struct(&gameState->WorldArena, fn_world_chunk);
    fn_world_chunk* chunks = gameWorld->Chunks;

    gameWorld->Player = fn_memory_reserve_struct(&gameState->WorldArena, fn_player);
    fn_player* player = gameWorld->Player;

    player->SpeedFactor = 70.0f;
    player->Position.x = 50.0f;
    player->Position.y = 50.0f;

    memory->IsInitialized = true;
}

FN_GAME_API FN_GAME_PROCESS_INPUT(fn_game_process_input)
{
    assert(sizeof(game_state) <= memory->PersistentStorageSize);

    game_state* gameState = (game_state*)memory->PersistentStorage;

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

FN_GAME_API FN_GAME_TICK(fn_game_tick)
{
    assert(sizeof(game_state) <= memory->PersistentStorageSize);

    game_state* gameState = (game_state*)memory->PersistentStorage;


}

FN_GAME_API FN_GAME_UPDATE(fn_game_update)
{
    assert(sizeof(game_state) <= memory->PersistentStorageSize);

    game_state* gameState = (game_state*)memory->PersistentStorage;

    fn_world* gameWorld = gameState->GameWorld;
    fn_player* player = gameWorld->Player;

    float dt = memory->DeltaTime;

    player->Position.x += player->Velocity.x * dt * player->SpeedFactor;
    player->Position.y += player->Velocity.y * dt * player->SpeedFactor;
}

FN_GAME_API FN_GAME_RENDER(fn_game_render)
{
    assert(sizeof(game_state) <= memory->PersistentStorageSize);

    game_state* gameState = (game_state*)memory->PersistentStorage;

    fn_world* gameWorld = gameState->GameWorld;
    fn_player* player = gameWorld->Player;

    fn_renderer_clear_screen(offScreenBuffer, 0x00, 0x00, 0x00);
    fn_renderer_draw_quad(offScreenBuffer, (int32)player->Position.x, (int32)player->Position.y, 80, 80, 0x00, 0xFF, 0xFF);
}

FN_GAME_API FN_GAME_OUTPUT_SOUND(fn_game_output_sound)
{
    assert(sizeof(game_state) <= memory->PersistentStorageSize);

    game_state* gameState = (game_state*)memory->PersistentStorage;

    int16 toneVolume = 1000;
    int32 toneHz = 256;
    int wavePeriod = soundBuffer->SamplesPerSecond / toneHz;

    int16* sampleOut = soundBuffer->Samples;

    for (int sampleIndex = 0; sampleIndex < soundBuffer->SampleCount; sampleIndex++)
    {
        float sineValue = sinf(gameState->tSine);
        int16 sampleValue = (int16)(sineValue * toneVolume);

        *sampleOut++ = sampleValue;
        *sampleOut++ = sampleValue;

        gameState->tSine += 2.0f * PI * 1.0f / (float)wavePeriod;
    }
}
