/*
 * custommessage.c
 *
 *  Created on: Sep 21, 2019
 *      Author: compukidmike
 */

#include "eeprom_emul.h"
#include "matrix.h"
#include "keyboard.h"
#include "lampboard.h"

#define CURSOR_BLINK_RATE 15

extern volatile uint8_t nextFrame;
extern uint16_t VirtAddVarTab[];

uint8_t custommessage[255];
int custommessagelength = 0;
int custommessagelengthpixels = 0;
int newchar = 65; //start at A
int cursorposition = 0;
int framecounter = 0;


void EditCustomMessage(){

	lampboard_clear();
	lampboard_setLetter('W', COLOR_RED);
	lampboard_setLetter('A', COLOR_RED);
	lampboard_setLetter('S', COLOR_RED);
	lampboard_setLetter('D', COLOR_RED);
	lampboard_setLetter('K', COLOR_RED);
	lampboard_setLetter('O', COLOR_RED);
	lampboard_update();
	uint8_t data = 0;
	EE_Status status = 0;
	status = EE_ReadVariable8bits(VirtAddVarTab[EEP_CUSTOM_MESSAGE_LENGTH], &data);
	if(status != EE_OK) data = 0;
	//EE_ReadVariable8bits(10, &data);
	if(data == 0xFF) data=0;
	if(data == 0){
		custommessagelength = 1;
		custommessagelengthpixels = 6;
		custommessage[0] = newchar;
	} else {
		custommessagelength = data;
		custommessagelengthpixels = (data*6)+6;
		cursorposition = custommessagelength-1; //start cursor at end of message
		for(int x=0; x<custommessagelength; x++){
			EE_ReadVariable8bits(VirtAddVarTab[EEP_CUSTOM_MESSAGE_START + x], &data);
			custommessage[x] = data;
		}
	}

	uint8_t loop = 1;
	while(loop){
		if(nextFrame == 1){ //Set by Timer16 - 30FPS
			nextFrame = 0;
			framecounter ++;
			uint32_t buttons = 0;
			buttons = keyboardUpdate();
			if(buttons>>22 & 1){ //W
				//newchar ++;
				//if(newchar > 95) newchar = 0;
				custommessage[cursorposition] ++;
				if(custommessage[cursorposition] > 127) custommessage[cursorposition] = 32;
			}
			if(buttons>>0 & 1){ //A
				//keyA=1;
				cursorposition --;
				if(cursorposition < 0) cursorposition = 0;
			}
			if(buttons>>18 & 1){ //S
				//newchar --;
				//if(newchar < 0) newchar = 95;
				custommessage[cursorposition] --;
				if(custommessage[cursorposition] < 32) custommessage[cursorposition] = 127;
			}
			if(buttons>>3 & 1){//D
				//keyD=1;
				cursorposition ++;
				if(cursorposition > 254){
					cursorposition = 254;
				} else if(cursorposition > custommessagelength-1) {
					custommessagelength ++;
					custommessage[custommessagelength-1] = newchar;
					//custommessagelength ++;
					custommessagelengthpixels += 6;
				}
			}
			if(buttons>>10 & 1) { //K
				EE_Status status;
				status = EE_WriteVariable8bits(VirtAddVarTab[EEP_CUSTOM_MESSAGE_LENGTH], strlen(custommessage));
				if(status != EE_OK){
					if(status == EE_CLEANUP_REQUIRED){
						status = EE_CleanUp();
						if(status != EE_OK){
							EE_CleanUp();
						}
					}
				}
				for(int x=0; x < strlen(custommessage); x++){
					status = EE_WriteVariable8bits(VirtAddVarTab[EEP_CUSTOM_MESSAGE_START + x], custommessage[x]);
							if(status != EE_OK){
								if(status == EE_CLEANUP_REQUIRED){
									status = EE_CleanUp();
									if(status != EE_OK){
										EE_CleanUp();
									}
								}
							}
				}
				loop = 0;
			}
			if(buttons>>14 & 1){//O
				if(custommessagelength > 1){
				custommessage[custommessagelength-1] = 0;
				custommessagelength --;
				cursorposition --;
				custommessagelengthpixels -= 6;
				}
			}

			matrix_clear();
			//if(custommessagelengthpixels < 64){
			if(cursorposition < 10){
				matrix_drawChar(&custommessage, 0, 0, COLOR_WHITE);
			} else {
				matrix_drawChar(&custommessage, 0-((cursorposition-9)*6),0, COLOR_WHITE);
			}

			if(framecounter >= 15){
				if(cursorposition < 9){
					matrix_drawHorizontalLine(cursorposition * 6, 7, (cursorposition * 6) + 4, COLOR_RED);
				} else {
					matrix_drawHorizontalLine(54, 8, 59, COLOR_RED);
				}
			}
			if(framecounter >= 30) framecounter = 0;

			matrix_update();
		}
	}

}
