/**
 * @file saveSystem.c
 * @brief Implementation of the save system module.
 *
 * This module provides functions for saving and loading game progress
 * to and from the Raspberry Pi Pico W's flash memory.  It also handles
 * the creation and loading of the game buffer.
 */

#include "saveSystem.h"
#include <string.h>
#include <stdio.h>

/**
 * @brief Saves the game progress to flash memory.
 *
 * This function erases the flash sector and programs the provided
 * data, ensuring that interrupts are disabled during the operation.
 *
 * @param progressString Pointer to the data to be saved. Must be less than FLASH_PAGE_SIZE bytes.
 * @note It's crucial that progressString points to a valid buffer, and that its size is less than FLASH_PAGE_SIZE (256 bytes)
 * This function also uses interrupts and a printf command for debug porpuses.
 */
void saveProgress(uint8_t *progressString)
{
    // Pause interruptions.
    uint32_t interruptions = save_and_disable_interrupts();
    flash_range_erase(FLASH_TARGET_OFFSET, FLASH_SECTOR_SIZE);
    flash_range_program(FLASH_TARGET_OFFSET, progressString, FLASH_PAGE_SIZE);
    restore_interrupts(interruptions);
    printf("Game saved.\n");
}

/**
 * @brief Loads the game progress from flash memory.
 *
 * This function copies the saved data from flash memory into the provided buffer.
 *
 * @param buffer Pointer to the buffer where the loaded data will be stored.
 * @param tamanho The number of bytes to load.
 * @note It is important that a valid buffer is provided and that it has already been declared and initialized.
 * This function access a flash memory area.
 */
void loadProgress(uint8_t *buffer, size_t tamanho)
{
    // Encontrar o local exato da memoria flash onde está o save
    uint8_t *address = (uint8_t *)(XIP_BASE + FLASH_TARGET_OFFSET);
    memcpy(buffer, address, tamanho);
}

/**
 * @brief Clears the saved game data in flash memory.
 *
 * This function erases the flash sector, effectively resetting the
 * game's saved progress.
 *
 * @note It is important to use only to erase data of a game or save, don't use if to erase core functions of SO.
 */
void clearSaveData()
{
    // Find the exact location in flash memory where the save is
    uint32_t interruptions = save_and_disable_interrupts();
    flash_range_erase(FLASH_TARGET_OFFSET, FLASH_SECTOR_SIZE);
    flash_range_program(FLASH_TARGET_OFFSET, 0x00, FLASH_PAGE_SIZE);
    restore_interrupts(interruptions);
}

/**
 * @brief Creates a buffer containing the highscore.
 *
 * This function convert a number to array in order to save it to the Flash memory.
 * The highscore is saved as an uint16, but it is converted to a 2-byte array in this function.
 *
 * @param highscore The highscore number to be added to the array
 * @param buffer The created array
 * @note For performance and clear memory pourpouses, array size is 2
 */
void createBuffer(uint16_t highscore, uint8_t *buffer)
{
    buffer[0] = (highscore >> 8) & 0xFF;
    buffer[1] = highscore & 0xFF;
}

/**
 * @brief Loads the number from the buffer array
 *
 * This function convert the array to an Number.
 * The highscore is created from a 2-byte array.
 *
 * @param buffer The array to be convertered.
 * @param highscore pointer to the number that would be saved the array.
 * @note For performance and clear memory pourpouses, array size is 2
 */
void loadBuffer(uint8_t *buffer, uint16_t *highscore)
{
    *highscore = (buffer[0] << 8) | buffer[1];
}
