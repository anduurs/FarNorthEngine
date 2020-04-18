#include "win32_audio.h"

internal void win32_audio_init_dsound(HWND window, int32 samplesPerSecond, int32 bufferSize)
{
    HMODULE DSoundLibrary = LoadLibraryA("dsound.dll");

    if (DSoundLibrary)
    {
        direct_sound_create* DirectSoundCreate = (direct_sound_create*)GetProcAddress(DSoundLibrary, "DirectSoundCreate");
        LPDIRECTSOUND directSound;

        if (DirectSoundCreate && SUCCEEDED(DirectSoundCreate(0, &directSound, 0)))
        {
            WAVEFORMATEX waveFormat = {};
            waveFormat.wFormatTag = WAVE_FORMAT_PCM;
            waveFormat.nChannels = 2;
            waveFormat.nSamplesPerSec = samplesPerSecond;
            waveFormat.wBitsPerSample = 16;
            waveFormat.nBlockAlign = (waveFormat.nChannels * waveFormat.wBitsPerSample) / 8;
            waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
            waveFormat.cbSize = 0;

            if (SUCCEEDED(directSound->SetCooperativeLevel(window, DSSCL_PRIORITY)))
            {
                DSBUFFERDESC bufferDescription = {};
                bufferDescription.dwSize = sizeof(bufferDescription);
                bufferDescription.dwFlags = DSBCAPS_PRIMARYBUFFER;

                //NOTE(Anders): Get a handle to the primary sound device so we can set it to a format. 
                // This will put the sound card into a mode that allows it to play in the format we want.
                LPDIRECTSOUNDBUFFER primaryBuffer;
                if (SUCCEEDED(directSound->CreateSoundBuffer(&bufferDescription, &primaryBuffer, 0)))
                {
                    OutputDebugStringA("Primary buffer created succesfully. \n");

                    //Tell windows the sound format we want to play
                    if (SUCCEEDED(primaryBuffer->SetFormat(&waveFormat)))
                    {
                        OutputDebugStringA("Primary buffer format was set. \n");
                    }
                } 
            }

            DSBUFFERDESC bufferDescription = {};
            bufferDescription.dwSize = sizeof(bufferDescription);
            bufferDescription.dwFlags = 0;
            bufferDescription.dwBufferBytes = bufferSize;
            bufferDescription.lpwfxFormat = &waveFormat;

            if (SUCCEEDED(directSound->CreateSoundBuffer(&bufferDescription, &GlobalSoundBuffer, 0)))
            {
                OutputDebugStringA("Secondary buffer created succesfully \n");
            }
        }
    }
}

internal void win32_audio_clear_sound_buffer(win32_sound_output_buffer* soundOutput)
{
    VOID* region1;
    DWORD region1Size;
    VOID* region2;
    DWORD region2Size;

    if (SUCCEEDED(GlobalSoundBuffer->Lock(
        0, soundOutput->BufferSize, 
        &region1, &region1Size, 
        &region2, &region2Size, 0
    )))
    {
        int8* destSample = (int8*)region1;

        for (DWORD byteIndex = 0; byteIndex < region1Size; byteIndex++)
        {
            *destSample++ = 0;
        }

        destSample = (int8*)region2;

        for (DWORD byteIndex = 0; byteIndex < region2Size; byteIndex++)
        {
            *destSample++ = 0;
        }

        GlobalSoundBuffer->Unlock(region1, region1Size, region2, region2Size);
    }
}

internal void win32_audio_fill_sound_buffer(
    win32_sound_output_buffer* soundOutput, 
    DWORD byteToLock, 
    DWORD bytesToWrite, 
    game_sound_output_buffer* sourceBuffer)
{
    VOID* region1;
    DWORD region1Size;
    VOID* region2;
    DWORD region2Size;

    if (SUCCEEDED(GlobalSoundBuffer->Lock(
        byteToLock, 
        bytesToWrite, 
        &region1, 
        &region1Size, 
        &region2, 
        &region2Size, 
        0
    )))
    {
        DWORD region1SampleCount = region1Size / soundOutput->BytesPerSample;
        int16* destSample = (int16*)region1;
        int16* sourceSample = sourceBuffer->Samples;

        for (DWORD sampleIndex = 0; sampleIndex < region1SampleCount; sampleIndex++)
        {
            *destSample++ = *sourceSample++;
            *destSample++ = *sourceSample++;
            ++soundOutput->RunningSampleIndex;
        }

        DWORD region2SampleCount = region2Size / soundOutput->BytesPerSample;
        destSample = (int16*)region2;

        for (DWORD sampleIndex = 0; sampleIndex < region2SampleCount; sampleIndex++)
        {
            *destSample++ = *sourceSample++;
            *destSample++ = *sourceSample++;
            ++soundOutput->RunningSampleIndex;
        }

        GlobalSoundBuffer->Unlock(region1, region1Size, region2, region2Size);
    }
}