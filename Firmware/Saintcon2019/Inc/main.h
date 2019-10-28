/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);

void UART_Right_IRQ(UART_HandleTypeDef *huart);
void UART_Left_IRQ(UART_HandleTypeDef *huart);
void CheckSerial();


/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define KBD_A_Pin GPIO_PIN_2
#define KBD_A_GPIO_Port GPIOE
#define KBD_B_Pin GPIO_PIN_3
#define KBD_B_GPIO_Port GPIOE
#define KBD_C_Pin GPIO_PIN_4
#define KBD_C_GPIO_Port GPIOE
#define KBD_D_Pin GPIO_PIN_5
#define KBD_D_GPIO_Port GPIOE
#define KBD_E_Pin GPIO_PIN_6
#define KBD_E_GPIO_Port GPIOE
#define PlugX_Pin GPIO_PIN_13
#define PlugX_GPIO_Port GPIOC
#define PlugY_Pin GPIO_PIN_14
#define PlugY_GPIO_Port GPIOC
#define PlugZ_Pin GPIO_PIN_15
#define PlugZ_GPIO_Port GPIOC
#define I2CRST_Pin GPIO_PIN_1
#define I2CRST_GPIO_Port GPIOH
#define PlugK_Pin GPIO_PIN_0
#define PlugK_GPIO_Port GPIOC
#define PlugL_Pin GPIO_PIN_1
#define PlugL_GPIO_Port GPIOC
#define PlugM_Pin GPIO_PIN_2
#define PlugM_GPIO_Port GPIOC
#define PlugN_Pin GPIO_PIN_3
#define PlugN_GPIO_Port GPIOC
#define PlugA_Pin GPIO_PIN_0
#define PlugA_GPIO_Port GPIOA
#define PlugB_Pin GPIO_PIN_1
#define PlugB_GPIO_Port GPIOA
#define PlugC_Pin GPIO_PIN_4
#define PlugC_GPIO_Port GPIOA
#define PlugD_Pin GPIO_PIN_5
#define PlugD_GPIO_Port GPIOA
#define PlugE_Pin GPIO_PIN_6
#define PlugE_GPIO_Port GPIOA
#define PlugF_Pin GPIO_PIN_7
#define PlugF_GPIO_Port GPIOA
#define PlugO_Pin GPIO_PIN_4
#define PlugO_GPIO_Port GPIOC
#define PlugP_Pin GPIO_PIN_5
#define PlugP_GPIO_Port GPIOC
#define PlugH_Pin GPIO_PIN_0
#define PlugH_GPIO_Port GPIOB
#define PlugI_Pin GPIO_PIN_1
#define PlugI_GPIO_Port GPIOB
#define PlugJ_Pin GPIO_PIN_2
#define PlugJ_GPIO_Port GPIOB
#define KBD_F_Pin GPIO_PIN_7
#define KBD_F_GPIO_Port GPIOE
#define KBD_G_Pin GPIO_PIN_8
#define KBD_G_GPIO_Port GPIOE
#define KBD_H_Pin GPIO_PIN_9
#define KBD_H_GPIO_Port GPIOE
#define KBD_I_Pin GPIO_PIN_10
#define KBD_I_GPIO_Port GPIOE
#define KBD_J_Pin GPIO_PIN_11
#define KBD_J_GPIO_Port GPIOE
#define KBD_K_Pin GPIO_PIN_12
#define KBD_K_GPIO_Port GPIOE
#define KBD_L_Pin GPIO_PIN_13
#define KBD_L_GPIO_Port GPIOE
#define KBD_M_Pin GPIO_PIN_14
#define KBD_M_GPIO_Port GPIOE
#define KBD_N_Pin GPIO_PIN_15
#define KBD_N_GPIO_Port GPIOE
#define MB_CS1_Pin GPIO_PIN_12
#define MB_CS1_GPIO_Port GPIOB
#define MB_RESET_Pin GPIO_PIN_8
#define MB_RESET_GPIO_Port GPIOD
#define KBD_W_Pin GPIO_PIN_9
#define KBD_W_GPIO_Port GPIOD
#define KBD_X_Pin GPIO_PIN_10
#define KBD_X_GPIO_Port GPIOD
#define KBD_Y_Pin GPIO_PIN_11
#define KBD_Y_GPIO_Port GPIOD
#define KBD_Z_Pin GPIO_PIN_12
#define KBD_Z_GPIO_Port GPIOD
#define MB_CS2_Pin GPIO_PIN_13
#define MB_CS2_GPIO_Port GPIOD
#define MB_CS3_Pin GPIO_PIN_14
#define MB_CS3_GPIO_Port GPIOD
#define SYNC_Pin GPIO_PIN_15
#define SYNC_GPIO_Port GPIOD
#define SYNC_EXTI_IRQn EXTI15_10_IRQn
#define PlugQ_Pin GPIO_PIN_6
#define PlugQ_GPIO_Port GPIOC
#define PlugR_Pin GPIO_PIN_7
#define PlugR_GPIO_Port GPIOC
#define PlugS_Pin GPIO_PIN_8
#define PlugS_GPIO_Port GPIOC
#define PlugT_Pin GPIO_PIN_9
#define PlugT_GPIO_Port GPIOC
#define PlugG_Pin GPIO_PIN_8
#define PlugG_GPIO_Port GPIOA
#define PlugU_Pin GPIO_PIN_10
#define PlugU_GPIO_Port GPIOC
#define PlugV_Pin GPIO_PIN_11
#define PlugV_GPIO_Port GPIOC
#define PlugW_Pin GPIO_PIN_12
#define PlugW_GPIO_Port GPIOC
#define KBD_O_Pin GPIO_PIN_0
#define KBD_O_GPIO_Port GPIOD
#define KBD_P_Pin GPIO_PIN_1
#define KBD_P_GPIO_Port GPIOD
#define KBD_Q_Pin GPIO_PIN_2
#define KBD_Q_GPIO_Port GPIOD
#define KBD_R_Pin GPIO_PIN_3
#define KBD_R_GPIO_Port GPIOD
#define KBD_S_Pin GPIO_PIN_4
#define KBD_S_GPIO_Port GPIOD
#define KBD_T_Pin GPIO_PIN_5
#define KBD_T_GPIO_Port GPIOD
#define KBD_U_Pin GPIO_PIN_6
#define KBD_U_GPIO_Port GPIOD
#define KBD_V_Pin GPIO_PIN_7
#define KBD_V_GPIO_Port GPIOD
#define ICE_CRESET_Pin GPIO_PIN_6
#define ICE_CRESET_GPIO_Port GPIOB
#define ICE_CDONE_Pin GPIO_PIN_7
#define ICE_CDONE_GPIO_Port GPIOB
#define TEST1_Pin GPIO_PIN_3
#define TEST1_GPIO_Port GPIOH
#define MB_CS4_Pin GPIO_PIN_0
#define MB_CS4_GPIO_Port GPIOE
#define SDB_Pin GPIO_PIN_1
#define SDB_GPIO_Port GPIOE
/* USER CODE BEGIN Private defines */
#define LED5_Pin GPIO_PIN_13
#define LED5_GPIO_Port GPIOC
#define SPI1_MUX_S_Pin GPIO_PIN_14
#define SPI1_MUX_S_GPIO_Port GPIOC
#define SPI1_MUX_OE_Pin GPIO_PIN_15
#define SPI1_MUX_OE_GPIO_Port GPIOC
#define AN0_Pin GPIO_PIN_0
#define AN0_GPIO_Port GPIOC
#define AN1_Pin GPIO_PIN_1
#define AN1_GPIO_Port GPIOC
#define AN2_Pin GPIO_PIN_2
#define AN2_GPIO_Port GPIOC
#define AN3_Pin GPIO_PIN_3
#define AN3_GPIO_Port GPIOC
#define AN4_Pin GPIO_PIN_0
#define AN4_GPIO_Port GPIOA
#define AN5_Pin GPIO_PIN_1
#define AN5_GPIO_Port GPIOA
#define QSPI_CS_Pin GPIO_PIN_2
#define QSPI_CS_GPIO_Port GPIOA
#define QSPI_CLK_Pin GPIO_PIN_3
#define QSPI_CLK_GPIO_Port GPIOA
#define DIG9_Pin GPIO_PIN_4
#define DIG9_GPIO_Port GPIOA
#define DIG5_Pin GPIO_PIN_5
#define DIG5_GPIO_Port GPIOA
#define QSPI_D3_Pin GPIO_PIN_6
#define QSPI_D3_GPIO_Port GPIOA
#define QSPI_D2_Pin GPIO_PIN_7
#define QSPI_D2_GPIO_Port GPIOA
#define DIG0_Pin GPIO_PIN_4
#define DIG0_GPIO_Port GPIOC
#define DIG1_Pin GPIO_PIN_5
#define DIG1_GPIO_Port GPIOC
#define QSPI_D1_Pin GPIO_PIN_0
#define QSPI_D1_GPIO_Port GPIOB
#define QSPI_D0_Pin GPIO_PIN_1
#define QSPI_D0_GPIO_Port GPIOB
#define DIG2_Pin GPIO_PIN_2
#define DIG2_GPIO_Port GPIOB
#define DIG3_Pin GPIO_PIN_10
#define DIG3_GPIO_Port GPIOB
#define DIG4_Pin GPIO_PIN_11
#define DIG4_GPIO_Port GPIOB
#define DIG10_Pin GPIO_PIN_12
#define DIG10_GPIO_Port GPIOB
#define DIG13_Pin GPIO_PIN_13
#define DIG13_GPIO_Port GPIOB
#define DIG12_Pin GPIO_PIN_14
#define DIG12_GPIO_Port GPIOB
#define DIG11_Pin GPIO_PIN_15
#define DIG11_GPIO_Port GPIOB
#define DIG7_Pin GPIO_PIN_6
#define DIG7_GPIO_Port GPIOC
#define DIG8_Pin GPIO_PIN_7
#define DIG8_GPIO_Port GPIOC
#define B1_Pin GPIO_PIN_8
#define B1_GPIO_Port GPIOC
#define B2_Pin GPIO_PIN_9
#define B2_GPIO_Port GPIOC
#define DIG6_Pin GPIO_PIN_8
#define DIG6_GPIO_Port GPIOA
#define ICE40_SPI_CS_Pin GPIO_PIN_15
#define ICE40_SPI_CS_GPIO_Port GPIOA
#define DIG17_Pin GPIO_PIN_10
#define DIG17_GPIO_Port GPIOC
#define DIG18_Pin GPIO_PIN_11
#define DIG18_GPIO_Port GPIOC
#define DIG19_Pin GPIO_PIN_12
#define DIG19_GPIO_Port GPIOC
#define DIG16_Pin GPIO_PIN_2
#define DIG16_GPIO_Port GPIOD
#define ICE40_CRST_Pin GPIO_PIN_6
#define ICE40_CRST_GPIO_Port GPIOB
#define ICE40_CDONE_Pin GPIO_PIN_7
#define ICE40_CDONE_GPIO_Port GPIOB
#define TEST_Pin GPIO_PIN_3
#define TEST_GPIO_Port GPIOH



//Plugboard Pins
#define PLUGA_Pin GPIO_PIN_0
#define PLUGA_GPIO_Port GPIOA
#define PLUGB_Pin GPIO_PIN_1
#define PLUGB_GPIO_Port GPIOA
#define PLUGC_Pin GPIO_PIN_4
#define PLUGC_GPIO_Port GPIOA
#define PLUGD_Pin GPIO_PIN_5
#define PLUGD_GPIO_Port GPIOA
#define PLUGE_Pin GPIO_PIN_6
#define PLUGE_GPIO_Port GPIOA
#define PLUGF_Pin GPIO_PIN_7
#define PLUGF_GPIO_Port GPIOA
#define PLUGG_Pin GPIO_PIN_8
#define PLUGG_GPIO_Port GPIOA
#define PLUGH_Pin GPIO_PIN_0
#define PLUGH_GPIO_Port GPIOB
#define PLUGI_Pin GPIO_PIN_1
#define PLUGI_GPIO_Port GPIOB
#define PLUGJ_Pin GPIO_PIN_2
#define PLUGJ_GPIO_Port GPIOB
#define PLUGK_Pin GPIO_PIN_0
#define PLUGK_GPIO_Port GPIOC
#define PLUGL_Pin GPIO_PIN_1
#define PLUGL_GPIO_Port GPIOC
#define PLUGM_Pin GPIO_PIN_2
#define PLUGM_GPIO_Port GPIOC
#define PLUGN_Pin GPIO_PIN_3
#define PLUGN_GPIO_Port GPIOC
#define PLUGO_Pin GPIO_PIN_4
#define PLUGO_GPIO_Port GPIOC
#define PLUGP_Pin GPIO_PIN_5
#define PLUGP_GPIO_Port GPIOC
#define PLUGQ_Pin GPIO_PIN_6
#define PLUGQ_GPIO_Port GPIOC
#define PLUGR_Pin GPIO_PIN_7
#define PLUGR_GPIO_Port GPIOC
#define PLUGS_Pin GPIO_PIN_8
#define PLUGS_GPIO_Port GPIOC
#define PLUGT_Pin GPIO_PIN_9
#define PLUGT_GPIO_Port GPIOC
#define PLUGU_Pin GPIO_PIN_10
#define PLUGU_GPIO_Port GPIOC
#define PLUGV_Pin GPIO_PIN_11
#define PLUGV_GPIO_Port GPIOC
#define PLUGW_Pin GPIO_PIN_12
#define PLUGW_GPIO_Port GPIOC
#define PLUGX_Pin GPIO_PIN_13
#define PLUGX_GPIO_Port GPIOC
#define PLUGY_Pin GPIO_PIN_14
#define PLUGY_GPIO_Port GPIOC
#define PLUGZ_Pin GPIO_PIN_15
#define PLUGZ_GPIO_Port GPIOC

typedef struct {
	uint8_t R;
	uint8_t G;
	uint8_t B;
} RGB;

#define COLOR_BLACK (RGB){0,0,0}
#define COLOR_WHITE (RGB){128,128,128}
#define COLOR_RED (RGB){128,0,0}
#define COLOR_GREEN (RGB){0,128,0}
#define COLOR_BLUE (RGB){0,0,128}
#define COLOR_ORANGE (RGB){255,165,0}
#define COLOR_YELLOW (RGB){255,255,0}


#define SYNC 200
#define ESACPE 0xAB
#define SYNC_ESCAPED 0xAC
#define ESCAPE_ESCAPED 0xAD

#define UART_BUFFER_SIZE 256



//Uncomment for Commander badge (for Committee members)
//#define COMMANDER_BADGE

//Master Commander can send ring messages without a full ring
//#define MASTER_COMMANDER

//Hut 6 is for the display ring
//#define HUT6

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
