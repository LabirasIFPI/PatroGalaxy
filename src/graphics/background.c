#include "background.h"
#include "ssd1306.h"
#include <stdlib.h>
#include "initialize.h"
#include "display.h"

// Variável global para as estrelas
Star stars[10];

void initStars()
{
    for (int i = 0; i < 10; i++)
    {
        stars[i].x = rand() % SCREEN_WIDTH;
        stars[i].y = rand() % SCREEN_HEIGHT;
    }
}

void moveStars(float starsSpeed)
{
    for (int i = 0; i < 10; i++)
    {
        stars[i].x -= starsSpeed;
        if (stars[i].x < 0)
        {
            stars[i].x = SCREEN_WIDTH;
            stars[i].y = rand() % SCREEN_HEIGHT;
        }
    }
}

void drawStars()
{
    for (int i = 0; i < 10; i++)
    {
        ssd1306_draw_pixel(&display, stars[i].x, stars[i].y);
    }
}
