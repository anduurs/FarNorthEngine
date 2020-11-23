#include "fn_game.h"

#define STB_IMAGE_IMPLEMENTATION
#include "../../dependencies/stb_image/stb_image.h"

enum fn_input_action_type
{
    MOVE_FORWARD,
    MOVE_BACK,
    MOVE_RIGHT,
    MOVE_LEFT
};

internal void fn_game_initialize(game_memory* memory, game_state* gameState)
{
    fn_memory_initialize_arena(
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
    fn_world* gameWorld = gameState->GameWorld;
    fn_player* player = gameWorld->Player;

    float dt = memory->DeltaTime;

    player->Position.x += player->Velocity.x * dt * player->SpeedFactor;
    player->Position.y += player->Velocity.y * dt * player->SpeedFactor;
}

internal void fn_game_render(game_memory* memory, game_state* gameState, game_offscreen_buffer* offScreenBuffer)
{
    fn_world* gameWorld = gameState->GameWorld;
    fn_player* player = gameWorld->Player;

    fn_renderer_clear_screen(offScreenBuffer, 0x00, 0x00, 0x00);
    //fn_renderer_draw_quad(offScreenBuffer, (int32)player->Position.x, (int32)player->Position.y, 80, 80, 0x00, 0xFF, 0xFF);

    transient_state* transientState = (transient_state*)memory->TransientStorage;
    fn_bitmap* bitmap = fn_get_bitmap(&transientState->GameAssets, GAI_Player);

    fn_renderer_draw_bitmap(offScreenBuffer, bitmap, (int32)player->Position.x, (int32)player->Position.y);
}

internal void fn_game_output_sound(game_memory* memory, game_state* gameState, game_sound_output_buffer* soundBuffer)
{
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
            &transientState->GameAssets.AssetsArena,
            memory->TransientStorageSize - sizeof(transient_state),
            (uint8*)memory->TransientStorage + sizeof(transient_state)
        );

        fn_bitmap* playerBitmap = fn_memory_reserve_struct(&transientState->GameAssets.AssetsArena, fn_bitmap);

        platform_file_result file = memory->PlatformAPI->ReadFile("C:/dev/FarNorthEngine/data/images/playerspritesheet.png");

        int32 width;
        int32 height;
        int32 comp;

        stbi_set_flip_vertically_on_load(1);
        uint8* imageData = stbi_load_from_memory((uint8*)file.Data, (int)file.FileSize, &width, &height, &comp, 4);
        memory->PlatformAPI->FreeFile(&file);

        uint32* pixel = (uint32*)imageData;

        for (int32 y = 0; y < height; y++) {
            for (int32 x = 0; x < width; x++) {
                uint8 r = (uint8)(*pixel & 0x0000ff);
                uint8 g = (uint8)((*pixel & 0x00ff00) >> 8);
                uint8 b = (uint8)((*pixel & 0xff0000) >> 16);
                uint8 a = (uint8)((*pixel & 0xff000000) >> 24);
                *pixel++ = b | (g << 8) | (r << 16) | (a << 24);
            }
        }

        playerBitmap->Data = imageData;
        playerBitmap->Width = width;
        playerBitmap->Height = height;

        transientState->GameAssets.Bitmaps[GAI_Player] = playerBitmap;

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
