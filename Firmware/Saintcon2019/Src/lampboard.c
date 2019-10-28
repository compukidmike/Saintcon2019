/*
 * lampboard.c
 *
 *  Created on: Aug 24, 2019
 *      Author: compukidmike
 */

#include "lampboard.h"
#include "drv_is31fl.h"


RGB lampboard[26];

void lampboard_setLetter(char letter, RGB color){
	if((letter >= 64) & (letter <= 90)){
		lampboard[(uint8_t)letter-65] = color;
	}
}

void lampboard_setLamp(uint8_t lamp, RGB color){
	if(lamp < 26){
		lampboard[lamp] = color;
	}
}

void lampboard_fill(RGB color){
	for(int x=0; x<26; x++){
			lampboard[x] = color;
		}
}

void lampboard_update(){

	drv_is31fl_send_value(ARED,lampboard[0].R);
	drv_is31fl_send_value(AGRN,lampboard[0].G);
	drv_is31fl_send_value(ABLU,lampboard[0].B);
	drv_is31fl_send_value(BRED,lampboard[1].R);
	drv_is31fl_send_value(BGRN,lampboard[1].G);
	drv_is31fl_send_value(BBLU,lampboard[1].B);
	drv_is31fl_send_value(CRED,lampboard[2].R);
	drv_is31fl_send_value(CGRN,lampboard[2].G);
	drv_is31fl_send_value(CBLU,lampboard[2].B);
	drv_is31fl_send_value(DRED,lampboard[3].R);
	drv_is31fl_send_value(DGRN,lampboard[3].G);
	drv_is31fl_send_value(DBLU,lampboard[3].B);
	drv_is31fl_send_value(ERED,lampboard[4].R);
	drv_is31fl_send_value(EGRN,lampboard[4].G);
	drv_is31fl_send_value(EBLU,lampboard[4].B);
	drv_is31fl_send_value(FRED,lampboard[5].R);
	drv_is31fl_send_value(FGRN,lampboard[5].G);
	drv_is31fl_send_value(FBLU,lampboard[5].B);
	drv_is31fl_send_value(GRED,lampboard[6].R);
	drv_is31fl_send_value(GGRN,lampboard[6].G);
	drv_is31fl_send_value(GBLU,lampboard[6].B);
	drv_is31fl_send_value(HRED,lampboard[7].R);
	drv_is31fl_send_value(HGRN,lampboard[7].G);
	drv_is31fl_send_value(HBLU,lampboard[7].B);
	drv_is31fl_send_value(IRED,lampboard[8].R);
	drv_is31fl_send_value(IGRN,lampboard[8].G);
	drv_is31fl_send_value(IBLU,lampboard[8].B);
	drv_is31fl_send_value(JRED,lampboard[9].R);
	drv_is31fl_send_value(JGRN,lampboard[9].G);
	drv_is31fl_send_value(JBLU,lampboard[9].B);
	drv_is31fl_send_value(KRED,lampboard[10].R);
	drv_is31fl_send_value(KGRN,lampboard[10].G);
	drv_is31fl_send_value(KBLU,lampboard[10].B);
	drv_is31fl_send_value(LRED,lampboard[11].R);
	drv_is31fl_send_value(LGRN,lampboard[11].G);
	drv_is31fl_send_value(LBLU,lampboard[11].B);
	drv_is31fl_send_value(MRED,lampboard[12].R);
	drv_is31fl_send_value(MGRN,lampboard[12].G);
	drv_is31fl_send_value(MBLU,lampboard[12].B);
	drv_is31fl_send_value(NRED,lampboard[13].R);
	drv_is31fl_send_value(NGRN,lampboard[13].G);
	drv_is31fl_send_value(NBLU,lampboard[13].B);
	drv_is31fl_send_value(ORED,lampboard[14].R);
	drv_is31fl_send_value(OGRN,lampboard[14].G);
	drv_is31fl_send_value(OBLU,lampboard[14].B);
	drv_is31fl_send_value(PRED,lampboard[15].R);
	drv_is31fl_send_value(PGRN,lampboard[15].G);
	drv_is31fl_send_value(PBLU,lampboard[15].B);
	drv_is31fl_send_value(QRED,lampboard[16].R);
	drv_is31fl_send_value(QGRN,lampboard[16].G);
	drv_is31fl_send_value(QBLU,lampboard[16].B);
	drv_is31fl_send_value(RRED,lampboard[17].R);
	drv_is31fl_send_value(RGRN,lampboard[17].G);
	drv_is31fl_send_value(RBLU,lampboard[17].B);
	drv_is31fl_send_value(SRED,lampboard[18].R);
	drv_is31fl_send_value(SGRN,lampboard[18].G);
	drv_is31fl_send_value(SBLU,lampboard[18].B);
	drv_is31fl_send_value(TRED,lampboard[19].R);
	drv_is31fl_send_value(TGRN,lampboard[19].G);
	drv_is31fl_send_value(TBLU,lampboard[19].B);
	drv_is31fl_send_value(URED,lampboard[20].R);
	drv_is31fl_send_value(UGRN,lampboard[20].G);
	drv_is31fl_send_value(UBLU,lampboard[20].B);
	drv_is31fl_send_value(VRED,lampboard[21].R);
	drv_is31fl_send_value(VGRN,lampboard[21].G);
	drv_is31fl_send_value(VBLU,lampboard[21].B);
	drv_is31fl_send_value(WRED,lampboard[22].R);
	drv_is31fl_send_value(WGRN,lampboard[22].G);
	drv_is31fl_send_value(WBLU,lampboard[22].B);
	drv_is31fl_send_value(XRED,lampboard[23].R);
	drv_is31fl_send_value(XGRN,lampboard[23].G);
	drv_is31fl_send_value(XBLU,lampboard[23].B);
	drv_is31fl_send_value(YRED,lampboard[24].R);
	drv_is31fl_send_value(YGRN,lampboard[24].G);
	drv_is31fl_send_value(YBLU,lampboard[24].B);
	drv_is31fl_send_value(ZRED,lampboard[25].R);
	drv_is31fl_send_value(ZGRN,lampboard[25].G);
	drv_is31fl_send_value(ZBLU,lampboard[25].B);
}

void lampboard_clear(){
	for(int x=0; x<26; x++){
		lampboard[x] = COLOR_BLACK;
	}
}
