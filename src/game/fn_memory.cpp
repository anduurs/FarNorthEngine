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