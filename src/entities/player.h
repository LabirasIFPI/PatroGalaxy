#ifndef PLAYER_H
#define PLAYER_H
#include <stdint.h>
#include <stdio.h>
#include "pico/stdlib.h"
#include "boundingBox.h"

#define MAX_BULLETS 3
#define MAX_PARTICLES 10

typedef struct
{
    int x;
    int y;
    int dx;
    int time;
} ShipParticle;

// Estrutura do Player
typedef struct
{
    BoundingBox box;
    ShipParticle particles[10]; // Array of particles
} Player;

// Esrtrutura da Bala
typedef struct
{
    BoundingBox box;
    int dx;
    int dy;
    int active;
} Bullet;

// Declarações das funções
void initPlayer(Player *player);
void movePlayer(Player *player, int deltaX, int deltaY);
void drawPlayer(Player *player);
bool checkPlayerCollision();

void initPlayerParticles(Player *player);

void initBullets();
void updateBullets();
void drawBullets();
void shoot(Player *player);
bool checkBulletsCollisions();

// Variável global para o Player
extern Player player;
extern Bullet bullets[MAX_BULLETS];
extern int playerInvulnerableTimer; // Tempo de invulnerabilidade do player

#endif // PLAYER_H