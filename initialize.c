#include <stdio.h>
#include <stdint.h> // Para garantir que uint32_t seja definido
#include "initialize.h"
#include "hardware/adc.h" // Analógico

void initAnalog()
{
    adc_init();
    adc_gpio_init(ANALOG_X);
    adc_gpio_init(ANALOG_Y);
    gpio_init(ANALOG_BTN);
    gpio_set_dir(ANALOG_BTN, GPIO_IN);
    gpio_pull_up(ANALOG_BTN);
    printf("Analogs inicializado\n");
}

// Função auxiliar para mapear um valor de um intervalo para outro
int32_t mapValue(uint32_t value, uint32_t in_min, uint32_t in_max, int32_t out_min, int32_t out_max)
{
    return (int32_t)((value - in_min) * (out_max - out_min) / (in_max - in_min) + out_min);
}

#define DEADZONE 2

int32_t applyThreshold(int32_t value, int32_t threshold) {
    if (value > threshold || value < -threshold) {
        return value;
    } else {
        return 0;
    }
}

// Inverte o valor retornado de readAnalogY
int32_t readAnalogY() {
    adc_select_input(0);
    uint32_t adc_value = adc_read();
    int32_t mapped_value = mapValue(adc_value, 0, 4095, -5, 5);
    int32_t inverted_value = -mapped_value;
    return applyThreshold(inverted_value, DEADZONE);
}

int32_t readAnalogX() {
    adc_select_input(1);
    uint32_t adc_value = adc_read();
    int32_t mapped_value = mapValue(adc_value, 0, 4095, -5, 5);
    return applyThreshold(mapped_value, DEADZONE);
}
