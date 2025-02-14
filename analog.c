#include "analog.h"
int analog_x = 0;
int analog_y = 0;
/**
 * @brief Initializes the analog inputs and button.
 *
 * This function sets up the ADC (Analog-to-Digital Converter) and configures
 * the GPIO (General-Purpose Input/Output) pins for the analog inputs and button.
 * It initializes the ADC, sets up the GPIO pins for the analog X and Y inputs,
 * and configures the button pin as an input with a pull-up resistor.
 */
void initAnalog()
{
    adc_init();
    adc_gpio_init(ANALOG_X);
    adc_gpio_init(ANALOG_Y);
    gpio_init(ANALOG_BTN);
    gpio_set_dir(ANALOG_BTN, GPIO_IN);
    gpio_pull_up(ANALOG_BTN);
}

int32_t readAnalogY()
{
    adc_select_input(0);
    uint32_t adc_value = adc_read();
    int32_t mapped_value = mapValue(adc_value, 0, 4095, -ANALOG_MAX_VALUE, ANALOG_MAX_VALUE);
    int32_t inverted_value = -mapped_value;
    return applyThreshold(inverted_value);
}

int32_t readAnalogX()
{
    adc_select_input(1);
    uint32_t adc_value = adc_read();
    int32_t mapped_value = mapValue(adc_value, 0, 4095, -ANALOG_MAX_VALUE, ANALOG_MAX_VALUE);
    return applyThreshold(mapped_value);
}

int32_t applyThreshold(int32_t value)
{
    if (value > DEADZONE || value < -DEADZONE)
    {
        return value;
    }
    else
    {
        return 0;
    }
}

void updateAxis()
{
    analog_x = readAnalogX();
    analog_y = readAnalogY();
}