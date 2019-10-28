/*
 * keyboard.c
 *
 *  Created on: Aug 24, 2019
 *      Author: compukidmike
 */

#include "keyboard.h"
#include "gpio.h"

#define gpio_ishigh(pin)	(HAL_GPIO_ReadPin(pin##_GPIO_Port, pin##_Pin) == GPIO_PIN_SET)
#define gpio_ispressed(pin)	!(HAL_GPIO_ReadPin(pin##_GPIO_Port, pin##_Pin) == GPIO_PIN_SET)
#define LONG_PRESS_MS 3000

static uint32_t PreviousButtonState;

static uint32_t APressedTime = 0;
static uint32_t BPressedTime = 0;
static uint32_t CPressedTime = 0;
static uint32_t DPressedTime = 0;
static uint32_t EPressedTime = 0;
static uint32_t FPressedTime = 0;
static uint32_t GPressedTime = 0;
static uint32_t HPressedTime = 0;
static uint32_t IPressedTime = 0;
static uint32_t JPressedTime = 0;
static uint32_t KPressedTime = 0;
static uint32_t LPressedTime = 0;
static uint32_t MPressedTime = 0;
static uint32_t NPressedTime = 0;
static uint32_t OPressedTime = 0;
static uint32_t PPressedTime = 0;
static uint32_t QPressedTime = 0;
static uint32_t RPressedTime = 0;
static uint32_t SPressedTime = 0;
static uint32_t TPressedTime = 0;
static uint32_t UPressedTime = 0;
static uint32_t VPressedTime = 0;
static uint32_t WPressedTime = 0;
static uint32_t XPressedTime = 0;
static uint32_t YPressedTime = 0;
static uint32_t ZPressedTime = 0;

uint32_t keyboardUpdate(){

	uint32_t retval = 0;

	if(gpio_ispressed(KBDA)){
		if(PreviousButtonState & 1<<0){
			if(HAL_GetTick()-APressedTime > LONG_PRESS_MS){
				retval |= 1<<31;//ButtonLongPressed
			}
		} else {
			APressedTime = HAL_GetTick();
			PreviousButtonState |= 1<<0;
			retval |= 1<<0;//A
		}
	} else {
		PreviousButtonState = PreviousButtonState & ~(1<<0);
	}
	if(gpio_ispressed(KBDB)){
		if(PreviousButtonState & 1<<1){
			if(HAL_GetTick()-BPressedTime > LONG_PRESS_MS){
				retval |= 1<<31;//ButtonLongPressed
			}
		} else {
			BPressedTime = HAL_GetTick();
			PreviousButtonState |= 1<<1;
			retval |= 1<<1;//B
		}
	} else {
		PreviousButtonState = PreviousButtonState & ~(1<<1);
	}
	if(gpio_ispressed(KBDC)){
			if(PreviousButtonState & 1<<2){
				if(HAL_GetTick()-CPressedTime > LONG_PRESS_MS){
					retval |= 1<<31;//ButtonLongPressed
				}
			} else {
				CPressedTime = HAL_GetTick();
				PreviousButtonState |= 1<<2;
				retval |= 1<<2;//C
			}
		} else {
			PreviousButtonState = PreviousButtonState & ~(1<<2);
		}
	if(gpio_ispressed(KBDD)){
			if(PreviousButtonState & 1<<3){
				if(HAL_GetTick()-DPressedTime > LONG_PRESS_MS){
					retval |= 1<<31;//ButtonLongPressed
				}
			} else {
				DPressedTime = HAL_GetTick();
				PreviousButtonState |= 1<<3;
				retval |= 1<<3;//D
			}
		} else {
			PreviousButtonState = PreviousButtonState & ~(1<<3);
		}
	if(gpio_ispressed(KBDE)){
			if(PreviousButtonState & 1<<4){
				if(HAL_GetTick()-EPressedTime > LONG_PRESS_MS){
					retval |= 1<<31;//ButtonLongPressed
				}
			} else {
				EPressedTime = HAL_GetTick();
				PreviousButtonState |= 1<<4;
				retval |= 1<<4;//E
			}
		} else {
			PreviousButtonState = PreviousButtonState & ~(1<<4);
		}
	if(gpio_ispressed(KBDF)){
			if(PreviousButtonState & 1<<5){
				if(HAL_GetTick()-FPressedTime > LONG_PRESS_MS){
					retval |= 1<<31;//ButtonLongPressed
				}
			} else {
				FPressedTime = HAL_GetTick();
				PreviousButtonState |= 1<<5;
				retval |= 1<<5;//F
			}
		} else {
			PreviousButtonState = PreviousButtonState & ~(1<<5);
		}
	if(gpio_ispressed(KBDG)){
			if(PreviousButtonState & 1<<6){
				if(HAL_GetTick()-GPressedTime > LONG_PRESS_MS){
					retval |= 1<<31;//ButtonLongPressed
				}
			} else {
				GPressedTime = HAL_GetTick();
				PreviousButtonState |= 1<<6;
				retval |= 1<<6;//G
			}
		} else {
			PreviousButtonState = PreviousButtonState & ~(1<<6);
		}
	if(gpio_ispressed(KBDH)){
			if(PreviousButtonState & 1<<7){
				if(HAL_GetTick()-HPressedTime > LONG_PRESS_MS){
					retval |= 1<<31;//ButtonLongPressed
				}
			} else {
				HPressedTime = HAL_GetTick();
				PreviousButtonState |= 1<<7;
				retval |= 1<<7;//H
			}
		} else {
			PreviousButtonState = PreviousButtonState & ~(1<<7);
		}
	if(gpio_ispressed(KBDI)){
			if(PreviousButtonState & 1<<8){
				if(HAL_GetTick()-IPressedTime > LONG_PRESS_MS){
					retval |= 1<<31;//ButtonLongPressed
				}
			} else {
				IPressedTime = HAL_GetTick();
				PreviousButtonState |= 1<<8;
				retval |= 1<<8;//I
			}
		} else {
			PreviousButtonState = PreviousButtonState & ~(1<<8);
		}
	if(gpio_ispressed(KBDJ)){
			if(PreviousButtonState & 1<<9){
				if(HAL_GetTick()-JPressedTime > LONG_PRESS_MS){
					retval |= 1<<31;//ButtonLongPressed
				}
			} else {
				JPressedTime = HAL_GetTick();
				PreviousButtonState |= 1<<9;
				retval |= 1<<9;//J
			}
		} else {
			PreviousButtonState = PreviousButtonState & ~(1<<9);
		}
	if(gpio_ispressed(KBDK)){
			if(PreviousButtonState & 1<<10){
				if(HAL_GetTick()-KPressedTime > LONG_PRESS_MS){
					retval |= 1<<31;//ButtonLongPressed
				}
			} else {
				KPressedTime = HAL_GetTick();
				PreviousButtonState |= 1<<10;
				retval |= 1<<10;//K
			}
		} else {
			PreviousButtonState = PreviousButtonState & ~(1<<10);
		}
	if(gpio_ispressed(KBDL)){
			if(PreviousButtonState & 1<<11){
				if(HAL_GetTick()-LPressedTime > LONG_PRESS_MS){
					retval |= 1<<31;//ButtonLongPressed
				}
			} else {
				LPressedTime = HAL_GetTick();
				PreviousButtonState |= 1<<11;
				retval |= 1<<11;//L
			}
		} else {
			PreviousButtonState = PreviousButtonState & ~(1<<11);
		}
	if(gpio_ispressed(KBDM)){
			if(PreviousButtonState & 1<<12){
				if(HAL_GetTick()-MPressedTime > LONG_PRESS_MS){
					retval |= 1<<31;//ButtonLongPressed
				}
			} else {
				MPressedTime = HAL_GetTick();
				PreviousButtonState |= 1<<12;
				retval |= 1<<12;//M
			}
		} else {
			PreviousButtonState = PreviousButtonState & ~(1<<12);
		}
	if(gpio_ispressed(KBDN)){
			if(PreviousButtonState & 1<<13){
				if(HAL_GetTick()-NPressedTime > LONG_PRESS_MS){
					retval |= 1<<31;//ButtonLongPressed
				}
			} else {
				NPressedTime = HAL_GetTick();
				PreviousButtonState |= 1<<13;
				retval |= 1<<13;//N
			}
		} else {
			PreviousButtonState = PreviousButtonState & ~(1<<13);
		}
	if(gpio_ispressed(KBDO)){
			if(PreviousButtonState & 1<<14){
				if(HAL_GetTick()-OPressedTime > LONG_PRESS_MS){
					retval |= 1<<31;//ButtonLongPressed
				}
			} else {
				OPressedTime = HAL_GetTick();
				PreviousButtonState |= 1<<14;
				retval |= 1<<14;//O
			}
		} else {
			PreviousButtonState = PreviousButtonState & ~(1<<14);
		}
	if(gpio_ispressed(KBDP)){
			if(PreviousButtonState & 1<<15){
				if(HAL_GetTick()-PPressedTime > LONG_PRESS_MS){
					retval |= 1<<31;//ButtonLongPressed
				}
			} else {
				PPressedTime = HAL_GetTick();
				PreviousButtonState |= 1<<15;
				retval |= 1<<15;//P
			}
		} else {
			PreviousButtonState = PreviousButtonState & ~(1<<15);
		}
	if(gpio_ispressed(KBDQ)){
			if(PreviousButtonState & 1<<16){
				if(HAL_GetTick()-QPressedTime > LONG_PRESS_MS){
					retval |= 1<<31;//ButtonLongPressed
				}
			} else {
				QPressedTime = HAL_GetTick();
				PreviousButtonState |= 1<<16;
				retval |= 1<<16;//Q
			}
		} else {
			PreviousButtonState = PreviousButtonState & ~(1<<16);
		}
	if(gpio_ispressed(KBDR)){
			if(PreviousButtonState & 1<<17){
				if(HAL_GetTick()-RPressedTime > LONG_PRESS_MS){
					retval |= 1<<31;//ButtonLongPressed
				}
			} else {
				RPressedTime = HAL_GetTick();
				PreviousButtonState |= 1<<17;
				retval |= 1<<17;//R
			}
		} else {
			PreviousButtonState = PreviousButtonState & ~(1<<17);
		}
	if(gpio_ispressed(KBDS)){
			if(PreviousButtonState & 1<<18){
				if(HAL_GetTick()-SPressedTime > LONG_PRESS_MS){
					retval |= 1<<31;//ButtonLongPressed
				}
			} else {
				SPressedTime = HAL_GetTick();
				PreviousButtonState |= 1<<18;
				retval |= 1<<18;//S
			}
		} else {
			PreviousButtonState = PreviousButtonState & ~(1<<18);
		}
	if(gpio_ispressed(KBDT)){
			if(PreviousButtonState & 1<<19){
				if(HAL_GetTick()-TPressedTime > LONG_PRESS_MS){
					retval |= 1<<31;//ButtonLongPressed
				}
			} else {
				TPressedTime = HAL_GetTick();
				PreviousButtonState |= 1<<19;
				retval |= 1<<19;//T
			}
		} else {
			PreviousButtonState = PreviousButtonState & ~(1<<19);
		}
	if(gpio_ispressed(KBDU)){
			if(PreviousButtonState & 1<<20){
				if(HAL_GetTick()-UPressedTime > LONG_PRESS_MS){
					retval |= 1<<31;//ButtonLongPressed
				}
			} else {
				UPressedTime = HAL_GetTick();
				PreviousButtonState |= 1<<20;
				retval |= 1<<20;//U
			}
		} else {
			PreviousButtonState = PreviousButtonState & ~(1<<20);
		}
	if(gpio_ispressed(KBDV)){
			if(PreviousButtonState & 1<<21){
				if(HAL_GetTick()-VPressedTime > LONG_PRESS_MS){
					retval |= 1<<31;//ButtonLongPressed
				}
			} else {
				VPressedTime = HAL_GetTick();
				PreviousButtonState |= 1<<21;
				retval |= 1<<21;//V
			}
		} else {
			PreviousButtonState = PreviousButtonState & ~(1<<21);
		}
	if(gpio_ispressed(KBDW)){
			if(PreviousButtonState & 1<<22){
				if(HAL_GetTick()-WPressedTime > LONG_PRESS_MS){
					retval |= 1<<31;//ButtonLongPressed
				}
			} else {
				WPressedTime = HAL_GetTick();
				PreviousButtonState |= 1<<22;
				retval |= 1<<22;//W
			}
		} else {
			PreviousButtonState = PreviousButtonState & ~(1<<22);
		}
	if(gpio_ispressed(KBDX)){
			if(PreviousButtonState & 1<<23){
				if(HAL_GetTick()-XPressedTime > LONG_PRESS_MS){
					retval |= 1<<31;//ButtonLongPressed
				}
			} else {
				XPressedTime = HAL_GetTick();
				PreviousButtonState |= 1<<23;
				retval |= 1<<23;//X
			}
		} else {
			PreviousButtonState = PreviousButtonState & ~(1<<23);
		}
	if(gpio_ispressed(KBDY)){
			if(PreviousButtonState & 1<<24){
				if(HAL_GetTick()-YPressedTime > LONG_PRESS_MS){
					retval |= 1<<31;//ButtonLongPressed
				}
			} else {
				YPressedTime = HAL_GetTick();
				PreviousButtonState |= 1<<24;
				retval |= 1<<24;//Y
			}
		} else {
			PreviousButtonState = PreviousButtonState & ~(1<<24);
		}
	if(gpio_ispressed(KBDZ)){
			if(PreviousButtonState & 1<<25){
				if(HAL_GetTick()-ZPressedTime > LONG_PRESS_MS){
					retval |= 1<<31;//ButtonLongPressed
				}
			} else {
				ZPressedTime = HAL_GetTick();
				PreviousButtonState |= 1<<25;
				retval |= 1<<25;//Z
			}
		} else {
			PreviousButtonState = PreviousButtonState & ~(1<<25);
		}
	//PreviousButtonState = retval;
	return retval;
}

uint32_t keyboardInstant(){
	uint32_t retval = 0;

	if(gpio_ispressed(KBDA)) retval |= 1<<0;//A
	if(gpio_ispressed(KBDB)) retval |= 1<<1;//B
	if(gpio_ispressed(KBDC)) retval |= 1<<2;//C
	if(gpio_ispressed(KBDD)) retval |= 1<<3;//D
	if(gpio_ispressed(KBDE)) retval |= 1<<4;//E
	if(gpio_ispressed(KBDF)) retval |= 1<<5;//F
	if(gpio_ispressed(KBDG)) retval |= 1<<6;//G
	if(gpio_ispressed(KBDH)) retval |= 1<<7;//H
	if(gpio_ispressed(KBDI)) retval |= 1<<8;//I
	if(gpio_ispressed(KBDJ)) retval |= 1<<9;//J
	if(gpio_ispressed(KBDK)) retval |= 1<<10;//K
	if(gpio_ispressed(KBDL)) retval |= 1<<11;//L
	if(gpio_ispressed(KBDM)) retval |= 1<<12;//M
	if(gpio_ispressed(KBDN)) retval |= 1<<13;//N
	if(gpio_ispressed(KBDO)) retval |= 1<<14;//O
	if(gpio_ispressed(KBDP)) retval |= 1<<15;//P
	if(gpio_ispressed(KBDQ)) retval |= 1<<16;//Q
	if(gpio_ispressed(KBDR)) retval |= 1<<17;//R
	if(gpio_ispressed(KBDS)) retval |= 1<<18;//S
	if(gpio_ispressed(KBDT)) retval |= 1<<19;//T
	if(gpio_ispressed(KBDU)) retval |= 1<<20;//U
	if(gpio_ispressed(KBDV)) retval |= 1<<21;//V
	if(gpio_ispressed(KBDW)) retval |= 1<<22;//W
	if(gpio_ispressed(KBDX)) retval |= 1<<23;//X
	if(gpio_ispressed(KBDY)) retval |= 1<<24;//Y
	if(gpio_ispressed(KBDZ)) retval |= 1<<25;//Z

	return retval;
}
