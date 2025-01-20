#ifndef ASTEROIDS_H
#define ASTEROIDS_H

#include <stdint.h>
#include "boundingBox.h"

// Asteroid structure
typedef struct
{
    BoundingBox box;
    int dx;
    int dy;
    int active;
    int angle;
} Asteroid;

// Asteroid functions
void initAsteroids();
void moveAsteroids(float asteroidsSpeed);
void drawAsteroids();
void spawnAsteroid();
int getAsteroidsActive();

// Variável global para os asteroides
#define MAX_ASTEROIDS 10
extern Asteroid asteroids[MAX_ASTEROIDS];

#endif // ASTEROIDS_H