#include <stdio.h>
#include <stdint.h> // Para garantir que uint32_t seja definido
#include "initialize.h"
#include "hardware/adc.h" // Analógico

// Definição do tipo de função de callback
typedef void (*callback_t)(uint gpio, uint32_t events);

/**
 * @brief Initializes buttons with the specified callback function.
 *
 * This function initializes two buttons (BTA and BTB) by setting up their GPIO pins,
 * configuring them as input, enabling pull-up resistors, and setting up an interrupt
 * on the rising edge with the provided callback function.
 *
 * @param handleButtonGPIOEvent The callback function to be called when a button is pressed.
 *                         If NULL, the function returns immediately without initializing the buttons.
 */
void initButtons(callback_t handleButtonGPIOEvent)
{
    if (handleButtonGPIOEvent == NULL)
        return;
    int buttons[2] = {BTA, BTB};
    for (int i = 0; i < 2; i++)
    {
        gpio_init(buttons[i]);
        gpio_set_dir(buttons[i], GPIO_IN);
        gpio_pull_up(buttons[i]);
        gpio_set_irq_enabled_with_callback(buttons[i], GPIO_IRQ_EDGE_RISE, true, handleButtonGPIOEvent);
        printf("Botão %d inicializado\n", i);
    }
}