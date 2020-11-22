struct win32_state
{
    uint64 TotalGameMemorySize;
    void* GameMemoryBlock;

    HANDLE RecordingHandle;
    int32 InputRecordingIndex;

    HANDLE PlaybackHandle;
    int32 InputPlayingIndex;

    char ExePath[MAX_PATH];
    int ExeDirLength;
};

internal void win32_build_path_in_exe_dir(win32_state* state, char* src, int srcSize, char* dst, int dstSize)
{
    strncpy_s(dst, dstSize, state->ExePath, state->ExeDirLength);
    strncpy_s(dst + state->ExeDirLength, dstSize - state->ExeDirLength, src, srcSize);
}

internal void win32_begin_recording_input(win32_state* win32State, int32 inputRecordingIndex)
{
    win32State->InputRecordingIndex = inputRecordingIndex;
    const char* fileName = "input_record.fni";
    win32State->RecordingHandle = CreateFileA(fileName, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, 0, 0);
    DWORD bytesToWrite = (DWORD)win32State->TotalGameMemorySize;
    assert(bytesToWrite == win32State->TotalGameMemorySize);
    DWORD bytesWritten;
    WriteFile(win32State->RecordingHandle, win32State->GameMemoryBlock, bytesToWrite, &bytesWritten, 0);
    OutputDebugStringA("input recording file written\n");
}

internal void win32_end_recording_input(win32_state* win32State)
{
    CloseHandle(win32State->RecordingHandle);
    win32State->InputRecordingIndex = 0;
}

internal void win32_record_input(win32_state* win32State, game_input* input)
{
    DWORD bytesWritten;
    WriteFile(win32State->RecordingHandle, input, sizeof(*input), &bytesWritten, 0);
}

internal void win32_begin_play_back_input(win32_state* win32State, int32 inputPlayingIndex)
{
    win32State->InputPlayingIndex = inputPlayingIndex;

    const char* fileName = "input_record.fni";
    win32State->PlaybackHandle = CreateFileA(fileName, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
    DWORD bytesToRead = (DWORD)win32State->TotalGameMemorySize;
    assert(bytesToRead == win32State->TotalGameMemorySize);
    DWORD bytesRead;
    if (!ReadFile(win32State->PlaybackHandle, win32State->GameMemoryBlock, bytesToRead, &bytesRead, 0))
    {

    }
}

internal void win32_end_play_back_input(win32_state* win32State)
{
    CloseHandle(win32State->PlaybackHandle);
    win32State->InputPlayingIndex = 0;
}

internal void win32_play_back_input(win32_state* win32State, game_input* input)
{
    DWORD bytesRead = 0;
    if (ReadFile(win32State->PlaybackHandle, input, sizeof(*input), &bytesRead, 0))
    {
        // there is still input
        if (bytesRead == 0)
        {
            // we hit the end of the stream, go back to the beginning
            int32 playingIndex = win32State->InputPlayingIndex;
            win32_end_play_back_input(win32State);
            win32_begin_play_back_input(win32State, playingIndex);
            if (!ReadFile(win32State->PlaybackHandle, input, sizeof(*input), &bytesRead, 0))
            {

            }
        }
    }
}