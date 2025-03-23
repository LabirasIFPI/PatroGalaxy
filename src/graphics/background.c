/**
 * @file background.c
 * @brief Implementation for the background module.
 *
 * This module handles the background elements of the game,
 * such as the stars.
 */

#include "background.h"
#include "ssd1306.h"
#include <stdlib.h>
#include "initialize.h"
#include "display.h"

/**
 * @brief Global array for the stars.
 */
Star stars[10];

/**
 * @brief Initializes the stars.
 *
 * This function initializes the positions of the stars randomly on the screen.
 */
void initStars()
{
    for (int i = 0; i < 10; i++)
    {
        stars[i].x = rand() % SCREEN_WIDTH;
        stars[i].y = rand() % SCREEN_HEIGHT;
    }
}

/**
 * @brief Moves the stars.
 *
 * This function moves the stars across the screen, wrapping them around
 * when they reach the edge.
 *
 * @param starsSpeed Speed of the stars.
 */
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

/**
 * @brief Draws the stars.
 *
 * This function draws the stars on the screen using the
 * ssd1306_draw_pixel function.
 */
void drawStars()
{
    for (int i = 0; i < 10; i++)
    {
        ssd1306_draw_pixel(&display, stars[i].x, stars[i].y);
    }
}