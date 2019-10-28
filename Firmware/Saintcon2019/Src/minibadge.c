#include "main.h"
#include "minibadge.h"
#include "eeprom_emul.h"
#include "matrix.h"
#include "keyboard.h"
#include "lampboard.h"
#include "i2c.h"

#define CURSOR_BLINK_RATE 15
#define MINIBADGE_NUM_STATES 5

// Minibadge States
// 0 - Off
// 1 - On
// 2 - Pattern 1 - Chase
// 3 - Pattern 2 - Circle
// 4 - Pattern 3 - Horizontal Lines
// 5 - Pattern 4 - Vertical Lines
// 6 - Pattern 5 -

extern volatile uint8_t nextFrame;

uint8_t numMinibadgeBoards = 0;

uint8_t minibadgeState = 0;

uint8_t minibadgeMatrix[16];

uint8_t minibadgeAddr[128];
uint8_t numMinibadgeAddr = 0;
uint32_t minibadgeTimeout[128];
const uint32_t timeout = 30*1000; // Thirty Seconds


void MinibadgeDisplayMode(){


	//EE_ReadVariable8bits(EEP_MINIBADGE_STATE, &minibadgeState);
	//EE_ReadVariable8bits(10, &data);
	//if(minibadgeState > MINIBADGE_NUM_STATES) minibadgeState=1;
	InitMinibadges();


	lampboard_clear();
		lampboard_setLetter('W', COLOR_RED);
		lampboard_setLetter('S', COLOR_RED);
		lampboard_setLetter('K', COLOR_RED);
		lampboard_update();

	uint8_t loop = 1;
	while(loop){
		if(nextFrame == 1){ //Set by Timer16 - 30FPS
			nextFrame = 0;
			uint32_t buttons = 0;
			buttons = keyboardUpdate();
			if(buttons>>22 & 1){ //W
				minibadgeState --;
				if(minibadgeState == 255) minibadgeState = MINIBADGE_NUM_STATES;
				InitMinibadgeMatrix();
			}
			if(buttons>>18 & 1){ //S
				minibadgeState ++;
				if(minibadgeState > MINIBADGE_NUM_STATES) minibadgeState = 0;
				InitMinibadgeMatrix();
			}
			if(buttons>>10 & 1) { //K
				EE_WriteVariable8bits(EEP_MINIBADGE_STATE, minibadgeState);
				loop = 0;
			}

			matrix_clear();
			matrix_drawCharCentered("MINIBADGES",0,0,COLOR_WHITE);

			switch(minibadgeState){
			case 0:
				matrix_drawCharCentered("OFF",0,8,COLOR_WHITE);
				break;
			case 1:
				matrix_drawCharCentered("ON",0,8,COLOR_WHITE);
				break;
			case 2:
				matrix_drawCharCentered("Pattern 1",0,8,COLOR_WHITE);
				break;
			case 3:
				matrix_drawCharCentered("Pattern 2",0,8,COLOR_WHITE);
				break;
			case 4:
				matrix_drawCharCentered("Pattern 3",0,8,COLOR_WHITE);
				break;
			case 5:
				matrix_drawCharCentered("Pattern 4",0,8,COLOR_WHITE);
				break;
			}
			UpdateMinibadges(1);
			for(int x=0; x<16; x++){
				if(minibadgeMatrix[x] & 0x01) matrix_drawPixel(60, x, COLOR_WHITE);
				if(minibadgeMatrix[x] & 0x02) matrix_drawPixel(61, x, COLOR_WHITE);
				if(minibadgeMatrix[x] & 0x04) matrix_drawPixel(62, x, COLOR_WHITE);
				if(minibadgeMatrix[x] & 0x08) matrix_drawPixel(63, x, COLOR_WHITE);
			}
			matrix_update();

		}
	}

}

void UpdateMinibadges(uint8_t allowInterruptions){

	static int minibadgeAnimationCounter = 0;
	static int minibadgeFrameCounter = 0;
	static int minibadgeMatrixX = 0;
	static int minibadgeMatrixY = 0;

	minibadgeFrameCounter ++;
	uint8_t data[2] = {0x01,0x00};

	//Update Minibadge Holder Boards
	switch(minibadgeState){
	case 0:
		for(int x=0; x<numMinibadgeBoards; x++){
			data[1] = 0x00;
			HAL_I2C_Master_Transmit(&hi2c2, (32+x)<<1, data, 2, 100);
		}
		break;
	case 1:
		for(int x=0; x<numMinibadgeBoards; x++){
			data[1] = 0xFF;
			HAL_I2C_Master_Transmit(&hi2c2, (32+x)<<1, data, 2, 100);
		}
		break;
	case 2:
		if(minibadgeFrameCounter > 6){
			minibadgeFrameCounter = 0;
			minibadgeAnimationCounter ++;
			minibadgeMatrix[minibadgeMatrixY] = 0x00;
			minibadgeMatrixX ++;
			if(minibadgeMatrixX > 3){
				minibadgeMatrixX = 0;
				minibadgeMatrixY ++;
				if(minibadgeMatrixY > (numMinibadgeBoards*2)-1){
					minibadgeMatrixY = 0;
				}
			}
			minibadgeMatrix[minibadgeMatrixY] = 1<<minibadgeMatrixX;
			for(int x=0; x<numMinibadgeBoards; x++){
				data[1] = minibadgeMatrix[x*2];
				data[1] |= minibadgeMatrix[(x*2)+1]<<4;
				HAL_I2C_Master_Transmit(&hi2c2, (32+x)<<1, data, 2, 100);
			}
		}
		break;
	case 3:
		if(minibadgeFrameCounter > 6){
			minibadgeFrameCounter = 0;
			minibadgeAnimationCounter ++;
			if(minibadgeAnimationCounter > 3) minibadgeAnimationCounter = 0;
			for(int x=0; x<16; x++){
				minibadgeMatrix[x] = 1<<minibadgeAnimationCounter;
				minibadgeAnimationCounter--;
				if(minibadgeAnimationCounter < 0) minibadgeAnimationCounter = 3;
			}
			for(int x=0; x<numMinibadgeBoards; x++){
				data[1] = minibadgeMatrix[x*2];
				data[1] |= minibadgeMatrix[(x*2)+1]<<4;
				HAL_I2C_Master_Transmit(&hi2c2, (32+x)<<1, data, 2, 100);
			}
		}
		break;
	case 4:

		break;
	case 5:

		break;
		//default:
			//data[1] = 0x00;
			//HAL_I2C_Master_Transmit(&hi2c2, x+64, data, 2, 100);
	}

	if(allowInterruptions){
		//Update Minibadge Comms
		uint8_t status = 0;
		uint8_t messageLength = 0;
		uint8_t inData[256];
		for(int x=0; x<numMinibadgeAddr; x++){
			uint8_t data[2] = {0, 0}; // Set EEPROM page to zero
			HAL_I2C_Master_Transmit(&hi2c2, minibadgeAddr[x]<<1, data, 2, 100 );
			HAL_I2C_Master_Receive(&hi2c2, minibadgeAddr[x]<<1, &status, 1, 100); //Get minibadge status
			if(status == 2 || status == 3 || status == 4){
				if(minibadgeTimeout[x] > HAL_GetTick()){
					continue;
				}else{
					minibadgeTimeout[x] = HAL_GetTick() + timeout;
				}
			}
			switch(status){
			case 0:
				//Nothing to report
				break;
			case 1:
				HAL_I2C_Master_Receive(&hi2c2, minibadgeAddr[x]<<1, inData, 1, 100); //Get Button status
				break;
			case 2:
				HAL_I2C_Master_Receive(&hi2c2, minibadgeAddr[x]<<1, &messageLength, 1, 100); //Get Text Message length
				HAL_I2C_Master_Receive(&hi2c2, minibadgeAddr[x]<<1, inData, messageLength, 100); //Get Text data
				inData[messageLength] = '\0'; //add null terminator for strlen
				if(messageLength < 11){
					matrix_clear();
					matrix_drawCharCentered(inData, 0, 4, COLOR_WHITE);
					matrix_update();
					HAL_Delay(2000);
				} else {
					int scrollCounter = 64;
					int debug = (0-(messageLength*6));
					while(scrollCounter > debug){
						matrix_clear();
						matrix_drawChar(inData, scrollCounter--, 4, COLOR_WHITE);
						matrix_update();
						HAL_Delay(30);
					}
				}
				break;
			case 3:
				HAL_I2C_Master_Receive(&hi2c2, minibadgeAddr[x]<<1, inData, 32, 100); //Pixel Message (32 bytes)
				matrix_clear();
				for(int pixX=0; pixX<32; pixX++){
					for(int pixY=0; pixY<8; pixY++){
						if(inData[pixX]>>pixY & 0x01){
							matrix_drawPixel(pixX+16, pixY+4, COLOR_WHITE);
						}
					}
				}
				matrix_update();
				HAL_Delay(2000);
				break;
			case 4:
				HAL_I2C_Master_Receive(&hi2c2, minibadgeAddr[x]<<1, inData, 33, 100); //Pixel Message w/time (33 bytes)
				matrix_clear();
								for(int pixX=0; pixX<32; pixX++){
									for(int pixY=0; pixY<8; pixY++){
										if(inData[pixX+1]>>pixY & 0x01){
											matrix_drawPixel(pixX+16, pixY+4, COLOR_WHITE);
										}
									}
								}
								matrix_update();
								HAL_Delay(inData[0]);
				break;
			case 5:
				HAL_I2C_Master_Receive(&hi2c2, minibadgeAddr[x]<<1, inData, 1, 100); //Custom Data Length
				HAL_I2C_Master_Receive(&hi2c2, minibadgeAddr[x]<<1, inData, 1, 100); //Custom Data
				//Do something with Custom Data
				break;
			}
		}

		uint8_t outData[3];
		outData[0] = 1;
		outData[1] = 0;
		outData[3] = 0;
		for(int x=0; x<numMinibadgeAddr; x++){
			HAL_I2C_Master_Transmit(&hi2c2, minibadgeAddr[x]<<1, outData, 3, 100); //Send HC score 0
		}
		outData[0] = 1;
		outData[1] = 127;
		for(int x=0; x<numMinibadgeAddr; x++){
			HAL_I2C_Master_Transmit(&hi2c2, minibadgeAddr[x]<<1, outData, 2, 100); //Send brightness of 127
		}
	}


}

void InitMinibadgeMatrix(){
	switch(minibadgeState){
	case 0:
	case 1:
	case 2:
		for(int x=0; x<16; x++){
			minibadgeMatrix[x] = 0;
		}
		break;
	case 3:

		break;
	case 4:

		break;
	case 5:

		break;

	}
}

void InitMinibadges(){
	EE_ReadVariable8bits(EEP_MINIBADGE_STATE, &minibadgeState);
	if(minibadgeState > MINIBADGE_NUM_STATES) minibadgeState=1;

	uint8_t data[] = {0x03,0x00};
	  for(uint8_t address = 32; address < 40; address++){
		  if (HAL_I2C_Master_Transmit(&hi2c2, address<<1, data, 2, 100) == HAL_OK){
			  numMinibadgeBoards++;
		  }
	  }

	  //Check for connected minibadges
	  for(int x=1; x<32; x++){
		  if (HAL_I2C_IsDeviceReady(&hi2c2, x<<1, 1, 100) == HAL_OK){
			  minibadgeAddr[numMinibadgeAddr] = x;
			  numMinibadgeAddr++;
		  }
	  }
	  //Skip Minibadge Holder Boards
	  for(int x=40; x<127; x++){
		  //if (HAL_I2C_IsDeviceReady(&hi2c2, x<<1, 1, 100) == HAL_OK){
		  if (HAL_I2C_Master_Receive(&hi2c2, x<<1, data, 1, 100) == HAL_OK){
			  minibadgeAddr[numMinibadgeAddr] = x;
			  numMinibadgeAddr++;
		  }
	  }

	  for(int x = 0; x < numMinibadgeAddr; x++){
		  minibadgeTimeout[x] = HAL_GetTick() + timeout;
	  }
}

