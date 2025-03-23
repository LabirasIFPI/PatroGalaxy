/**
 * @file saveSystem.h
 * @brief Header file for the save system module.
 */

#ifndef SAVESYSTEM_H
#define SAVESYSTEM_H

#include "pico/stdlib.h"
#include "hardware/flash.h"
#include "hardware/sync.h"

/**
 * @brief Flash memory offset for game data.
 */
#define FLASH_TARGET_OFFSET (512 * 1024)

/**
 * @brief Saves the game progress to flash memory.
 * @param progressString Data to be saved.
 */
void saveProgress(uint8_t *progressString);

/**
 * @brief Loads the game progress from flash memory.
 * @param buffer Buffer for loaded data.
 * @param size Number of bytes to load.
 */
void loadProgress(uint8_t *buffer, size_t size);

/**
 * @brief Clears the saved game data in flash memory.
 */
void clearSaveData();

/**
 * @brief Creates a buffer containing the highscore.
 * @param highscore The highscore number.
 * @param buffer The created array
 */
void createBuffer(uint16_t highscore, uint8_t *buffer);

/**
 * @brief Loads the number from the buffer array.
 * @param buffer The array to be convertered.
 * @param highscore pointer to the number that would be saved the array.
 */
void loadBuffer(uint8_t *buffer, uint16_t *highscore);

#endif // SAVESYSTEM_H