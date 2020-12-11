#define STB_IMAGE_IMPLEMENTATION
#include "../../dependencies/stb_image/stb_image.h"

#include "fn_asset.h"

internal fn_mesh_data fn_asset_mesh_load(memory_arena* arena, const char* fileName)
{
    fn_mesh_data result = {};

    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(fileName, aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_CalcTangentSpace);
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

        aiVector3D tangent = mesh->mTangents[i];
        vertex.Tangent.x = tangent.x;
        vertex.Tangent.y = tangent.y;
        vertex.Tangent.z = tangent.z;

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

    return result;
}

internal fn_shader_data fn_asset_shader_load(const platform_api* platformAPI, const char* vertexShaderFileName, const char* fragmentShaderFileName)
{
    fn_shader_data result = {};

    platform_file_result vertexShaderFile = platformAPI->ReadFile(vertexShaderFileName);
    platform_file_result fragmentShaderFile = platformAPI->ReadFile(fragmentShaderFileName);

    result.VertexShader = vertexShaderFile;
    result.FragmentShader = fragmentShaderFile;

    return result;
}

internal fn_bitmap fn_asset_bitmap_load(const platform_api* platformAPI, const char* fileName)
{
    fn_bitmap result = {};

    platform_file_result file = platformAPI->ReadFile(fileName);

    int32 width;
    int32 height;
    int32 comp;

    uint8* imageData = stbi_load_from_memory((uint8*)file.Data, (int)file.FileSize, &width, &height, &comp, 4);
    platformAPI->FreeFile(file);

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

internal inline fn_material* fn_asset_material_get(game_assets& assets, game_asset_id id)
{
    fn_material* mat = assets.Materials[id];
    return mat;
}

internal inline fn_shader* fn_asset_shader_get(game_assets& assets, fn_shader_type type)
{
    fn_shader* shader = assets.Shaders[type];
    return shader;
}