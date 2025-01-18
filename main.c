#include <stdio.h>
#include <stdint.h>
#include "initialize.h"
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "player.h"
#include "background.h"

// Wi-fi
#define WIFI_SSID "pat"
#define WIFI_PASSWORD "rajadaaaa"
#include "lwip/tcp.h"

ssd1306_t display;

// Pinos
#define BTA 5
#define BTB 6
typedef void (*callback_t)(uint gpio, uint32_t events);
void initButtons(callback_t callbackFunction)
{
    if (callbackFunction == NULL)
        return;
    int buttons[2] = {BTA, BTB};
    for (int i = 0; i < 2; i++)
    {
        gpio_init(buttons[i]);
        gpio_set_dir(buttons[i], GPIO_IN);
        gpio_pull_up(buttons[i]);
        gpio_set_irq_enabled_with_callback(buttons[i], GPIO_IRQ_EDGE_RISE, true, callbackFunction);
        printf("Botão %d inicializado\n", i);
    }
}

void initI2C()
{
    // Inicializa I2C no canal 1
    i2c_init(i2c1, 400 * 1000); // 400 kHz
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
    printf("I2C inicializado\n");
}

void initDisplay()
{
    // Inicializa o display
    if (!ssd1306_init(&display, SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_ADDRESS, i2c1))
    {
        printf("Falha ao inicializar o display SSD1306\n");
    }
    else
    {
        printf("Display SSD1306 inicializado\n");
    }
}

void clearDisplay()
{
    ssd1306_clear(&display);
}

// Debug Row
int textInd = 0;
void drawText(char *text)
{
    int ind = textInd;
    ssd1306_draw_string(&display, 0, ind * 10, 1, text);
    ssd1306_show(&display);
    textInd = ind + 1;
    textInd = textInd >= 6 ? 0 : textInd;
}

int timerShowing = 0;
void callbackFunction(uint gpio, uint32_t events)
{
    printf("Botão %d pressionado\n", gpio);
    timerShowing = 12;
    // char text[50];
    // clearDisplay();
    // sprintf(text, "Botão %d pressionado", gpio);
    // drawText(text, 0);
}

char ip_str[16];
int tryToConnect()
{
    // Initialise the Wi-Fi chip
    if (cyw43_arch_init())
    {
        printf("Wi-Fi init failed\n");
        drawText("Wi-Fi init failed");
        return -1;
    }

    drawText("Wi-Fi init success");

    // Enable wifi station
    cyw43_arch_enable_sta_mode();

    printf("Connecting to Wi-Fi...\n");
    if (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 30000))
    {
        printf("failed to connect.\n");
        drawText("Failed to connect");
        return 1;
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
    }
}

// Atualiza os valores apontados por analog_x e analog_y
void updateAxis(uint *analog_x, uint *analog_y)
{
    *analog_x = readAnalogX();
    *analog_y = readAnalogY();
}

void sendInfoToServer(int info)
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

int lives = 3;
int score = 450;

int main()
{
    stdio_init_all();

    initI2C();
    initDisplay();
    clearDisplay();

    initAnalog();
    initButtons(callbackFunction);

    int connected = 0;
    while (!connected)
    {
        connected = tryToConnect();
    }

    sleep_ms(1000);

    initStars();

    while (true)
    {
        clearDisplay();
        // Header
        ssd1306_draw_string(&display, 0, 0, 1, ip_str);
        ssd1306_clear_square(&display, 0, 12, SCREEN_WIDTH, SCREEN_HEIGHT - 12);
        ssd1306_draw_line(&display, 0, 12, SCREEN_WIDTH, 12);

        moveStars();
        drawStars();

        int analog_x, analog_y;
        updateAxis(&analog_x, &analog_y);
        movePlayer(&player, analog_x, analog_y);
        drawPlayer(&player);

        if (player.x < 5)
        {
            sendInfoToServer(1);
            sleep_ms(5000);
        }

        // Exibir timeShowing no canto da tela
        ssd1306_draw_line(&display, 0, SCREEN_HEIGHT - 12, SCREEN_WIDTH, SCREEN_HEIGHT - 12);
        ssd1306_clear_square(&display, 0, SCREEN_HEIGHT - 10, SCREEN_WIDTH, SCREEN_HEIGHT);

        // Draw Lives
        char text[50];
        sprintf(text, "Lives: %d", timerShowing);
        ssd1306_draw_string(&display, 0, SCREEN_HEIGHT - 8, 1, text);

        // Draw Score
        sprintf(text, "Score: %d", score);
        ssd1306_draw_string(&display, SCREEN_WIDTH / 2, SCREEN_HEIGHT - 8, 1, text);

        timerShowing = timerShowing > 0 ? timerShowing - 1 : 0;

        ssd1306_show(&display);
        sleep_ms(30);
    }
}
