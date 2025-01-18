#ifndef PLAYER_H
#define PLAYER_H
#include <stdint.h>
#include <stdio.h>
#include "pico/stdlib.h"

#define MAX_BULLETS 10

// Estrutura do Player
typedef struct
{
    int x;
    int y;
} Player;

// Esrtrutura da Bala
typedef struct
{
    int x;
    int y;
    int dx;
    int dy;
    int active;
} Bullet;

// Declarações das funções
void initPlayer(Player *player);
void movePlayer(Player *player, int deltaX, int deltaY);
void drawPlayer(Player *player);

void initBullets();
void updateBullets();
void drawBullets();
void shoot(Player *player);

// Variável global para o Player
extern Player player;

#endif // PLAYER_H