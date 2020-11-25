
internal void fn_renderer_draw_bitmap(game_offscreen_buffer* buffer, fn_bitmap* bitmap, int32 xOffset, int32 yOffset)
{
    int32 width = bitmap->Width;
    int32 height = bitmap->Height;

    if (xOffset <= 0) xOffset = 1;
    if (xOffset + width >= buffer->Width) xOffset = buffer->Width - width;
    if (yOffset <= 0) yOffset = 0;
    if (yOffset + height >= buffer->Height) yOffset = buffer->Height - height;

    uint32* srcPixels = (uint32*)bitmap->Data;
    uint32* dstPixels = (uint32*)buffer->Data;
    
    for (int32 y = yOffset; y < yOffset + height; y++)
    {
        for (int32 x = xOffset; x < xOffset + width; x++)
        {
            uint32* pixel = (dstPixels + x) + y * buffer->Width;

            uint32 srcColor = *srcPixels++;

            // extract the rgba components of the bitmap source image
            uint8 r = (uint8)(srcColor & 0x0000ff);
            uint8 g = (uint8)((srcColor & 0x00ff00) >> 8);
            uint8 b = (uint8)((srcColor & 0xff0000) >> 16);
            uint8 a = (uint8)((srcColor & 0xff000000) >> 24);

            *pixel++ = b | (g << 8) | (r << 16) | (a << 24);
        }
    }
}

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