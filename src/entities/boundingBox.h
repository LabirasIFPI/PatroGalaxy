#ifndef BOUDINGBOX_H
#define BOUDINGBOX_H

// Bounding box structure
typedef struct
{
    int x;
    int y;
    int w;
    int h;
} BoundingBox;

int checkCollision(BoundingBox *a, BoundingBox *b);

#endif // BOUDINGBOX_H