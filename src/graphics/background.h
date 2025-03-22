#ifndef BACKGROUND_H
#define BACKGROUND_H
#define STARS_SPEED 1

#include <stdint.h>

// Estrutura para representar uma estrela
typedef struct
{
    int x;
    int y;
} Star;

// Declarações das funções
void initStars();
void moveStars(float starsSpeed);
void drawStars();

// Variáveis globais
extern Star stars[10];

#endif // BACKGROUND_H
