struct fn_player
{
    float SpeedFactor;
    vec2 Position;
    vec2 Velocity;
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
    fn_player* Player;
    fn_entity_storage* Entities;
};