constexpr uint32 MAX_ENTITIES = 65536;

constexpr uint32 ENTITY_INDEX_BITS = 22;
constexpr uint32 ENTITY_INDEX_MASK = (1 << ENTITY_INDEX_BITS) - 1;

constexpr uint32 ENTITY_GENERATION_BITS = 8;
constexpr uint32 ENTITY_GENERATION_MASK = (1 << ENTITY_GENERATION_BITS) - 1;

struct fn_entity
{
    uint32 EntityId;
};

struct position2d_component
{
    vec2 Position;
};

struct velocity2d_component
{
    vec2 Velocity;
};

struct radius_component
{
    float Radius;
};

struct speed_component
{
    float Speed;
};

enum fn_entity_flags
{
    Movement,
    Count
};

struct fn_entity_storage
{
    memory_arena Arena;

    fn_entity* Entities;

    fn_entity_flags BehaviourFlags[Count];

    position2d_component* PositionComponents;
    velocity2d_component* VelocityComponents;
    radius_component* RadiusComponents;
    speed_component* SpeedComponents;
};

// returns the index of the entity stored in fn_entity_storage
internal uint32 fn_entity_index(uint32 entityId)
{
    return entityId & ENTITY_INDEX_MASK;
}

// generation is used to distinguish entities that are created at the same index slot
//internal uint32 fn_entity_generation(uint32 entityId)
//{
//    return (entityId >> ENTITY_INDEX_MASK) & ENTITY_GENERATION_MASK;
//}




