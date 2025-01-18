#ifndef BACKGROUND_H
#define BACKGROUND_H

#include <stdint.h>

// Estrutura para representar uma estrela
typedef struct
{
    int x;
    int y;
} Star;

// Declarações das funções
void initStars();
void moveStars();
void drawStars();

// Variáveis globais
extern Star stars[10];
#define STARS_SPEED 1

#endif // BACKGROUND_H
