#pragma once

#include "../common/fn_common.h"
#include "../math/fn_math.h"
#include "fn_game_input.h"

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

struct game_sound_output_buffer
{
    int32 SamplesPerSecond;
    int32 SampleCount;
    int16* Samples;
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

    platform_file_write* PlatformWriteFile;
    platform_file_read* PlatformReadFile;
    platform_file_free* PlatformFreeFile;
    platform_debug_log* PlatformDebugLog;

    uint32 WindowWidth;
    uint32 WindowHeight;
};

struct game_button_state
{
    int32 HalfTransitionCount;
    bool EndedDown;
};

struct game_controller_input
{
    bool IsAnalog;
    bool IsConnected;

    float LStickAverageX;
    float LStickAverageY;

    float RStickAverageX;
    float RStickAverageY;

    game_button_state Start;
    game_button_state Back;

    game_button_state Dpad_Up;
    game_button_state Dpad_Down;
    game_button_state Dpad_Right;
    game_button_state Dpad_Left;

    game_button_state LStickUp;
    game_button_state LStickDown;
    game_button_state LStickRight;
    game_button_state LStickLeft;

    game_button_state RStickUp;
    game_button_state RStickDown;
    game_button_state RStickRight;
    game_button_state RStickLeft;

    game_button_state ButtonUp;
    game_button_state ButtonDown;
    game_button_state ButtonRight;
    game_button_state ButtonLeft;

    game_button_state LeftShoulder;
    game_button_state RightShoulder;

    game_button_state LeftTrigger;
    game_button_state RightTrigger;
};

struct game_keyboard_input
{
    uint32 KeyCode;
    bool Pressed;
    bool Released;
};

struct game_mouse_input
{
    float MouseCursorX;
    float MouseCursorY;

    bool LeftMouseButtonPressed;
    bool RightMouseButtonPressed;
};

struct game_input
{
    game_controller_input Gamepads[4];
    game_keyboard_input Keyboard;
    game_mouse_input Mouse;
};

struct player
{
    vec2 Position;
    vec2 Velocity;
};

struct fn_transform
{
    vec3 Position;
    quaternion Rotation;
    vec3 Scale;
};

struct fn_mesh
{
    uint32 Id;

    uint32 VertexCount;
    float* Vertices;

    uint32 IndicesCount;
    uint16* Indices;
};

struct fn_shader
{
    uint32 Id;
};

struct fn_texture
{
    uint32 Id;
};

struct fn_material
{
    fn_shader Shader;
    fn_texture DiffuseMap;
};

struct fn_camera
{
    float FieldOfView;
    float zNear;
    float zFar;

    vec3 Position;
    quaternion Rotation;

    mat4 ProjectionMatrix;

    bool MoveForward;
    bool MoveBack;
    bool MoveRight;
    bool MoveLeft;

    float CurrentMouseX;
    float CurrentMouseY;
    float PreviousMouseX;
    float PreviousMouseY;
};

struct fn_entity
{
    fn_transform Transform;
    fn_mesh Mesh;
    fn_material Material;
};

struct fn_world_chunk
{
    uint16 ChunkX;
    uint16 ChunkY;
    uint8 ChunkSize;

    uint8* TileIds;
};

struct fn_world
{
    uint16 ChunkDimension;
    fn_world_chunk* Chunks;
    fn_entity* Entities;
};

struct game_state
{
    float tSine;

    fn_camera Camera;

    memory_arena WorldArena;
    fn_world* GameWorld;
};

// Services that the game provides to the platform layer
#define FN_GAME_INIT(name) void name(game_memory* memory)
typedef FN_GAME_INIT(game_init);
FN_GAME_INIT(fn_game_init_stub){}

#define FN_GAME_PROCESS_INPUT(name) void name(game_memory* memory, game_input* input)
typedef FN_GAME_PROCESS_INPUT(game_process_input);
FN_GAME_PROCESS_INPUT(fn_game_process_input_stub){}

#define FN_GAME_TICK(name) void name(game_memory* memory, float dt)
typedef FN_GAME_TICK(game_tick);
FN_GAME_TICK(fn_game_tick_stub){}

#define FN_GAME_RENDER(name) void name(game_memory* memory)
typedef FN_GAME_RENDER(game_render);
FN_GAME_RENDER(fn_game_render_stub){}

#define FN_GAME_OUTPUT_SOUND(name) void name(game_memory* memory, game_sound_output_buffer* soundBuffer)
typedef FN_GAME_OUTPUT_SOUND(game_output_sound);
FN_GAME_OUTPUT_SOUND(fn_game_output_sound_stub){}
