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

/* The Enigma machines were a family of portable cipher machines
with rotor scramblers. Good operating procedures, properly enforced,
would have made the cipher unbreakable.
However, most of the German armed and secret services and civilian agencies
that used Enigma employed poor procedures and it was these that allowed the
cipher to be broken.

The German plugboard-equipped Enigma became the Third Reich's
principal crypto-system. It was reconstructed by the Polish
General Staff's Cipher Bureau in December 1932 with the aid of
French-supplied intelligence material that had been obtained
from a German spy. Shortly before the outbreak of World War II,
the Polish Cipher Bureau initiated the French and British into its
Enigma-breaking techniques and technology at a conference held in Warsaw.

From this beginning, the British Government Code and Cypher School at
Bletchley Park built up an extensive cryptanalytic facility. Initially,
the decryption was mainly of Luftwaffe and a few Army messages,
as the German Navy employed much more secure procedures for using Enigma.

Alan Turing, a Cambridge University mathematician and logician,
provided much of the original thinking that led to the design of
the cryptanalytical Bombe machines, and the eventual breaking of naval Enigma.
However, when the German Navy introduced an Enigma version with a
fourth rotor for its U-boats, there was a prolonged period when those messages
could not be decrypted. With the capture of relevant cipher keys and the use
of much faster U.S. Navy Bombes, regular, rapid reading of
German naval messages resumed.

The rotors (alternatively wheels or drums, Walzen in German)
formed the heart of an Enigma machine. Each rotor was a disc
approximately 10 cm (3.9 in) in diameter made from hard rubber
or bakelite with brass spring-loaded pins on one face arranged
in a circle; on the other side are a corresponding number
of circular electrical contacts. The pins and contacts represent
the alphabet, typically the 26 letters A to Z.

Setting Wiring                      Notch   Window  Turnover
Base    ABCDEFGHIJKLMNOPQRSTUVWXYZ
I       EKMFLGDQVZNTOWYHXUSPAIBRCJ  Y       Q       R
II      AJDKSIRUXBLHWTMCQGZNPYFVOE  M       E       F
III     BDFHJLCPRTXVZNYEIWGAKMUSQO  D       V       W
IV      ESOVPZJAYQUIRHXLNFTGKDCMWB  R       J       K
V       VZBRGITYUPSDNHLXAWMJQOFECK  H       Z       A
VI      JPGVOUMFYQBENHZRDKASXLICTW  H/U     Z/M     A/N
VII     NZJHGRCXMYSWBOUFAIVLPEKQDT  H/U     Z/M     A/N
VIII    FKQHTLXOCBJSPDZRAMEWNIUYGV  H/U     Z/M     A/N

With the exception of the early Enigma models A and B,
the last rotor came before a reflector (German: Umkehrwalze,
meaning reversal rotor), a patented feature distinctive of the
Enigma family amongst the various rotor machines designed
in the period. The reflector connected outputs of the
last rotor in pairs, redirecting current back through the
rotors by a different route. The reflector ensured that
Enigma is self-reciprocal: conveniently, encryption was
the same as decryption. However, the reflector also gave
Enigma the property that no letter ever encrypted to itself.
This was a severe conceptual flaw and a cryptological mistake
subsequently exploited by codebreakers.

Setting     Wiring
Base        ABCDEFGHIJKLMNOPQRSTUVWXYZ
A           EJMZALYXVBWFCRQUONTSPIKHGD
B           YRUHQSLDPXNGOKMIEBFZCWVJAT
C           FVPJIAOYEDRZXWGCTKUQSBNMHL
*/

#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define ROTATE 26

struct Rotor {
    int             offset;
    int             turnnext;
    int             turnover_offset;
    const char      *cipher;
    const char      *turnover;
    const char      *notch;
};

struct Enigma {
    int             numrotors;
    const char      *plugboard;
    const char      *reflector;
    struct Rotor    rotors[8];
};

struct Rotor new_rotor(struct Enigma *machine, int rotornumber);
char cycle_letter_alphabet(char letter, int offset);
void cycle_string_alphabet(char *string, int offset);
int str_index(const char *str, int character);
void rotor_cycle(struct Rotor *rotor);
int rotor_forward(struct Rotor *rotor, int index);
int rotor_reverse(struct Rotor *rotor, int index);
char uppercase(char letter);
void apply_rotor_ringsetting(struct Rotor *rotor, char rotor_ring_setting, int opt);
char enigma_encode_char(struct Enigma *machine, char letter);
void enigma_setup(struct Enigma *new_machine, char *rotors, char *rotors_initial_value, char *rotor_ring_setting, char *plugboard);
