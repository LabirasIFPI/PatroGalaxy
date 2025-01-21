#ifndef SAVESYSTEM_H
#define SAVESYSTEM_H

#include "pico/stdlib.h"
#include "hardware/flash.h"
#include "hardware/sync.h"

#define FLASH_TARGET_OFFSET (256 * 1024)

void saveProgress(uint8_t *progressString);
void loadProgress(uint8_t *buffer, size_t tamanho);
void clearSaveData();

void createBuffer(uint16_t highscore, uint8_t *buffer);
void loadBuffer(uint8_t *buffer, uint16_t *highscore);

#endif // SAVESYSTEM_H