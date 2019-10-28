#include "main.h"
#include "menu.h"
#include "keyboard.h"
#include "lampboard.h"
#include "matrix.h"
#include "string.h"
#include "custommessage.h"
#include "snake.h"
#include "snake_images.h"
#include "minibadge.h"
#include "enigma_emulator.h"
#include "user_profile.h"
#include "screen_animations.h"

#define NUMMENUSTATES 7

#ifndef BADGE_TEST_CODE
const char INSTRUCTIONS_TEXT[] = "DIRECTIONS:";
const char INSTRUCTIONS2_TEXT[] = "Use the lighted keys to navigate the menu: WASD for direction, K for enter. ";
#else
const char INSTRUCTIONS_TEXT[] = "PROGRAM ME";
const char INSTRUCTIONS2_TEXT[] = "Please take me to the nearest flashing station to receive the latest code.";
#endif

const char ENIGMA_TEXT[] = "ENIGMA";
const char ENIGMA2_TEXT[] = "EMULATOR";
const char GAMES_TEXT[] = "GAMES";
const char CHALLENGE_TEXT[] = "CHALLENGE";
const char CHALLENGE2_TEXT[] = "INFO";
const char CUSTOM_TEXT[] = "CUSTOM";
const char CUSTOM2_TEXT[] = "MESSAGE";
const char MINIBADGE_TEXT[] = "MINBADGE";
const char MINIBADGE2_TEXT[] = "CONTROL";
const char EXIT_TEXT[] = "EXIT";
const char EXIT2_TEXT[] = "MENU";

extern volatile uint8_t nextFrame;

int menuState = 0;
/*
 * Menu States:
 * 0-Instructions
 * 1-Enigma
 * 2-Games
 * 3-Challenge Info
 * 4-Custom Message
 * 5-Minibadges
 * 6-Exit Menu
 */

void menu(){
	uint8_t keyW = 0;
	uint8_t keyA = 0;
	uint8_t keyS = 0;
	uint8_t keyD = 0;
	uint8_t keyK = 0;
	lampboard_clear();
	lampboard_setLetter('W', COLOR_RED);
	//lampboard_setLetter('A', COLOR_RED);
	lampboard_setLetter('S', COLOR_RED);
	//lampboard_setLetter('D', COLOR_RED);
	lampboard_setLetter('K', COLOR_RED);
	lampboard_update();
	uint8_t loop = 1;
	int scrollCounter = 64;
	int scrollCount = 0;
	while (loop){
		  if(nextFrame == 1){ //Set by Timer16 - 30FPS
			  nextFrame = 0;
			  CheckSerial();
			  UpdateMinibadges(1);
			  lampboard_clear();
			  	lampboard_setLetter('W', COLOR_RED);
			  	//lampboard_setLetter('A', COLOR_RED);
			  	lampboard_setLetter('S', COLOR_RED);
			  	//lampboard_setLetter('D', COLOR_RED);
			  	lampboard_setLetter('K', COLOR_RED);
			  	lampboard_update();
			  //Button Test Code
			  uint32_t buttons = 0;
			  #ifndef BADGE_TEST_CODE
			  buttons = keyboardUpdate();
			  #endif
			  if(buttons>>22 & 1){ //W
				  keyW=1;
				  menuState -= 1;
				  if(menuState < 0){
					  menuState = NUMMENUSTATES-1;
				  }
				  scrollCounter = 64;
			  }
			  if(buttons>>0 & 1){ //A
				  keyA=1;
			  }
			  if(buttons>>18 & 1){ //S
				  keyS=1;
				  menuState += 1;
				  if(menuState > NUMMENUSTATES-1){
					  menuState = 0;
				  }
				  scrollCounter = 64;
			  }
			  if(buttons>>3 & 1) keyD=1; //D
			  if(buttons>>10 & 1) { //K
				  keyK=1;
				  switch(menuState){
				  case 1:
					  enigma_menu();
					  break;
				  case 2:
					  snake();
					  break;
				  case 3:
					  user_profile();
					  break;
				  case 4:
					  EditCustomMessage();
					  break;
				  case 5:
					  MinibadgeDisplayMode();
					  break;
				  case 6:
					  displayAnimations();
					  break;
				  }
				  lampboard_clear();
				  lampboard_setLetter('W', COLOR_RED);
				  lampboard_setLetter('A', COLOR_RED);
				  lampboard_setLetter('S', COLOR_RED);
				  lampboard_setLetter('D', COLOR_RED);
				  lampboard_setLetter('K', COLOR_RED);
				  lampboard_update();
			  }
			  switch(menuState){
			  case 0: //Instructions
				  matrix_clear();
				  matrix_drawChar(INSTRUCTIONS_TEXT,0,0,COLOR_WHITE);
				  matrix_drawChar(INSTRUCTIONS2_TEXT,scrollCounter,8,COLOR_GREEN);
				  scrollCounter--;
				  if(scrollCounter < -456){
					  //if(scrollCounter < (0-((strlen(INSTRUCTIONS2_TEXT)*6)+64))){
					  scrollCounter = 64;
					  scrollCount ++;
					  if(scrollCount == 2){
						  displayAnimations();
					  }
				  }
				  matrix_update();
				  break;
			  case 1: //Enigma
				  matrix_clear();
				  matrix_drawCharCentered(ENIGMA_TEXT,0,0,COLOR_WHITE);
				  matrix_drawCharCentered(ENIGMA2_TEXT,0,8,COLOR_GREEN);
				  matrix_update();
				  break;
			  case 2: //Games
				  matrix_clear();
				  matrix_drawCharCentered(GAMES_TEXT,0,0,COLOR_WHITE);
				  matrix_drawImage(image_data_snake, 6, 8, 6, 7);
				  matrix_update();
				  break;
			  case 3: //Challenge Info
				  matrix_clear();
				  matrix_drawCharCentered(CHALLENGE_TEXT,0,0,COLOR_WHITE);
				  matrix_drawCharCentered(CHALLENGE2_TEXT,0,8,COLOR_GREEN);
				  matrix_update();
				  break;
			  case 4: //Custom Message
				  matrix_clear();
				  matrix_drawCharCentered(CUSTOM_TEXT,0,0,COLOR_WHITE);
				  matrix_drawCharCentered(CUSTOM2_TEXT,0,8,COLOR_GREEN);
				  matrix_update();
				  break;
			  case 5: //Minibadges
				  matrix_clear();
				  matrix_drawCharCentered(MINIBADGE_TEXT,0,0,COLOR_WHITE);
				  matrix_drawCharCentered(MINIBADGE2_TEXT,0,8,COLOR_GREEN);
				  matrix_update();
				  break;
			  case 6: //Exit Menu
				  matrix_clear();
				  matrix_drawCharCentered(EXIT_TEXT,0,0,COLOR_WHITE);
				  matrix_drawCharCentered(EXIT2_TEXT,0,8,COLOR_GREEN);
				  matrix_update();
				  break;
			  }
		  }

	  }
}
