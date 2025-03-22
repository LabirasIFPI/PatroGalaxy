#include "saveSystem.h"
#include <string.h>
#include <stdio.h>

void saveProgress(uint8_t *progressString)
{
    // Pausar interrupções
    uint32_t interruptions = save_and_disable_interrupts();
    flash_range_erase(FLASH_TARGET_OFFSET, FLASH_SECTOR_SIZE);
    flash_range_program(FLASH_TARGET_OFFSET, progressString, FLASH_PAGE_SIZE);
    restore_interrupts(interruptions);
    printf("Game saved\n");
}

void loadProgress(uint8_t *buffer, size_t tamanho)
{
    // Encontrar o local exato da memoria flash onde está o save
    uint8_t *address = (uint8_t *)(XIP_BASE + FLASH_TARGET_OFFSET);
    memcpy(buffer, address, tamanho);
}

void clearSaveData()
{
    // Encontrar o local exato da memoria flash onde etá o save
    uint32_t interruptions = save_and_disable_interrupts();
    flash_range_erase(FLASH_TARGET_OFFSET, FLASH_SECTOR_SIZE);
    flash_range_program(FLASH_TARGET_OFFSET, 0x00, FLASH_PAGE_SIZE);
    restore_interrupts(interruptions);
}

void createBuffer(uint16_t highscore, uint8_t *buffer)
{
    buffer[0] = (highscore >> 8) & 0xFF;
    buffer[1] = highscore & 0xFF;
}

void loadBuffer(uint8_t *buffer, uint16_t *highscore)
{
    *highscore = (buffer[0] << 8) | buffer[1];
}
