#define STB_IMAGE_IMPLEMENTATION
#include "../../dependencies/stb_image/stb_image.h"

#include "fn_asset.h"

internal fn_texture fn_asset_texture_load(const platform_api& platformAPI, const char* fileName)
{
    fn_texture result = {};

    platform_file_result file = platformAPI.ReadFile(fileName);

    int32 width;
    int32 height;
    int32 comp;

    stbi_set_flip_vertically_on_load(true);
    uint8* imageData = stbi_load_from_memory((uint8*)file.Data, (int)file.FileSize, &width, &height, &comp, 0);
    platformAPI.FreeFile(&file);

    result.Id = fn_opengl_texture_create(imageData, width, height);

    stbi_image_free(imageData);

    return result;
}

internal fn_mesh fn_asset_mesh_load(memory_arena* arena, const char* fileName)
{
    fn_mesh result = {};

    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(fileName, aiProcess_Triangulate | aiProcess_JoinIdenticalVertices);

    const aiMesh* mesh = scene->mMeshes[0];
    
    uint32 numOfVertices = mesh->mNumVertices;

    fn_vertex* vertices = fn_memory_alloc_array(arena, numOfVertices, fn_vertex);

    for (uint32 i = 0; i < numOfVertices; i++)
    {
        fn_vertex vertex = {};

        aiVector3D pos = mesh->mVertices[i];
        vertex.Position.x = pos.x;
        vertex.Position.y = pos.y;
        vertex.Position.z = pos.z;

        aiVector3D normal = mesh->mNormals[i];
        vertex.Normal.x = normal.x;
        vertex.Normal.y = normal.y;
        vertex.Normal.z = normal.z;

        aiVector3D uv = mesh->mTextureCoords[0][i];
        vertex.TextureCoords.x = uv.x;
        vertex.TextureCoords.y = uv.y;

        vertices[i] = vertex;
    }

    const uint32 vertsPerFace = 3;
    uint32 numOfIndices = mesh->mNumFaces * vertsPerFace;

    uint32* indices = fn_memory_alloc_array(arena, numOfIndices, uint32);

    for (uint32 i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        assert(face.mNumIndices == vertsPerFace);
        for (uint32 j = 0; j < vertsPerFace; j++)
        {
            indices[j + i * vertsPerFace] = face.mIndices[j];
        }
    }

    result.Vertices = vertices;
    result.VerticesCount = numOfVertices;
    result.Indices = indices;
    result.IndicesCount = numOfIndices;

    result.Id = fn_opengl_mesh_create(vertices, numOfVertices, indices, numOfIndices);

    return result;
}

internal fn_shader fn_asset_shader_load(const platform_api& platformAPI, const char* vertexShaderFileName, const char* fragmentShaderFileName)
{
    fn_shader result = {};

    platform_file_result vertexShaderFile = platformAPI.ReadFile(vertexShaderFileName);
    platform_file_result fragmentShaderFile = platformAPI.ReadFile(fragmentShaderFileName);

    result.VertexShaderData = vertexShaderFile;
    result.FragmentShaderData = fragmentShaderFile;

    result.Id = fn_opengl_shader_create((const char*)vertexShaderFile.Data, (const char*)fragmentShaderFile.Data);

    return result;
}

internal fn_bitmap fn_asset_bitmap_load(const platform_api& platformAPI, const char* fileName)
{
    fn_bitmap result = {};

    platform_file_result file = platformAPI.ReadFile(fileName);

    int32 width;
    int32 height;
    int32 comp;

    uint8* imageData = stbi_load_from_memory((uint8*)file.Data, (int)file.FileSize, &width, &height, &comp, 4);
    platformAPI.FreeFile(&file);

    result.Data = imageData;
    result.Width = width;
    result.Height = height;

    return result;
}

internal inline fn_bitmap* fn_asset_bitmap_get(const game_assets& assets, game_asset_id id)
{
    fn_bitmap* bitmap = assets.Bitmaps[id];
    return bitmap;
}

internal inline fn_mesh* fn_asset_mesh_get(const game_assets& assets, game_asset_id id)
{
    fn_mesh* mesh = assets.Meshes[id];
    return mesh;
}

internal inline fn_texture* fn_asset_texture_get(game_assets& assets, game_asset_id id)
{
    fn_texture* texture = assets.Textures[id];
    return texture;
}

internal inline fn_shader* fn_asset_shader_get(game_assets& assets, game_asset_id id)
{
    fn_shader* shader = assets.Shaders[id];
    return shader;
}