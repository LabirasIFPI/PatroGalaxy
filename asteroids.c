#include "asteroids.h"
#include "ssd1306.h"
#include <stdlib.h>
#include "initialize.h"
#include <math.h>

#define DEG2RAD 0.0174532925 // Degrees to radians: (PI / 180)

Asteroid asteroids[MAX_ASTEROIDS]; // Definição da variável global para os asteroides

void initAsteroids()
{
    for (int i = 0; i < MAX_ASTEROIDS; i++)
    {
        asteroids[i].box.x = SCREEN_WIDTH + (rand() % 100);
        asteroids[i].box.y = rand() % SCREEN_HEIGHT;
        asteroids[i].box.w = 8; // Largura dos asteroides
        asteroids[i].box.h = 8; // Altura dos asteroides
        asteroids[i].dx = -1;   // Velocidade em x
        asteroids[i].dy = 0;    // Velocidade em y
        asteroids[i].angle = rand() % 360;
        // asteroids[i].dy = (rand() % 3) - 1; // Velocidade em y (-1, 0, 1)
        asteroids[i].active = 1; // Ativar o asteroide
    }
}

void moveAsteroids()
{
    for (int i = 0; i < MAX_ASTEROIDS; i++)
    {
        if (asteroids[i].active)
        {
            asteroids[i].box.x += asteroids[i].dx;
            asteroids[i].box.y += asteroids[i].dy;

            asteroids[i].angle += 3;
            asteroids[i].angle = asteroids[i].angle % 360;

            // Verificar se o asteroide saiu da tela e reposicionar
            if (asteroids[i].box.x < asteroids[i].box.w * -1)
                asteroids[i].active = 0; // Desativar o asteroide ao sair da tela
            if (asteroids[i].box.y < 0)
                asteroids[i].box.y = SCREEN_HEIGHT - 1;
            if (asteroids[i].box.y >= SCREEN_HEIGHT)
                asteroids[i].box.y = 0;
        }
    }
}

void drawAsteroids()
{
    for (int i = 0; i < MAX_ASTEROIDS; i++)
    {
        if (asteroids[i].active)
        {

            int _x = asteroids[i].box.x;
            int _y = asteroids[i].box.y;
            int _w = asteroids[i].box.w / 2; // Usando metade da largura para centralizar
            int _h = asteroids[i].box.h / 2; // Usando metade da altura para centralizar

            int ang = asteroids[i].angle;

            // Coordenadas do quadrado rotatório
            int _x1 = _x + cos(ang * DEG2RAD) * _w - sin(ang * DEG2RAD) * _h;
            int _y1 = _y + sin(ang * DEG2RAD) * _w + cos(ang * DEG2RAD) * _h;
            int _x2 = _x - cos(ang * DEG2RAD) * _w - sin(ang * DEG2RAD) * _h;
            int _y2 = _y - sin(ang * DEG2RAD) * _w + cos(ang * DEG2RAD) * _h;
            int _x3 = _x - cos(ang * DEG2RAD) * _w + sin(ang * DEG2RAD) * _h;
            int _y3 = _y - sin(ang * DEG2RAD) * _w - cos(ang * DEG2RAD) * _h;
            int _x4 = _x + cos(ang * DEG2RAD) * _w + sin(ang * DEG2RAD) * _h;
            int _y4 = _y + sin(ang * DEG2RAD) * _w - cos(ang * DEG2RAD) * _h;

            // Desenhando o quadrado rotatório
            ssd1306_draw_line(&display, _x1, _y1, _x2, _y2);
            ssd1306_draw_line(&display, _x2, _y2, _x3, _y3);
            ssd1306_draw_line(&display, _x3, _y3, _x4, _y4);
            ssd1306_draw_line(&display, _x4, _y4, _x1, _y1);

            // Quadrado fixo na frente do asteroide
            ssd1306_clear_square(&display, _x - _w, _y - _h, _w * 2, _h * 2);
            ssd1306_draw_empty_square(&display, _x - _w, _y - _h, _w * 2, _h * 2);

            // Ponto central do asteroide
            ssd1306_draw_pixel(&display, _x, _y);
        }
    }
}

int getAsteroidsActive()
{
    int count = 0;
    for (int i = 0; i < MAX_ASTEROIDS; i++)
    {
        if (asteroids[i].active)
        {
            count++;
        }
    }
    return count;
}

void spawnAsteroid()
{
    for (int i = 0; i < MAX_ASTEROIDS; i++)
    {
        if (!asteroids[i].active)
        {
            asteroids[i].box.x = SCREEN_WIDTH + 32;
            asteroids[i].box.y = rand() % SCREEN_HEIGHT;
            asteroids[i].box.w = 8;
            asteroids[i].box.h = 8;
            asteroids[i].dx = -1;
            asteroids[i].dy = 0;
            // asteroids[i].dy = (rand() % 3) - 1; // -1, 0, 1
            asteroids[i].active = 1;
            asteroids[i].angle = rand() % 360;
            break;
        }
    }
}