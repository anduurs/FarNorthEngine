#include "fn_game.h"

enum fn_input_action_type
{
    MOVE_FORWARD,
    MOVE_BACK,
    MOVE_RIGHT,
    MOVE_LEFT
};

struct fn_asset_load_job
{
    game_assets* Assets;
    game_asset_id AssetId;
    task_with_memory* Task;
};

struct fn_asset_bitmap_load_job : fn_asset_load_job
{
    char* FileName;
    fn_bitmap* Bitmap;
};

struct fn_asset_mesh_load_job : fn_asset_load_job
{
    char* FileName;
    fn_mesh* Mesh;
};

struct fn_asset_texture_load_job : fn_asset_load_job
{
    char* TextureFileName[TextureType_Count];
    fn_texture* Texture;
    fn_shader* Shader;
};

struct fn_asset_shader_load_job : fn_asset_load_job
{
    char* VertexShaderFileName;
    char* GeometryShaderFileName;
    char* FragmentShaderFileName;

    fn_shader* Shader;
};

internal task_with_memory* fn_begin_task_with_memory(transient_state* transientState)
{
    task_with_memory* foundTask = 0;

    for (uint32 i = 0; i < array_length(transientState->Tasks); i++)
    {
        task_with_memory* task = transientState->Tasks + i;
        if (!task->BeingUsed)
        {
            foundTask = task;
            task->BeingUsed = true;
            task->MemoryFlush = fn_memory_temporary_begin(&task->Arena);
            break;
        }
    }

    return foundTask;
}

internal void fn_end_task_with_memory(task_with_memory* task)
{
    fn_memory_temporary_end(task->MemoryFlush);
    _WriteBarrier();
    task->BeingUsed = false;
}

internal void fn_assets_bitmap_load_job_callback(platform_job_queue* queue, void* data)
{
    fn_asset_bitmap_load_job* job = (fn_asset_bitmap_load_job*)data;

    *job->Bitmap = fn_assets_bitmap_load(job->Assets->PlatformAPI, job->FileName);
    _WriteBarrier();
    job->Assets->Bitmaps[job->AssetId] = job->Bitmap;
    
    fn_end_task_with_memory(job->Task);
}

internal void fn_assets_mesh_load_job_callback(platform_job_queue* queue, void* data)
{
    fn_asset_mesh_load_job* job = (fn_asset_mesh_load_job*)data;

    *job->Mesh = fn_assets_mesh_load(job->Assets->PlatformAPI, job->FileName);
    _WriteBarrier();
    job->Assets->Meshes[job->AssetId] = job->Mesh;

    fn_end_task_with_memory(job->Task);
}

internal void fn_assets_texture_load_job_callback(platform_job_queue* queue, void* data)
{
    fn_asset_texture_load_job* job = (fn_asset_texture_load_job*)data;

    *job->Texture = fn_assets_texture_load(job->Assets->PlatformAPI, job->TextureFileName[TextureType_Diffuse]);
    _WriteBarrier();
    job->Assets->Textures[job->AssetId] = job->Texture;

    fn_end_task_with_memory(job->Task);
}

internal void fn_assets_shader_load_job_callback(platform_job_queue* queue, void* data)
{
    fn_asset_shader_load_job* job = (fn_asset_shader_load_job*)data;

    *job->Shader = fn_assets_shader_load(job->Assets->PlatformAPI, job->VertexShaderFileName, job->FragmentShaderFileName);
    _WriteBarrier();
    job->Assets->Shaders[job->AssetId] = job->Shader;

    fn_end_task_with_memory(job->Task);
}

internal void fn_assets_load_async(game_assets* assets, game_asset_id assetId)
{
    platform_api* platformAPI = assets->PlatformAPI;
    platform_job_queue* lowPriorityQueue = assets->TransientState->LowPriorityQueue;

    switch (assetId)
    {
        case AssetId_Container:
        {
            task_with_memory* meshTask = fn_begin_task_with_memory(assets->TransientState);
            fn_asset_mesh_load_job* meshJob = fn_memory_alloc_struct(&meshTask->Arena, fn_asset_mesh_load_job);

            meshJob->Task = meshTask;
            meshJob->AssetId = assetId;
            meshJob->Assets = assets;
            meshJob->FileName = "C:/dev/FarNorthEngine/data/models/container.glb";
            meshJob->Mesh = fn_memory_alloc_struct(&assets->Arena, fn_mesh);

            platformAPI->ScheduleJob(lowPriorityQueue, fn_assets_mesh_load_job_callback, meshJob);

            // @TODO(Anders E): All shaders must be loaded and compiled before the textures

            //task_with_memory* textureTask = fn_begin_task_with_memory(assets->TransientState);
            //fn_asset_texture_load_job* textureJob = fn_memory_alloc_struct(&textureTask->Arena, fn_asset_texture_load_job);

            //textureJob->Task = textureTask;
            //textureJob->AssetId = assetId;
            //textureJob->Assets = assets;
            //textureJob->TextureFileName[TextureType_Diffuse] = "C:/dev/FarNorthEngine/data/textures/container.png";
            //textureJob->Texture = fn_memory_alloc_struct(&assets->Arena, fn_texture);

            //platformAPI->ScheduleJob(lowPriorityQueue, fn_assets_texture_load_job_callback, textureJob);

           /* task_with_memory* shaderTask = fn_begin_task_with_memory(assets->TransientState);
            fn_asset_shader_load_job* shaderJob = fn_memory_alloc_struct(&shaderTask->Arena, fn_asset_shader_load_job);

            shaderJob->Task = shaderTask;
            shaderJob->AssetId = assetId;
            shaderJob->Assets = assets;
            shaderJob->VertexShaderFileName = "C:/dev/FarNorthEngine/data/shaders/fn_standard_vertex_shader.vert";
            shaderJob->FragmentShaderFileName = "C:/dev/FarNorthEngine/data/shaders/fn_standard_fragment_shader.frag";
            shaderJob->Shader = fn_memory_alloc_struct(&assets->Arena, fn_shader);

            platformAPI->ScheduleJob(lowPriorityQueue, fn_assets_shader_load_job_callback, shaderJob);*/
        } break;
    };
}

internal void fn_camera_initalize(fn_camera* camera, uint32 windowWidth, uint32 windowHeight)
{
    camera->FoV = 70.0f;
    camera->zNear = 0.1f;
    camera->zFar = 130.0f;
    camera->Position = vec3f{ 0.0f, 0.0f, 0.0f };
    camera->Rotation = fn_math_quat_angle_axis(0.0f, vec3f{ 0.0f, 0.0f, 0.0f });

    f32 aspectRatio = (f32)windowWidth / (f32)windowHeight;

    camera->ProjectionMatrix = fn_math_mat4_perspective(
        camera->FoV,
        aspectRatio,
        camera->zNear,
        camera->zFar
    );
}

internal void fn_game_initialize(game_memory* memory, game_state* gameState)
{
    fn_camera* camera = &gameState->Camera;
    fn_camera_initalize(camera, memory->WindowWidth, memory->WindowHeight);

    fn_memory_initialize_arena(
        &gameState->WorldArena,
        memory->PersistentStorageSize - sizeof(game_state),
        (uint8*)memory->PersistentStorage + sizeof(game_state)
    );

    /*fn_memory_initialize_sub_arena(&gameState->EntityStore.Arena, &gameState->WorldArena, megabytes(10));

    gameState->EntityStore.Entities = (fn_entity*)fn_memory_alloc(&gameState->EntityStore.Arena, sizeof(fn_entity) * MAX_ENTITIES);
    gameState->EntityStore.PositionComponents = (position2d_component*)fn_memory_alloc(&gameState->EntityStore.Arena, sizeof(position2d_component) * MAX_ENTITIES);
    gameState->EntityStore.VelocityComponents = (velocity2d_component*)fn_memory_alloc(&gameState->EntityStore.Arena, sizeof(velocity2d_component) * MAX_ENTITIES);
    gameState->EntityStore.SpeedComponents = (speed_component*)fn_memory_alloc(&gameState->EntityStore.Arena, sizeof(speed_component) * MAX_ENTITIES);*/

    gameState->GameWorld = fn_memory_alloc_struct(&gameState->WorldArena, fn_world);
    fn_world* gameWorld = gameState->GameWorld;

    gameWorld->Renderables = fn_memory_alloc_array(&gameState->WorldArena, sizeof(fn_renderable) * MAX_ENTITIES, fn_renderable);
    fn_renderable* renderables = gameWorld->Renderables;

    fn_renderable renderable = {};
    renderable.AssetId = game_asset_id::AssetId_Container;
    fn_transform transform = {};
    transform.Position = vec3f {0, 0, -8.0f};
    transform.Scale = vec3f {1, 1, 1};
    transform.Rotation = fn_math_quat_angle_axis(0.0f, vec3f{ 0.0f, 0.0f, 0.0f });
    renderable.Transform = transform;

    *renderables++ = renderable;

    fn_opengl_initalize();
}

internal void fn_game_process_input(game_memory* memory, game_state* gameState, game_input* input)
{
    fn_world* gameWorld = gameState->GameWorld;
    fn_player* player = gameWorld->Player;

    game_controller_input* gamepad = &input->Gamepads[0];
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

internal void fn_game_tick(game_memory* memory, game_state* gameState)
{

}

internal void fn_game_update(game_memory* memory, game_state* gameState)
{
    fn_world* gameWorld = gameState->GameWorld;
    fn_renderable* renderables = gameWorld->Renderables;

    f32 dt = memory->DeltaTime;

    renderables[0].Transform.Rotation = fn_math_quat_rotate(dt * 150.0f, vec3f{ 0, 1.0f, 0 }, renderables[0].Transform.Rotation);

    fn_camera* camera = &gameState->Camera;
    float speed = 25.0f;

    if (camera->MoveForward)
    {
        vec3f forward = fn_math_quat_forward(camera->Rotation);
        camera->Position = fn_math_vec3f_move_in_direction(camera->Position, forward, -dt * speed);
    }
    else if (camera->MoveBack)
    {
        vec3f forward = fn_math_quat_forward(camera->Rotation);
        camera->Position = fn_math_vec3f_move_in_direction(camera->Position, forward, dt * speed);
    }

    if (camera->MoveRight)
    {
        vec3f right = fn_math_quat_right(camera->Rotation);
        camera->Position = fn_math_vec3f_move_in_direction(camera->Position, right, dt * speed);
    }
    else if (camera->MoveLeft)
    {
        vec3f right = fn_math_quat_right(camera->Rotation);
        camera->Position = fn_math_vec3f_move_in_direction(camera->Position, right, -dt * speed);
    }
}

internal void fn_game_render(game_memory* memory, game_state* gameState, game_offscreen_buffer* offScreenBuffer)
{
    transient_state* transientState = (transient_state*)memory->TransientStorage;
    fn_world* gameWorld = gameState->GameWorld;
    fn_renderable* renderable = &gameWorld->Renderables[0];

    fn_camera* camera = &gameState->Camera;

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (renderable)
    {
        fn_shader* shader = fn_assets_shader_get(&transientState->Assets, renderable->AssetId);
        fn_mesh* mesh = fn_assets_mesh_get(&transientState->Assets, renderable->AssetId);

        if (shader && mesh)
        {
            glUseProgram(shader->Id);

            mat4 localToWorldMatrix = fn_math_mat4_local_to_world(
                renderable->Transform.Position,
                renderable->Transform.Rotation,
                renderable->Transform.Scale
            );

            mat4 cameraViewMatrix = fn_math_mat4_camera_view(camera->Position, camera->Rotation);
            mat4 projectionMatrix = camera->ProjectionMatrix;

            fn_opengl_shader_load_mat4(shader, "localToWorldMatrix", &localToWorldMatrix);
            fn_opengl_shader_load_mat4(shader, "cameraViewMatrix", &cameraViewMatrix);
            fn_opengl_shader_load_mat4(shader, "projectionMatrix", &projectionMatrix);

            glBindVertexArray(mesh->Id);
            int32 size;
            glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
            glDrawElements(GL_TRIANGLES, size / sizeof(uint16), GL_UNSIGNED_SHORT, 0);

            glBindVertexArray(0);
            glUseProgram(0);
        }
    }

    //fn_renderer_clear_screen(offScreenBuffer, 0x00, 0x00, 0x00);
    //fn_renderer_draw_quad(offScreenBuffer, 500, 400, 80, 80, 0x00, 0xFF, 0xFF);

    //fn_renderer_draw_triangle(offScreenBuffer, vec2i{ 500, 500 }, vec2i{ 700, 200 }, vec2i{ 900, 500 }, 0xFF, 0x00, 0x00);
    //fn_renderer_draw_triangle(offScreenBuffer, vec2i{ 500, 500 }, vec2i{ 700, 200 }, vec2i{ 900, 500 }, 0x00, 0xFF, 0x00);

    //fn_renderer_draw_line_dda(offScreenBuffer, vec2i{ 500, 500 }, vec2i{ 700, 200 }, 0xFF, 0x00, 0x00);
    //fn_renderer_draw_line_dda(offScreenBuffer, vec2i{ 900, 500 }, vec2i{ 700, 200 }, 0x00, 0x00, 0xFF);
    //fn_renderer_draw_line_dda(offScreenBuffer, vec2i{ 500, 500 }, vec2i{ 900, 500 }, 0x00, 0xFF, 0x00);

   /* transient_state* transientState = (transient_state*)memory->TransientStorage;
    fn_bitmap* bitmap = fn_assets_bitmap_get(&transientState->Assets, AssetId_Player);
    if (bitmap)
        fn_renderer_draw_bitmap(offScreenBuffer, bitmap, (int32)player->Position.x + 200, (int32)player->Position.y);*/
}

internal void fn_game_output_sound(game_memory* memory, game_state* gameState, game_sound_output_buffer* soundBuffer)
{
    int16 toneVolume = 0;
    int32 toneHz = 256;
    int32 wavePeriod = soundBuffer->SamplesPerSecond / toneHz;

    int16* sampleOut = soundBuffer->Samples;

    for (int32 sampleIndex = 0; sampleIndex < soundBuffer->SampleCount; sampleIndex++)
    {
        f32 sineValue = sinf(gameState->tSine);
        int16 sampleValue = (int16)(sineValue * toneVolume);

        *sampleOut++ = sampleValue;
        *sampleOut++ = sampleValue;

        gameState->tSine += 2.0f * PI * 1.0f / (f32)wavePeriod;
    }
}

fn_api FN_GAME_RUN_FRAME(RunFrame)
{
    assert(sizeof(game_state) <= memory->PersistentStorageSize);

    game_state* gameState = (game_state*)memory->PersistentStorage;

    if (!memory->IsInitialized)
    {
        fn_game_initialize(memory, gameState);
        memory->IsInitialized = true;
    }

    assert(sizeof(transient_state) <= memory->TransientStorageSize);

    transient_state* transientState = (transient_state*)memory->TransientStorage;
    
    if (!transientState->IsInitialized)
    {
        fn_memory_initialize_arena(
            &transientState->TransientArena,
            memory->TransientStorageSize - sizeof(transient_state),
            (uint8*)memory->TransientStorage + sizeof(transient_state)
        );

        fn_memory_initialize_sub_arena(&transientState->Assets.Arena, &transientState->TransientArena, megabytes(64));

        for (uint32 i = 0; i < array_length(transientState->Tasks); i++)
        {
            task_with_memory* task = transientState->Tasks + i;
            task->BeingUsed = false;
            fn_memory_initialize_sub_arena(&task->Arena, &transientState->TransientArena, megabytes(1));
        }

        transientState->Assets.PlatformAPI = memory->PlatformAPI;
        transientState->Assets.TransientState = transientState;

        transientState->LowPriorityQueue = memory->LowPriorityQueue;
        transientState->HighPriorityQueue = memory->HighPriorityQueue;

        //fn_assets_load_async(&transientState->Assets, game_asset_id::AssetId_Container);

        const char* vertShader = "C:/dev/FarNorthEngine/data/shaders/fn_standard_vertex_shader.vert";
        const char* fragShader = "C:/dev/FarNorthEngine/data/shaders/fn_standard_fragment_shader.frag";
        fn_shader* shader = fn_memory_alloc_struct(&transientState->Assets.Arena, fn_shader);
        *shader = fn_assets_shader_load(memory->PlatformAPI, vertShader, fragShader);
        transientState->Assets.Shaders[AssetId_Container] = shader;

        fn_mesh* mesh = fn_memory_alloc_struct(&transientState->Assets.Arena, fn_mesh);
        *mesh = fn_assets_mesh_load(memory->PlatformAPI, "");
        transientState->Assets.Meshes[AssetId_Container] = mesh;

        transientState->IsInitialized = true;
    }

    fn_game_process_input(memory, gameState, input);
    fn_game_update(memory, gameState);
    fn_game_render(memory, gameState, offScreenBuffer);
}

fn_api FN_GAME_TICK(Tick)
{
    assert(sizeof(game_state) <= memory->PersistentStorageSize);
    game_state* gameState = (game_state*)memory->PersistentStorage;
    fn_game_tick(memory, gameState);
}

fn_api FN_GAME_OUTPUT_SOUND(OutputSound)
{
    assert(sizeof(game_state) <= memory->PersistentStorageSize);
    game_state* gameState = (game_state*)memory->PersistentStorage;
    fn_game_output_sound(memory, gameState, soundBuffer);
}
