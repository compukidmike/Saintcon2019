#include "enigma_emulator.h"
#include "eeprom_emul.h"
#include "keyboard.h"
#include "lampboard.h"
#include "plugboard.h"
#include "matrix.h"
#include "string.h"


#define ENIGMA_NUMMENUSTATES 2

extern volatile uint8_t nextFrame;
extern uint8_t badgeLevel;

const char ENIGMA_STANDARD_TEXT[] = "ENIGMA";
const char ENIGMA_HELP_TEXT[] = "Start Encoding - Hold ANY key to exit";



const char ENIGMA_SETTINGS_ROTORS_TEXT[] = "Set Rotors";
const char ENIGMA_SETTINGS_MACHINE_HEADER_TEXT[] = "Machine";
const char ENIGMA_SETTINGS_TYPE_TEXT[] = "Rotor";
const char ENIGMA_SETTINGS_TYPE1_TEXT[] = "3";
const char ENIGMA_SETTINGS_TYPE2_TEXT[] = "4";
const char ENIGMA_SETTINGS_RING_TEXT[] = "Set Rings";
const char ENIGMA_SETTINGS_INITIAL_TEXT[] = "Start Pos.";
const char ENIGMA_SETTINGS_PLUGBOARD_TEXT[] = "Plugboard";
//const char ENIGMA_SETTINGS_PLUGBOARD2_TEXT[] = "Plugboard?";

const char ENIGMA_LEVEL_UP_TEXT[] = "LEVELED UP";



const char ENIGMA_EXIT_TEXT[] = "EXIT";
const char ENIGMA_EXIT2_TEXT[] = "EMULATOR";


int enigma_menuState = 0;

char plugboard[26];
char pairs[39];
int pairlength = 0;
int x=0;

void enigma_menu(){
	lampboard_clear();
	lampboard_update();
	struct Enigma standard_machine = {};
	enigma_settings(&standard_machine);
	lampboard_clear();
	lampboard_update();


	char message[11] = "          ";
	char *message_location = message;
	char encryptedMessage[11] = "          ";

	matrix_clear();
	matrix_drawCharCentered(ENIGMA_STANDARD_TEXT,0,0,COLOR_WHITE);
	matrix_update();
	uint8_t loop = 1;
	int scrollCounter = 64;
	int levelUpPauseCounter = 150; // at 30FPS, this is 5s
	uint8_t eeprom_level = 0;
	char eeprom_level_code[7] = "";
	uint8_t eeprom_code_letter = 0;
	uint8_t minibadgeState = 0;
	uint8_t leveledUp = 0;
	// get level code
	EE_ReadVariable8bits(EEP_CHALLENGE_LEVEL, &eeprom_level);
	for(int x=0; x<6; x++){
		switch(eeprom_level){
			case 1:
				EE_ReadVariable8bits(EEP_MESSAGE_CODE_1 + x, &eeprom_code_letter);
				break;
			case 2:
				EE_ReadVariable8bits(EEP_MESSAGE_CODE_2 + x, &eeprom_code_letter);
				break;
			case 3:
				EE_ReadVariable8bits(EEP_MESSAGE_CODE_3 + x, &eeprom_code_letter);
				break;
			case 5:
				EE_ReadVariable8bits(EEP_MESSAGE_CODE_4 + x, &eeprom_code_letter);
				break;
			case 6:
				EE_ReadVariable8bits(EEP_MESSAGE_CODE_5 + x, &eeprom_code_letter);
				break;
			case 7:
				EE_ReadVariable8bits(EEP_MESSAGE_CODE_6 + x, &eeprom_code_letter);
				break;
			case 9:
				EE_ReadVariable8bits(EEP_MESSAGE_CODE_7 + x, &eeprom_code_letter);
				break;
			case 10:
				EE_ReadVariable8bits(EEP_MESSAGE_CODE_8 + x, &eeprom_code_letter);
				break;
			case 11:
				EE_ReadVariable8bits(EEP_MESSAGE_CODE_9 + x, &eeprom_code_letter);
				break;
		}
		eeprom_level_code[x] = eeprom_code_letter;
	}

	while (loop){
		  if(nextFrame == 1){ //Set by Timer16 - 30FPS
			  nextFrame = 0;
			  UpdateMinibadges();
			  //Button Test Code
			  uint32_t buttons = 0;
			  buttons = keyboardUpdate();
			  if(buttons>>31 & 1)
				  loop = 0;

			  // clear matrix, setup top row
			  matrix_clear();
			  if(scrollCounter > -226){ // 290 is the strlen of ENIGMA_HELP_TEXT * 6(char width) - 64 (length of screen)
				  matrix_drawChar(ENIGMA_HELP_TEXT,scrollCounter,0,COLOR_WHITE);
				  scrollCounter--;
			  } else if(!leveledUp) {
				  matrix_drawCharCentered(ENIGMA_STANDARD_TEXT,0,0,COLOR_WHITE);
			  } else {
				  // level up animation
				  matrix_drawCharCentered(ENIGMA_LEVEL_UP_TEXT,0,0,COLOR_RED);
				  levelUpPauseCounter--;
				  if(levelUpPauseCounter<0)
					  loop = 0;
			  }

			  // change message if bottom row is different
			  if(buttons > 0 && !leveledUp){ // if button pressed
				  if(buttons && !(buttons & (buttons-1))){ // if only 1 button pressed
					  int pos, temp;
					  for (pos = 0, temp = ~buttons & (buttons - 1); temp > 0; temp >>= 1, ++pos);

					  char letter = enigma_encode_char(&standard_machine, pos+65);
					  lampboard_clear();
					  lampboard_setLetter(letter, COLOR_RED);
					  lampboard_update();

					  // set message text
					  if(*message_location == '\0'){
						  //shift message, dropping the first char and adding the new one
						  int length = strlen(message);
						  for (int i = 0; i < length - 1; i++){
						    message[i] = message[i+1];
						  }
						  message[length-1] = letter;
					  }
					  else{
						  *message_location = letter;
						  message_location++;
					  }

					  for(int x=0; x<9; x++){
						  encryptedMessage[x] = encryptedMessage[x+1];
					  }
					  encryptedMessage[9] = pos+65;

					  if( /*message[9] == eeprom_level_code[5] &&
						  message[8] == eeprom_level_code[4] &&
						  message[7] == eeprom_level_code[3] &&
						  message[6] == eeprom_level_code[2] &&
						  message[5] == eeprom_level_code[1] &&
						  message[4] == eeprom_level_code[0] ){*/

							  encryptedMessage[9] == eeprom_level_code[5] + 65 &&
							  encryptedMessage[8] == eeprom_level_code[4] + 65 &&
							  encryptedMessage[7] == eeprom_level_code[3] + 65 &&
							  encryptedMessage[6] == eeprom_level_code[2] + 65 &&
							  encryptedMessage[5] == eeprom_level_code[1] + 65 &&
							  encryptedMessage[4] == eeprom_level_code[0] + 65 ){

						  leveledUp = 1;
						  badgeLevel ++;
						  if(EE_WriteVariable8bits(EEP_CHALLENGE_LEVEL, badgeLevel) == EE_CLEANUP_REQUIRED) EE_CleanUp();
					  }

				  }
			  }

			  // draw bottom row and push to screen
			  matrix_drawCharCentered(message,0,8,COLOR_GREEN);
			  matrix_update();

			  if(keyboardInstant() == 0){
				  lampboard_clear();
				  lampboard_update();
			  }

		  }

	  }
}

void enigma_settings(struct Enigma *engi_machine){
	lampboard_clear();
	lampboard_setLetter('W', COLOR_RED);
	lampboard_setLetter('A', COLOR_RED);
	lampboard_setLetter('S', COLOR_RED);
	lampboard_setLetter('D', COLOR_RED);
	lampboard_setLetter('K', COLOR_RED);
	lampboard_update();
	uint8_t loop = 1;


	//matrix_clear();
	//matrix_drawCharCentered(ENIGMA_SETTINGS_TEXT,0,0,COLOR_WHITE);
	//matrix_update();

	static char default_rotor[] = "1 2 3 GAMMA";
	static int selectedMachine = 0;
	static char default_ring_settings[] = "A A A A";
	static char default_start_pos_settings[] = "A A A A";

	int settingsMenuState = 0;
	int selectedRotor = 0;

	int scrollCounter = 64;

	/*
	 * Menu States:
	 * 0-Machine Type (3 or 4)
	 * 1-Set Rotors
	 * 2-Ring Setting
	 * 3-Initial Ring
	 * 4-Plugboard
	 */

	while (loop){
		  if(nextFrame == 1){ //Set by Timer16 - 30FPS
			  nextFrame = 0;
			  UpdateMinibadges();
			  //Button Test Code
			  uint32_t buttons = 0;
			  buttons = keyboardUpdate();
			  if(buttons>>31 & 1) // If any key is held, exit
				  loop = 0;
			  if(buttons>>22 & 1){ //W
				  switch(settingsMenuState){
					  case 1:
						  if(selectedRotor == -1){// 4th rotor
							  if(default_rotor[6] == 'G'){
								  default_rotor[6] = 'B';
								  default_rotor[7] = 'E';
								  default_rotor[8] = 'T';
								  default_rotor[9] = 'A';
								  default_rotor[10] = '\0';
							  }else{
								  default_rotor[6] = 'G';
								  default_rotor[7] = 'A';
								  default_rotor[8] = 'M';
								  default_rotor[9] = 'M';
								  default_rotor[10] = 'A';
							  }
						  }else { // the rest of the rotors
							  if(default_rotor[selectedRotor*2] < 50) // if ascii '1' or below
								  default_rotor[selectedRotor*2] = 57; // '9' in ascii
							  else
								  default_rotor[selectedRotor*2]--;
						  }
						  break;
					  case 2:
						  if(selectedMachine)
							  selectedRotor++;

						  if(default_ring_settings[selectedRotor*2] < 66) // if ascii 'A' or below
							  default_ring_settings[selectedRotor*2] = 90; // 'Z' in ascii
						  else
							  default_ring_settings[selectedRotor*2]--;

						  if(selectedMachine)
							  selectedRotor--;

						  break;
					  case 3:
						  if(selectedMachine)
							  selectedRotor++;

						  if(default_start_pos_settings[selectedRotor*2] < 66) // if ascii 'A' or below
							  default_start_pos_settings[selectedRotor*2] = 90; // 'Z' in ascii
						  else
							  default_start_pos_settings[selectedRotor*2]--;

						  if(selectedMachine)
							  selectedRotor--;

						  break;
				  }
			  }
			  if(buttons>>0 & 1){ //A
				  switch(settingsMenuState){
					  case 0:
						  selectedMachine = 0;
						  break;
					  case 1:
					  case 2:
					  case 3:
						  if(selectedMachine){	// 4 rotor
							  if(selectedRotor >= 0)
								  selectedRotor--;
						  } else {
							  if(selectedRotor > 0)
								  selectedRotor--;
						  }
						  break;
				  }

			  }
			  if(buttons>>18 & 1){ //S
				  switch(settingsMenuState){
					  case 1:
						  if(selectedRotor == -1){	// 4th rotor
							  if(default_rotor[6] == 'G'){
								  default_rotor[6] = 'B';
								  default_rotor[7] = 'E';
								  default_rotor[8] = 'T';
								  default_rotor[9] = 'A';
								  default_rotor[10] = '\0';
							  }else{
								  default_rotor[6] = 'G';
								  default_rotor[7] = 'A';
								  default_rotor[8] = 'M';
								  default_rotor[9] = 'M';
								  default_rotor[10] = 'A';
							  }
						  } else { // the rest of the rotors
							  if(default_rotor[selectedRotor*2] > 56) // if ascii '9' or above
								  default_rotor[selectedRotor*2] = 49; // '1' in ascii
							  else
								  default_rotor[selectedRotor*2]++;
						  }
						  break;
					  case 2:
						  if(selectedMachine)
							  selectedRotor++;

						  if(default_ring_settings[selectedRotor*2] > 89) // if ascii 'Z' or above
							  default_ring_settings[selectedRotor*2] = 65; // 'A' in ascii
						  else
							  default_ring_settings[selectedRotor*2]++;

						  if(selectedMachine)
							  selectedRotor--;
						  break;
					  case 3:
						  if(selectedMachine)
							  selectedRotor++;

						  if(default_start_pos_settings[selectedRotor*2] > 89) // if ascii 'Z' or above
							  default_start_pos_settings[selectedRotor*2] = 65; // 'A' in ascii
						  else
							  default_start_pos_settings[selectedRotor*2]++;

						  if(selectedMachine)
							  selectedRotor--;
						  break;
				  }
			  }
			  if(buttons>>3 & 1) { //D
				  switch(settingsMenuState){
					  case 0:
						  selectedMachine = 1;
						  break;
					  case 1:
					  case 2:
					  case 3:
						  if(selectedRotor < 2)
							  selectedRotor++;

						  break;
				  }
			  }
			  if(buttons>>10 & 1) { //K
				  //keyK=1;
				  switch(settingsMenuState){
				  case 0:
					  settingsMenuState = 1;
					  if(selectedMachine) // 4th rotor
						  selectedRotor = -1;
					  break;
				  case 1:
					  settingsMenuState = 2;
					  if(selectedMachine) // 4th rotor
						  selectedRotor = -1;
					  break;
				  case 2:
					  settingsMenuState = 3;
					  if(selectedMachine) // 4th rotor
						  selectedRotor = -1;
					  break;
				  case 3:
					  settingsMenuState = 4;
					  break;
				  case 4:
					  loop = 0;
					  break;
				  }
			  }


			  switch(settingsMenuState){
				  case 0: //Machine Type (3 or 4)
					  matrix_clear();
					  matrix_drawCharCentered(ENIGMA_SETTINGS_MACHINE_HEADER_TEXT,0,0,COLOR_WHITE);
					  matrix_drawChar(ENIGMA_SETTINGS_TYPE_TEXT,30,8,COLOR_GREEN);
					  matrix_drawChar(ENIGMA_SETTINGS_TYPE1_TEXT,6,8,COLOR_GREEN);
					  matrix_drawChar(ENIGMA_SETTINGS_TYPE2_TEXT,18,8,COLOR_GREEN);
					  if(selectedMachine){
						  // 4 rotor machine
						  matrix_drawHorizontalLine(17, 7, 23, COLOR_RED);
						  matrix_drawHorizontalLine(17, 15, 23, COLOR_RED);
						  matrix_drawVerticalLine(17, 7, 15, COLOR_RED);
						  matrix_drawVerticalLine(23, 7, 15, COLOR_RED);
						  default_rotor[5] = ' '; // put ' ' back to get string  back
					  }else{
						  // 3 rotor machine
						  matrix_drawHorizontalLine(5, 7, 11, COLOR_RED);
						  matrix_drawHorizontalLine(5, 15, 11, COLOR_RED);
						  matrix_drawVerticalLine(5, 7, 15, COLOR_RED);
						  matrix_drawVerticalLine(11, 7, 15, COLOR_RED);
						  default_rotor[5] = '\0'; // set ' ' to null to end string
					  }
					  matrix_update();
					  break;
				  case 1: //Set Rotors
					  matrix_clear();
					  matrix_drawCharCentered(ENIGMA_SETTINGS_ROTORS_TEXT,0,0,COLOR_WHITE);

					  if(selectedMachine){
						  // four rotors
						  // draw characters
						  char temp[6] = "";
						  temp[0] = default_rotor[0];
						  matrix_drawChar(temp,34,8,COLOR_GREEN);
						  temp[0] = default_rotor[2];
						  matrix_drawChar(temp,44,8,COLOR_GREEN);
						  temp[0] = default_rotor[4];
						  matrix_drawChar(temp,54,8,COLOR_GREEN);
						  for(int l = 0; default_rotor[l+6] != '\0'; l++){
							  temp[l] = default_rotor[l+6];
						  }
						  matrix_drawChar(temp,0,8,COLOR_GREEN);
						  if(selectedRotor==-1) // 4th rotor
							  matrix_drawHorizontalLine(0, 15, 28, COLOR_RED);
						  else
							  matrix_drawHorizontalLine((selectedRotor * 10)+33, 15, (selectedRotor * 10) + 33 + 6, COLOR_RED);

					  }else{
						  // 3 rotors
						  // draw characters
						  matrix_drawCharCentered(default_rotor,0,8,COLOR_GREEN);
						  matrix_drawHorizontalLine(16 + (selectedRotor * 12), 15, 22 + (selectedRotor * 12), COLOR_RED);
					  }
					  matrix_update();
					  break;
				  case 2: //Ring Setting
					  matrix_clear();
					  matrix_drawCharCentered(ENIGMA_SETTINGS_RING_TEXT,0,0,COLOR_WHITE);
					  if(selectedMachine){
						  // four rotors
						  // draw characters
						  default_ring_settings[5] = ' '; // Ensure text has no null chars
						  matrix_drawCharCentered(default_ring_settings,0,8,COLOR_GREEN);

						  if(selectedRotor==-1) // 4th rotor
							  matrix_drawHorizontalLine(10, 15, 16, COLOR_RED);
						  else
							  matrix_drawHorizontalLine((selectedRotor * 12)+22, 15, (selectedRotor * 12) + 22 + 6, COLOR_RED);
					  }else{
						  // 3 rotors
						  // draw characters
						  default_ring_settings[5] = '\0'; // Set null char to end string early
						  matrix_drawCharCentered(default_ring_settings,0,8,COLOR_GREEN);
						  matrix_drawHorizontalLine(16 + (selectedRotor * 12), 15, 22 + (selectedRotor * 12), COLOR_RED);
					  }
					  matrix_update();
					  break;
				  case 3: //Initial Ring
					  matrix_clear();
					  matrix_drawCharCentered(ENIGMA_SETTINGS_INITIAL_TEXT,0,0,COLOR_WHITE);
					  if(selectedMachine){
						  // four rotors
						  // draw characters
						  default_start_pos_settings[5] = ' '; // Ensure text has no null chars
						  matrix_drawCharCentered(default_start_pos_settings,0,8,COLOR_GREEN);

						  if(selectedRotor==-1) // 4th rotor
							  matrix_drawHorizontalLine(10, 15, 16, COLOR_RED);
						  else
							  matrix_drawHorizontalLine((selectedRotor * 12)+22, 15, (selectedRotor * 12) + 22 + 6, COLOR_RED);
					  }else{
						  // 3 rotors
						  // draw characters
						  default_start_pos_settings[5] = '\0'; // Set null char to end string early
						  matrix_drawCharCentered(default_start_pos_settings,0,8,COLOR_GREEN);
						  matrix_drawHorizontalLine(16 + (selectedRotor * 12), 15, 22 + (selectedRotor * 12), COLOR_RED);
					  }
					  matrix_update();
					  break;
				  case 4: //Plugboard
					  matrix_clear();
					  if(HAL_GPIO_ReadPin(TEST1_GPIO_Port, TEST1_Pin) == GPIO_PIN_SET){
						  matrix_drawCharCentered("TEST GOOD",0,0,COLOR_GREEN);
					  } else {
						  matrix_drawCharCentered(ENIGMA_SETTINGS_PLUGBOARD_TEXT,0,0,COLOR_WHITE);
					  }
					  plugboardUpdate();
					  pairlength = 0;
					  for(x=0; x<26; x++){
						  if(plugboard[x] > 65+x){
							  pairs[pairlength - 1] = ' ';
							  pairlength += 3;
							  pairs[pairlength - 3] = x+65;
							  pairs[pairlength - 2] = plugboard[x];
							  pairs[pairlength] = '\0';
						  }
					  }
					  if(pairlength > 0){
					  scrollCounter--;
					  if(scrollCounter < (0-((pairlength*6)+64))) scrollCounter = 64;
					  matrix_drawChar(pairs, scrollCounter, 8, COLOR_RED);
					  } else {
						  matrix_drawCharCentered("EMPTY", 0, 8, COLOR_RED);
					  }
					  //matrix_drawCharCentered(ENIGMA_SETTINGS_PLUGBOARD2_TEXT,0,8,COLOR_WHITE);
					  matrix_update();

					  break;
			  	}




		  }

	  }

	if(selectedMachine){ // 4 rotor machine
		char temp_rotor[5] = "";
		temp_rotor[0] = default_rotor[0];
		temp_rotor[1] = default_rotor[2];
		temp_rotor[2] = default_rotor[4];
		temp_rotor[3] = default_rotor[6];

		char temp_ring[5] = "";
		temp_ring[0] = default_ring_settings[2];
		temp_ring[1] = default_ring_settings[4];
		temp_ring[2] = default_ring_settings[6];
		temp_ring[3] = default_ring_settings[0];

		char temp_start_pos[5] = "";
		temp_start_pos[0] = default_start_pos_settings[2];
		temp_start_pos[1] = default_start_pos_settings[4];
		temp_start_pos[2] = default_start_pos_settings[6];
		temp_start_pos[3] = default_start_pos_settings[0];
		enigma_setup(engi_machine, temp_rotor, temp_start_pos, temp_ring, &plugboard);
	} else {
		char temp_rotor[4] = "";
		temp_rotor[0] = default_rotor[0];
		temp_rotor[1] = default_rotor[2];
		temp_rotor[2] = default_rotor[4];

		char temp_ring[4] = "";
		temp_ring[0] = default_ring_settings[0];
		temp_ring[1] = default_ring_settings[2];
		temp_ring[2] = default_ring_settings[4];

		char temp_start_pos[4] = "";
		temp_start_pos[0] = default_start_pos_settings[0];
		temp_start_pos[1] = default_start_pos_settings[2];
		temp_start_pos[2] = default_start_pos_settings[4];
		enigma_setup(engi_machine, temp_rotor, temp_start_pos, temp_ring, &plugboard);
	}


}
