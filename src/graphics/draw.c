/**
 * @file draw.c
 * @brief Implementation for the drawing functions.
 *
 * This module handles drawing image.
 */

#include "draw.h"

/**
 * @brief Draws an image on the SSD1306 display.
 *
 * This function uses the `ssd1306_bmp_show_image_with_offset` function
 * from the `ssd1306` library to draw a bitmap image on the display at the
 * specified coordinates.
 *
 * @param data Pointer to the image data (bitmap).
 * @param size Size of the image data in bytes.
 * @param x X-coordinate of the top-left corner of the image.
 * @param y Y-coordinate of the top-left corner of the image.
 */
void drawImage(const uint8_t *data, const long size, int x, int y)
{
    ssd1306_bmp_show_image_with_offset(&display, data, size, x, y);
}