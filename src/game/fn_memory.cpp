struct memory_arena
{
    size_t Size;
    size_t Used;
    uint8* Base;
};

internal inline void fn_memory_initialize(memory_arena* arena, size_t size, void* base)
{
    arena->Size = size;
    arena->Base = (uint8*)base;
    arena->Used = 0;
}

#define fn_memory_reserve(arena, size, ...)fn_memory_reserve_(arena, size, ## __VA_ARGS__)
#define fn_memory_reserve_struct(arena, type, ...) (type*)fn_memory_reserve_(arena, sizeof(type), ## __VA_ARGS__)
#define fn_memory_reserve_array(arena, count, type, ...) (type*)fn_memory_reserve_(arena, count * sizeof(type), ## __VA_ARGS__)
internal inline void* fn_memory_reserve_(memory_arena* arena, size_t size, size_t alignment = 4)
{
    // ensures that the passed in alignment is not zero and that it is power of 2
    assert(alignment != 0 && (alignment & (alignment - 1)) == 0);

    size_t arenaBase = (size_t)arena->Base;
    size_t resultPointer = arenaBase + arena->Used;
    size_t alignmentOffset = 0;
    size_t alignmentMask = alignment - 1;

    if (resultPointer & alignmentMask)
    {
        alignmentOffset = alignment - (resultPointer & alignmentMask);
    }

    // adjust the reserve size based on new alignment offset
    size += alignmentOffset;

    assert((arena->Used + size) <= arena->Size);

    void* memory = (void*)(resultPointer + alignmentOffset);
    arena->Used += size;

    return memory;
}