#include "player.h"
#include "initialize.h"

#include "ssd1306.h"

#include "pico/stdlib.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "boundingBox.h"

// Definição do Player (global)
Player player;

// Definição das balas (global)
Bullet bullets[MAX_BULLETS];

void initBullets()
{
    for (int i = 0; i < MAX_BULLETS; i++)
    {
        bullets[i].active = 0;
    }
}

void updateBullets()
{
    for (int i = 0; i < MAX_BULLETS; i++)
    {
        if (bullets[i].active)
        {
            bullets[i].box.x += bullets[i].dx;
            bullets[i].box.y += bullets[i].dy;
            if (bullets[i].box.x < 0 || bullets[i].box.x >= SCREEN_WIDTH || bullets[i].box.y < 0 || bullets[i].box.y >= SCREEN_HEIGHT)
            {
                bullets[i].active = 0;
            }
        }
    }
}

void drawBullets()
{
    for (int i = 0; i < MAX_BULLETS; i++)
    {
        if (bullets[i].active)
        {
            ssd1306_draw_char(&display, bullets[i].box.x, bullets[i].box.y, 1, '>');
        }
    }
}

/**
 * @brief Inicializa o Player na posição central da tela.
 * @param player Ponteiro para a estrutura do Player.
 */
void initPlayer(Player *player)
{
    player->box.x = -40;
    player->box.y = SCREEN_HEIGHT / 2;
    player->box.w = 14;
    player->box.h = 6;
}

/**
 * @brief Limitar posição do player dentro dos limites da tela.
 * @param player Ponteiro para a estrutura do Player.
 */
void limitPlayerPosition(Player *player)
{
    int limitY = 10;

    // Limitar posição no eixo X
    if (player->box.x - player->box.w / 2 < 0)
        player->box.x = player->box.w / 2;
    if (player->box.x + player->box.w / 2 >= SCREEN_WIDTH)
        player->box.x = SCREEN_WIDTH - player->box.w / 2;

    // Limitar posição no eixo Y
    if (player->box.y - player->box.h / 2 < limitY)
        player->box.y = limitY + player->box.h / 2;
    if (player->box.y + player->box.h / 2 >= SCREEN_HEIGHT - limitY)
        player->box.y = SCREEN_HEIGHT - limitY - player->box.h / 2;
}

void movePlayer(Player *player, int deltaX, int deltaY)
{
    player->box.x += deltaX / 2;
    player->box.y += deltaY / 2;

    limitPlayerPosition(player);

    printf("Player position: (%d, %d)\n", player->box.x, player->box.y);
}

void drawPlayer(Player *player)
{
    char text[4] = "3=D";
    int textWidth = 5 * strlen(text); // Assumindo que a fonte padrão tem 5 pixels de largura
    ssd1306_draw_string(&display, player->box.x - player->box.w / 2, player->box.y, 1, text);
}

void shoot(Player *player)
{
    for (int i = 0; i < MAX_BULLETS; i++)
    {
        if (!bullets[i].active)
        {
            bullets[i].box.x = player->box.x + 10;
            bullets[i].box.y = player->box.y;
            bullets[i].box.w = 2;
            bullets[i].box.h = 6;
            bullets[i].dx = 4;
            bullets[i].dy = 0;
            bullets[i].active = 1;
            break;
        }
    }
}
