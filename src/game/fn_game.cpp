#include "fn_game.h"

#include "fn_memory.cpp"
#include "fn_renderer.cpp"

extern "C" __declspec(dllexport) FN_GAME_INIT(fn_game_init)
{
    assert(sizeof(game_state) <= memory->PersistentStorageSize);

    game_state* gameState = (game_state*)memory->PersistentStorage;

    fn_mem_arena_init(
        &gameState->WorldArena, 
        memory->PersistentStorageSize - sizeof(game_state), 
        (uint8*)memory->PersistentStorage + sizeof(game_state)
    );

    gameState->GameWorld = fn_mem_arena_reserve(&gameState->WorldArena, fn_world);
    fn_world* gameWorld = gameState->GameWorld;
    gameWorld->Chunks = fn_mem_arena_reserve(&gameState->WorldArena, fn_world_chunk);
    fn_world_chunk* chunks = gameWorld->Chunks;
    
    memory->IsInitialized = true;
}

extern "C" __declspec(dllexport) FN_GAME_PROCESS_INPUT(fn_game_process_input)
{
    assert(sizeof(game_state) <= memory->PersistentStorageSize);

    game_state* gameState = (game_state*)memory->PersistentStorage;

    game_controller_input* gamepad = &input->Gamepads[0];

    if (gamepad->IsConnected)
    {
        if (gamepad->IsAnalog)
        {

        }
    }
    else
    {
        game_keyboard_input* keyboard = &input->Keyboard;
        game_mouse_input* mouse = &input->Mouse;
    }
}

extern "C" __declspec(dllexport) FN_GAME_TICK(fn_game_tick)
{
    assert(sizeof(game_state) <= memory->PersistentStorageSize);

    game_state* gameState = (game_state*)memory->PersistentStorage;

    
}

extern "C" __declspec(dllexport) FN_GAME_RENDER(fn_game_render)
{
    assert(sizeof(game_state) <= memory->PersistentStorageSize);

    game_state* gameState = (game_state*)memory->PersistentStorage;

    fn_renderer_clear_screen(offScreenBuffer, 0x00, 0x00, 0x00);
    fn_renderer_draw_quad(offScreenBuffer, 200, 100, 80, 80, 0x00, 0xFF, 0xFF);
}

extern "C" __declspec(dllexport) FN_GAME_OUTPUT_SOUND(fn_game_output_sound)
{
    assert(sizeof(game_state) <= memory->PersistentStorageSize);

    game_state* gameState = (game_state*)memory->PersistentStorage;

    int16 toneVolume = 0;
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
