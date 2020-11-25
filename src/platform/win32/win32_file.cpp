internal bool win32_file_write(const char* fileName, uint32 size, void* data);
internal platform_file_result win32_file_read(const char* fileName);
internal void win32_file_free(void* data);

internal bool win32_file_write(const char* fileName, uint32 size, void* data)
{
    bool result = false;
    HANDLE fileHandle = CreateFile(fileName, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, 0, 0);

    if (fileHandle != INVALID_HANDLE_VALUE)
    {
        DWORD bytesWritten;
        if (WriteFile(fileHandle, data, size, &bytesWritten, 0))
        {
            result = bytesWritten == size;
        }

        CloseHandle(fileHandle);
    }

    return result;
}

internal platform_file_result win32_file_read(const char* fileName)
{
    platform_file_result result = {};

    HANDLE fileHandle = CreateFile(fileName, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);

    if (fileHandle != INVALID_HANDLE_VALUE)
    {
        LARGE_INTEGER fileSize;
        if (GetFileSizeEx(fileHandle, &fileSize))
        {
            assert(fileSize.QuadPart <= 0xFFFFFFFF);

            uint32 size = (uint32)fileSize.QuadPart;

            // @TODO(Anders E): optimize this by passing a memory arena from the game and read data into that instead of doing a new heap allocation for every read 
            void* fileData = VirtualAlloc(0, size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

            if (fileData)
            {
                DWORD bytesRead;
                if (ReadFile(fileHandle, fileData, size, &bytesRead, 0) && size == (uint32)bytesRead)
                {
                    result.FileSize = size;
                    result.Data = fileData;
                }
                else
                {
                    win32_file_free(fileData);
                    fileData = 0;
                }
            }
        }

        CloseHandle(fileHandle);
    }

    return result;
}

internal void win32_file_free(void* data)
{
    if (data)
    {
        VirtualFree(data, 0, MEM_RELEASE);
    }
}
