

internal void fn_renderer_clear_screen(game_offscreen_buffer* buffer, uint8 red, uint8 green, uint8 blue)
{
    uint8* row = (uint8*)buffer->Data;

    for (int32 y = 0; y < buffer->Height; y++)
    {
        uint32* pixel = (uint32*)row;
        for (int32 x = 0; x < buffer->Width; x++)
        {
            *pixel++ = ((red << 16) | (green << 8) | blue);
        }

        row += buffer->Pitch;
    }
}

internal void fn_renderer_draw_quad(game_offscreen_buffer* buffer,
    int32 xStart, int32 yStart,
    int32 width, int32 height,
    uint8 red, uint8 green, uint8 blue)
{
    if (xStart <= 0) xStart = 1;
    if (xStart + width >= buffer->Width) xStart = buffer->Width - width;
    if (yStart <= 0) yStart = 0;
    if (yStart + height >= buffer->Height) yStart = buffer->Height - height;

    uint32* row = (uint32*)buffer->Data;

    for (int32 y = yStart; y < yStart + height; y++)
    {
        for (int32 x = xStart; x < xStart + width; x++)
        {
            uint32* pixel = (row + x) + y * buffer->Width;
            *pixel++ = ((red << 16) | (green << 8) | blue);
        }
    }
}