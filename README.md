# Saintcon 2019 Enigma Badge

## About the Badge
There is a writeup available at https://mkfactor.com/?p=100

There is also a video of my badge talk at Saintcon https://www.youtube.com/watch?v=MAhJ2W_oN14

## Completing the Badge Challenge
Since you probably don't have 26 badges and a Hut 6 "teleprinter" at your house, it will be difficult to complete the challenge. I have created a modified version of the badge firmware that will allow you to complete the challenge at home! You'll need to complete the steps for reprogramming your badge below. Once that's done, grab the Saintcon2019BadgeChallenge.pdf file from this repository. It has all the intercepted messages for you to decode. Be sure to decode them in order! Your badge should say "LEVELED UP" after successfully decoding a message. Good luck! If you get stuck, feel free to reach out on the SaintComms slack, or via twitter @compukidmike or even email compukidmike @ gmail.


## Reprogramming your badge
Note: This will completely erase the memory on your badge. Any progress in the challenge, as well as custom messages will be lost.

You will need the STM32CubeProgrammer software from https://www.st.com/en/development-tools/stm32cubeprog.html to reprogram your badge. It's cross-platform, though I've only tested it on Windows 10. Unfortunately ST requires you to create a login to download software :(

You'll also need the 2 hex files called Saintcon2019.hex and fpga.hex from this repository as well as a micro USB cable and one of the plugboard jumper wires from your badge.

Programming Steps:
1. Turn off the badge.
2. Connect the 2 TEST pins on your badge using the jumper (On the bottom-right side of your badge). If you haven't soldered that connector, you can put the jumper in from the back and hold it.
3. Connect the badge to your computer using a micro USB cable.
4. Turn on the badge. It will boot into DFU (Device Firmware Upgrade) mode.
5. From the STM32CubeProgrammer application, select USB from the drop-down menu at the top-right.
6. If you don't see anything next to "Serial number", click the little refresh button.
7. Click the green "Connect" button in the top-right corner. It should now say "Connected" above that button.
8. Click the Eraser button on the bottom-left corner (right above the question mark) and click OK to erase the badge memory. This is required for the new program to run correctly as there were many changes made to the way the internal EEPROM is organized.
9. Now click the "+" tab next to Device Memory and select "Open file". Choose one of the hex files you downloaded earlier. Now do the same for the other hex file.
10. With one of the hex file tabs selected, click the blue Download button. This will load the file onto your badge. Now do the same for the other hex file tab.
11. Remove the jumper and turn it off and back on. The first time you turn it on, it will go into a testing mode, where all the LEDs are turned on white. This is normal. Simply press each keyboard button until all the lampboard lights are off. It will say "TEST DONE". Turn it off and back on, and you are finished programming your badge. Enjoy!


## Editing Badge Code
The badge code can be found in the Firmware folder. It was written using the STM32CubeIDE. It's cross-platform and works reasonably well. You'll need an ST-Link V2 USB adapter to program and debug your badge. They can be had for ~$10 from Amazon or even less from Aliexpress if you're willing to wait for shipping.
The IDE can be downloaded from ST here: https://www.st.com/en/development-tools/stm32cubeide.html

The FPGA code was written using Icestudio https://icestudio.io/ It's a cross-platform IDE for Lattice ICE40 FPGAs

The FPGA code is in SaintconFPGA.ice
