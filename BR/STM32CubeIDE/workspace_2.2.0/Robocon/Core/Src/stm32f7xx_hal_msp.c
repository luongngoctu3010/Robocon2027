/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file         stm32f7xx_hal_msp.c
  * @brief        MSP Initialization and De-Initialization
  *               STM32F767ZIT6 - USART1 PA9/PA10
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN Define */

/* USER CODE END Define */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN Macro */

/* USER CODE END Macro */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* External functions --------------------------------------------------------*/
/* USER CODE BEGIN ExternalFunctions */

/* USER CODE END ExternalFunctions */

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */
/**
  * Initializes the Global MSP.
  */
void HAL_MspInit(void)
{

  /* USER CODE BEGIN MspInit 0 */

  /* USER CODE END MspInit 0 */

  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_RCC_SYSCFG_CLK_ENABLE();

  /* System interrupt init*/

  /* USER CODE BEGIN MspInit 1 */

  /* USER CODE END MspInit 1 */
}

/* USER CODE BEGIN 1 */
/* USER CODE BEGIN 1 */

/**
 * @brief Ethernet MSP Initialization
 * @param heth: ETH handle
 *
 * NUCLEO-F767ZI - LAN8742A - RMII
 *
 * PA1  - ETH_REF_CLK
 * PA2  - ETH_MDIO
 * PA7  - ETH_CRS_DV
 *
 * PC1  - ETH_MDC
 * PC4  - ETH_RXD0
 * PC5  - ETH_RXD1
 *
 * PG11 - ETH_TX_EN
 * PG13 - ETH_TXD0
 * PB13 - ETH_TXD1
 */
void HAL_ETH_MspInit(ETH_HandleTypeDef *heth)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    if (heth->Instance == ETH)
    {
        /* Enable Ethernet peripheral clock */
        __HAL_RCC_ETH_CLK_ENABLE();

        /* Enable GPIO clocks */
        __HAL_RCC_GPIOA_CLK_ENABLE();
        __HAL_RCC_GPIOB_CLK_ENABLE();
        __HAL_RCC_GPIOC_CLK_ENABLE();
        __HAL_RCC_GPIOG_CLK_ENABLE();

        /* Configure PA1, PA2, PA7 */
        GPIO_InitStruct.Pin =
                GPIO_PIN_1 |
                GPIO_PIN_2 |
                GPIO_PIN_7;

        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF11_ETH;

        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        /* Configure PB13: ETH_TXD1 */
        GPIO_InitStruct.Pin = GPIO_PIN_13;
        GPIO_InitStruct.Alternate = GPIO_AF11_ETH;

        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

        /* Configure PC1, PC4, PC5 */
        GPIO_InitStruct.Pin =
                GPIO_PIN_1 |
                GPIO_PIN_4 |
                GPIO_PIN_5;

        GPIO_InitStruct.Alternate = GPIO_AF11_ETH;

        HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

        /* Configure PG11, PG13 */
        GPIO_InitStruct.Pin =
                GPIO_PIN_11 |
                GPIO_PIN_13;

        GPIO_InitStruct.Alternate = GPIO_AF11_ETH;

        HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);
    }
}

/* USER CODE END 1 */
/* USER CODE END 1 */
