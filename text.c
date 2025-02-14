#include "text.h"

void drawHeader(int x, int y, char *text)
{
    ssd1306_draw_string(&display, x, y, 2, text);
}

void drawText(int x, int y, char *text)
{
    ssd1306_draw_string(&display, x, y, 1, text);
}

void drawTextCentered(char *text, int _y)
{
    if (_y == -1)
    {
        _y = SCREEN_HEIGHT / 2 - 6;
    }
    int _x = SCREEN_WIDTH / 2 - 6 * strlen(text) / 2 - 1;
    ssd1306_draw_string(&display, _x, _y, 1, text);
}

void drawWave(int y, float speed, float amplitude)
{
    static float time = 0;
    time += speed / 100.0;
    int _points = 12;
    for (int i = 0; i < _points; i++)
    {
        int _x1 = SCREEN_WIDTH / _points * i;
        int _x2 = SCREEN_WIDTH / _points * (i + 1);
        int _y1 = y + sin(time + i * 30) * amplitude;
        int _y2 = y + sin(time + (i + 1) * 30) * amplitude;
        ssd1306_draw_line(&display, _x1, _y1, _x2, _y2);
    }
}