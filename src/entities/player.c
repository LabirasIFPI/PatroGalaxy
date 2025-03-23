/**
 * @file player.c
 * @brief Implementation for the player module.
 *
 * This module manages the player character, including its movement,
 * drawing, collision detection, and bullets.
 */

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

/**
 * @brief Global variable for the Player.
 */
Player player;

/**
 * @brief Global array for the bullets.
 */
Bullet bullets[MAX_BULLETS];

/**
 * @brief Time of invulnerability for player
 */
int playerInvulnerableTimer = 90;

/**
 * @brief Initializes the bullets.
 *
 * Set all bullets to innactive
 */
void initBullets()
{
    for (int i = 0; i < MAX_BULLETS; i++)
    {
        bullets[i].active = 0;
    }
}

/**
 * @brief Updates the bullets.
 *
 * Moves the bullets, checks their bounds.
 */
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

/**
 * @brief Draws the bullets.
 *
 * Print all active bullets to the screen
 */
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
 * @brief Initializes the Player in the center of the screen.
 *
 * Sets the Player's initial position and dimensions.
 *
 * @param player Pointer to the Player structure.
 * @note The Player will spawn outside of the screen, and after the transition effect ends, the player will be playable
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
 * @brief Initializes the Player's particles.
 *
 * Sets the initial position, direction, and time for each particle.
 *
 * @param player Pointer to the Player structure.
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
 * @brief Limitar position of the player inside the limits of the screen.
 *
 * Limits the player's position within the screen boundaries.
 *
 * @param player Pointer to the Player structure.
 */
void limitPlayerPosition(Player *player)
{
    int limitY = 10;

    // Limitar position in X axis
    if (player->box.x - player->box.w / 2 < 0)
        player->box.x = player->box.w / 2;
    if (player->box.x + player->box.w / 2 >= SCREEN_WIDTH)
        player->box.x = SCREEN_WIDTH - player->box.w / 2;

    // Limitar position in Y axis
    if (player->box.y - player->box.h / 2 < limitY)
        player->box.y = limitY + player->box.h / 2;
    if (player->box.y + player->box.h / 2 >= SCREEN_HEIGHT - limitY)
        player->box.y = SCREEN_HEIGHT - limitY - player->box.h / 2;
}

/**
 * @brief Moves the Player based on input.
 *
 * Updates the player's position based on the input from the analog stick.
 *
 * @param player Pointer to the Player structure.
 * @param deltaX Change in X position.
 * @param deltaY Change in Y position.
 */
void movePlayer(Player *player, int deltaX, int deltaY)
{
    player->box.x += deltaX / 2;
    player->box.y += deltaY / 2;

    limitPlayerPosition(player);

    // Update Particles

    // printf("Player position: (%d, %d)\n", player->box.x, player->box.y);
}

/**
 * @brief Draws the Player.
 *
 * Draws the spaceship using the current location, and draws particles.
 *
 * @param player Pointer to the Player structure.
 */
void drawPlayer(Player *player)
{
    char text[4] = "]=D";
    int textWidth = 5 * strlen(text); // Assuming that the default font has 5 pixels of width
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

/**
 * @brief Makes the player shoot.
 *
 * Creates a new bullet and adds it to the active bullets list.
 *
 * @param player Pointer to the Player structure.
 */
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

/**
 * @brief Checks for collisions between the Player and asteroids.
 *
 * Checks if the Player's bounding box intersects with any active asteroid's
 * bounding box.
 *
 * @return true if a collision occurs, false otherwise.
 * @note The variable `playerInvulnerableTimer` prevent the player of dying in start of game or after a respawn.
 */
bool checkPlayerCollision()
{
    // Se o player está invulnerável, não verificar colisões
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

/**
 * @brief Checks for collisions between the bullets and asteroids.
 *
 * Checks if any active bullet's bounding box intersects with any active
 * asteroid's bounding box.
 *
 * @return true if a collision occurs, false otherwise.
 */
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