/*
 * screen_animations.c
 *
 *  Created on: Oct 15, 2019
 *      Author: bashNinja
 */

#include "keyboard.h"
#include "lampboard.h"
#include "matrix.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "minibadge.h"
#include "main.h"
#include "math.h"
#include "screen_animations.h"
#include "eeprom_emul.h"

extern volatile uint8_t nextFrame;
#define SCREEN_SIZE_X 64
#define SCREEN_SIZE_Y 16

typedef struct {
    RGB color;
} pixel;

void displayAnimations(){
	uint8_t loop = 1;
	uint8_t selectedAnimation = 0;

	while (loop){
		  if(nextFrame == 1){ //Set by Timer16 - 30FPS
			  uint32_t buttons = 0;
			  buttons = keyboardUpdate();
			  if(buttons>>10 & 1) { //K
				  loop = 0;
			  }
			  int response = 0;
			  switch(selectedAnimation){
			  	  case 0:
			  		  response = waterfall();
			  		  break;
			  	  case 1:
			  		  response = line_wave();
			  		  break;
			  	  case 2:
			  		  response = LED_waveEffect();
			  		  break;
			  	  case 3:
			  		  response = plasma();
			  		  break;
			  	  case 4:
			  		  response = LED_waveEffect();
			  		  break;
			  	  case 5:
			  		  response = rainbow_triangles();
			  		  break;
			  	  case 6:
			  		  response = screen_cycle();
			  		  break;
			  	  case 7:
			  		  response = customMessage();
			  		  break;


			  }
			  if(response == 0){
				  loop = 0;
			  }
			  else {
				  selectedAnimation += response;
				  if(selectedAnimation > 7) selectedAnimation = 0;
				  if(selectedAnimation < 0) selectedAnimation = 7;
				  //if ((selectedAnimation + response >= 0 ) && (selectedAnimation + response <= 7 ) ) {
				  //  selectedAnimation += response;
				  //}
			  }

		  }
	}

}


int waterfall(){
	uint8_t loop = 1;
	RGB pixelColor = {255, 0, 0};
	uint8_t step = 48;
	uint32_t seed;
	seed = HAL_GetTick(); //Time since boot
	srand(seed);

	pixel screen[SCREEN_SIZE_X][SCREEN_SIZE_Y];
	  for(int x = 0; x < SCREEN_SIZE_X; x++){
		  for (int y = 0; y < SCREEN_SIZE_Y; y++){
			  screen[x][y].color = (RGB){0, 0, 0};
		  }
	  }

	for(int x = 0; x < SCREEN_SIZE_X; x++){
	  int random = rand() % 15;
	  if(random == 0){
		  screen[x][0].color = (RGB){255, 0, 0};
		  matrix_drawPixel(x, 0, screen[x][0].color);
	  }
	  else {
		  screen[x][0].color = (RGB){0, 0, 0};
	  }
	}

	while (loop){
		  uint32_t buttons = 0;
		  buttons = keyboardUpdate();

		  if(buttons>>22 & 1){ //W
			  return -1;
		  }
		  if(buttons>>18 & 1){//S
			  return 1;
		  }
		  if(buttons>>10 & 1) { //K
			  return 0;
		  }

		  if(nextFrame == 1){ //Set by Timer16 - 30FPS


			  nextFrame = 0;
			  UpdateMinibadges(1);
			  CheckSerial();

			  matrix_clear();

			  for(int x = SCREEN_SIZE_X - 1; x >= 0; x--){
				  for (int y = SCREEN_SIZE_Y - 1; y >= 1; y--){
					  screen[x][y].color = screen[x][y-1].color;
					  screen[x][y].color = pulseLEDcolor(screen[x][y].color, step);
					  matrix_drawPixel(x, y, screen[x][y].color);
				  }
			  }

			  for(int x = 0; x < SCREEN_SIZE_X; x++){
				  int random = rand() % 15;
				  if(random == 0){
					  screen[x][0].color = (RGB){255, 0, 0};
					  matrix_drawPixel(x, 0, screen[x][0].color);
				  }
				  else {
					  screen[x][0].color = (RGB){0, 0, 0};
				  }
			  }

			  matrix_update();

		  }
	}

}

int line_wave(){
	uint8_t loop = 1;
	RGB pixelColor = {255, 0, 0};
	const uint8_t pixMaxHeight = 14;
	const uint8_t pixMinHeight = 1;
	uint8_t pixPos = pixMinHeight;
	uint8_t pixDirection = 1;

	pixel screen[SCREEN_SIZE_X][SCREEN_SIZE_Y];
	  for(int x = 0; x < SCREEN_SIZE_X; x++){
		  for (int y = 0; y < SCREEN_SIZE_Y; y++){
			  screen[x][y].color = (RGB){0, 0, 0};
		  }
	  }

	while (loop){
			  uint32_t buttons = 0;
			  buttons = keyboardUpdate();

			  if(buttons>>22 & 1){ //W
				  return -1;
			  }
			  if(buttons>>18 & 1){//S
				  return 1;
			  }
			  if(buttons>>10 & 1) { //K
				  return 0;
			  }
		  if(nextFrame == 1){ //Set by Timer16 - 30FPS


			  nextFrame = 0;
			  UpdateMinibadges(1);
			  CheckSerial();

			  matrix_clear();
			  pixelColor = pulseLEDcolor(pixelColor, 24);

			  screen[0][pixPos].color = (RGB){0, 0, 0};
			  pixPos+=pixDirection;
			  screen[0][pixPos].color = pixelColor;
			  if(pixPos == pixMaxHeight)
				  pixDirection = -1;
			  else if(pixPos == pixMinHeight)
				  pixDirection = 1;


			  for(int x = SCREEN_SIZE_X - 1; x > 0 ; x--){
				  for (int y = 0; y < SCREEN_SIZE_Y; y++){
					  screen[x][y].color = screen[x-1][y].color;
				  }
			  }

			  for(int x = 0; x < SCREEN_SIZE_X; x++){
				  for (int y = 0; y < SCREEN_SIZE_Y; y++){
					  matrix_drawPixel(x, y, screen[x][y].color);
				  }
			  }

			  matrix_update();

		  }
	}

}

int rainbow_triangles(){
	uint8_t loop = 1;
	RGB pixelColor = {255, 0, 0};
	const uint8_t pixMaxHeight = 14;
	const uint8_t pixMinHeight = 1;
	uint8_t pixPos = pixMinHeight;
	uint8_t pixDirection = 1;

	pixel screen[SCREEN_SIZE_X][SCREEN_SIZE_Y];
	  for(int x = 0; x < SCREEN_SIZE_X; x++){
		  for (int y = 0; y < SCREEN_SIZE_Y; y++){
			  screen[x][y].color = (RGB){0, 0, 0};
		  }
	  }

	while (loop){
		  uint32_t buttons = 0;
		  buttons = keyboardUpdate();

		  if(buttons>>22 & 1){ //W
			  return -1;
		  }
		  if(buttons>>18 & 1){//S
			  return 1;
		  }
		  if(buttons>>10 & 1) { //K
			  return 0;
		  }
		  if(nextFrame == 1){ //Set by Timer16 - 30FPS


			  nextFrame = 0;
			  UpdateMinibadges(1);
			  CheckSerial();

			  matrix_clear();
			  pixelColor = pulseLEDcolor(pixelColor, 24);

			  //screen[0][pixPos].color = (RGB){0, 0, 0}; // remove for cool magic
			  pixPos+=pixDirection;
			  screen[0][pixPos].color = pixelColor;
			  if(pixPos == pixMaxHeight)
				  pixDirection = -1;
			  else if(pixPos == pixMinHeight)
				  pixDirection = 1;


			  for(int x = SCREEN_SIZE_X - 1; x > 0 ; x--){
				  for (int y = 0; y < SCREEN_SIZE_Y; y++){
					  screen[x][y].color = screen[x-1][y].color;
				  }
			  }

			  for(int x = 0; x < SCREEN_SIZE_X; x++){
				  for (int y = 0; y < SCREEN_SIZE_Y; y++){
					  matrix_drawPixel(x, y, screen[x][y].color);
				  }
			  }

			  matrix_update();

		  }
	}

}

/*
 * might be buggy
 */
int weirdRainbow(){
	uint8_t loop = 1;
	RGB pixelColor = {255, 0, 0};

	uint8_t step = 48;
	uint32_t seed;
	seed = HAL_GetTick(); //Time since boot
	srand(seed);
	while (loop){
		  //Button Test Code
		  uint32_t buttons = 0;
		  buttons = keyboardUpdate();

		  if(buttons>>22 & 1){ //W
			  return -1;
		  }
		  if(buttons>>18 & 1){//S
			  return 1;
		  }
		  if(buttons>>10 & 1) { //K
			  return 0;
		  }


		  pixel screen[SCREEN_SIZE_X][SCREEN_SIZE_Y];

		  if(nextFrame == 1){ //Set by Timer16 - 30FPS
			  nextFrame = 0;
			  UpdateMinibadges(1);
			  CheckSerial();

			  matrix_clear();

			  for(int x = 0; x < SCREEN_SIZE_X-1; x++){
				  for (int y = 1; y < SCREEN_SIZE_Y-1; y++){
					  screen[x][y].color = screen[x][y-1].color;
					  screen[x][y].color = pulseLEDcolor(screen[x][y].color, step);
					  matrix_drawPixel(x, y, screen[x][y].color);
				  }
			  }

			  for(int x = 0; x < SCREEN_SIZE_X; x++){
				  int random = rand() % 3;
				  if(random){
					  screen[x][0].color = (RGB){255, 0, 0};
					  matrix_drawPixel(x, 0, screen[x][0].color);
				  }
				  else {
					  screen[x][0].color = (RGB){0, 0, 0};
				  }
			  }

			  matrix_update();

		  }
	}
}

/*
 * whole screen color cycle
 */
int screen_cycle(){
	uint8_t loop = 1;
	RGB pixelColor = {255, 0, 0};

	uint8_t step = 4;
	while (loop){
		uint32_t buttons = 0;
		buttons = keyboardUpdate();

		if(buttons>>22 & 1){ //W
		  return -1;
		}
		if(buttons>>18 & 1){//S
		  return 1;
		}
		if(buttons>>10 & 1) { //K
		  return 0;
		}

		  if(nextFrame == 1){ //Set by Timer16 - 30FPS
			  nextFrame = 0;
			  UpdateMinibadges(1);
			  CheckSerial();

			  matrix_clear();
			  pixelColor = pulseLEDcolor(pixelColor, step);
			  matrix_fillRect(0, 0, 64, 16, pixelColor);
			  matrix_update();

		  }
	}
}

RGB pulseLEDcolor(RGB pixelColor, uint8_t step){
	  if(pixelColor.B == 0 && pixelColor.R > 0){

		  if(pixelColor.R > step)
			  pixelColor.R-=step;
		  else
			  pixelColor.R=0;

		  if(255 - pixelColor.G > step)
			  pixelColor.G+=step;
		  else
			  pixelColor.G=255;

	  } else if(pixelColor.R == 0 && pixelColor.G > 0){

		  if(pixelColor.G > step)
			  pixelColor.G-=step;
		  else
			  pixelColor.G=0;

		  if(255 - pixelColor.B > step)
			  pixelColor.B+=step;
		  else
			  pixelColor.B=255;

	  } else if(pixelColor.G == 0 && pixelColor.B > 0){
		  if(pixelColor.B > step)
			  pixelColor.B-=step;
		  else
			  pixelColor.B=0;

		  if(255 - pixelColor.R > step)
			  pixelColor.R+=step;
		  else
			  pixelColor.R=255;
	  }
	  return pixelColor;
}
#define M_PI 3.14159265358979323846264338327950288

/*
 * Generously borrowed from:
 * https://github.com/bikefrivolously/led_matrix
 */
float mapf(float value, float c_min, float c_max, float t_min, float t_max) {
    return (value - c_min) / (c_max - c_min) * (t_max - t_min) + t_min;
}

/*
 * Generously borrowed from:
 * https://github.com/bikefrivolously/led_matrix
 */
int plasma(){
	uint8_t loop = 1;
	static float time;
	uint8_t r, g, b;
	float xx, yy;
	float v;
	float delta = 0.025;

	while (loop){
		  if(nextFrame == 1){ //Set by Timer16 - 30FPS
			  uint32_t buttons = 0;
			  buttons = keyboardUpdate();

			  if(buttons>>22 & 1){ //W
				  return -1;
			  }
			  if(buttons>>18 & 1){//S
				  return 1;
			  }
			  if(buttons>>10 & 1) { //K
				  return 0;
			  }
			  nextFrame = 0;
			  UpdateMinibadges(1);
			  CheckSerial();

			  matrix_clear();




			  time += 0.125;
			  if(time > 12*M_PI) {
				  delta *= -1;
			  }

			  for(uint8_t y = 0; y < SCREEN_SIZE_Y; y++) {
				  yy = mapf(y, 0, SCREEN_SIZE_Y-1, 0, 2*M_PI);
				  for(uint8_t x = 0; x < SCREEN_SIZE_X; x++) {
					  xx = mapf(x, 0, SCREEN_SIZE_X-1, 0, 2*M_PI);

					  v = sinf(xx + time);
					  v += sinf((yy + time) / 2.0);
					  v += sinf((xx + yy + time) / 2.0);
					  float cx = xx + .5 * sinf(time/5.0);
					  float cy = yy + .5 * cosf(time/3.0);
					  v += sinf(sqrtf((cx*cx+cy*cy)+1)+time);
					  v /= 2.0;
					  b = 255 * (sinf(v * M_PI) + 1) / 2;
					  g = 255 * (cosf(v * M_PI) + 1) / 2;
					  r = 128 * (sinf(v * M_PI + 2*M_PI/3) + 1) / 2;
					  matrix_drawPixel(x, y, (RGB){r, g, b});
				  }
			  }


			  matrix_update();

		  }
	}

}

/*
 * Generously borrowed from:
 * https://github.com/bikefrivolously/led_matrix
 */
float bound(float value, float max, float min) {
    return fmaxf(fminf(value, max), min);
}

/*
 * Generously borrowed from:
 * https://github.com/bikefrivolously/led_matrix
 */
int LED_waveEffect() {
	uint8_t loop = 1;
    static float time;
    float xx;
    uint8_t r, g, b;

	while (loop){
		  if(nextFrame == 1){ //Set by Timer16 - 30FPS
			  uint32_t buttons = 0;
			  buttons = keyboardUpdate();

			  if(buttons>>22 & 1){ //W
				  return -1;
			  }
			  if(buttons>>18 & 1){//S
				  return 1;
			  }
			  if(buttons>>10 & 1) { //K
				  return 0;
			  }
			  nextFrame = 0;
			  UpdateMinibadges(1);
			  CheckSerial();

			  matrix_clear();

			if(time > 2*M_PI) {
				time = 0.0;
			}

			for(uint8_t y = 0; y < SCREEN_SIZE_Y; y++) {
				for(uint8_t x = 0; x < SCREEN_SIZE_X; x++) {
					xx = mapf(x, 0, SCREEN_SIZE_X-1, 0, 2*M_PI);
					r = 16 + 100 * (bound(sinf(xx + time + 2*M_PI/3), 0.5, -0.5) + 0.5);
					g = 16 + 100 * (bound(sinf(xx + time - 2*M_PI/3), 0.5, -0.5) + 0.5);
					b = 16 + 100 * (bound(sinf(xx + time         ), 0.5, -0.5) + 0.5);
					matrix_drawPixel(x, y, (RGB){r, g, b});
				}
			}
			time += 0.1;


			  matrix_update();

		  }
	}

}

int customMessage(){
	uint8_t loop = 1;
	    int scrollCounter = 64;
	    uint8_t custommessage[255];
	    int custommessagelength = 0;

	    //Read custom message from EEPROM
	    uint8_t data = 0;
	    EE_ReadVariable8bits(EEP_CUSTOM_MESSAGE_LENGTH, &data);
	    	//EE_ReadVariable8bits(10, &data);
	    	if(data == 0xFF) data=0;
	    	if(data == 0){
	    		custommessagelength = 0;
	    		return 1;
	    		//custommessagelengthpixels = 6;
	    		//custommessage[0] = newchar;
	    	} else {
	    		custommessagelength = data;
	    		//custommessagelengthpixels = (data*6)+6;
	    	}
	    	for(int x=0; x<custommessagelength; x++){
	    		EE_ReadVariable8bits(EEP_CUSTOM_MESSAGE_START + x, &data);
	    		custommessage[x] = data;
	    	}
	    	custommessage[custommessagelength+1] = '\0'; //Add null terminator

		while (loop){
			  if(nextFrame == 1){ //Set by Timer16 - 30FPS
				  nextFrame = 0;
				  uint32_t buttons = 0;
				  buttons = keyboardUpdate();

				  if(buttons>>22 & 1){ //W
					  return -1;
				  }
				  if(buttons>>18 & 1){//S
					  return 1;
				  }
				  if(buttons>>10 & 1) { //K
					  return 0;
				  }

				  UpdateMinibadges(1);
				  CheckSerial();

				  matrix_clear();
				  matrix_drawChar(custommessage,scrollCounter,4,COLOR_WHITE);
				  scrollCounter--;
				  if(scrollCounter < (0-((custommessagelength*6)+64))){
					  //if(scrollCounter < (0-((strlen(INSTRUCTIONS2_TEXT)*6)+64))){
					  scrollCounter = 64;
				  }
				  matrix_update();

			  }
		}
}
