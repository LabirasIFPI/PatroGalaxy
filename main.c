// Standard library imports
#include <stdio.h>
#include <stdint.h>

// General Imports
#include "initialize.h"
#include "utils.h"
#include <math.h>
#include <string.h>
#include <time.h>
#include "saveSystem.h"
#include "display.h"
#include "analog.h"
#include "text.h"

// Project-specific imports
#include "player.h"
#include "background.h"
#include "asteroids.h"
#include "patroGalaxyUtils.h"

// Pico SDK imports
#include "pico/stdlib.h"

// Clock Definitions
#include "hardware/timer.h"
#include "hardware/irq.h"

// Game Definitions
#define STEP_CYCLE 3

// Global Variables
int lives = 3;                       // Número de vidas
int score = 0;                       // Pontuação
int scoreDraw = 0;                   // Pontuação a ser desenhada
float gameSpeed = 1.0;               // Velocidade do jogo (aumenta com a pontuação)
uint16_t highScore = 0;              // High Score
bool newHighScore = false;           // Flag para indicar novo high score
int playerSpawnTime = 30;            // Tempo de spawn do player
int shootCooldown = 0;               // Cooldown de tiros
int headerMode = 1;                  // 0 - Mostra High Score, 1 - Mostra Nome do Level
int gameState = -1;                  // 0 - Menu, 1 - Game, 2 - Game Over
int flashScreen = 0;                 // Flag para piscar a tela
bool titleScreenInitialized = false; // Flag para indicar se a tela de título foi inicializada
int transitionProgress = 100;        // Progresso da transição de estados
int transitioningToState = -1;       // Variável de controle para transição de estados
bool gameSaved = false;              // Variável de controle para impedir múltiplos salvamentos.

void changeGameState(int state)
{
    transitioningToState = state;
    transitionProgress = 0;
}

/**
 * @brief Callback function to handle GPIO events.
 *
 * This function is called when a GPIO event occurs. It checks the current game state
 * and performs actions based on the GPIO pin that triggered the event.///@func
 *
 * @param gpio The GPIO pin number that triggered the event.
 * @param events The event mask indicating the type of event that occurred.
 */
void handleButtonGpioEvent(uint gpio, uint32_t events)
{
    switch (gameState)
    {
    case 0: // Title Screen
        if (gpio == BTB)
        {
            if (transitioningToState == -1)
                changeGameState(1);
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
                changeGameState(0);
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
 * The header alternates between displaying the IP address and a static text "Espaco Sideral" every 100 steps.
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
        sprintf(headerText, "Espaco Sideral");
    }
    ssd1306_draw_string(&display, 0, 0, 1, headerText);
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
    ssd1306_draw_string(&display, 0, SCREEN_HEIGHT - 8, 1, text);

    // Draw Score
    scoreDraw = scoreDraw < score ? scoreDraw + 10 : score;
    sprintf(text, "Score: %d", scoreDraw);
    ssd1306_draw_string(&display, SCREEN_WIDTH / 2 - 1, SCREEN_HEIGHT - 8, 1, text);
}

void drawTransition()
{
    if (transitioningToState == -1 && transitionProgress == 0)
    {
        // return;
    }
    transitionProgress += 6 * (1 - (2 * (transitioningToState == -1)));

    // Limit values between 0 and 100
    transitionProgress = transitionProgress > 100 ? 100 : transitionProgress;
    transitionProgress = transitionProgress < 0 ? 0 : transitionProgress;

    if (transitionProgress >= 100 && transitioningToState != -1)
    {
        gameState = transitioningToState;
        transitioningToState = -1;
    }

    // Desenhar um retangulo cobrindo a tela com base no valor de startProgress
    int rectHeight = (SCREEN_HEIGHT * transitionProgress) / 100;
    if (transitionProgress >= 100)
    {
        rectHeight = SCREEN_HEIGHT;
    }
    ssd1306_clear_square(&display, 0, SCREEN_HEIGHT - rectHeight, SCREEN_WIDTH, rectHeight);
}

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
        changeGameState(2);

        // Check high score
        if (score > highScore)
        {
            newHighScore = true;
            highScore = score;
            if (!gameSaved)
            {
                printf("New highscore: %d\n", highScore);
                // Criar um buffer para guardar o highscore
                uint8_t buffer[2];
                createBuffer(highScore, buffer);
                saveProgress(buffer);
                gameSaved = true;
                printf("Game saved: %d\n", gameSaved);
            }
        }
    }
}

int main()
{
    sleep_ms(30);

    // Inicialização
    stdio_init_all();
    initI2C();
    initDisplay();
    clearDisplay();
    initAnalog();
    initButtons(handleButtonGpioEvent);
    initStars();

    // Title Screen Variables
    int introTime;
    char patroName[50];
    int showPressStart;
    int pressStart;
    float amplitude;
    int _yAdd;

    // Tela de inicialização para evitar bugs ao resetar na tela de título.
    clearDisplay();
    drawTextCentered("Iniciando", -1);
    ssd1306_show(&display);
    sleep_ms(69);

    // Apagar dados ao pressionar o botão A
    if (!gpio_get(BTA))
    {
        clearDisplay();
        ssd1306_draw_string(&display, 0, 0, 1, "Apagando dados...");
        ssd1306_show(&display);
        clearSaveData();
        sleep_ms(2069);
    }

    gameState = 0;

    while (true)
    {
        // Title Screen
        while (gameState == 0)
        {

            if (!titleScreenInitialized)
            {
                introTime = 0;
                strcpy(patroName, "PatroGalaxy");
                showPressStart = 0;
                pressStart = 0;
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

            // PatroGalaxy
            for (int i = 0; i < strlen(patroName); i++)
            {
                char letter[2] = {patroName[i], '\0'};
                int _x = 64 - 5 * strlen(patroName) / 2 + 5 * i;
                int _y = SCREEN_HEIGHT / 2 + sin(ang + i * 60) * amplitude + _yAdd;
                ssd1306_draw_char(&display, _x, _y, 1, letter[0]);
            }

            // Press Start
            char startText[50];
            sprintf(startText, "Press Start");
            int _x = SCREEN_WIDTH / 2 - 5 * (strlen(startText) + 1) / 2;
            int _y = SCREEN_HEIGHT - 10;
            ssd1306_draw_string(&display, _x, _y, 1, showPressStart ? startText : "");

            // Draw Highscore
            if (highScore > 0)
            {
                char highScoreText[50];
                sprintf(highScoreText, "Highscore: %d", highScore);
                _x = SCREEN_WIDTH / 2 - 5 * (strlen(highScoreText) + 1) / 2;
                _y = -8 + 15 - MIN(15, _yAdd);
                ssd1306_draw_string(&display, _x, _y, 1, highScoreText);
            }

            drawTransition();

            introTime++;

            amplitude = amplitude > 0 ? amplitude - 0.069 : 0;

            if (introTime > 30 && amplitude == 0)
            {
                showPressStart = !showPressStart;
                introTime = 0;
            }

            ssd1306_show(&display);
            sleep_ms(STEP_CYCLE);
        }

        initAsteroids();

        // Game State
        while (gameState == 1)
        {
            clearDisplay();

            gameState = 1; // Game

            // A cada intervalo de pontos, aumenta a velocidade do jogo
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

            if (checkPlayerCollision())
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

            drawInterface();

            drawTransition();

            flashScreen = flashScreen > 0 ? flashScreen - 1 : 0;

            ssd1306_invert(&display, flashScreen);

            ssd1306_show(&display);

            sleep_ms(STEP_CYCLE);
        }

        int gameOverTime = 0;
        // Game Over
        while (gameState == 2)
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
            ssd1306_draw_string(&display, _x, _y, 1, "Game Over");

            char scoreText[50];
            sprintf(scoreText, "Score: %d", score);
            _x = SCREEN_WIDTH / 2 - 5 * (strlen(scoreText) + 1) / 2;
            _y = SCREEN_HEIGHT / 2 - 6 + 12;
            ssd1306_draw_string(&display, _x, _y, 1, scoreText);

            if (newHighScore)
            {
                char newRecordText[50];
                sprintf(newRecordText, "New record!");
                _x = SCREEN_WIDTH / 2 - 5 * (strlen(newRecordText) + 1) / 2;
                _y = SCREEN_HEIGHT / 2 - 6 + 24;
                ssd1306_draw_string(&display, _x, _y, 1, newRecordText);
            }

            drawTransition();
            ssd1306_show(&display);
            sleep_ms(STEP_CYCLE);
        }

        clearDisplay();
        ssd1306_show(&display);
    }
}
