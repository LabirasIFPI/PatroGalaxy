#ifndef DISPLAY_H
#define DISPLAY_H

#define SCREEN_WIDTH 128    // Display Width
#define SCREEN_HEIGHT 64    // Display Height
#define SCREEN_ADDRESS 0x3C // Display I2C Address
#define I2C_SDA 14          // SDA Pin
#define I2C_SCL 15          // SCL Pin

#include <stdio.h>
#include "hardware/i2c.h"
#include "ssd1306.h"
extern ssd1306_t display;

void initI2C();
void initDisplay();
void clearDisplay();
void showDisplay();

#endif // DISPLAY_H