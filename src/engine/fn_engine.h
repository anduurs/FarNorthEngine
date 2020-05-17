#pragma once

#include "../common/fn_common.h"
#include "../math/fn_math.h"

// Services that the platform layer provides to the game
struct platform_file_result
{
    uint32 FileSize;
    void* Data;
};

// @TODO(Anders): make async versions of read and write file. Look up I/O completion ports for windows platform layer

#define FN_PLATFORM_FILE_FREE(name) void name(void* data)
typedef FN_PLATFORM_FILE_FREE(platform_file_free);

#define FN_PLATFORM_FILE_WRITE(name) bool name(const char* fileName, uint32 size, void* data)
typedef FN_PLATFORM_FILE_WRITE(platform_file_write);

#define FN_PLATFORM_FILE_READ(name) platform_file_result name(const char* fileName)
typedef FN_PLATFORM_FILE_READ(platform_file_read);

#define FN_PLATFORM_DEBUG_LOG(name) void name(const char* message)
typedef FN_PLATFORM_DEBUG_LOG(platform_debug_log);

struct platform_api
{
    platform_file_write* PlatformWriteFile;
    platform_file_read* PlatformReadFile;
    platform_file_free* PlatformFreeFile;
    platform_debug_log* PlatformDebugLog;
};

struct memory_arena
{
    size_t Size;
    size_t Used;

    uint8* Base;
};

struct game_memory
{
    bool IsInitialized;

    uint64 PersistentStorageSize;
    void* PersistentStorage;

    uint64 TemporaryStorageSize;
    void* TemporaryStorage;

    uint64 TransientStorageSize;
    void* TransientStorage;

    platform_api PlatformAPI;

    uint32 WindowWidth;
    uint32 WindowHeight;
};
