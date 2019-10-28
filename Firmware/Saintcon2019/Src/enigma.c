/*
 * Enigma Machine Code
 *
 * This is the code that runs the actual encryption.
 *
 * The majority of this code was copied from:
 * http://corylutton.com/enigma-c.html
 *
 * There have been heavy modifications to this code by @_bashNinja
 * to allow it to run as a library for the badge to use.
 *
 * ------------------------------------------------------------------
 * Copyright 2013 Cory Lutton
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "enigma.h"

const char *alpha = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
const char *rotor_ciphers[] = {
    "EKMFLGDQVZNTOWYHXUSPAIBRCJ", // rotor I
    "AJDKSIRUXBLHWTMCQGZNPYFVOE", // rotor II
    "BDFHJLCPRTXVZNYEIWGAKMUSQO", // rotor III
    "ESOVPZJAYQUIRHXLNFTGKDCMWB", // rotor IV
    "VZBRGITYUPSDNHLXAWMJQOFECK", // rotor V
    "JPGVOUMFYQBENHZRDKASXLICTW", // rotor VI
    "NZJHGRCXMYSWBOUFAIVLPEKQDT", // rotor VII
    "FKQHTLXOCBJSPDZRAMEWNIUYGV", // rotor VIII
    "BASHCOMPUKIDZERGYJWLQTFXVN", // saintcon rotor aka IX
	"LEYJVCNIXWPBQMDRTAKZGFUHOS", // rotor beta
	"FSOKANUERHMBTIYCWLQPZXVGJD"  // rotor gamma
};

const char *rotor_notches[] = {"Q", "E", "V", "J", "Z", "ZM", "ZM", "ZM", "Y", "", ""};

const char *rotor_turnovers[] = {"R", "F", "W", "K", "A", "AN", "AN", "AN", "Z", "", ""};

const char *reflectors[] = { //UKW
    "EJMZALYXVBWFCRQUONTSPIKHGD", // reflector A
    "YRUHQSLDPXNGOKMIEBFZCWVJAT", // reflector B
    "FVPJIAOYEDRZXWGCTKUQSBNMHL", // reflector C
	"ENKQAUYWJICOPBLMDXZVFTHRGS", // reflector B Thin
	"RDOBJNTKVEHMLFCWZAXGYIPSUQ", // reflector C Thin
	"ABCDEFGHIJKLMNOPQRSTUVWXYZ"  // ETW (entry disc)
};


/*
 * Produce a rotor object
 * Setup the correct offset, cipher set and turn overs.
 */
struct Rotor new_rotor(struct Enigma *machine, int rotornumber) {
    switch (rotornumber)
    {
    case 66: // B
    case 98: // b
        rotornumber = 10; // beta
        break;
    case 71: // G
    case 103: // g
    	rotornumber = 11; // gamma
        break;
    }

    struct Rotor r;
    r.offset = 0;
    r.turnover_offset = 0;
    r.turnnext = 0;
    r.cipher = rotor_ciphers[rotornumber - 1];
    r.turnover = rotor_turnovers[rotornumber - 1];
    r.notch = rotor_notches[rotornumber - 1];
    machine->numrotors++;

    return r;
}

char cycle_letter_alphabet(char letter, int offset){
    letter = uppercase(letter);
    letter += offset; // 90 = Z, last char in the alphabet
    if (letter > 90)
        letter -= ROTATE; // rotate should be the full alphabet
    if(letter < 65)
        letter += ROTATE;

    return letter;
}

void cycle_string_alphabet(char *string, int offset){
    for (int g = 0; string[g] != '\0'; g++){
        string[g] = cycle_letter_alphabet(string[g], offset);
    }
}

/*
 * Return the index position of a character inside a string
 * if not found then -1
 **/
int str_index(const char *str, int character) {
    char *pos;
    int index;
    pos = strchr(str, character);

    // pointer arithmetic
    if (pos){
        index = (int) (pos - str);
    } else {
        index = -1;
    }

    return index;
}

/*
 * Cycle a rotor's offset but keep it in the array.
 */
void rotor_cycle(struct Rotor *rotor) {
    rotor->offset++;
    rotor->offset = rotor->offset % ROTATE;

    //
    char temp_turnover[strlen(rotor->turnover)];
    strncpy(temp_turnover, rotor->turnover, strlen(rotor->turnover)+1);
    cycle_string_alphabet(temp_turnover, rotor->turnover_offset);

    // Check if the notch is active, if so trigger the turnnext
    if(str_index(temp_turnover, alpha[rotor->offset]) >= 0) {
        rotor->turnnext = 1;
    }
}

/*
 * Pass through a rotor, right to left, cipher to alpha.
 * returns the exit index location.
 */
int rotor_forward(struct Rotor *rotor, int index) {

    // In the cipher side, out the alpha side
    index = (index + rotor->offset) % ROTATE;
    index = str_index(alpha, rotor->cipher[index]);
    index = (ROTATE + index - rotor->offset) % ROTATE;

    return index;
}

/*
 * Pass through a rotor, left to right, alpha to cipher.
 * returns the exit index location.
 */
int rotor_reverse(struct Rotor *rotor, int index) {

    // In the cipher side, out the alpha side
    index = (index + rotor->offset) % ROTATE;
    index = str_index(rotor->cipher, alpha[index]);
    index = (ROTATE + index - rotor->offset) % ROTATE;

    return index;

}
char uppercase(char letter){
    if (isalpha(letter) )
    {
       if (letter>=97 && letter<=122)
        {
          letter = letter-32;
        }
    }
    return letter;
}

void apply_rotor_ringsetting(struct Rotor *rotor, char rotor_ring_setting, int opt){

    rotor->turnover_offset = opt;
	rotor_ring_setting = uppercase(rotor_ring_setting);
	opt = rotor_ring_setting - opt - 1;

	opt = 90 - opt; // 90 = Z, last char in the alphabet
	if (opt > 90)
		opt = opt - ROTATE; // rotate should be the full alphabet

    rotor->turnover_offset = opt - rotor->turnover_offset;
    if (rotor->turnover_offset > ROTATE)
		rotor->turnover_offset -= ROTATE; // rotate should be the full alphabet

	rotor->offset = opt - 65;
}

char enigma_encode_char(struct Enigma *machine, char letter){
    if(isalpha(letter)){
        char character = letter;

        character = toupper(character);

        // Plugboard
        int index = str_index(machine->plugboard, character);

        // Cycle first rotor before pushing through,
        rotor_cycle(&machine->rotors[0]);

        // Double step the rotor
        char temp_notch[strlen(machine->rotors[1].notch)];
        strncpy(temp_notch, machine->rotors[1].notch, strlen(machine->rotors[1].notch)+1);
        cycle_string_alphabet(temp_notch, machine->rotors[1].turnover_offset);

        if(str_index(temp_notch, alpha[machine->rotors[1].offset]) >= 0 ) {
            rotor_cycle(&machine->rotors[1]);
        }

        // Stepping the rotors
        for(int t = 0; t < machine->numrotors - 1; t++) {
            character = alpha[machine->rotors[t].offset];

            if(machine->rotors[t].turnnext) {
                machine->rotors[t].turnnext = 0;
                if(t+1 <3)
                    rotor_cycle(&machine->rotors[t+1]);
            }
            }

        // Pass through all the rotors forward
        for(int rotor_number = 0; rotor_number < machine->numrotors; rotor_number++) {
            index = rotor_forward(&machine->rotors[rotor_number], index);
        }

        // Inbound
        character = machine->reflector[index];
        // Outbound
        index = str_index(alpha, character);

        // Pass back through the rotors in reverse
        for(int j = machine->numrotors - 1; j >= 0; j--) {
            index = rotor_reverse(&machine->rotors[j], index);
        }

        // Pass through Plugboard
        character = machine->plugboard[index];

        letter = character;
	}
    return letter;
}

/*
 * Run the enigma machine
 *
 * example: enigma(message_str, "123", "ABC", "XYZ");
 * the code will encode the message in message_str, and put the new value
 * back into the very same variable.
 * rotors = 1, 2, 3, 4, 5, 6, 7, 8, b(beta), g(gamma)
 **/
void enigma_setup(struct Enigma *new_machine, char *rotors, char *rotors_initial_value, char *rotor_ring_setting, char *plugboard)
{
    // -- Command Parsing
    // which rotors
	int opt_r3 = atoi(&rotors[0])/100;
	int opt_r2 = atoi(&rotors[1])/10;
	int opt_r1 = atoi(&rotors[2]);
	// which initial value
	int opt_o3 = rotors_initial_value[0];
	int opt_o2 = rotors_initial_value[1];
	int opt_o1 = rotors_initial_value[2];

    // Configure an enigma machine
    new_machine->reflector = reflectors[1];
    new_machine->rotors[0] = new_rotor(new_machine, opt_r1);
    new_machine->rotors[1] = new_rotor(new_machine, opt_r2);
    new_machine->rotors[2] = new_rotor(new_machine, opt_r3);
    if(rotors[3] != '\0'){ // look for special 4th rotor. Gamma/Beta
    	new_machine->rotors[3] = new_rotor(new_machine, rotors[3]);
    }

    // apply ring setting
	apply_rotor_ringsetting(&new_machine->rotors[2], rotor_ring_setting[0], rotors_initial_value[0]);
	apply_rotor_ringsetting(&new_machine->rotors[1], rotor_ring_setting[1], rotors_initial_value[1]);
	apply_rotor_ringsetting(&new_machine->rotors[0], rotor_ring_setting[2], rotors_initial_value[2]);
     if(rotors[3] != '\0'){ // look for special 4th rotor. Gamma/Beta
    	apply_rotor_ringsetting(&new_machine->rotors[3], rotor_ring_setting[3], rotors_initial_value[3]);
    }

    // set plugboard
    new_machine->plugboard = plugboard;
}
