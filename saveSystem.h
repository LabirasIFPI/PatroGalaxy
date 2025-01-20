#ifndef SAVESYSTEM_H
#define SAVESYSTEM_H

#include "pico/stdlib.h"
#include "hardware/flash.h"
#include "hardware/sync.h"

#define FLASH_TARGET_OFFSET (256 * 1024)

void saveProgress(uint16_t *progressString);
void loadProgress(uint16_t *buffer, size_t tamanho);
void clearSaveData();

#endif // SAVESYSTEM_H