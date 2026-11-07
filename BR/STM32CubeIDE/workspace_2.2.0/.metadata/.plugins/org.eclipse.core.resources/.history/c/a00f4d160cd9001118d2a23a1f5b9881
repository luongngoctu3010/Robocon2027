/* USER CODE BEGIN Header */
/*
 * STM32F767ZIT6
 *
 * Ubuntu miniPC
 *      |
 *      | Ethernet UDP
 *      v
 * STM32F767ZIT6
 *      |
 *      | USART1 - PA9 TX / PA10 RX
 *      v
 * Hoverboard STM32F103
 *
 * Phase 1:
 * - Chi dieu khien bo Hoverboard cau truoc.
 * - USART1: 115200 baud, 8N1.
 * - UDP Server: xu ly trong hoverboard.c.
 * - Khong dinh nghia Hoverboard_Process() tai day.
 */
/* USER CODE END Header */

#include "main.h"
#include "lwip.h"
#include "hoverboard.h"

/* Private variables ----------------------------------------------- */

UART_HandleTypeDef huart1;

/* Private function prototypes ------------------------------------- */

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART1_UART_Init(void);

/* ================================================================= */
/* MAIN                                                              */
/* ================================================================= */

int main(void)
{
    /* Reset of all peripherals, Initializes the Flash interface
       and the Systick. */
    HAL_Init();

    /* Configure system clock: 216 MHz */
    SystemClock_Config();

    /* Initialize GPIO */
    MX_GPIO_Init();

    /* Initialize USART1 */
    MX_USART1_UART_Init();

    /* Initialize Ethernet and LwIP */
    MX_LWIP_Init();

    /* Initialize Hoverboard UDP/UART communication */
    Hoverboard_Init();

    /* Main loop */
    while (1)
    {
        /* Process Ethernet packets and LwIP timers */
        MX_LWIP_Process();

        /* Process UDP command and forward to Hoverboard via USART1 */
        Hoverboard_Process();
    }
}

/* ================================================================= */
/* SYSTEM CLOCK CONFIGURATION                                        */
/* ================================================================= */

void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    /* Enable PWR clock */
    __HAL_RCC_PWR_CLK_ENABLE();

    /* Configure voltage scaling */
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    /*
     * HSI = 16 MHz
     *
     * PLLM = 8
     * PLLN = 216
     * PLLP = 2
     * PLLQ = 9
     *
     * SYSCLK = 16 / 8 * 216 / 2
     *        = 216 MHz
     */

    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;

    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
    RCC_OscInitStruct.PLL.PLLM = 8;
    RCC_OscInitStruct.PLL.PLLN = 216;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
    RCC_OscInitStruct.PLL.PLLQ = 9;

    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }

    /* Enable OverDrive for 216 MHz */
    if (HAL_PWREx_EnableOverDrive() != HAL_OK)
    {
        Error_Handler();
    }

    /* Configure CPU, AHB and APB clocks */

    RCC_ClkInitStruct.ClockType =
        RCC_CLOCKTYPE_HCLK |
        RCC_CLOCKTYPE_SYSCLK |
        RCC_CLOCKTYPE_PCLK1 |
        RCC_CLOCKTYPE_PCLK2;

    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;

    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;

    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;

    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_7) != HAL_OK)
    {
        Error_Handler();
    }
}

/* ================================================================= */
/* GPIO INITIALIZATION                                               */
/* ================================================================= */

static void MX_GPIO_Init(void)
{
    /* Enable GPIO clocks */

    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_GPIOE_CLK_ENABLE();
    __HAL_RCC_GPIOF_CLK_ENABLE();
    __HAL_RCC_GPIOG_CLK_ENABLE();
    __HAL_RCC_GPIOH_CLK_ENABLE();

    /*
     * USART1 GPIO configuration is handled by:
     * HAL_UART_MspInit() in stm32f7xx_hal_msp.c
     *
     * PA9  = USART1_TX
     * PA10 = USART1_RX
     */
}

/* ================================================================= */
/* USART1 INITIALIZATION                                             */
/* ================================================================= */

static void MX_USART1_UART_Init(void)
{
    huart1.Instance = USART1;

    huart1.Init.BaudRate = 115200;

    huart1.Init.WordLength = UART_WORDLENGTH_8B;

    huart1.Init.StopBits = UART_STOPBITS_1;

    huart1.Init.Parity = UART_PARITY_NONE;

    huart1.Init.Mode = UART_MODE_TX_RX;

    huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;

    huart1.Init.OverSampling = UART_OVERSAMPLING_16;

    if (HAL_UART_Init(&huart1) != HAL_OK)
    {
        Error_Handler();
    }
}

/* ================================================================= */
/* ERROR HANDLER                                                     */
/* ================================================================= */

void Error_Handler(void)
{
    __disable_irq();

    while (1)
    {
        /* Stay here if an error occurs */
    }
}

#ifdef USE_FULL_ASSERT

void assert_failed(uint8_t *file, uint32_t line)
{
    /*
     * User can add implementation to report:
     * file and line number where assert_param error occurred.
     */
}

#endif
