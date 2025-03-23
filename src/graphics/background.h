/**
 * @file background.h
 * @brief Header file for the background module.
 *
 * This module handles the background elements of the game,
 * such as the stars.
 */

#ifndef BACKGROUND_H
#define BACKGROUND_H

#include <stdint.h>

/** @brief The Star speed */
#define STARS_SPEED 1

/**
 * @brief Structure to represent a star.
 */
typedef struct
{
    int x; /**< X-coordinate of the star. */
    int y; /**< Y-coordinate of the star. */
} Star;

/**
 * @brief Initializes the stars.
 */
void initStars();

/**
 * @brief Moves the stars.
 * @param starsSpeed Speed of the stars.
 */
void moveStars(float starsSpeed);

/**
 * @brief Draws the stars.
 */
void drawStars();

/** @brief Global array to manage stars*/
extern Star stars[10];

#endif // BACKGROUND_H