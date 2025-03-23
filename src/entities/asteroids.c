/**
 * @file asteroids.c
 * @brief Implementation for the asteroid module.
 *
 * This module manages the asteroids, including their initialization,
 * movement, drawing, and spawning.
 */

#include "asteroids.h"
#include "ssd1306.h"
#include <stdlib.h>
#include "initialize.h"
#include <math.h>
#include "display.h"

/**
 * Degrees to radians conversion constant: (PI / 180)
 */
#define DEG2RAD 0.0174532925

/**
 * @brief Global variable for the asteroids array.
 */
Asteroid asteroids[MAX_ASTEROIDS];

/**
 * @brief Initializes the asteroids.
 *
 * Sets the initial position, velocity, and active state for each asteroid.
 */
void initAsteroids()
{
    for (int i = 0; i < MAX_ASTEROIDS; i++)
    {
        asteroids[i].box.x = SCREEN_WIDTH + (rand() % 100);    // initial x position offscreen
        asteroids[i].box.y = 8 + rand() % (SCREEN_HEIGHT - 8); // random y position
        asteroids[i].box.w = 8;                                // Asteroid width
        asteroids[i].box.h = 8;                                // Asteroid height
        asteroids[i].dx = -1;                                  // Velocity in x
        asteroids[i].dy = 0;                                   // Velocity in y
        asteroids[i].angle = rand() % 360;                     // Random angle
        asteroids[i].active = (i < 3);                         // Ativar os 3 primeiros asteroides.
    }
}

/**
 * @brief Moves the asteroids.
 *
 * Updates the position and angle of each active asteroid.
 *
 * @param asteroidsSpeed Speed multiplier for the asteroids' movement.
 */
void moveAsteroids(float asteroidsSpeed)
{
    for (int i = 0; i < MAX_ASTEROIDS; i++)
    {
        if (asteroids[i].active)
        {
            asteroids[i].box.x += asteroids[i].dx * asteroidsSpeed;
            asteroids[i].box.y += asteroids[i].dy * asteroidsSpeed;

            asteroids[i].angle += 3;
            asteroids[i].angle = asteroids[i].angle % 360;

            // Check if asteroid has left the screen and reposition it
            if (asteroids[i].box.x < asteroids[i].box.w * -1)
                asteroids[i].active = 0; // Deactivate asteroid when it leaves the screen
            if (asteroids[i].box.y < 0)
                asteroids[i].box.y = SCREEN_HEIGHT - 1;
            if (asteroids[i].box.y >= SCREEN_HEIGHT)
                asteroids[i].box.y = 0;
        }
    }
}

/**
 * @brief Draws the asteroids.
 *
 * Draws each active asteroid as a rotating square and a center pixel.
 */
void drawAsteroids()
{
    for (int i = 0; i < MAX_ASTEROIDS; i++)
    {
        if (asteroids[i].active)
        {

            int _x = asteroids[i].box.x;
            int _y = asteroids[i].box.y;
            int _w = asteroids[i].box.w / 2; // Using half of the width to centralize
            int _h = asteroids[i].box.h / 2; // Using half of the height to centralize

            int ang = asteroids[i].angle;

            // Coordinates of the rotating square
            int _x1 = _x + cos(ang * DEG2RAD) * _w - sin(ang * DEG2RAD) * _h;
            int _y1 = _y + sin(ang * DEG2RAD) * _w + cos(ang * DEG2RAD) * _h;
            int _x2 = _x - cos(ang * DEG2RAD) * _w - sin(ang * DEG2RAD) * _h;
            int _y2 = _y - sin(ang * DEG2RAD) * _w + cos(ang * DEG2RAD) * _h;
            int _x3 = _x - cos(ang * DEG2RAD) * _w + sin(ang * DEG2RAD) * _h;
            int _y3 = _y - sin(ang * DEG2RAD) * _w - cos(ang * DEG2RAD) * _h;
            int _x4 = _x + cos(ang * DEG2RAD) * _w + sin(ang * DEG2RAD) * _h;
            int _y4 = _y + sin(ang * DEG2RAD) * _w - cos(ang * DEG2RAD) * _h;

            // Drawing the rotating square
            ssd1306_draw_line(&display, _x1, _y1, _x2, _y2);
            ssd1306_draw_line(&display, _x2, _y2, _x3, _y3);
            ssd1306_draw_line(&display, _x3, _y3, _x4, _y4);
            ssd1306_draw_line(&display, _x4, _y4, _x1, _y1);

            // Fixed square in front of the asteroid
            ssd1306_clear_square(&display, _x - _w, _y - _h, _w * 2, _h * 2);
            ssd1306_draw_empty_square(&display, _x - _w, _y - _h, _w * 2, _h * 2);

            // Central point of the asteroid
            ssd1306_draw_pixel(&display, _x, _y);
        }
    }
}

/**
 * @brief Get number of active asteroids.
 *
 * Iterate for all possible asteroirds and verify if it's active.
 *
 * @return Amount of active asteroids
 */
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

/**
 * @brief Spawns a new asteroid.
 *
 * Activates a non used asteroid
 */
void spawnAsteroid()
{
    for (int i = 0; i < MAX_ASTEROIDS; i++)
    {
        if (!asteroids[i].active)
        {
            asteroids[i].box.x = SCREEN_WIDTH + 32;
            asteroids[i].box.y = 8 + rand() % (SCREEN_HEIGHT - 8);
            asteroids[i].box.w = 8;
            asteroids[i].box.h = 8;
            asteroids[i].dx = -1;
            asteroids[i].dy = 0;
            asteroids[i].active = 1;
            asteroids[i].angle = rand() % 360;
            break;
        }
    }
}