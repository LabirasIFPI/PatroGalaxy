#include "player.h"
#include "initialize.h"

#include "ssd1306.h"

#include "pico/stdlib.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

// Definição do Player (global)
Player player = {SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2};

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
    player->x = -40;
    player->y = SCREEN_HEIGHT / 2;
}

/**
 * @brief Limitar posição do player dentro dos limites da tela.
 * @param player Ponteiro para a estrutura do Player.
 */
void limitPlayerPosition(Player *player)
{
    int limitY = 10;

    if (player->x < 0)
        player->x = 0;
    if (player->x >= SCREEN_WIDTH)
        player->x = SCREEN_WIDTH - 1;
    if (player->y < limitY)
        player->y = limitY;
    if (player->y >= SCREEN_HEIGHT - limitY)
        player->y = SCREEN_HEIGHT - 1 - limitY;
}

void movePlayer(Player *player, int deltaX, int deltaY)
{
    player->x += deltaX / 2;
    player->y += deltaY / 2;

    limitPlayerPosition(player);

    printf("Player position: (%d, %d)\n", player->x, player->y);
}

void drawPlayer(Player *player)
{
    char text[4] = "3=D";
    int textWidth = 5 * strlen(text); // Assumindo que a fonte padrão tem 5 pixels de largura
    ssd1306_draw_string(&display, player->x - textWidth / 2, player->y, 1, text);
}

void shoot(Player *player)
{
    for (int i = 0; i < MAX_BULLETS; i++)
    {
        if (!bullets[i].active)
        {
            bullets[i].box.x = player->x + 10;
            bullets[i].box.y = player->y;
            bullets[i].box.w = 2;
            bullets[i].box.h = 6;
            bullets[i].dx = 4;
            bullets[i].dy = 0;
            bullets[i].active = 1;
            break;
        }
    }
}
