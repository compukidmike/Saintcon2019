/*
 * animations.c
 *
 *  Created on: Oct 13, 2019
 *      Author: compukidmike
 */
#include "main.h"
#include "images.h"
#include "keyboard.h"
#include "minibadge.h"
#include "matrix.h"
#include "stdlib.h"
#include "stdio.h"
#include "eeprom_emul.h"
#include "lampboard.h"

//#define TESTING
#ifdef TESTING
const int outlevel = 4;
#endif

//#define HUT6
#ifdef HUT6
uint16_t offsetCount = 0;
#endif



extern volatile uint8_t nextFrame;

extern volatile int syncCounter;

extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;

extern volatile int LeftRecvBufferHead;
extern volatile int LeftRecvBufferTail;
extern volatile uint8_t LeftRecvBuffer[];
extern volatile int RightRecvBufferHead;
extern volatile int RightRecvBufferTail;
extern volatile uint8_t RightRecvBuffer[];

extern const char ERROR_COMMANDER[];
extern const uint8_t ERROR_COMMANDER_LENGTH;
extern const char INITIAL_RING[];
extern const uint8_t INITIAL_RING_LENGTH;

extern const char RING1_MESSAGE[];
extern const char RING2_MESSAGE[];
extern const char RING3_MESSAGE[];
extern const uint8_t RING1_MESSAGE_LENGTH;
extern const uint8_t RING2_MESSAGE_LENGTH;
extern const uint8_t RING3_MESSAGE_LENGTH;

int test;

uint32_t lastLeftSerialTime = 0;
uint32_t lastRightSerialTime = 0;
uint8_t LeftSerialState = 0;
uint8_t RightSerialState = 0;
extern uint8_t badgeNum;
uint8_t revealAnimation = 0;
uint8_t messageLength = 0;
uint8_t messageRecvCounter = 0;
char message[256];
uint8_t outData[4];

uint32_t nextSyncPulseTime = 0;
int syncPulseDelay = 5;

uint8_t leftBadgeLevel = 0;
uint8_t leftBadgeNum = 0;
uint8_t Command = 0;
extern uint8_t badgeLevel;



void DisplayAnimation(){
	badgeNum = 1;
#ifdef HUT6
	Command = 3;
	revealAnimation = 0;
	const char HUT_MESSAGE[] = "Welcome to HUT 6  Welcome to HUT 6  Welcome to HUT 6  Welcome to HUT 6  Welcome to HUT 6  Welcome to HUT 6  Welcome to HUT 6  Welcome to HUT 6  Welcome to HUT 6  Welcome to HUT 6  Welcome to HUT 6  Welcome to HUT 6  Welcome to HUT 6  Welcome to HUT 6  Welcome to HUT 6";
	uint8_t outData[5];
	for(int x=0; x<strlen(HUT_MESSAGE); x++){
		message[x] = HUT_MESSAGE[x];
	}
#else
	Command = 4;
#endif
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	lastLeftSerialTime = HAL_GetTick();
	lastRightSerialTime = HAL_GetTick();
	syncCounter = 0;
	uint8_t messageSent = 0;
	uint8_t reveal = 0;

	//Commands:
	//0 - Enigma Rotor w/number (Default when connected)
	//3 - Display Text
	//4 - Display Animation

	//Reveal Animations:
	//0 - Text Wipe
	//1 - Decode
	uint8_t loop = 1;
	while (loop){
		if(nextFrame == 1){ //Set by Timer16 - 30FPS
			nextFrame = 0;
			uint32_t currentTime = HAL_GetTick();
			if(currentTime - lastLeftSerialTime > 500){
				leftBadgeNum = 0;
				badgeNum = 1;
				if(currentTime - lastRightSerialTime > 500) loop = 0;
#ifdef HUT6
				messageSent = 0;
#endif
			}
			UpdateMinibadges(1);
			//Button Test Code
			uint32_t buttons = 0;
			buttons = keyboardUpdate();
			if(buttons > 0) loop = 0;

#ifdef MASTER_COMMANDER
			ForceLevelUp();
#endif

			int startX = (badgeNum - 1)*64;
			int startChar = startX/6;
			char buffer[3];

			switch(Command){
			case 0:
				break;
			case 3: //Display Text
				switch(revealAnimation){
				case 0: //Text Wipe
					//if(syncCounter >= startX){
						matrix_clear();
						matrix_drawChar(message, 0-startX, 4, COLOR_WHITE);
						//matrix_drawRect(syncCounter % 64, 0, 64, 16, COLOR_BLACK);
						matrix_update();
					//} else {
					//	matrix_clear();
					//	matrix_update();
					//}
					break;
				case 1: //Decode
					matrix_clear();
					if(syncCounter >= startX){
						matrix_clear();
						matrix_drawChar(message, 0-startX, 0, COLOR_WHITE);
						matrix_drawRect((syncCounter - startChar)*6, 0, 64, 16, COLOR_BLACK);
						matrix_drawChar((char[1]){((rand()%96)+32)}, (syncCounter - startChar + 1)*6, 0, COLOR_WHITE);
						matrix_update();
					}
					break;
				case 2: //Scroll
					matrix_clear();
					uint16_t offset = message[0] + (message[1] << 8);
					matrix_drawChar(&message[2], offset-startX, 4, COLOR_WHITE);
					matrix_update();
					break;
				case 3: //Centered Text
					matrix_clear();
					matrix_drawCharCentered(message, 0, 4, COLOR_WHITE);
					matrix_update();
					break;
				}
				break;
			case 4: //Rotor
				matrix_clear();
				matrix_drawImageWrap(rotorImage, syncCounter, 0, 64, 16);
				sprintf(buffer, "%02d", badgeNum);
				matrix_drawChar(buffer, (26+syncCounter)%64, 6, COLOR_WHITE);
				matrix_update();
				lampboard_clear();
				lampboard_setLamp(badgeNum - 1, COLOR_WHITE);
				lampboard_update();
				break;
			}

			if(badgeNum == 1){
#ifdef COMMANDER_BADGE
					outData[1] = 13;
#endif
#ifdef TESTING
					outData[1] = outlevel;
#endif
#ifdef HUT6
					outData[0] = SYNC;
					outData[1] = badgeLevel;
					outData[2] = 1;
					outData[3] = 4;
					outData[4] = 0;
					HAL_UART_Transmit(&huart1, outData, 5, 100); //Send command to Right side
					//char * hutMessage = "HUT 6";
					//char sendMessage[strlen(hutMessage)+2];
					//sendMessage[0] = offsetCounter & 0xFF;
					//sendMessage[1] = (offsetCounter++ >> 8) & 0xFF;
					outData[0] = strlen(HUT_MESSAGE);
					HAL_UART_Transmit(&huart1, outData, 1, 100);
					HAL_UART_Transmit(&huart1, HUT_MESSAGE, strlen(HUT_MESSAGE), 100);
#else
					uint8_t outData[5] = {SYNC, badgeLevel, badgeNum, Command, revealAnimation};
					HAL_UART_Transmit(&huart1, outData, 5, 100); //Send command to Right side
#endif
			}


		} //End Frame


		//Check for serial data
		if(LeftRecvBufferHead != LeftRecvBufferTail){
			uint8_t data = LeftRecvBuffer[LeftRecvBufferTail];
			LeftRecvBufferTail = (LeftRecvBufferTail + 1) % UART_BUFFER_SIZE;
			//LeftSerialState ++;

			/*if(data == SYNC + 1 && badgeNum > 1){
				outData[0] = SYNC + 1;
				HAL_UART_Transmit(&huart1, outData, 1, 100); //Send command to Right side
				lastLeftSerialTime = HAL_GetTick();
				syncCounter ++;
			}

			if(data == SYNC + 2){
				outData[0] = SYNC + 1;
				HAL_UART_Transmit(&huart1, outData, 1, 100); //Send command to Right side
				lastLeftSerialTime = HAL_GetTick();
				syncCounter = 0;
			}*/

			if(data == SYNC){
				lastLeftSerialTime = HAL_GetTick();
				LeftSerialState = 0;
			}

			int x;
			uint8_t ack[4];
			uint8_t outData[5];
			//char data;
			switch(LeftSerialState){
			case 0: //Sync byte
				LeftSerialState ++;
				break;
			case 1: //Badge Level
				if(data < 14){
					leftBadgeLevel = data;
				} else {
					loop = 0;
				}
				LeftSerialState ++;
				break;
			case 2: // Badge Number
				if(data < 27){
					leftBadgeNum = data;
#ifndef COMMANDER_BADGE
					badgeNum = (leftBadgeNum < 26)?leftBadgeNum + 1:1;
#endif
#ifdef HUT6
					badgeNum = 1;
#endif
				} else {
					loop = 0;
				}
				LeftSerialState ++;
				break;
			case 3: //Command
				if(data > 1 && data < 7){
					if(badgeNum > 1) Command = data;
				} else {
					loop = 0;
				}
				LeftSerialState ++;
				break;
			case 4: //Animation
				revealAnimation = data;
				//Send incoming data to Right side
				outData[0] = SYNC;
#ifdef COMMANDER_BADGE
				outData[1] = leftBadgeLevel;
#else
				outData[1] = (leftBadgeLevel<badgeLevel)?leftBadgeLevel:badgeLevel;
#endif
#ifdef TESTING
				outData[1] = outlevel;
#endif
				outData[2] = badgeNum;
				outData[3] = Command;
				outData[4] = revealAnimation;
				if(badgeNum > 1){
				HAL_UART_Transmit(&huart1, outData, 5, 100); //Send command to Right
				}
				ack[0] = leftBadgeNum;
				HAL_UART_Transmit(&huart2, ack, 1, 100); //Send Ack to Left
				LeftSerialState ++;
				break;
			case 5: //Message Length
				messageLength = data;
				messageRecvCounter = 0;
				//Send incoming data to Right side
				if(badgeNum > 1){
					//uint8_t outData;
					outData[0] = data;
					HAL_UART_Transmit(&huart1, outData, 1, 100); //Send Length to Right
				}
				LeftSerialState ++;
				break;
			case 6:
				if(messageRecvCounter < messageLength){
					lastLeftSerialTime = HAL_GetTick();
					message[messageRecvCounter] = data;
					messageRecvCounter ++;
					//Send incoming data to Right side
					if(badgeNum > 1){
						outData[0] = data;
						HAL_UART_Transmit(&huart1, outData, 1, 100); //Send byte to Right
					}
				}
				if(messageRecvCounter == messageLength -1){ //End of message
					//message[messageLength+1] = '\0';
					if(messageLength == INITIAL_RING_LENGTH && badgeLevel == 0){
						if(EE_WriteVariable8bits(EEP_CHALLENGE_LEVEL, 1) == EE_CLEANUP_REQUIRED) EE_CleanUp();
						badgeLevel = 1;
					}
					if(messageLength == RING1_MESSAGE_LENGTH && badgeLevel == 4){
						if(EE_WriteVariable8bits(EEP_CHALLENGE_LEVEL, 5) == EE_CLEANUP_REQUIRED) EE_CleanUp();
						badgeLevel = 5;
					}
					if(messageLength == RING2_MESSAGE_LENGTH && badgeLevel == 8){
						if(EE_WriteVariable8bits(EEP_CHALLENGE_LEVEL, 9) == EE_CLEANUP_REQUIRED) EE_CleanUp();
						badgeLevel = 9;
					}
					if(messageLength == RING3_MESSAGE_LENGTH && badgeLevel == 12){
						if(EE_WriteVariable8bits(EEP_CHALLENGE_LEVEL, 13) == EE_CLEANUP_REQUIRED) EE_CleanUp();
						badgeLevel = 13;
					}
					LeftSerialState ++;
				}



				break;
			}
		}

		//Check for Right serial data
		if(RightRecvBufferHead != RightRecvBufferTail){
			uint8_t data = RightRecvBuffer[RightRecvBufferTail];
			RightRecvBufferTail = (RightRecvBufferTail + 1) % UART_BUFFER_SIZE;
			if(data == badgeNum){
				lastRightSerialTime = HAL_GetTick();
			}
		}
/*
		if(badgeNum == 1 && reveal > 0){
			if(nextSyncPulseTime < HAL_GetTick()){
				outData[0] = SYNC + 1;
				HAL_UART_Transmit(&huart1, outData, 1, 100); //Send byte to Right
				reveal --;
			}
		}*/


#ifndef HUT6
		if(leftBadgeNum == 26){
			if(messageSent == 0){
				//Set Sync Pin to Output
				GPIO_InitStruct.Pin = SYNC_Pin;
				GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
				GPIO_InitStruct.Pull = GPIO_PULLUP;
				GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
				HAL_GPIO_Init(SYNC_GPIO_Port, &GPIO_InitStruct);

				//outData[0] = SYNC + 2;
				//HAL_UART_Transmit(&huart1, outData, 1, 100); //Send byte to Right

				Command = 3; //Display Text
				revealAnimation = 0;
				outData[0] = SYNC;
#ifdef COMMANDER_BADGE
				outData[1] = 13;
#else
				outData[1] = badgeLevel;
#endif
#ifdef TESTING
				outData[1] = outlevel;
#endif

				outData[2] = badgeNum;
				outData[3] = 3;
				outData[4] = 0;
				HAL_UART_Transmit(&huart1, outData, 5, 100);
				if(leftBadgeLevel < 3){
					outData[0] = INITIAL_RING_LENGTH;
					HAL_UART_Transmit(&huart1, outData, 1, 100);
					HAL_UART_Transmit(&huart1, INITIAL_RING, INITIAL_RING_LENGTH, 100);

				} else if(leftBadgeLevel >= 4 && leftBadgeLevel < 8) {
#ifdef COMMANDER_BADGE
					outData[0] = RING1_MESSAGE_LENGTH;
					HAL_UART_Transmit(&huart1, outData, 1, 100);
					HAL_UART_Transmit(&huart1, RING1_MESSAGE, RING1_MESSAGE_LENGTH, 1000);
#else
					outData[0] = ERROR_COMMANDER_LENGTH;
					HAL_UART_Transmit(&huart1, outData, 1, 100);
					HAL_UART_Transmit(&huart1, ERROR_COMMANDER, ERROR_COMMANDER_LENGTH, 100);
#endif
				} else if(leftBadgeLevel >= 8 && leftBadgeLevel < 12) {
#ifdef COMMANDER_BADGE
					outData[0] = RING2_MESSAGE_LENGTH;
					HAL_UART_Transmit(&huart1, outData, 1, 100);
					HAL_UART_Transmit(&huart1, RING2_MESSAGE, RING2_MESSAGE_LENGTH, 100);
#else
					outData[0] = ERROR_COMMANDER_LENGTH;
					HAL_UART_Transmit(&huart1, outData, 1, 100);
					HAL_UART_Transmit(&huart1, ERROR_COMMANDER, ERROR_COMMANDER_LENGTH, 100);
#endif
				} else if(leftBadgeLevel >= 12){
#ifdef COMMANDER_BADGE
					outData[0] = RING3_MESSAGE_LENGTH;
					HAL_UART_Transmit(&huart1, outData, 1, 100);
					HAL_UART_Transmit(&huart1, RING3_MESSAGE, RING3_MESSAGE_LENGTH, 100);
#else
					outData[0] = ERROR_COMMANDER_LENGTH;
					HAL_UART_Transmit(&huart1, outData, 1, 100);
					HAL_UART_Transmit(&huart1, ERROR_COMMANDER, ERROR_COMMANDER_LENGTH, 100);
#endif
				}
				messageSent = 1;
				//reveal = 1600;
			}
		}
#endif

		if(loop == 0){
			GPIO_InitStruct.Pin = SYNC_Pin;
			GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
			GPIO_InitStruct.Pull = GPIO_PULLUP;
			HAL_GPIO_Init(SYNC_GPIO_Port, &GPIO_InitStruct);
		}

	}

	/**/






}

void ForceLevelUp(){
	Command = 3; //Display Text
	revealAnimation = 0;
	outData[0] = SYNC;
	outData[1] = 13;


	outData[2] = badgeNum;
	outData[3] = 3;
	outData[4] = 0;
	HAL_UART_Transmit(&huart1, outData, 5, 100);
	//if(leftBadgeLevel < 3){
	outData[0] = INITIAL_RING_LENGTH;
	HAL_UART_Transmit(&huart1, outData, 1, 100);
	HAL_UART_Transmit(&huart1, INITIAL_RING, INITIAL_RING_LENGTH, 100);

	revealAnimation = 0;
	outData[0] = SYNC;
	outData[1] = 13;


	outData[2] = badgeNum;
	outData[3] = 3;
	outData[4] = 0;
	HAL_UART_Transmit(&huart1, outData, 5, 100);

	outData[0] = RING1_MESSAGE_LENGTH;
	HAL_UART_Transmit(&huart1, outData, 1, 100);
	HAL_UART_Transmit(&huart1, RING1_MESSAGE, RING1_MESSAGE_LENGTH, 1000);

	revealAnimation = 0;
	outData[0] = SYNC;
	outData[1] = 13;


	outData[2] = badgeNum;
	outData[3] = 3;
	outData[4] = 0;
	HAL_UART_Transmit(&huart1, outData, 5, 100);
	outData[0] = RING2_MESSAGE_LENGTH;
	HAL_UART_Transmit(&huart1, outData, 1, 100);
	HAL_UART_Transmit(&huart1, RING2_MESSAGE, RING2_MESSAGE_LENGTH, 100);

	revealAnimation = 0;
	outData[0] = SYNC;
	outData[1] = 13;


	outData[2] = badgeNum;
	outData[3] = 3;
	outData[4] = 0;
	HAL_UART_Transmit(&huart1, outData, 5, 100);

	outData[0] = RING3_MESSAGE_LENGTH;
	HAL_UART_Transmit(&huart1, outData, 1, 100);
	HAL_UART_Transmit(&huart1, RING3_MESSAGE, RING3_MESSAGE_LENGTH, 100);

}




