
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

internal inline void fn_renderer_draw_pixel(game_offscreen_buffer* buffer, int32 x, int32 y, uint8 red, uint8 green, uint8 blue)
{
    uint32* pixels = (uint32*)buffer->Data;
    pixels[x + y * buffer->Width] = (red << 16) | (green << 8) | blue;
}

internal void fn_renderer_draw_line_dda(game_offscreen_buffer* buffer, vec2i start, vec2i end, uint8 red, uint8 green, uint8 blue)
{
    int32 x0 = start.x;
    int32 y0 = start.y;
    int32 x1 = end.x;
    int32 y1 = end.y;

    f32 dy = (float)(y1 - y0);
    f32 dx = (float)(x1 - x0);
    f32 steps = 0;

    if (fabsf(dx) > fabsf(dy))
        steps = fabsf(dx);
    else
        steps = fabsf(dy);

    f32 xStep = dx / steps;
    f32 yStep = dy / steps;

    f32 x = (float)x0;
    f32 y = (float)y0;

    for (int32 i = 0; i < steps; i++)
    {
        fn_renderer_draw_pixel(buffer, x0, y0, red, green, blue);
        x += xStep;
        y += yStep;
        x0 = (int32)(x);
        y0 = (int32)(y);
    }
}

/// <summary>
/// Goal is to determine whether or not to increment y based on the distance to the line from current y and next y. The closest will be the y for pixel.
/// We want to calculate y distance 1 (d1) and y distance2 (d2)
/// m = (yEnd - yStart) / (xEnd - xStart) (slope of the line)
/// xnext = xk+1
/// ynext = yk OR yk+1
/// 
/// y = mx + c
/// y = m(xk + 1) + c
/// 
/// d1 = y - yk = m(xk+1) + c - yk
/// d2 = yk+1 - y = yk+1 - m(xk+1) - c
/// 
/// if d1-d2 < 0 : ynext = yk  (this means that yk lies closer to the line than yk+1)
/// if d1-d2 > 0 : ynext = yk+1 (this means that yk+1 lies closer to the line than yk)
/// 
/// d1-d2 = m(xk+1) + c - yk - yk-1 + m(xk+1) + c
/// = 2*m*(xk+1) + 2c - 2yk - 1
/// what makes bresenham more efficient is that it should only operate on integer values. But here we still have a float with m (the slope).
/// to remove it we multiply the expression with dx since m = dy/dx
/// 
/// dx*(d1-d2) = dx*(2*(dy/dx)*(xk+1) + 2c - 2yk - 1)
/// Pk = dx*(d1-d2) = 2*dy*xk- 2*dx*yk + [2*dx*c - dx + 2dy] -> konstant
/// ->  Pk = dx*(d1-d2) = 2*dy*xk - 2*dx*yk
/// 
///  Pk = 2*dy*xk - 2*dx*yk
///  Pnext = 2*dy*xnext - 2*dx*ynext
///  Pnext - Pk = (2*dy*xnext - 2*dx*ynext) - (2*dy*xk - 2*dx*yk) = 2*dy*xnext - 2*dx*ynext - 2*dy*xk + 2*dx*yk
///  Pnext - Pk = 2*dy*(xnext - xk) - 2*dx*(ynext-yk) 
///  
///  if Pnext - Pk < 0 :
///     Pnext = Pk + 2dy(xk+1-xk) - 2dx(yk-yk) = Pk + 2dy  (ynext=yk)
/// 
///  if Pnext - Pk >= 0 :
///     Pnext = Pk + 2dy(xk+1-xk) - 2dx(yk+1-yk) = Pk + 2dy - 2dx (ynext=yk+1)
/// 
///  find the initial value of Pk:
///  
///  Pk = 2*dy*xk- 2*dx*yk + 2*dx*c - dx + 2dy
/// 
///  y = mx + c
///  -> c = (yk - (dy/dx) * xk)
/// 
///  Pk = 2*dy*xk- 2*dx*yk + 2*dx*(yk - (dy/dx) * xk) - dx + 2dy = [2*dy*xk] - [2*dx*yk] + 2*dy + [2*dx*yk] - [2*dy*xk] - dx
///  Pk = 2*dy - dx
/// </summary>
/// <param name="buffer"></param>
/// <param name="start"></param>
/// <param name="end"></param>
/// <param name="red"></param>
/// <param name="green"></param>
/// <param name="blue"></param>
/// <returns></returns>
internal void fn_renderer_draw_line_bresenham(game_offscreen_buffer* buffer, vec2i start, vec2i end, uint8 red, uint8 green, uint8 blue)
{
    int32 x0 = start.x;
    int32 y0 = start.y;
    int32 x1 = end.x;
    int32 y1 = end.y;

    bool steep = false;

    if (abs(x1 - x0) < abs(y1 - y0))
    {
        fn_swap(&x0, &y0);
        fn_swap(&x1, &y1);

        steep = true;
    }

    if (x1 < x0)
    {
        fn_swap(&x0, &x1);
        fn_swap(&y0, &y1);
    }

    int32 dy = y1 - y0;
    int32 dx = x1 - x0;

    int32 derror = abs(dy) * 2;
    int32 error = 0;
    int32 y = y0;

    const int32 yincr = y1 > y0 ? 1 : -1;

    if (steep)
    {
        for (int32 x = x0; x <= x1; x++)
        {
            fn_renderer_draw_pixel(buffer, y, x, red, green, blue);

            error += derror;

            if (error > dx)
            {
                y += yincr;
                error -= dx * 2;
            }
        }
    }
    else
    {
        for (int32 x = x0; x <= x1; x++)
        {
            fn_renderer_draw_pixel(buffer, x, y, red, green, blue);

            error += derror;

            if (error > dx)
            {
                y += yincr;
                error -= dx * 2;
            }
        }
    }
}

internal void fn_renderer_draw_triangle(game_offscreen_buffer* buffer, vec2i v1, vec2i v2, vec2i v3, uint8 red, uint8 green, uint8 blue)
{
    // sort vertices by y  (lower to upper)
    if (v1.y > v2.y) fn_swap(&v1, &v2);
    if (v1.y > v3.y) fn_swap(&v1, &v3);
    if (v2.y > v3.y) fn_swap(&v2, &v3);

    AABB boundingBox = fn_math_get_bounding_box_for_triangle(v1, v2, v3, true);

    for (int32 y = boundingBox.Ymin; y <= boundingBox.Ymax; y++)
    {
        for (int32 x = boundingBox.Xmin; x <= boundingBox.Xmax; x++)
        {
            if (fn_math_point_in_triangle(vec2i{x,y}, v1, v2, v3))
            {
                fn_renderer_draw_pixel(buffer, x, y, red, green, blue);
            }
        }
    }
}

