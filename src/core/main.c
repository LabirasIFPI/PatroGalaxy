/**
 * @file main.c
 * @brief Main source file for the PatroGalaxy game.
 *
 * @author Luis Felipe Patrocinio <patrocinioluisf@gmail.com>
 * @version 0.2
 *
 * This file contains the main game loop, state management,
 * and initialization routines for the PatroGalaxy game.
 */

// Standard library imports
#include <stdio.h>
#include <stdint.h>

// General Imports
#include <math.h>
#include <string.h>
#include <time.h>

// Project Utils Imports
#include "initialize.h"
#include "utils.h"
#include "saveSystem.h"
#include "display.h"
#include "analog.h"
#include "text.h"
#include "draw.h"

// Project-specific imports
#include "player.h"
#include "background.h"
#include "asteroids.h"
#include "patroGalaxyUtils.h"

// Pico SDK imports
#include "pico/stdlib.h"

// Clock Definitions
// #include "hardware/timer.h"  // Deactivated for now.
// #include "hardware/irq.h"    // Deactivated for now.

// Images
#include "ifpilogo.h"

// Game Definitions
/** @brief The time of every cicle */
#define STEP_CYCLE 3

/**
 * @brief Enum to represent the game states.
 */
enum GAME_STATES
{
    /** @brief Initializing state*/
    INITIALIZING = -1, // Initializing
    /** @brief Title Screen state*/
    TITLE_SCREEN = 0, // Title Screen
    /** @brief Game running state*/
    GAME = 1, // Game
    /** @brief Game over state*/
    GAME_OVER = 2, // Game Over
};

// Global Variables
/** @brief The number of lives available to player */
int lives = 3;
/** @brief The score of the game */
int score = 0;
/** @brief The value the the game will to draw (animated) */
int scoreDraw = 0;
/** @brief Game Speed, affects the game logic */
float gameSpeed = 1.0;
/** @brief Hightscore in a previous game */
uint16_t highScore = 0;
/** @brief Flag for a new hightscore or not */
bool newHighScore = false;
/** @brief Time of the player spawn on the screen */
int playerSpawnTime = 30;
/** @brief Time before the player shoot again */
int shootCooldown = 0;
/** @brief Header display mode (0 to show High Score, 1 to show Level Name)*/
int headerMode = 1;
/** @brief Flag for flash the screen on or off*/
int flashScreen = 0;
/** @brief If initializations has already happened */
bool titleScreenInitialized = false;
/** @brief Current state of the game */
enum GAME_STATES gameState = INITIALIZING;
/** @brief Progress control on screen transition */
int transitionProgress = 100;
/** @brief Control Variable of the game on state transition */
int transitioningToState = -1;
/** @brief Prevents the player of saving the record every time a value is hit*/
bool gameSaved = false;

/**
 * @brief Changes the game state.
 *
 * Transitions the game to a new state by setting the `transitioningToState`
 * variable and initiating the transition effect.
 *
 * @param state The new game state (a value from the GAME_STATES enum).
 */
void changeGameState(enum GAME_STATES state)
{
    transitioningToState = state;
    transitionProgress = 0;
}

/**
 * @brief Callback function to handle GPIO events.
 *
 * This function is called when a GPIO event occurs. It checks the current game state
 * and performs actions based on the GPIO pin that triggered the event.
 *
 * @param gpio The GPIO pin number that triggered the event.
 * @param events The event mask indicating the type of event that occurred.
 */
void handleButtonGPIOEvent(uint gpio, uint32_t events)
{
    switch (gameState)
    {
    case 0: // Title Screen
        if (gpio == BTB)
        {
            if (transitioningToState == -1)
                changeGameState(GAME);
        }
        break;
    case 1: // Game
        if (gpio == BTB)
        {
            if (shootCooldown == 0)
            {
                shootCooldown = 6;
                shoot(&player);
            }
        }
        break;
    case 2: // Game Over
        if (gpio == BTB)
        {
            if (transitioningToState == -1)
            {
                changeGameState(TITLE_SCREEN);
                titleScreenInitialized = false;
            }
        }
        break;
    default:
        break;
    }
}

/**
 * @brief Draws the user interface on the SSD1306 display.
 *
 * This function updates the display with the current interface elements, including the header, bottom bar, lives, and score.
 * The header alternates between displaying the IP address and a static text "EmbarcaTech" every 100 steps.
 * The bottom bar displays the current number of lives and the score.
 *
 * @note The function assumes the existence of global variables: `display`, `headerMode`, `ip_str`, `steps`, `lives`, `score`, and `scoreDraw`.
 * @note The function uses the SSD1306 library functions to draw on the display.
 */
void drawInterface()
{
    static int steps = 0;

    // Header
    ssd1306_clear_square(&display, 0, 0, SCREEN_WIDTH, 9);
    char headerText[50];
    if (headerMode == 0)
    {
        sprintf(headerText, "High Score: %d", highScore);
    }
    else
    {
        sprintf(headerText, "EmbarcaTech");
    }
    drawText(0, 0, headerText);
    ssd1306_draw_line(&display, 0, 9, SCREEN_WIDTH, 9);

    steps++;
    if (steps % 100 == 0)
    {
        headerMode = !headerMode;
    }
    // If there is no high score, do not show header 0.
    if (highScore <= 0)
    {
        headerMode = 1;
    }

    // Draw Bottom Bar
    ssd1306_draw_line(&display, 0, SCREEN_HEIGHT - 11, SCREEN_WIDTH, SCREEN_HEIGHT - 11);
    ssd1306_clear_square(&display, 0, SCREEN_HEIGHT - 10, SCREEN_WIDTH, SCREEN_HEIGHT);

    // Draw Lives
    char text[50];
    sprintf(text, "Lives: %d", lives);
    drawText(0, SCREEN_HEIGHT - 8, text);

    // Draw Score
    scoreDraw = scoreDraw < score ? scoreDraw + 10 : score;
    sprintf(text, "Score: %d", scoreDraw);
    drawText(SCREEN_WIDTH / 2 - 1, SCREEN_HEIGHT - 8, text);
}

/**
 * @brief Draws a transition effect on the screen.
 *
 * This function handles the transition effect by updating the transition progress
 * and drawing a rectangle that covers the screen based on the current progress.
 * The transition progress is incremented or decremented based on whether the
 * transition is fading out or not. The progress is clamped between 0 and 100.
 * When the transition progress reaches 100 and a new state is specified, the
 * game state is updated to the new state.
 *
 * @note The function assumes the existence of the following global variables:
 * - transitioningToState: An integer representing the state to transition to.
 *   A value of -1 indicates no transition.
 * - transitionProgress: An integer representing the current progress of the transition.
 * - gameState: An integer representing the current game state.
 * - SCREEN_HEIGHT: An integer representing the height of the screen.
 * - SCREEN_WIDTH: An integer representing the width of the screen.
 * - display: A display object used for drawing.
 *
 * The function uses the ssd1306_clear_square function to draw the transition effect.
 */
void drawTransition()
{
    int fadingOut = (transitioningToState == -1);
    transitionProgress += 6 * (1 - (2 * (fadingOut)));

    // Limit values between 0 and 100
    transitionProgress = transitionProgress > 100 ? 100 : transitionProgress;
    transitionProgress = transitionProgress < 0 ? 0 : transitionProgress;

    if (transitionProgress >= 100 && transitioningToState != -1)
    {
        gameState = transitioningToState;
        transitioningToState = -1;
    }

    // Draw a rectangle covering the screen based on the value of startProgress
    int rectHeight = (SCREEN_HEIGHT * transitionProgress) / 100;
    if (transitionProgress >= 100)
    {
        rectHeight = SCREEN_HEIGHT;
    }
    ssd1306_clear_square(&display, 0, SCREEN_HEIGHT - rectHeight, SCREEN_WIDTH, rectHeight);
}

/**
 * @brief Handles the player's death event.
 *
 * This function is called when the player dies. It decrements the player's lives,
 * sets the player to be invulnerable for a short period, and respawns the player
 * at a specific location. If the player has no remaining lives, it changes the game state
 * to the game over state and checks if the current score is a new high score. If a new high score
 * is achieved, it saves the high score.
 *
 * @note This function assumes the existence of global variables such as `lives`, `playerInvulnerableTimer`,
 * `playerSpawnTime`, `player`, `flashScreen`, `score`, `highScore`, `newHighScore`, and `gameSaved`.
 */
void playerDeath()
{
    lives--;
    playerInvulnerableTimer = 90;
    playerSpawnTime = 30;
    player.box.x = -40;
    player.box.y = SCREEN_HEIGHT / 2;

    flashScreen = 2;

    if (lives == 0)
    {
        changeGameState(GAME_OVER);

        // Check high score
        if (score > highScore)
        {
            newHighScore = true;
            highScore = score;
            if (!gameSaved)
            {
                printf("New highscore: %d\n", highScore);
                // Create a buffer to store the high score
                uint8_t buffer[2];
                createBuffer(highScore, buffer);
                saveProgress(buffer);
                gameSaved = true;
                printf("Game saved: %d\n", gameSaved);
            }
        }
    }
}

/**
 * @brief Main function of the PatroGalaxy game.
 *
 * This function initializes the hardware, loads the high score, and runs the game loop.
 * The game loop handles the title screen, the game itself, and the game over screen.
 */
int main()
{
    // Wait for 30 milliseconds before initialization to ensure proper startup timing
    sleep_ms(30);

    // Initialization
    stdio_init_all();
    initI2C();
    initDisplay();
    clearDisplay();
    initAnalog();
    initButtons(handleButtonGPIOEvent);
    initStars();

    // Title Screen Variables
    int introTime;      // Time since the title screen started
    char patroName[50]; // Name of the game
    int showPressStart; // Flag to show the "Press Start" text
    float amplitude;    // Amplitude of the title screen text
    int _yAdd;          // Y offset for the title screen text

    // Boot Screen
    clearDisplay();
    drawTextCentered("Initializing...", -1);
    showDisplay();
    sleep_ms(69);

    // Splash Screen
    clearDisplay();
    drawImage(ifpilogo_bmp_data, ifpilogo_bmp_size, 30, 0);
    showDisplay();
    sleep_ms(3000);

    int splashTimer = 0;
    bool introPlayerInitialized = false;
    Player introPlayer = {.box = {.x = 0, .y = 0, .w = 16, .h = 16}};
    while (splashTimer < 220)
    {
        clearDisplay();
        int _y = 2;
        int _spac = 8;
        drawTextCentered("PatroGalaxy", _y);
        _y += _spac;
        drawTextCentered("Desenvolvido por:", _y);
        _y += _spac + 2;
        drawTextCentered("Luis Felipe", _y);
        _y += _spac;
        drawTextCentered("Patrocinio", _y);

        // Draw Playership
        int _shipX = SCREEN_WIDTH / 2 - 6 + cos(splashTimer / 26.9) * 5;
        int _shipY = SCREEN_HEIGHT / 2 + 8 + sin(splashTimer / 36.9) * 2;
        introPlayer.box.x = _shipX;
        introPlayer.box.y = _shipY;
        if (!introPlayerInitialized)
        {
            initPlayerParticles(&introPlayer);
            introPlayerInitialized = true;
        }
        drawPlayer(&introPlayer);

        _y = SCREEN_HEIGHT - 12;
        drawTextCentered("EmbarcaTech - 2025", _y);

        showDisplay();
        splashTimer += 1;
        sleep_ms(STEP_CYCLE);
    }

    // Erase data when button A is pressed
    if (!gpio_get(BTA))
    {
        clearDisplay();
        drawText(0, 0, "Erasing data...");
        showDisplay();
        clearSaveData();
        sleep_ms(2069);
    }

    // Main Loop
    gameState = TITLE_SCREEN;
    while (true)
    {
        // Title Screen
        while (gameState == TITLE_SCREEN)
        {

            if (!titleScreenInitialized)
            {
                introTime = 0;
                strcpy(patroName, "PatroGalaxy");
                showPressStart = 0;
                amplitude = 8.0;
                _yAdd = 64;
                lives = 3;
                score = 0;
                scoreDraw = 0;
                gameSpeed = 1.0;
                newHighScore = false;
                gameSaved = false;
                initPlayer(&player);

                // Carregar Dados
                uint8_t buffer[2];
                loadProgress(buffer, 2);
                loadBuffer(buffer, &highScore);

                titleScreenInitialized = true;
            }
            clearDisplay();
            int ang = introTime * 6;
            _yAdd = _yAdd > 0 ? _yAdd - 1 : 0;

            // Background
            moveStars(1.0);
            drawStars();

            // PatroGalaxy Text
            for (int i = 0; i < strlen(patroName); i++)
            {
                char letter[2] = {patroName[i], '\0'};
                int _x = 64 - 5 * strlen(patroName) / 2 + 5 * i;
                int _y = SCREEN_HEIGHT / 2 + sin(ang + i * 60) * amplitude + _yAdd;
                drawText(_x, _y, letter);
            }

            // Press Start
            char startText[50];
            sprintf(startText, "Press Start");
            int _x = SCREEN_WIDTH / 2 - 5 * (strlen(startText) + 1) / 2;
            int _y = SCREEN_HEIGHT - 10;
            drawText(_x, _y, showPressStart ? startText : "");

            // Draw Highscore
            if (highScore > 0)
            {
                char highScoreText[50];
                sprintf(highScoreText, "Highscore: %d", highScore);
                _x = SCREEN_WIDTH / 2 - 5 * (strlen(highScoreText) + 1) / 2;
                int _y = -8 + 15 - MIN(15, _yAdd);
                drawText(_x, _y, highScoreText);
            }

            drawTransition();

            introTime++;

            amplitude = amplitude > 0 ? amplitude - 0.069 : 0;

            if (introTime > 30 && amplitude == 0)
            {
                showPressStart = !showPressStart;
                introTime = 0;
            }

            showDisplay();
            sleep_ms(STEP_CYCLE);
        }

        initAsteroids();

        // Game State
        while (gameState == GAME)
        {
            clearDisplay();

            gameState = 1; // Game

            // Increase game speed at each score interval
            gameSpeed = 1.0 + (score / (300.0 + 100.0 * gameSpeed));

            if (playerSpawnTime > 0)
            {
                player.box.x = -40 + (30 - playerSpawnTime) * 2;
                playerSpawnTime--;
            }
            // Background
            moveStars(gameSpeed);
            drawStars();

            // Player
            int canMove = (playerSpawnTime == 0);
            if (canMove)
            {
                updateAxis();
                movePlayer(&player, analog_x, analog_y);
            }

            if (checkPlayerCollision(&player))
            {
                playerDeath();
            }
            shootCooldown = shootCooldown > 0 ? shootCooldown - 1 : 0;
            playerInvulnerableTimer = playerInvulnerableTimer > 0 ? playerInvulnerableTimer - 1 : 0;

            if (playerInvulnerableTimer % 2 == 0)
            {
                drawPlayer(&player);
            }

            // Asteroids
            if (getAsteroidsActive() < MIN(3 + gameSpeed / 4, MAX_ASTEROIDS))
            {
                spawnAsteroid();
            }

            // Update game entities
            moveAsteroids(1 + gameSpeed / 4);
            updateBullets();

            if (checkBulletsCollisions())
            {
                score += 100;
            }

            // Draw game entities
            drawAsteroids();
            drawBullets();

            // Draw Interface
            drawInterface();

            // Draw Transition Above Everything
            drawTransition();

            // Flash Screen
            flashScreen = flashScreen > 0 ? flashScreen - 1 : 0;
            invertDisplay(flashScreen);

            // Update Display
            showDisplay();

            // Sleep Step Cycle
            sleep_ms(STEP_CYCLE);
        }

        int gameOverTime = 0;
        // Game Over
        while (gameState == GAME_OVER)
        {
            clearDisplay();

            // Background
            if (gameOverTime > 0)
            {
            }
            ssd1306_draw_square(&display, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
            gameOverTime++;
            gameOverTime = gameOverTime > SCREEN_HEIGHT ? -16 : gameOverTime;

            ssd1306_clear_square(&display, 8, 0, 2, SCREEN_HEIGHT);
            ssd1306_clear_square(&display, 16, 0, 2, SCREEN_HEIGHT);
            ssd1306_clear_square(&display, 20, 0, 2, SCREEN_HEIGHT);
            ssd1306_clear_square(&display, SCREEN_WIDTH - 20 - 1, 0, 2, SCREEN_HEIGHT);
            ssd1306_clear_square(&display, SCREEN_WIDTH - 16 - 1, 0, 2, SCREEN_HEIGHT);
            ssd1306_clear_square(&display, SCREEN_WIDTH - 8 - 1, 0, 2, SCREEN_HEIGHT);

            for (int i = 0; i < SCREEN_HEIGHT; i += 2)
            {
                ssd1306_clear_square(&display, 0, i, SCREEN_WIDTH, 1);
            }

            ssd1306_clear_square(&display, 24, 0, SCREEN_WIDTH - 48, SCREEN_HEIGHT);

            // Game Over Text
            char gameOverText[50];
            sprintf(gameOverText, "Game Over");
            int _x = SCREEN_WIDTH / 2 - 5 * (strlen(gameOverText) + 1) / 2;
            int _y = SCREEN_HEIGHT / 2 - 6;
            drawText(_x, _y, "Game Over");

            char scoreText[50];
            sprintf(scoreText, "Score: %d", score);
            _x = SCREEN_WIDTH / 2 - 5 * (strlen(scoreText) + 1) / 2;
            _y = SCREEN_HEIGHT / 2 - 6 + 12;
            drawText(_x, _y, scoreText);

            if (newHighScore)
            {
                char newRecordText[50];
                sprintf(newRecordText, "New record!");
                int _x = SCREEN_WIDTH / 2 - 5 * (strlen(newRecordText) + 1) / 2;
                int _y = SCREEN_HEIGHT / 2 - 6 + 24;
                drawText(_x, _y, newRecordText);
            }

            drawTransition();
            showDisplay();
            sleep_ms(STEP_CYCLE);
        }

        clearDisplay();
        showDisplay();
    }
}