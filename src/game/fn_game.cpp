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

extern "C" __declspec(dllexport) FN_GAME_INIT(fn_game_init)
{
    assert(sizeof(game_state) <= memory->PermanentStorageSize);

    game_state* gameState = (game_state*)memory->PermanentStorage;

    gameState->X = 1280/2;
    gameState->Y = 720/2;

    const char* fileName = __FILE__;
    file_result file = memory->PlatformReadFile(fileName);

    if (file.Data)
    {
        memory->PlatformDebugLog("Writing file\n");
        memory->PlatformWriteFile("E:/Github/FarNorthEngine/res/test.out", file.FileSize, file.Data);
        memory->PlatformFreeFile(file.Data);
    }
    
    memory->IsInitialized = true;
}

extern "C" __declspec(dllexport) FN_GAME_PROCESS_INPUT(fn_game_process_input)
{
    game_gamepad_input* gamepad = &input->Gamepads[0];
    if (gamepad->IsAnalog)
    {

    }
}

extern "C" __declspec(dllexport) FN_GAME_TICK(fn_game_tick)
{
}

extern "C" __declspec(dllexport) FN_GAME_RENDER(fn_game_render)
{
    assert(sizeof(game_state) <= memory->PermanentStorageSize);

    game_state* gameState = (game_state*)memory->PermanentStorage;

    renderer_clear_screen(buffer, 0x00, 0x00, 0x00);
    renderer_draw_quad(buffer, gameState->X+150, gameState->Y-100, 50, 50, 0xFF, 0x00, 0x00);
}

extern "C" __declspec(dllexport) FN_GAME_OUTPUT_SOUND(fn_game_output_sound)
{
    assert(sizeof(game_state) <= memory->PermanentStorageSize);

    game_state* gameState = (game_state*)memory->PermanentStorage;

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

        gameState->tSine += 2.0f * Pi32 * 1.0f / (float)wavePeriod;
    }
}
