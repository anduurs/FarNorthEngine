#include "fn_game.h"

internal void renderer_clear_screen(game_offscreen_buffer* buffer, uint8 red, uint8 green, uint8 blue)
{
    uint8* row = (uint8*)buffer->Data; 
    
    for (int32 y = 0; y < buffer->Height; y++)
    {
        uint32* pixel = (uint32*)row;
        for (int32 x = 0; x < buffer->Width; x++)
        {
            *pixel++ = ((red << 16) | (green << 8) | blue);
        }
        
        row += buffer->Pitch;
    }
}

internal void renderer_draw_quad(game_offscreen_buffer* buffer, 
    int32 xStart, int32 yStart, 
    int32 width, int32 height,
    uint8 red, uint8 green, uint8 blue)
{
    if (xStart <= 0) xStart = 1;
    if (xStart + width >= buffer->Width) xStart = buffer->Width - width;
    if (yStart <= 0) yStart = 0;
    if (yStart + height >= buffer->Height) yStart = buffer->Height - height;

    uint32* row = (uint32*)buffer->Data; 
    
    for (int32 y = yStart; y < yStart + height; y++)
    {
        for (int32 x = xStart; x < xStart + width; x++)
        {
            uint32* pixel = (row + x) + y * buffer->Width;  
            *pixel++ = ((red << 16) | (green << 8) | blue);
        }
    }
}

internal void fn_mem_arena_init(memory_arena* arena, size_t size, uint8* base)
{
    arena->Size = size;
    arena->Base = base;
    arena->Used = 0;
}

#define fn_mem_arena_reserve(arena, type) (type*)fn_mem_arena_reserve_(arena, sizeof(type))
#define fn_mem_arena_reserve_array(arena, count, type) (type*)fn_mem_arena_reserve_(arena, count * sizeof(type))
internal void* fn_mem_arena_reserve_(memory_arena* arena, size_t size)
{
    assert((arena->Used + size) <= arena->Size);

    void* memory = arena->Base + arena->Used;
    arena->Used += size;

    return memory;
}

extern "C" __declspec(dllexport) FN_GAME_INIT(fn_game_init)
{
    assert(sizeof(game_state) <= memory->PersistentStorageSize);

    game_state* gameState = (game_state*)memory->PersistentStorage;

    gameState->PlayerX = 1280/2;
    gameState->PlayerY = 720/2;

    gameState->PlayerVelocityX = 0.0f;
    gameState->PlayerVelocityY = 0.0f;

    fn_mem_arena_init(
        &gameState->WorldMemoryArena, 
        memory->PersistentStorageSize - sizeof(game_state), 
        (uint8*)memory->PersistentStorage + sizeof(game_state)
    );

    gameState->GameWorld = fn_mem_arena_reserve(&gameState->WorldMemoryArena, world);
    world* gameWorld = gameState->GameWorld;
    gameWorld->Chunks = fn_mem_arena_reserve(&gameState->WorldMemoryArena, world_chunk);
    world_chunk* chunks = gameWorld->Chunks;

    const char* fileName = __FILE__;
    platform_file_result file = memory->PlatformReadFile(fileName);

    if (file.Data)
    {
        memory->PlatformDebugLog("Writing file\n");
        memory->PlatformWriteFile("C:/dev/FarNorthEngine/data/test.out", file.FileSize, file.Data);
        memory->PlatformFreeFile(file.Data);
    }

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

        float speed = 5.0f;

        if (keyboard->KeyCode == FN_KEY_W)
        {
            gameState->PlayerVelocityY = keyboard->Pressed ? -speed : 0;
        }

        if (keyboard->KeyCode == FN_KEY_S)
        {
            gameState->PlayerVelocityY = keyboard->Pressed ? speed : 0;
        }

        if (keyboard->KeyCode == FN_KEY_A)
        {
            gameState->PlayerVelocityX = keyboard->Pressed ? -speed : 0;
        }

        if (keyboard->KeyCode == FN_KEY_D)
        {
            gameState->PlayerVelocityX = keyboard->Pressed ? speed : 0;
        }
    }
}

extern "C" __declspec(dllexport) FN_GAME_TICK(fn_game_tick)
{
    assert(sizeof(game_state) <= memory->PersistentStorageSize);

    game_state* gameState = (game_state*)memory->PersistentStorage;

    gameState->PlayerX += gameState->PlayerVelocityX;
    gameState->PlayerY += gameState->PlayerVelocityY;
}

extern "C" __declspec(dllexport) FN_GAME_RENDER(fn_game_render)
{
    assert(sizeof(game_state) <= memory->PersistentStorageSize);

    game_state* gameState = (game_state*)memory->PersistentStorage;

    renderer_clear_screen(buffer, 0x00, 0x00, 0x00);
    renderer_draw_quad(buffer, (int32)gameState->PlayerX, (int32)gameState->PlayerY, 25, 25, 0xFF, 0xFF, 0x00);
}

extern "C" __declspec(dllexport) FN_GAME_OUTPUT_SOUND(fn_game_output_sound)
{
    assert(sizeof(game_state) <= memory->PersistentStorageSize);

    game_state* gameState = (game_state*)memory->PersistentStorage;

    int16 toneVolume = 1000;
    int32 toneHz = 256;
    int wavePeriod = soundBuffer->SamplesPerSecond / toneHz;

    int16* sampleOut = soundBuffer->Samples;

    for (int sampleIndex = 0; sampleIndex < soundBuffer->SampleCount; sampleIndex++)
    {
        float sineValue = glm::sin(gameState->tSine);
        int16 sampleValue = (int16)(sineValue * toneVolume);

        *sampleOut++ = sampleValue;
        *sampleOut++ = sampleValue;

        gameState->tSine += 2.0f * PI * 1.0f / (float)wavePeriod;
    }
}
