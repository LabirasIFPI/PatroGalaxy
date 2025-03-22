#include "draw.h"

void drawImage(const uint8_t *data, const long size, int x, int y)
{
    ssd1306_bmp_show_image_with_offset(&display, data, size, x, y);
}