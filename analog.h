#ifndef ANALOG_H
#define ANALOG_H
#include "hardware/adc.h"

#define ANALOG_X 27
#define ANALOG_Y 26
#define ANALOG_BTN 22

#define ANALOG_MAX_VALUE 5

#define DEADZONE 2

extern int analog_x;
extern int analog_y;

void initAnalog();

int32_t mapValue();
int32_t readAnalogY();
int32_t readAnalogX();
int32_t applyThreshold(int32_t value);

void updateAxis();

#endif // ANALOG_H