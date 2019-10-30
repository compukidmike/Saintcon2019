# Saintcon 2019 Enigma Badge

## About the Badge
There is a writeup available at https://mkfactor.com/?p=100

There is also my badge presentation at Saintcon https://www.youtube.com/watch?v=MAhJ2W_oN14

## Reprogramming your badge
You will need the STM32CubeProgrammer software from https://www.st.com/en/development-tools/stm32cubeprog.html to reprogram your badge.
It's cross-platform, though I've only tested it on Windows 10. Unfortunately ST requires you to create a login to download software :(

You'll also need the 2 hex files called Saintcon2019.hex and fpga.hex from this repository as well as a micro USB cable and one of the plugboard jumper wires from your badge.

Programming Steps:
1. Turn off the badge.
2. Connect the 2 TEST pins on your badge using the jumper (On the bottom-right side of your badge). If you haven't soldered that connector, you can put the jumper in from the back and hold it.
3. Connect the badge to your computer using a micro USB cable.
4. Turn on the badge. It will boot into DFU (Device Firmware Upgrade) mode.
5. From the STM32CubeProgrammer application, select USB from the drop-down menu at the top-right.
6. If you don't see anything next to "Serial number", click the little refresh button.
7. Now click the "+" tab next to Device Memory. Choose one of the hex files you downloaded earlier. Now do the same for the other hex file.
8. With one of the hex file tabs selected, click the blue Download button. This will load the file onto your badge. Now do the same for the other hex file tab.

Your badge is now programmed! Remove the jumper and turn it off and back on. Enjoy!


## Completing the Badge Challenge
Since you probably don't have 26 badges and a Hut 6 "teleprinter" at your house, it will be difficult to complete the challenges. For this reason I will be releasing a new badge program that will allow you to complete the challenges at home. Stay tuned for that soon!

## Editing Badge Code
The badge code was written using the STM32CubeIDE. It's cross-platform and works reasonably well. You'll need an ST-Link V2 USB adapter to program and debug your badge. They can be had for ~$10 from Amazon or even less from Aliexpress if you're willing to wait for shipping.
The IDE can be downloaded from ST here: https://www.st.com/en/development-tools/stm32cubeide.html

The FPGA code was written using Icestudio https://icestudio.io/ It's a cross-platform IDE for Lattice ICE40 FPGAs

The FPGA code is in SaintconFPGA.ice
