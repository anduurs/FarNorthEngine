struct memory_arena
{
    size_t Size;
    size_t Used;
    uint8* Base;
    int32 TempCount;
};

struct temporary_memory
{
    memory_arena* Arena;
    size_t Used;
};

internal inline temporary_memory fn_memory_temporary_begin(memory_arena* arena)
{
    temporary_memory result = {};

    result.Arena = arena;
    result.Used = arena->Used;
    result.Arena->TempCount++;

    return result;
}

internal inline void fn_memory_temporary_end(temporary_memory tempMemory)
{
    memory_arena* arena = tempMemory.Arena;
    assert(arena->Used >= tempMemory.Used);
    arena->Used = tempMemory.Used;
    assert(arena->TempCount > 0);
    arena->TempCount--;
}

internal inline void fn_memory_initialize_arena(memory_arena* arena, size_t size, void* base)
{
    arena->Size = size;
    arena->Base = (uint8*)base;
    arena->Used = 0;
    arena->TempCount = 0;
}

internal inline void fn_memory_check_arena(memory_arena* arena)
{
    assert(arena->TempCount == 0);
}

#define fn_memory_alloc(arena, size, ...)fn_memory_alloc_(arena, size, ## __VA_ARGS__)
#define fn_memory_alloc_struct(arena, type, ...) (type*)fn_memory_alloc_(arena, sizeof(type), ## __VA_ARGS__)
#define fn_memory_alloc_array(arena, count, type, ...) (type*)fn_memory_alloc_(arena, count * sizeof(type), ## __VA_ARGS__)
internal inline void* fn_memory_alloc_(memory_arena* arena, size_t size, size_t alignment = 4)
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

#define fn_memory_clear_struct(instance) fn_memory_clear(&instance, sizeof(instance))
internal inline void fn_memory_clear(void* ptr, size_t size)
{
    uint8* byte = (uint8*)ptr;
    while (size--)
        *byte++ = 0;
}

internal inline void fn_memory_initialize_sub_arena(memory_arena* newArena, memory_arena* oldArena, size_t size)
{
    newArena->Size = size;
    newArena->Base = (uint8*)fn_memory_alloc(oldArena, size);
    newArena->TempCount = 0;
    newArena->Used = 0;
}