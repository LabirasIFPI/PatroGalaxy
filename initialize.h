#ifndef INIT_H
#define INIT_H
#include <stdint.h>
#include "pico/stdlib.h"

// Analógico
#define ANALOG_X 27
#define ANALOG_Y 26
#define ANALOG_BTN 22

// Buttons
#define BTA 5
#define BTB 6

void initI2C();
void initDisplay();
void initAnalog();
void initButtons(void (*callbackFunction)(uint gpio, uint32_t events));
int32_t mapValue();
int32_t readAnalogY();
int32_t readAnalogX();

// Display
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define SCREEN_ADDRESS 0x3C // Endereço I2C do display
#define I2C_SDA 14          // Pino SDA
#define I2C_SCL 15          // Pino SCL
#include "hardware/i2c.h"
#include "ssd1306.h"
extern ssd1306_t display;

#endif