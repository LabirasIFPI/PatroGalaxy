#include <stdio.h>
#include <stdint.h> // Para garantir que uint32_t seja definido
#include "initialize.h"
#include "hardware/adc.h" // Analógico

/**
 * @brief Initializes the I2C interface with a specified frequency and configures the GPIO pins.
 *
 * This function sets up the I2C interface on the specified I2C peripheral (i2c1) with a frequency of 400 kHz.
 * It configures the GPIO pins for I2C data (SDA) and clock (SCL) functions and enables the pull-up resistors
 * on these pins. After initialization, it prints a confirmation message to the console.
 *
 * @note Ensure that the I2C peripheral and GPIO pins are correctly defined and available in your hardware setup.
 */
void initI2C()
{
    i2c_init(i2c1, 400 * 1000); // 400 kHz
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
    printf("I2C inicializado\n");
}

/**
 * @brief Initializes the SSD1306 display.
 *
 * This function initializes the SSD1306 display with the specified parameters.
 * It checks if the initialization is successful and prints a message accordingly.
 *
 * @note The function uses the global variables `display`, `SCREEN_WIDTH`, `SCREEN_HEIGHT`,
 * `SCREEN_ADDRESS`, and `i2c1` for initialization.
 *
 * @return void
 */
void initDisplay()
{
    if (!ssd1306_init(&display, SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_ADDRESS, i2c1))
    {
        printf("Falha ao inicializar o display SSD1306\n");
    }
    else
    {
        printf("Display SSD1306 inicializado\n");
    }
}

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
    printf("Analogs inicializado\n");
}

// Definição do tipo de função de callback
typedef void (*callback_t)(uint gpio, uint32_t events);


/**
 * @brief Initializes buttons with the specified callback function.
 *
 * This function initializes two buttons (BTA and BTB) by setting up their GPIO pins,
 * configuring them as input, enabling pull-up resistors, and setting up an interrupt
 * on the rising edge with the provided callback function.
 *
 * @param callbackFunction The callback function to be called when a button is pressed.
 *                         If NULL, the function returns immediately without initializing the buttons.
 */
void initButtons(callback_t callbackFunction)
{
    if (callbackFunction == NULL)
        return;
    int buttons[2] = {BTA, BTB};
    for (int i = 0; i < 2; i++)
    {
        gpio_init(buttons[i]);
        gpio_set_dir(buttons[i], GPIO_IN);
        gpio_pull_up(buttons[i]);
        gpio_set_irq_enabled_with_callback(buttons[i], GPIO_IRQ_EDGE_RISE, true, callbackFunction);
        printf("Botão %d inicializado\n", i);
    }
}



#define DEADZONE 2

int32_t applyThreshold(int32_t value, int32_t threshold)
{
    if (value > threshold || value < -threshold)
    {
        return value;
    }
    else
    {
        return 0;
    }
}

// Inverte o valor retornado de readAnalogY
int32_t readAnalogY()
{
    adc_select_input(0);
    uint32_t adc_value = adc_read();
    int32_t mapped_value = mapValue(adc_value, 0, 4095, -5, 5);
    int32_t inverted_value = -mapped_value;
    return applyThreshold(inverted_value, DEADZONE);
}

int32_t readAnalogX()
{
    adc_select_input(1);
    uint32_t adc_value = adc_read();
    int32_t mapped_value = mapValue(adc_value, 0, 4095, -5, 5);
    return applyThreshold(mapped_value, DEADZONE);
}
