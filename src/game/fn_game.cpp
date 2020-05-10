#include "fn_game.h"

#include "../renderer/opengl/opengl_renderer.cpp"

//internal void renderer_clear_screen(game_offscreen_buffer* buffer, uint8 red, uint8 green, uint8 blue)
//{
//    uint8* row = (uint8*)buffer->Data; 
//    
//    for (int32 y = 0; y < buffer->Height; y++)
//    {
//        uint32* pixel = (uint32*)row;
//        for (int32 x = 0; x < buffer->Width; x++)
//        {
//            *pixel++ = ((red << 16) | (green << 8) | blue);
//        }
//        
//        row += buffer->Pitch;
//    }
//}
//
//internal void renderer_draw_quad(game_offscreen_buffer* buffer, 
//    int32 xStart, int32 yStart, 
//    int32 width, int32 height,
//    uint8 red, uint8 green, uint8 blue)
//{
//    if (xStart <= 0) xStart = 1;
//    if (xStart + width >= buffer->Width) xStart = buffer->Width - width;
//    if (yStart <= 0) yStart = 0;
//    if (yStart + height >= buffer->Height) yStart = buffer->Height - height;
//
//    uint32* row = (uint32*)buffer->Data; 
//    
//    for (int32 y = yStart; y < yStart + height; y++)
//    {
//        for (int32 x = xStart; x < xStart + width; x++)
//        {
//            uint32* pixel = (row + x) + y * buffer->Width;  
//            *pixel++ = ((red << 16) | (green << 8) | blue);
//        }
//    }
//}

internal void fn_mem_arena_init(memory_arena* arena, size_t size, uint8* base)
{
    arena->Size = size;
    arena->Base = base;
    arena->Used = 0;
}

#define fn_mem_arena_reserve(arena, type) (type*)fn_mem_arena_reserve_(arena, sizeof(type))
#define fn_mem_arena_reserve_array(arena, count, type) (type*)fn_mem_arena_reserve_(arena, count * sizeof(type))
internal void* fn_mem_arena_reserve_(memory_arena* arena, size_t size, uint32 alignment = 4)
{
    assert((arena->Used + size) <= arena->Size);
    // @TODO(Anders): assert that the alignment is power of 2

    void* memory = arena->Base + arena->Used;
    arena->Used += size;

    return memory;
}

internal void fn_initalize_camera(fn_camera* camera, uint32 windowWidth, uint32 windowHeight)
{
    camera->FieldOfView = 70.0f;
    camera->zNear = 0.1f;
    camera->zFar = 130.0f;
    camera->Position = vec3{ 0.0f, 0.0f, 0.0f };
    camera->Rotation = fn_math_quat_angle_axis(0.0f, vec3{ 0.0f, 0.0f, 0.0f });

    float aspectRatio = (float)windowWidth / (float)windowHeight;

    camera->ProjectionMatrix = fn_math_mat4_perspective(
        camera->FieldOfView,
        aspectRatio,
        camera->zNear,
        camera->zFar
    );
}

extern "C" __declspec(dllexport) FN_GAME_INIT(fn_game_init)
{
    assert(sizeof(game_state) <= memory->PersistentStorageSize);

    game_state* gameState = (game_state*)memory->PersistentStorage;

    fn_camera* camera = &gameState->Camera;
    fn_initalize_camera(camera, memory->WindowWidth, memory->WindowHeight);

    fn_mem_arena_init(
        &gameState->WorldArena, 
        memory->PersistentStorageSize - sizeof(game_state), 
        (uint8*)memory->PersistentStorage + sizeof(game_state)
    );

    gameState->GameWorld = fn_mem_arena_reserve(&gameState->WorldArena, fn_world);
    fn_world* gameWorld = gameState->GameWorld;
    gameWorld->Chunks = fn_mem_arena_reserve(&gameState->WorldArena, fn_world_chunk);
    fn_world_chunk* chunks = gameWorld->Chunks;

    //world gen here

    gameWorld->Entities = fn_mem_arena_reserve_array(&gameState->WorldArena, 100, fn_entity);

    fn_entity* entities = gameWorld->Entities;

    fn_transform transform = {};
    transform.Position = vec3{ 0.0f, 0.0f, -8.0f };
    transform.Rotation = fn_math_quat_angle_axis(0.0f, vec3{ 0.0f, 0.0f, 0.0f });
    transform.Scale = vec3{ 1.0f, 1.0f, 1.0f };

    float vertices[] =
    {
        // front
        -1.0, -1.0,  1.0,
         1.0, -1.0,  1.0,
         1.0,  1.0,  1.0,
        -1.0,  1.0,  1.0,
        // back
        -1.0, -1.0, -1.0,
         1.0, -1.0, -1.0,
         1.0,  1.0, -1.0,
        -1.0,  1.0, -1.0
    };

    uint16 indices[] =
    {
        // front
        0, 1, 2,
        2, 3, 0,
        // right
        1, 5, 6,
        6, 2, 1,
        // back
        7, 6, 5,
        5, 4, 7,
        // left
        4, 0, 3,
        3, 7, 4,
        // bottom
        4, 5, 1,
        1, 0, 4,
        // top
        3, 2, 6,
        6, 7, 3
    };

    fn_mesh mesh = {};
    mesh.VertexCount = array_length(vertices);
    mesh.Vertices = vertices;
    mesh.IndicesCount = array_length(indices);
    mesh.Indices = indices;

    opengl_initalize();
    
    mesh.Id = opengl_create_vertex_buffer(vertices, mesh.VertexCount, indices, mesh.IndicesCount);
   
    platform_file_result vertexShader = memory->PlatformReadFile("C:/dev/FarNorthEngine/data/shaders/test_vertex_shader.vert");
    platform_file_result fragmentShader = memory->PlatformReadFile("C:/dev/FarNorthEngine/data/shaders/test_fragment_shader.frag");

    fn_shader shader = {};
    shader.Id = opengl_create_shader_program((const char*)vertexShader.Data, (const char*)fragmentShader.Data);

    memory->PlatformFreeFile(vertexShader.Data);
    memory->PlatformFreeFile(fragmentShader.Data);

    fn_texture diffuseMap = opengl_create_texture("C:/dev/FarNorthEngine/data/textures/container.png");

    glUseProgram(shader.Id);
    glUniform1i(glGetUniformLocation(shader.Id, "diffuseMap"), 0);
    glUseProgram(0);

    fn_material material = {};
    material.Shader = shader;
    material.DiffuseMap = diffuseMap;
    
    entities->Transform = transform;
    entities->Mesh = mesh;
    entities->Material = material;
    
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
        game_mouse_input* mouse = &input->Mouse;

        fn_camera* camera = &gameState->Camera;

        if (keyboard->KeyCode == FN_KEY_W)
        {
            camera->MoveForward = keyboard->Pressed;
        }
        else if (keyboard->KeyCode == FN_KEY_S)
        {
            camera->MoveBack = keyboard->Pressed;
        }

        if (keyboard->KeyCode == FN_KEY_A)
        {
            camera->MoveLeft = keyboard->Pressed;
        }
        else  if (keyboard->KeyCode == FN_KEY_D)
        {
            camera->MoveRight = keyboard->Pressed;
        }

        if (camera->CurrentMouseX != mouse->MouseCursorX)
            camera->CurrentMouseX = mouse->MouseCursorX;

        if (camera->CurrentMouseY != mouse->MouseCursorY)
            camera->CurrentMouseY = mouse->MouseCursorY;
    }
}

extern "C" __declspec(dllexport) FN_GAME_TICK(fn_game_tick)
{
    assert(sizeof(game_state) <= memory->PersistentStorageSize);

    game_state* gameState = (game_state*)memory->PersistentStorage;

    fn_entity* entity = gameState->GameWorld->Entities;

    entity->Transform.Rotation = fn_math_quat_rotate(dt * 15.0f, vec3{ 0, 1.0f, 0 }, entity->Transform.Rotation);

    fn_camera* camera = &gameState->Camera;
    float speed = 25.0f;

    if (camera->MoveForward)
    {
        vec3 forward = fn_math_quat_forward(camera->Rotation);
        camera->Position = fn_math_vec3_move_in_direction(camera->Position, forward, -dt * speed);
    }
    else if (camera->MoveBack)
    {
        vec3 forward = fn_math_quat_forward(camera->Rotation);
        camera->Position = fn_math_vec3_move_in_direction(camera->Position, forward, dt * speed);
    }

    if (camera->MoveRight)
    {
        vec3 right = fn_math_quat_right(camera->Rotation);
        camera->Position = fn_math_vec3_move_in_direction(camera->Position, right, dt * speed);
    }
    else if (camera->MoveLeft)
    {
        vec3 right = fn_math_quat_right(camera->Rotation);
        camera->Position = fn_math_vec3_move_in_direction(camera->Position, right, -dt * speed);
    }

    /*float currentMouseX = camera->CurrentMouseX;
    float currentMouseY = camera->CurrentMouseY;

    float previousMouseX = camera->PreviousMouseX;
    float previousMouseY = camera->PreviousMouseY;

    bool shouldRotateAroundX = currentMouseY != previousMouseY;
    bool shouldRotateAroundY = currentMouseX != previousMouseX;

    if (shouldRotateAroundX)
    {
        float deltaMouseY = currentMouseY - previousMouseY;
        float pitchChange = deltaMouseY * 10.0f * dt;
        vec3 right = fn_math_quat_right(camera->Rotation);
        quaternion targetRotation = fn_math_quat_angle_axis(pitchChange, right);
        camera->Rotation = fn_math_quat_mul(targetRotation, camera->Rotation);
        fn_math_quat_normalize(&camera->Rotation);
    }

    if (shouldRotateAroundY)
    {
        float deltaMouseX = currentMouseX - previousMouseX;
        float yawChange = deltaMouseX * 10.0f * dt;
        vec3 yAxis = {0, 1, 0};
        quaternion targetRotation = fn_math_quat_angle_axis(yawChange, yAxis);
        camera->Rotation = fn_math_quat_mul(targetRotation, camera->Rotation);
        fn_math_quat_normalize(&camera->Rotation);
    }

    camera->PreviousMouseX = currentMouseX;
    camera->PreviousMouseY = currentMouseY;*/
}

extern "C" __declspec(dllexport) FN_GAME_RENDER(fn_game_render)
{
    assert(sizeof(game_state) <= memory->PersistentStorageSize);

    game_state* gameState = (game_state*)memory->PersistentStorage;

    fn_world* gameWorld = gameState->GameWorld;
    fn_entity* entity = gameWorld->Entities;
    fn_camera* camera = &gameState->Camera;

    opengl_render_frame(camera, entity, 1);

    //renderer_clear_screen(buffer, 0x00, 0x00, 0x00);
    //renderer_draw_quad(buffer, (int32)gameState->PlayerX, (int32)gameState->PlayerY, 25, 25, 0xFF, 0xFF, 0x00);
}

extern "C" __declspec(dllexport) FN_GAME_OUTPUT_SOUND(fn_game_output_sound)
{
    assert(sizeof(game_state) <= memory->PersistentStorageSize);

    game_state* gameState = (game_state*)memory->PersistentStorage;

    int16 toneVolume = 0;
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
