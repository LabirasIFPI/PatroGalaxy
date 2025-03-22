#include "player.h"
#include "initialize.h"

#include "ssd1306.h"

#include "pico/stdlib.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "display.h"
#include "boundingBox.h"
#include "asteroids.h"

// Definição do Player (global)
Player player;

// Definição das balas (global)
Bullet bullets[MAX_BULLETS];

int playerInvulnerableTimer = 90; // Tempo de invulnerabilidade do player

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

    initPlayerParticles(player);
}

/**
 * @brief Inicializa as partículas do Player.
 * @param player Ponteiro para a estrutura do Player.
 */
void initPlayerParticles(Player *player)
{
    for (int i = 0; i < MAX_PARTICLES; i++)
    {
        player->particles[i].x = player->box.x;
        player->particles[i].y = player->box.y;
        player->particles[i].dx = -1;
        player->particles[i].time = 0;
    }
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

    // Update Particles

    // printf("Player position: (%d, %d)\n", player->box.x, player->box.y);
}

void drawPlayer(Player *player)
{
    char text[4] = "]=D";
    int textWidth = 5 * strlen(text); // Assumindo que a fonte padrão tem 5 pixels de largura
    ssd1306_draw_string(&display, player->box.x - player->box.w / 2, player->box.y, 1, text);

    // Draw Particles
    for (int i = 0; i < MAX_PARTICLES; i++)
    {
        if (player->particles[i].time >= 0)
        {
            ssd1306_draw_pixel(&display, player->particles[i].x, player->particles[i].y);
            player->particles[i].x += player->particles[i].dx;
            player->particles[i].time--;

            // Reset particle upon reaching time limit
            if (player->particles[i].time <= 0)
            {
                player->particles[i].x = player->box.x - player->box.w / 2;
                player->particles[i].y = player->box.y + rand() % 5;
                player->particles[i].dx = -1 - rand() % 2;
                player->particles[i].time = 8 + rand() % 4;
            }
        }
    }
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

bool checkPlayerCollision()
{
    // If player is invulnerable, don't check for collisions
    if (playerInvulnerableTimer > 0)
        return false;

    for (int i = 0; i < MAX_ASTEROIDS; i++)
    {
        if (asteroids[i].active)
        {
            if (checkCollision(&player.box, &asteroids[i].box))
            {
                asteroids[i].active = 0;
                return true;
            }
        }
    }
    return false;
}

bool checkBulletsCollisions()
{
    for (int i = 0; i < MAX_BULLETS; i++)
    {
        if (bullets[i].active)
        {
            for (int j = 0; j < MAX_ASTEROIDS; j++)
            {
                if (asteroids[j].active)
                {
                    if (checkCollision(&bullets[i].box, &asteroids[j].box))
                    {
                        bullets[i].active = 0;
                        asteroids[j].active = 0;
                        return true;
                    }
                }
            }
        }
    }
    return false;
}
