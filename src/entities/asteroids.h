/**
 * @file asteroids.h
 * @brief Header file for the asteroid module.
 *
 * This module manages the asteroids, including their initialization,
 * movement, drawing, and spawning.
 */

#ifndef ASTEROIDS_H
#define ASTEROIDS_H

#include <stdint.h>
#include "boundingBox.h"

/**
 * @brief Asteroid structure
 */
typedef struct
{
    BoundingBox box; /**< Bounding box for collision detection. */
    int dx;          /**< Horizontal velocity */
    int dy;          /**< Vertical velocity. */
    int active;      /**< Is active asteroid or not? */
    int angle;       /**< Rotation angle (in degrees). */
} Asteroid;

/**
 * @brief Asteroid functions
 */

/**
 *  @brief Initializes the asteroids.
 */
void initAsteroids();

/**
 * @brief Moves the asteroids.
 * @param asteroidsSpeed Speed of the asteroid movement.
 */
void moveAsteroids(float asteroidsSpeed);

/**
 * @brief Draws the asteroids.
 */
void drawAsteroids();

/**
 * @brief Spawns a new asteroid.
 */
void spawnAsteroid();

/**
 * @brief Get number of active asteroids.
 * @return Amount of active asteroids
 */
int getAsteroidsActive();

/** @brief Global variable for the asteroids array. */
#define MAX_ASTEROIDS 10
extern Asteroid asteroids[MAX_ASTEROIDS];

#endif // ASTEROIDS_H