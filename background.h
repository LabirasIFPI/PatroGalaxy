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

// Estrutura de bounding box
typedef struct
{
    int x;
    int y;
    int w;
    int h;
} BoundingBox;

// Estrutura para representar um asteroide
typedef struct
{
    BoundingBox box;
    int dx;
    int dy;
    int active;
} Asteroid;

// asteroid.box.x;    asteroid.box.width 

// struct de coordenadas limites de uma bound box
typedef struct {
    int x1;
    int y1;
    int x2;
    int y2;
}BoxCoords;

// BoxCoords getBoxCoords(BoundingBox box){
//     BoxCoords coords;
//     coords.x1 = box.x - box.w/2;
//     coords.y1 = box.y - box.h/2;
//     coords.x2 = box.x + box.w/2;
//     coords.y2 = box.y + box.h/2;
//     return coords;
// }

#endif // BACKGROUND_H
