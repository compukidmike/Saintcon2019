/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32l4xx_it.c
  * @brief   Interrupt Service Routines.
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

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32l4xx_it.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
 
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */
extern void FLASH_PageErase(uint32_t Page, uint32_t Banks);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/
extern PCD_HandleTypeDef hpcd_USB_FS;
extern I2C_HandleTypeDef hi2c1;
extern I2C_HandleTypeDef hi2c2;
extern DMA_HandleTypeDef hdma_spi1_tx;
extern SPI_HandleTypeDef hspi1;
extern SPI_HandleTypeDef hspi2;
extern TIM_HandleTypeDef htim16;
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
/* USER CODE BEGIN EV */
extern volatile uint8_t spiDone;
/* USER CODE END EV */

/******************************************************************************/
/*           Cortex-M4 Processor Interruption and Exception Handlers          */ 
/******************************************************************************/
/**
  * @brief This function handles Non maskable interrupt.
  */
void NMI_Handler(void)
{
  /* USER CODE BEGIN NonMaskableInt_IRQn 0 */
	uint32_t corruptedflashaddress;

	  /* Check if NMI is due to flash ECCD (error detection) */
	  if (__HAL_FLASH_GET_FLAG(FLASH_FLAG_ECCD))
	  {
	    /* Compute corrupted flash address */
	    corruptedflashaddress = FLASH_BASE + (FLASH->ECCR & FLASH_ECCR_ADDR_ECC);

	#if defined(FLASH_OPTR_BFB2)
	    /* Add bank size to corrupteflashaddress if fail bank is bank 2 */
	    if (READ_BIT(FLASH->ECCR, FLASH_ECCR_BK_ECC) == FLASH_ECCR_BK_ECC)
	    {
	      corruptedflashaddress += FLASH_BANK_SIZE;
	    }
	#endif

	    /* Check if corrupted flash address is in eeprom emulation pages */
	    if ((corruptedflashaddress >= START_PAGE_ADDRESS) || (corruptedflashaddress <= END_EEPROM_ADDRESS))
	    {
	      /* Delete the corrupted flash address */
	      if (EE_DeleteCorruptedFlashAddress(corruptedflashaddress) == EE_OK)
	      {
	        /* Resume execution if deletion succeeds */
	        return;
	      }
	    }
	  }

	  /* Go to infinite loop when NMI occurs in case:
	     - ECCD is raised in eeprom emulation flash pages but corrupted flash address deletion fails
	     - ECCD is raised out of eeprom emulation flash pages
	     - no ECCD is raised */
	    while (1)
	  {
	  }

  /* USER CODE END NonMaskableInt_IRQn 0 */
  /* USER CODE BEGIN NonMaskableInt_IRQn 1 */

  /* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler(void)
{
  /* USER CODE BEGIN HardFault_IRQn 0 */
	/***************************************************************************/
	  /* When a power down or external reset occurs during a Flash Write operation, */
	  /* the first line at 0x0 may be corrupted at 0x0 (low occurence).          */
	  /* In this case the Flash content is restored.                             */
	  /*    address : flash bank1 base address                                   */
	  /*    data : first flash line (64 bits). This variable must be updated after each build.*/
	  /*    sram2address : sram2 start address to copy and restore first flash page */
	  /***************************************************************************/
	  uint32_t address = FLASH_BASE;
	  uint64_t data = 0x08002df120001bc0;
	  uint32_t sram2address = 0x20030000;
	  uint32_t page = 0;
	  uint32_t banks = FLASH_BANK_1;
	  uint32_t element = 0U;

	  if ((*(__IO uint32_t*)address == 0x0) && (*(__IO uint32_t*)(address+4) == 0x0))
	  {
	    /* Authorize the FLASH Registers access */
	    FLASH->KEYR = FLASH_KEY1;
	    FLASH->KEYR = FLASH_KEY2;

	    /* Save first flash page in SRAM2 */
	    for ( element=2; element < FLASH_PAGE_SIZE; element++)
	    {
	      *(__IO uint32_t*)(sram2address+(element*4)) = *(__IO uint32_t*)(address+(element*4));
	    }

	    /* Restore the first flash line in SRAM2 to its correct value */
	    *(__IO uint32_t*)sram2address = (uint32_t)data;
	    *(__IO uint32_t*)(sram2address+4) = (uint32_t)(data >> 32);

	    /* Clear FLASH all errors */
	    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_ALL_ERRORS);

	    /* Erase first flash page */
	#if defined (STM32L471xx) || defined (STM32L475xx) || defined (STM32L476xx) || defined (STM32L485xx) || defined (STM32L486xx) || \
	    defined (STM32L496xx) || defined (STM32L4A6xx) || \
	    defined (STM32L4R5xx) || defined (STM32L4R7xx) || defined (STM32L4R9xx) || defined (STM32L4S5xx) || defined (STM32L4S7xx) || defined (STM32L4S9xx)
	#if defined (STM32L4R5xx) || defined (STM32L4R7xx) || defined (STM32L4R9xx) || defined (STM32L4S5xx) || defined (STM32L4S7xx) || defined (STM32L4S9xx)
	    if(READ_BIT(FLASH->OPTR, FLASH_OPTR_DBANK) == RESET)
	    {
	      CLEAR_BIT(FLASH->CR, FLASH_CR_BKER);
	    }
	    else
	#endif
	    {
	      if((banks & FLASH_BANK_1) != RESET)
	      {
	        CLEAR_BIT(FLASH->CR, FLASH_CR_BKER);
	      }
	      else
	      {
	        SET_BIT(FLASH->CR, FLASH_CR_BKER);
	      }
	    }
	#endif

	    /* Proceed to erase the page */
	    MODIFY_REG(FLASH->CR, FLASH_CR_PNB, (page << POSITION_VAL(FLASH_CR_PNB)));
	    SET_BIT(FLASH->CR, FLASH_CR_PER);
	    SET_BIT(FLASH->CR, FLASH_CR_STRT);

	    /* Wait for last operation to be completed */
	    while(__HAL_FLASH_GET_FLAG(FLASH_FLAG_BSY)) { }
	    /* If the erase operation is completed, disable the PER Bit */
	    CLEAR_BIT(FLASH->CR, (FLASH_CR_PER | FLASH_CR_PNB));

	    /* Restore first flash page in flash from SRAM2*/
	    for ( element=0; element < FLASH_PAGE_SIZE; element++)
	    {
	      /* Wait for last operation to be completed */
	      while(__HAL_FLASH_GET_FLAG(FLASH_FLAG_BSY)) { }

	      /* Set PG bit */
	      SET_BIT(FLASH->CR, FLASH_CR_PG);

	      /* Write in flash */
	      *(__IO uint32_t*)(address+(element*4)) = *(__IO uint32_t*)(sram2address+(element*4));
	    }

	    /* System reset */
	    NVIC_SystemReset();
	  }
  /* USER CODE END HardFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_HardFault_IRQn 0 */
    /* USER CODE END W1_HardFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Memory management fault.
  */
void MemManage_Handler(void)
{
  /* USER CODE BEGIN MemoryManagement_IRQn 0 */

  /* USER CODE END MemoryManagement_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_MemoryManagement_IRQn 0 */
    /* USER CODE END W1_MemoryManagement_IRQn 0 */
  }
}

/**
  * @brief This function handles Prefetch fault, memory access fault.
  */
void BusFault_Handler(void)
{
  /* USER CODE BEGIN BusFault_IRQn 0 */

  /* USER CODE END BusFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_BusFault_IRQn 0 */
    /* USER CODE END W1_BusFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Undefined instruction or illegal state.
  */
void UsageFault_Handler(void)
{
  /* USER CODE BEGIN UsageFault_IRQn 0 */

  /* USER CODE END UsageFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_UsageFault_IRQn 0 */
    /* USER CODE END W1_UsageFault_IRQn 0 */
  }
}

/**
  * @brief This function handles System service call via SWI instruction.
  */
void SVC_Handler(void)
{
  /* USER CODE BEGIN SVCall_IRQn 0 */

  /* USER CODE END SVCall_IRQn 0 */
  /* USER CODE BEGIN SVCall_IRQn 1 */

  /* USER CODE END SVCall_IRQn 1 */
}

/**
  * @brief This function handles Debug monitor.
  */
void DebugMon_Handler(void)
{
  /* USER CODE BEGIN DebugMonitor_IRQn 0 */

  /* USER CODE END DebugMonitor_IRQn 0 */
  /* USER CODE BEGIN DebugMonitor_IRQn 1 */

  /* USER CODE END DebugMonitor_IRQn 1 */
}

/**
  * @brief This function handles Pendable request for system service.
  */
void PendSV_Handler(void)
{
  /* USER CODE BEGIN PendSV_IRQn 0 */

  /* USER CODE END PendSV_IRQn 0 */
  /* USER CODE BEGIN PendSV_IRQn 1 */

  /* USER CODE END PendSV_IRQn 1 */
}

/**
  * @brief This function handles System tick timer.
  */
void SysTick_Handler(void)
{
  /* USER CODE BEGIN SysTick_IRQn 0 */

  /* USER CODE END SysTick_IRQn 0 */
  HAL_IncTick();
  /* USER CODE BEGIN SysTick_IRQn 1 */

  /* USER CODE END SysTick_IRQn 1 */
}

/******************************************************************************/
/* STM32L4xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32l4xx.s).                    */
/******************************************************************************/

/**
  * @brief This function handles DMA1 channel3 global interrupt.
  */
void DMA1_Channel3_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Channel3_IRQn 0 */
	spiDone = 1;
  /* USER CODE END DMA1_Channel3_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_spi1_tx);
  /* USER CODE BEGIN DMA1_Channel3_IRQn 1 */

  /* USER CODE END DMA1_Channel3_IRQn 1 */
}

/**
  * @brief This function handles TIM1 update interrupt and TIM16 global interrupt.
  */
void TIM1_UP_TIM16_IRQHandler(void)
{
  /* USER CODE BEGIN TIM1_UP_TIM16_IRQn 0 */

  /* USER CODE END TIM1_UP_TIM16_IRQn 0 */
  HAL_TIM_IRQHandler(&htim16);
  /* USER CODE BEGIN TIM1_UP_TIM16_IRQn 1 */

  /* USER CODE END TIM1_UP_TIM16_IRQn 1 */
}

/**
  * @brief This function handles I2C1 event interrupt.
  */
void I2C1_EV_IRQHandler(void)
{
  /* USER CODE BEGIN I2C1_EV_IRQn 0 */

  /* USER CODE END I2C1_EV_IRQn 0 */
  HAL_I2C_EV_IRQHandler(&hi2c1);
  /* USER CODE BEGIN I2C1_EV_IRQn 1 */

  /* USER CODE END I2C1_EV_IRQn 1 */
}

/**
  * @brief This function handles I2C1 error interrupt.
  */
void I2C1_ER_IRQHandler(void)
{
  /* USER CODE BEGIN I2C1_ER_IRQn 0 */

  /* USER CODE END I2C1_ER_IRQn 0 */
  HAL_I2C_ER_IRQHandler(&hi2c1);
  /* USER CODE BEGIN I2C1_ER_IRQn 1 */

  /* USER CODE END I2C1_ER_IRQn 1 */
}

/**
  * @brief This function handles I2C2 event interrupt.
  */
void I2C2_EV_IRQHandler(void)
{
  /* USER CODE BEGIN I2C2_EV_IRQn 0 */

  /* USER CODE END I2C2_EV_IRQn 0 */
  HAL_I2C_EV_IRQHandler(&hi2c2);
  /* USER CODE BEGIN I2C2_EV_IRQn 1 */

  /* USER CODE END I2C2_EV_IRQn 1 */
}

/**
  * @brief This function handles I2C2 error interrupt.
  */
void I2C2_ER_IRQHandler(void)
{
  /* USER CODE BEGIN I2C2_ER_IRQn 0 */

  /* USER CODE END I2C2_ER_IRQn 0 */
  HAL_I2C_ER_IRQHandler(&hi2c2);
  /* USER CODE BEGIN I2C2_ER_IRQn 1 */

  /* USER CODE END I2C2_ER_IRQn 1 */
}

/**
  * @brief This function handles SPI1 global interrupt.
  */
void SPI1_IRQHandler(void)
{
  /* USER CODE BEGIN SPI1_IRQn 0 */

  /* USER CODE END SPI1_IRQn 0 */
  HAL_SPI_IRQHandler(&hspi1);
  /* USER CODE BEGIN SPI1_IRQn 1 */

  /* USER CODE END SPI1_IRQn 1 */
}

/**
  * @brief This function handles SPI2 global interrupt.
  */
void SPI2_IRQHandler(void)
{
  /* USER CODE BEGIN SPI2_IRQn 0 */

  /* USER CODE END SPI2_IRQn 0 */
  HAL_SPI_IRQHandler(&hspi2);
  /* USER CODE BEGIN SPI2_IRQn 1 */

  /* USER CODE END SPI2_IRQn 1 */
}

/**
  * @brief This function handles USART1 global interrupt.
  */
void USART1_IRQHandler(void)
{
  /* USER CODE BEGIN USART1_IRQn 0 */
  UART_Right_IRQ(&huart1);
  //return;
  /* USER CODE END USART1_IRQn 0 */
  HAL_UART_IRQHandler(&huart1);
  /* USER CODE BEGIN USART1_IRQn 1 */

  /* USER CODE END USART1_IRQn 1 */
}

/**
  * @brief This function handles USART2 global interrupt.
  */
void USART2_IRQHandler(void)
{
  /* USER CODE BEGIN USART2_IRQn 0 */
  UART_Left_IRQ(&huart2);
  return;
  /* USER CODE END USART2_IRQn 0 */
  HAL_UART_IRQHandler(&huart2);
  /* USER CODE BEGIN USART2_IRQn 1 */

  /* USER CODE END USART2_IRQn 1 */
}

/**
  * @brief This function handles EXTI line[15:10] interrupts.
  */
void EXTI15_10_IRQHandler(void)
{
  /* USER CODE BEGIN EXTI15_10_IRQn 0 */

  /* USER CODE END EXTI15_10_IRQn 0 */
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_15);
  /* USER CODE BEGIN EXTI15_10_IRQn 1 */

  /* USER CODE END EXTI15_10_IRQn 1 */
}

/**
  * @brief This function handles USB event interrupt through EXTI line 17.
  */
void USB_IRQHandler(void)
{
  /* USER CODE BEGIN USB_IRQn 0 */

  /* USER CODE END USB_IRQn 0 */
  HAL_PCD_IRQHandler(&hpcd_USB_FS);
  /* USER CODE BEGIN USB_IRQn 1 */

  /* USER CODE END USB_IRQn 1 */
}

/* USER CODE BEGIN 1 */

/**
  * @brief  This function handles Flash interrupt request.
  * @param  None
  * @retval None
  */
void FLASH_IRQHandler(void)
{
  HAL_FLASH_IRQHandler();
}

/**
  * @brief  This function handles PVD interrupt request.
  * @param  None
  * @retval None
  */
void PVD_PVM_IRQHandler(void)
{
  /* Loop inside the handler to prevent the Cortex from using the Flash,
     allowing the flash interface to finish any ongoing transfer. */
  while (__HAL_PWR_GET_FLAG(PWR_FLAG_PVDO) != RESET)
  {
  }
}

/* USER CODE END 1 */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
