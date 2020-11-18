#pragma once

#include <dsound.h>

#define DIRECT_SOUND_CREATE(name) HRESULT WINAPI name(LPCGUID pcGuidDevice, LPDIRECTSOUND* ppDS, LPUNKNOWN pUnkOuter)
typedef DIRECT_SOUND_CREATE(direct_sound_create); 

struct win32_sound_output_buffer
{
    int32 SamplesPerSecond;
    int32 ToneHz;
    int16 ToneVolume;
    uint32 RunningSampleIndex;
    int32 WavePeriod;
    int32 BytesPerSample;
    int32 BufferSize;
    float tSine;
    int LatencySampleCount;
};

global_variable LPDIRECTSOUNDBUFFER GlobalSoundBuffer;

internal void win32_audio_init_dsound(HWND window, int32 samplesPerSecond, int32 bufferSize);
internal void win32_audio_clear_sound_buffer(win32_sound_output_buffer* soundOutput);
internal void win32_audio_fill_sound_buffer(win32_sound_output_buffer* soundOutput, DWORD byteToLock, DWORD bytesToWrite, game_sound_output_buffer* sourceBuffer);

