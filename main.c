// Standard library imports
#include <stdio.h>
#include <stdint.h>

// General Imports
#include "initialize.h"
#include "utils.h"
#include <math.h>
#include <time.h>

// Project-specific imports
#include "player.h"
#include "background.h"
#include "asteroids.h"

// Pico SDK imports
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"

// Wi-Fi configuration
#define WIFI_SSID "pat"
#define WIFI_PASSWORD "rajadaaaa"

// lwIP library import
#include "lwip/tcp.h"

// Game Definitions
#define STEP_CYCLE 3

// Display Instance
ssd1306_t display;

/**
 * @brief Clears the SSD1306 display.
 *
 * This function clears the contents of the SSD1306 display by calling the
 * ssd1306_clear function and passing the display instance.
 */
void clearDisplay()
{
    ssd1306_clear(&display);
}

// Debug Row Number
int textInd = 0;

/**
 * @brief Draws text on the SSD1306 display.
 *
 * This function draws the given text on the SSD1306 display at a position
 * determined by the current text index. After drawing the text, it updates
 * the display and increments the text index. If the text index reaches 6,
 * it wraps around to 0.
 *
 * @param text The text string to be drawn on the display.
 */
void drawText(char *text)
{
    int ind = textInd;
    ssd1306_draw_string(&display, 0, ind * 10, 1, text);
    ssd1306_show(&display);
    textInd = ind + 1;
    textInd = textInd >= 6 ? 0 : textInd;
}

/**
 * @brief Initializes the Wi-Fi chip and enables Wi-Fi station mode.
 *
 * This function initializes the Wi-Fi chip using the `cyw43_arch_init` function.
 * If the initialization fails, it prints an error message and returns 1.
 * If the initialization is successful, it prints a success message and enables
 * the Wi-Fi station mode using the `cyw43_arch_enable_sta_mode` function.
 *
 * @return int Returns 0 if the initialization is successful, otherwise returns 1.
 */
int initWifi()
{
    // Initialise the Wi-Fi chip
    if (cyw43_arch_init())
    {
        printf("Wi-Fi init failed\n");
        drawText("Wi-Fi init failed");
        return 1;
    }

    drawText("Wi-Fi init success");

    // Enable wifi station
    cyw43_arch_enable_sta_mode();

    return 0;
}

// IP address string
char ip_str[16];

/**
 * @brief Attempts to connect to a Wi-Fi network using predefined SSID and password.
 *
 * This function tries to establish a connection to a Wi-Fi network using the
 * credentials defined by WIFI_SSID and WIFI_PASSWORD. It provides feedback
 * through console output and a graphical interface by calling `drawText`.
 *
 * @return int Returns 1 if the connection is successful, otherwise returns 0.
 */
int tryToConnect()
{
    printf("Connecting to Wi-Fi...\n");
    drawText("Connecting to Wi-Fi...");
    if (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 8000))
    {
        printf("failed to connect.\n");
        drawText("Failed to connect");
        return 0;
    }
    else
    {
        printf("Connected.\n");
        drawText("Connected");
        // Read the ip address in a human readable way
        uint8_t *ip_address = (uint8_t *)&(cyw43_state.netif[0].ip_addr.addr);
        snprintf(ip_str, sizeof(ip_str), "%d.%d.%d.%d", ip_address[0], ip_address[1], ip_address[2], ip_address[3]);
        printf("IP address %s\n", ip_str);
        drawText(ip_str);
        return 1;
    }
}

/**
 * @brief Updates the values of the analog X and Y axes.
 *
 * This function reads the current values of the analog X and Y axes
 * and updates the provided pointers with these values.
 *
 * @param analog_x Pointer to an unsigned integer where the analog X value will be stored.
 * @param analog_y Pointer to an unsigned integer where the analog Y value will be stored.
 */
void updateAxis(uint *analog_x, uint *analog_y)
{
    *analog_x = readAnalogX();
    *analog_y = readAnalogY();
}

/**
 * @brief Sends information to the server.
 *
 * This function creates a new TCP protocol control block (PCB), connects to the server
 * at the specified IP address and port, and sends the provided information to the server.
 * It also provides feedback through the `drawText` function to indicate the status of the operation.
 *
 * @param info The information to be sent to the server.
 *
 * @note This function is still under development and may not handle all edge cases or errors.
 *       The connection callback function is currently set to NULL and the PCB is not closed after sending data.
 */
void sendInfoToServer(int info) // Incomplete function
{
    struct tcp_pcb *pcb = tcp_new();
    if (!pcb)
    {
        drawText("Failed to create PCB");
        return;
    }

    ip_addr_t server_ip;
    IP4_ADDR(&server_ip, 192, 168, 137, 1);

    if (tcp_connect(pcb, &server_ip, 5000, NULL) != ERR_OK)
    { // Saber de onde vem o ERR_OK
        drawText("Failed to connect to server");
        return;
    }

    drawText("Connected to server");

    char infoStr[10];
    sprintf(infoStr, "%d", info);

    tcp_write(pcb, infoStr, sizeof(info), TCP_WRITE_FLAG_COPY);
    tcp_output(pcb);
    // tcp_close(pcb);

    drawText("Info sent to server");
}

// Game Variables
int lives = 3;
int score = 0;
int scoreDraw = 0;
int steps = 0;
int headerMode = 1;      // 0 - Mostra IP, 1 - Mostra Nome do Level
int gameState = 0;       // 0 - Menu, 1 - Game, 2 - Game Over
int startProgress = 100; // 0 to 100
int transitioningToState = -1;
int playerSpawnTime = 30;
int playerInvulnerableTimer = 90;
int shootCooldown = 0;
int titleScreenInitialized = 0;

void changeGameState(int state)
{
    transitioningToState = state;
    startProgress = 0;
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
void callbackFunction(uint gpio, uint32_t events)
{
    switch (gameState)
    {
    case 0:
        if (gpio == BTB)
        {
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
            changeGameState(0);
            titleScreenInitialized = 0;
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
    // Header
    ssd1306_clear_square(&display, 0, 0, SCREEN_WIDTH, 9);
    char headerText[50];
    if (headerMode == 0)
    {
        sprintf(headerText, "IP: %s", ip_str);
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
    if (transitioningToState == -1 && startProgress == 0)
    {
        return;
    }
    startProgress += 6 * (1 - (2 * (transitioningToState == -1)));

    // Limit values between 0 and 100
    startProgress = startProgress > 100 ? 100 : startProgress;
    startProgress = startProgress < 0 ? 0 : startProgress;

    if (startProgress >= 100 && transitioningToState != -1)
    {
        gameState = transitioningToState;
        transitioningToState = -1;
        return;
    }

    // Desenhar um retangulo cobrindo a tela com base no valor de startProgress
    int rectHeight = (SCREEN_HEIGHT * startProgress) / 100;
    ssd1306_clear_square(&display, 0, SCREEN_HEIGHT - rectHeight, SCREEN_WIDTH, rectHeight);
    ssd1306_show(&display);
}

int checkCollision(BoundingBox *a, BoundingBox *b)
{
    return (a->x - a->w / 2 < b->x + b->w / 2 &&
            a->x + a->w / 2 > b->x - b->w / 2 &&
            a->y - a->h / 2 < b->y + b->h / 2 &&
            a->y + a->h / 2 > b->y - b->h / 2);
}

void checkBulletsCollisions()
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
                        score += 100;
                    }
                }
            }
        }
    }
}

void playerDeath()
{
    lives--;
    playerInvulnerableTimer = 90;
    playerSpawnTime = 30;
    player.box.x = -40;
    player.box.y = SCREEN_HEIGHT / 2;

    if (lives == 0)
    {
        changeGameState(2);
    }
}

void checkPlayerCollision()
{
    // If player is invulnerable, don't check for collisions
    if (playerInvulnerableTimer > 0)
        return;

    for (int i = 0; i < MAX_ASTEROIDS; i++)
    {
        if (asteroids[i].active)
        {
            if (checkCollision(&player.box, &asteroids[i].box))
            {
                playerDeath();
                asteroids[i].active = 0;
            }
        }
    }
}

int main()
{
    stdio_init_all();

    initI2C();
    initDisplay();
    clearDisplay();

    initAnalog();
    initButtons(callbackFunction);

    // initWifi();
    // int connectTries = 0;
    // int connected = 0;
    // while (!connected && connectTries < 3)
    // {
    //     connected = tryToConnect();
    //     connectTries++;
    // }

    // if (!connected)
    // {
    //     drawText("Disconnected");
    //     strcpy(ip_str, "Disconnected");
    // }

    initStars();

    gameState = 0; // Menu

    // Title Screen Variables
    int introTime;
    char patroName[50];
    int showPressStart;
    int pressStart;
    float amplitude;
    int _yAdd;

    while (1)
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
                titleScreenInitialized = 1;
                initPlayer(&player);
            }
            clearDisplay();
            int ang = introTime * 6;
            _yAdd = _yAdd > 0 ? _yAdd - 1 : 0;

            // Background
            moveStars();
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

            if (playerSpawnTime > 0)
            {
                player.box.x = -40 + (30 - playerSpawnTime) * 2;
                playerSpawnTime--;
            }
            // Background
            moveStars();
            drawStars();

            // Player
            int canMove = (playerSpawnTime == 0);
            if (canMove)
            {
                int analog_x, analog_y;
                updateAxis(&analog_x, &analog_y);
                movePlayer(&player, analog_x, analog_y);
            }

            checkPlayerCollision();
            shootCooldown = shootCooldown > 0 ? shootCooldown - 1 : 0;
            playerInvulnerableTimer = playerInvulnerableTimer > 0 ? playerInvulnerableTimer - 1 : 0;

            if (playerInvulnerableTimer % 2 == 0)
            {
                drawPlayer(&player);
            }

            // Asteroids
            if (getAsteroidsActive() < MAX_ASTEROIDS)
            {
                spawnAsteroid();
            }

            // Update game entities
            moveAsteroids();
            updateBullets();

            checkBulletsCollisions();

            // Draw game entities
            drawAsteroids();
            drawBullets();

            // Get score (temporary)
            if (player.box.x < 2)
            {
                // score += rand() % 5 * 100;
            }

            drawInterface();

            drawTransition();

            ssd1306_show(&display);
            sleep_ms(STEP_CYCLE);
        }

        // Game Over
        while (gameState == 2)
        {
            clearDisplay();
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

            drawTransition();
            ssd1306_show(&display);
            sleep_ms(STEP_CYCLE);
        }
    }
}
