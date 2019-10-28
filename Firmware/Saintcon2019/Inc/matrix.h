#include "stdint.h"
#include "main.h"



void matrix_drawPixel(int x, int y, RGB color);

void matrix_drawHorizontalLine(uint8_t x1, uint8_t y, uint8_t x2, RGB color);

void matrix_drawVerticalLine(uint8_t x, uint8_t y1, uint8_t y2, RGB color);

void matrix_drawLine(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, RGB color);

void matrix_drawRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, RGB color);

void matrix_fillRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, RGB color);

void matrix_drawChar(char *text, int x, int y, RGB color);

void matrix_drawCharCentered(char *text, int x, int y, RGB color);

void matrix_drawImage(const uint8_t *data, uint8_t x, uint8_t y, uint8_t w, uint8_t h);

void matrix_drawImageWrap(const uint8_t *data, uint8_t x, uint8_t y, uint8_t w, uint8_t h);

void matrix_clear();

void matrix_update();

static int spi_write(uint8_t *p, uint32_t len);
