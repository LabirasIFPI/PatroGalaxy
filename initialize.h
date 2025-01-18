#ifndef INIT_H
#define INIT_H
#include <stdint.h>
#include "pico/stdlib.h"

// Analógico
#define ANALOG_X 27
#define ANALOG_Y 26
#define ANALOG_BTN 22

void initAnalog();
int32_t mapValue();
int32_t readAnalogY();
int32_t readAnalogX();

#endif