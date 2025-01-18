#ifndef PLAYER_H
#define PLAYER_H
#include <stdint.h>
#include <stdio.h>
#include "pico/stdlib.h"

// Estrutura do Player
typedef struct
{
    int x;
    int y;
} Player;

// Declarações das funções
void initPlayer(Player *player);
void movePlayer(Player *player, int deltaX, int deltaY);
void drawPlayer(Player *player);

// Variável global para o Player
extern Player player;

#endif // PLAYER_H