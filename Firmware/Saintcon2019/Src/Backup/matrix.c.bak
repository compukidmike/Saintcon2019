/*
 * matrix.c
 *
 *  Created on: Aug 20, 2019
 *      Author: compukidmike
 */

#include "matrix.h"
#include "spi.h"
#include "gpio.h"
#include "string.h"
#include "font5x7.h"
//#include "main.h"

#define gpio_low(pin)	HAL_GPIO_WritePin(pin##_GPIO_Port, pin##_Pin, GPIO_PIN_RESET)
#define gpio_high(pin)	HAL_GPIO_WritePin(pin##_GPIO_Port, pin##_Pin, GPIO_PIN_SET)
#define gpio_ishigh(pin)	(HAL_GPIO_ReadPin(pin##_GPIO_Port, pin##_Pin) == GPIO_PIN_SET)
#define gpio_toggle(pin)	HAL_GPIO_TogglePin(pin##_GPIO_Port, pin##_Pin)

const uint8_t gamma8[] = {
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,
    1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,
    2,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  5,  5,  5,
    5,  6,  6,  6,  6,  7,  7,  7,  7,  8,  8,  8,  9,  9,  9, 10,
   10, 10, 11, 11, 11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16, 16,
   17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 24, 24, 25,
   25, 26, 27, 27, 28, 29, 29, 30, 31, 32, 32, 33, 34, 35, 35, 36,
   37, 38, 39, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 50,
   51, 52, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 66, 67, 68,
   69, 70, 72, 73, 74, 75, 77, 78, 79, 81, 82, 83, 85, 86, 87, 89,
   90, 92, 93, 95, 96, 98, 99,101,102,104,105,107,109,110,112,114,
  115,117,119,120,122,124,126,127,129,131,133,135,137,138,140,142,
  144,146,148,150,152,154,156,158,160,162,164,167,169,171,173,175,
  177,180,182,184,186,189,191,193,196,198,200,203,205,208,210,213,
  215,218,220,223,225,228,231,233,236,239,241,244,247,249,252,255 };

RGB framebuffer[16][64];
RGB bufferout[16][64];


void matrix_drawPixel(int x, int y, RGB color){
	if((x>=0)&(x<64)&(y>=0)&(y<16)){
		framebuffer[y][x] = color;
	}
}

void matrix_drawHorizontalLine(uint8_t x1, uint8_t y, uint8_t x2, RGB color){
	for(int i=x1; i<=x2; i++){
		framebuffer[y][i] = color;
	}
}

void matrix_drawVerticalLine(uint8_t x, uint8_t y1, uint8_t y2, RGB color){
	for(int i=y1; i<=y2; i++){
		framebuffer[i][x] = color;
	}
}

void matrix_drawLine(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, RGB color){
	int m_new = 2 * (y2 - y1);
	   int slope_error_new = m_new - (x2 - x1);
	   for (int x = x1, y = y1; x <= x2; x++)
	   {
	      //cout << "(" << x << "," << y << ")\n";
		  matrix_drawPixel(x,y,color);

	      // Add slope to increment angle formed
	      slope_error_new += m_new;

	      // Slope error reached limit, time to
	      // increment y and update slope error.
	      if (slope_error_new >= 0)
	      {
	         y++;
	         slope_error_new  -= 2 * (x2 - x1);
	      }
	   }
}

void matrix_drawRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, RGB color){
	matrix_drawHorizontalLine(x,y,x+w-1,color);
	matrix_drawHorizontalLine(x,y+h-1,x+w-1,color);
	matrix_drawVerticalLine(x,y,y+h-1,color);
	matrix_drawVerticalLine(x+w-1,y,y+h-1,color);
}

void matrix_fillRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, RGB color){
	for(int j=y; j<y+h; j++){
		for(int i=x; i<x+w; i++){
			matrix_drawPixel(i,j,color);
		}
	}
}

void matrix_drawChar(char *text, int x, int y, RGB color){
	int textlength = strlen(text);;
	for(int n=0; n<textlength; n++){
		for(int i=0; i<5; i++){
			for (int j=0; j<7; j++){
				if((font5x7[(((int)text[n])-32)*5+i]>>j)&1){
					matrix_drawPixel(x+n*6+i,y+j,color);
				}
			}
		}
	}
}

void matrix_drawCharCentered(char *text, int x, int y, RGB color){
	int textlength = strlen(text);;
	int textwidth = (textlength * 6)-1;
	if(textwidth < 64){
		x = 63-textwidth;
		x = x/2;
	}
	for(int n=0; n<textlength; n++){
		for(int i=0; i<5; i++){
			for (int j=0; j<7; j++){
				if((font5x7[(((int)text[n])-32)*5+i]>>j)&1){
					matrix_drawPixel(x+n*6+i,y+j,color);
				}
			}
		}
	}
}

void matrix_drawImage(const uint8_t *data, uint8_t x, uint8_t y, uint8_t w, uint8_t h){
	//if(sizeof(&data) >= w*h*3){
		for(int j=0; j<h; j++){
			for(int i=0; i<w; i++){
				matrix_drawPixel(i+x,j+y,(RGB){gamma8[data[(w*j*3)+(i*3)]],gamma8[data[(w*j*3)+(i*3)+1]],gamma8[data[(w*j*3)+(i*3)+2]]});
			}
		}
	//}
}

void matrix_drawImageWrap(const uint8_t *data, uint8_t x, uint8_t y, uint8_t w, uint8_t h){
	//if(sizeof(&data) >= w*h*3){
		for(int j=0; j<h; j++){
			for(int i=0; i<w; i++){
				matrix_drawPixel((i+x)%64,(j+y)%64,(RGB){gamma8[data[(w*j*3)+(i*3)]],gamma8[data[(w*j*3)+(i*3)+1]],gamma8[data[(w*j*3)+(i*3)+2]]});
			}
		}
	//}
}

void matrix_drawImageScaled(uint8_t *data, uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t w2, uint8_t h2){
	//if(sizeof(&data) >= w*h*3){
		for(int j=y; j<y+h; j++){
			int g=0;
			//for(int g=0; g<w-w2+1; g++){
				for(int i=x+(g*(x/(w-w2))); i<((x/(w-w2))*g)+w; i++){
					if(i%(w/(w-w2+1)) == 0){
						g++;
					} else {
					matrix_drawPixel(i-g,j,(RGB){gamma8[data[(w*j*3)+(i*3)]],gamma8[data[(w*j*3)+(i*3)+1]],gamma8[data[(w*j*3)+(i*3)+2]]});
					}
				}
			//}
		}
	//}
}

void matrix_clear(){
	memset(framebuffer, 0, sizeof(framebuffer));
}

void matrix_update(){
	memcpy(bufferout, framebuffer, sizeof(bufferout));
	uint8_t data;
	gpio_low(ICE40_SPI_CS);
	data = 1;
	spi_write(&data,1);
	data = 0;
	spi_write(&data,1);
	spi_write(&data,1);
	spi_write((uint8_t *)bufferout,sizeof(bufferout));

	gpio_high(ICE40_SPI_CS);
}

static int spi_write(uint8_t *p, uint32_t len)
{
	int ret;
	uint16_t n;

	ret = HAL_OK;
	n = 0x8000;
	while (len > 0) {
		if (len < n)
			n = len;
		ret = HAL_SPI_Transmit(&hspi1, p, n, HAL_MAX_DELAY);
		if (ret != HAL_OK)
			return ret;
		len -= n;
		p += n;
	}
	return ret;
}
