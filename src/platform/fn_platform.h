#pragma once

#include "../common/fn_common.h"

// Services that the platform layer provides to the game
struct platform_file_result
{
    uint32 FileSize;
    void* Data;
};

struct platform_net_socket
{
    int32 Handle;
    uint16 Port;
};

struct platform_net_address
{
    uint32 Address;
    uint16 Port;
};

// @TODO(Anders): make async versions of read and write file. Look up I/O completion ports for windows platform layer

#define FN_PLATFORM_FILE_FREE(name) void name(void* data)
typedef FN_PLATFORM_FILE_FREE(platform_file_free);

#define FN_PLATFORM_FILE_WRITE(name) bool name(const char* fileName, uint32 size, void* data)
typedef FN_PLATFORM_FILE_WRITE(platform_file_write);

#define FN_PLATFORM_FILE_READ(name) platform_file_result name(const char* fileName)
typedef FN_PLATFORM_FILE_READ(platform_file_read);

#define FN_PLATFORM_NET_SOCKET_OPEN(name) platform_net_socket name(uint16 port, bool blocking)
typedef FN_PLATFORM_NET_SOCKET_OPEN(platform_net_socket_open);

#define FN_PLATFORM_DEBUG_LOG(name) void name(const char* message)
typedef FN_PLATFORM_DEBUG_LOG(platform_debug_log);

struct platform_api
{
    platform_file_write* PlatformWriteFile;
    platform_file_read* PlatformReadFile;
    platform_file_free* PlatformFreeFile;

    platform_net_socket_open* PlatformOpenSocket;

    platform_debug_log* PlatformDebugLog;
};