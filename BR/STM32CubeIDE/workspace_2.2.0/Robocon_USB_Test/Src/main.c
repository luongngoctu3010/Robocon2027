/* ============================================================
 * FILE: Src/main.c
 * MCU : STM32F767ZI
 *
 * UDP <-> UART BRIDGE
 *
 * Ubuntu -> STM32F767ZI -> Hoverboard
 * Hoverboard -> STM32F767ZI -> Ubuntu
 *
 * UDP command:
 *   STM32 listens on port 5005
 *   Packet size: 8 bytes
 *
 * UDP feedback:
 *   STM32 sends to Ubuntu port 5001
 *   Packet size: 18 bytes
 *
 * USART1:
 *   PA9  = USART1_TX
 *   PA10 = USART1_RX
 *   Baud = 115200
 *   Format = 8N1
 * ============================================================ */

#include "main.h"
#include "lwip.h"
#include "lwip/udp.h"
#include "lwip/pbuf.h"
#include "lwip/ip_addr.h"
#include "lwip/err.h"
#include "../Core/Inc/gpio.h"

#include <stdint.h>
#include <string.h>

/* ============================================================
 * CONFIGURATION
 * ============================================================ */

#define UDP_PORT                 5005U
#define UBUNTU_FEEDBACK_PORT     5001U

#define HOVERBOARD_BAUDRATE      115200U
#define UART_TIMEOUT             10U

#define COMMAND_PACKET_SIZE      8U
#define FEEDBACK_PACKET_SIZE     18U

#define START_FRAME              0xABCDU

/* ============================================================
 * HOVERBOARD COMMAND STRUCTURE
 *
 * 8 bytes:
 *   start
 *   steer
 *   speed
 *   checksum
 * ============================================================ */

typedef struct __attribute__((packed))
{
    uint16_t start;
    int16_t  steer;
    int16_t  speed;
    uint16_t checksum;

} SerialCommand;

_Static_assert(sizeof(SerialCommand) == COMMAND_PACKET_SIZE,
               "SerialCommand must be exactly 8 bytes");

/* ============================================================
 * HOVERBOARD FEEDBACK STRUCTURE
 *
 * 18 bytes:
 *   start
 *   cmd1
 *   cmd2
 *   speedR_meas
 *   speedL_meas
 *   batVoltage
 *   boardTemp
 *   cmdLed
 *   checksum
 * ============================================================ */

typedef struct __attribute__((packed))
{
    uint16_t start;

    int16_t cmd1;
    int16_t cmd2;

    int16_t speedR_meas;
    int16_t speedL_meas;

    int16_t batVoltage;
    int16_t boardTemp;

    uint16_t cmdLed;
    uint16_t checksum;

} SerialFeedback;

_Static_assert(sizeof(SerialFeedback) == FEEDBACK_PACKET_SIZE,
               "SerialFeedback must be exactly 18 bytes");

/* ============================================================
 * GLOBAL VARIABLES
 * ============================================================ */

UART_HandleTypeDef huart1;

static struct udp_pcb *udp_pcb_bridge = NULL;

/* ---------------- UDP statistics ---------------- */

volatile uint32_t udp_rx_packets = 0;
volatile uint32_t udp_rx_bytes   = 0;
volatile uint32_t udp_rx_errors = 0;

volatile uint32_t udp_tx_packets = 0;
volatile uint32_t udp_tx_errors  = 0;

/* ---------------- UART statistics ---------------- */

volatile uint32_t uart_tx_bytes   = 0;
volatile uint32_t uart_tx_errors  = 0;

volatile uint32_t uart_rx_bytes   = 0;
volatile uint32_t uart_rx_errors  = 0;

/* ---------------- UART interrupt reception ---------------- */

static uint8_t uart_rx_byte;

/* ---------------- Feedback parser ---------------- */

static uint8_t feedback_buffer[FEEDBACK_PACKET_SIZE];

static uint8_t feedback_index = 0;

/* ---------------- Feedback shared with main loop ---------------- */

static SerialFeedback feedback_pending;

static volatile uint8_t feedback_ready = 0;

/* ---------------- Ubuntu IP address ---------------- */

static ip_addr_t ubuntu_ip;

static volatile uint8_t ubuntu_ip_valid = 0;

/* ============================================================
 * FUNCTION PROTOTYPES
 * ============================================================ */

void SystemClock_Config(void);
void MPU_Config(void);
void Error_Handler(void);

static void USART1_Init(void);

static void UDP_Bridge_Init(void);

static void UDP_Receive_Callback(
    void *arg,
    struct udp_pcb *pcb,
    struct pbuf *p,
    const ip_addr_t *addr,
    u16_t port
);

static uint8_t ValidateCommand(uint8_t *data);

static uint8_t ValidateFeedback(uint8_t *data);

static void ProcessUARTByte(uint8_t data);

static void UDP_SendFeedback(void);

/* ============================================================
 * USART1 INITIALIZATION
 *
 * STM32F767ZI:
 *   PA9  = USART1_TX
 *   PA10 = USART1_RX
 *
 * Connection:
 *   F767 PA9  TX -> Hoverboard PA3 RX
 *   F767 PA10 RX <- Hoverboard PA2 TX
 *   F767 GND     -- Hoverboard GND
 * ============================================================ */

static void USART1_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_USART1_CLK_ENABLE();

    GPIO_InitStruct.Pin =
        GPIO_PIN_9 | GPIO_PIN_10;

    GPIO_InitStruct.Mode =
        GPIO_MODE_AF_PP;

    GPIO_InitStruct.Pull =
        GPIO_PULLUP;

    GPIO_InitStruct.Speed =
        GPIO_SPEED_FREQ_VERY_HIGH;

    GPIO_InitStruct.Alternate =
        GPIO_AF7_USART1;

    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    huart1.Instance = USART1;

    huart1.Init.BaudRate = HOVERBOARD_BAUDRATE;
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

/* ============================================================
 * VALIDATE COMMAND FROM UBUNTU
 *
 * Command checksum:
 *
 * checksum = start ^ steer ^ speed
 *
 * Only valid 8-byte commands are forwarded to Hoverboard.
 * The original 8 bytes are transmitted unchanged.
 * ============================================================ */

static uint8_t ValidateCommand(uint8_t *data)
{
    SerialCommand cmd;

    memcpy(&cmd, data, sizeof(SerialCommand));

    if (cmd.start != START_FRAME)
    {
        return 0U;
    }

    uint16_t calculated_checksum =
        cmd.start ^
        (uint16_t)cmd.steer ^
        (uint16_t)cmd.speed;

    if (calculated_checksum != cmd.checksum)
    {
        return 0U;
    }

    return 1U;
}

/* ============================================================
 * VALIDATE HOVERBOARD FEEDBACK
 *
 * Feedback checksum:
 *
 * start
 * ^ cmd1
 * ^ cmd2
 * ^ speedR_meas
 * ^ speedL_meas
 * ^ batVoltage
 * ^ boardTemp
 * ^ cmdLed
 * ============================================================ */

static uint8_t ValidateFeedback(uint8_t *data)
{
    SerialFeedback fb;

    memcpy(&fb, data, sizeof(SerialFeedback));

    if (fb.start != START_FRAME)
    {
        return 0U;
    }

    uint16_t calculated_checksum =
        fb.start ^
        (uint16_t)fb.cmd1 ^
        (uint16_t)fb.cmd2 ^
        (uint16_t)fb.speedR_meas ^
        (uint16_t)fb.speedL_meas ^
        (uint16_t)fb.batVoltage ^
        (uint16_t)fb.boardTemp ^
        fb.cmdLed;

    if (calculated_checksum != fb.checksum)
    {
        return 0U;
    }

    return 1U;
}

/* ============================================================
 * PROCESS UART RECEIVED BYTE
 *
 * Feedback frame starts with:
 *
 *   Byte 0 = 0xCD
 *   Byte 1 = 0xAB
 *
 * Total frame size = 18 bytes.
 *
 * If a frame is invalid, search again for the next possible
 * 0xCD 0xAB sequence instead of discarding all remaining data.
 * ============================================================ */

static void ProcessUARTByte(uint8_t data)
{
    /* ----------------------------------------
     * Find first byte: 0xCD
     * ---------------------------------------- */

    if (feedback_index == 0U)
    {
        if (data == 0xCDU)
        {
            feedback_buffer[0] = data;
            feedback_index = 1U;
        }

        return;
    }

    /* ----------------------------------------
     * Find second byte: 0xAB
     * ---------------------------------------- */

    if (feedback_index == 1U)
    {
        if (data == 0xABU)
        {
            feedback_buffer[1] = data;
            feedback_index = 2U;
        }
        else if (data == 0xCDU)
        {
            /* Possible new frame start */

            feedback_buffer[0] = data;
            feedback_index = 1U;
        }
        else
        {
            feedback_index = 0U;
        }

        return;
    }

    /* ----------------------------------------
     * Receive remaining bytes
     * ---------------------------------------- */

    feedback_buffer[feedback_index] = data;
    feedback_index++;

    /* ----------------------------------------
     * Complete 18-byte frame
     * ---------------------------------------- */

    if (feedback_index >= FEEDBACK_PACKET_SIZE)
    {
        if (ValidateFeedback(feedback_buffer))
        {
            SerialFeedback temp_feedback;

            memcpy(
                &temp_feedback,
                feedback_buffer,
                sizeof(SerialFeedback)
            );

            /*
             * This function runs inside UART interrupt.
             * Main loop copies this structure with IRQ disabled.
             */

            feedback_pending = temp_feedback;

            feedback_ready = 1U;

            feedback_index = 0U;
        }
        else
        {
            /*
             * Invalid frame:
             * Search for another 0xCD 0xAB sequence
             * inside the received data.
             */

            uint8_t found = 0U;

            for (uint8_t i = 1U;
                 i < (FEEDBACK_PACKET_SIZE - 1U);
                 i++)
            {
                if ((feedback_buffer[i] == 0xCDU) &&
                    (feedback_buffer[i + 1U] == 0xABU))
                {
                    uint8_t remaining =
                        FEEDBACK_PACKET_SIZE - i;

                    memmove(
                        feedback_buffer,
                        &feedback_buffer[i],
                        remaining
                    );

                    feedback_index = remaining;

                    found = 1U;

                    break;
                }
            }

            if (found == 0U)
            {
                /*
                 * Preserve the last byte if it is 0xCD.
                 * It may be the first byte of the next frame.
                 */

                if (feedback_buffer[FEEDBACK_PACKET_SIZE - 1U] == 0xCDU)
                {
                    feedback_buffer[0] = 0xCDU;
                    feedback_index = 1U;
                }
                else
                {
                    feedback_index = 0U;
                }
            }
        }
    }
}

/* ============================================================
 * UART RECEIVE COMPLETE CALLBACK
 *
 * Receive one byte at a time using interrupt.
 * ============================================================ */

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1)
    {
        uart_rx_bytes++;

        ProcessUARTByte(uart_rx_byte);

        /* Receive next byte */

        if (HAL_UART_Receive_IT(
                &huart1,
                &uart_rx_byte,
                1U
            ) != HAL_OK)
        {
            uart_rx_errors++;
        }
    }
}

/* ============================================================
 * UART ERROR CALLBACK
 *
 * Restart UART reception after an error.
 * ============================================================ */

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1)
    {
        uart_rx_errors++;

        HAL_UART_AbortReceive(&huart1);

        feedback_index = 0U;

        if (HAL_UART_Receive_IT(
                &huart1,
                &uart_rx_byte,
                1U
            ) != HAL_OK)
        {
            uart_rx_errors++;
        }
    }
}

/* ============================================================
 * UDP RECEIVE CALLBACK
 *
 * Ubuntu -> STM32F767ZI
 *
 * UDP destination port = 5005
 * Payload size = 8 bytes
 *
 * Validate command and forward original bytes to USART1.
 * ============================================================ */

static void UDP_Receive_Callback(
    void *arg,
    struct udp_pcb *pcb,
    struct pbuf *p,
    const ip_addr_t *addr,
    u16_t port
)
{
    uint8_t command_buffer[COMMAND_PACKET_SIZE];

    (void)arg;
    (void)pcb;
    (void)port;

    if (p == NULL)
    {
        return;
    }

    /* Check packet size */

    if (p->tot_len != COMMAND_PACKET_SIZE)
    {
        udp_rx_errors++;

        pbuf_free(p);

        return;
    }

    /*
     * Copy the complete UDP payload.
     * This also works if the pbuf contains multiple blocks.
     */

    u16_t copied = pbuf_copy_partial(
        p,
        command_buffer,
        COMMAND_PACKET_SIZE,
        0U
    );

    if (copied != COMMAND_PACKET_SIZE)
    {
        udp_rx_errors++;

        pbuf_free(p);

        return;
    }

    /* Validate command */

    if (ValidateCommand(command_buffer) == 0U)
    {
        udp_rx_errors++;

        pbuf_free(p);

        return;
    }

    /*
     * Remember Ubuntu IP.
     * Feedback will be sent to this IP address.
     */

    ip_addr_copy(ubuntu_ip, *addr);

    ubuntu_ip_valid = 1U;

    udp_rx_packets++;

    udp_rx_bytes += COMMAND_PACKET_SIZE;

    /*
     * Forward original 8-byte command to Hoverboard.
     * Do not modify the command data.
     */

    if (HAL_UART_Transmit(
            &huart1,
            command_buffer,
            COMMAND_PACKET_SIZE,
            UART_TIMEOUT
        ) == HAL_OK)
    {
        uart_tx_bytes += COMMAND_PACKET_SIZE;
    }
    else
    {
        uart_tx_errors++;
    }

    pbuf_free(p);
}

/* ============================================================
 * UDP BRIDGE INITIALIZATION
 *
 * Listen on UDP port 5005.
 * ============================================================ */

static void UDP_Bridge_Init(void)
{
    err_t err;

    udp_pcb_bridge = udp_new();

    if (udp_pcb_bridge == NULL)
    {
        Error_Handler();
    }

    err = udp_bind(
        udp_pcb_bridge,
        IP_ADDR_ANY,
        UDP_PORT
    );

    if (err != ERR_OK)
    {
        udp_remove(udp_pcb_bridge);

        udp_pcb_bridge = NULL;

        Error_Handler();
    }

    udp_recv(
        udp_pcb_bridge,
        UDP_Receive_Callback,
        NULL
    );
}

/* ============================================================
 * SEND HOVERBOARD FEEDBACK TO UBUNTU
 *
 * STM32F767ZI -> Ubuntu
 *
 * Destination UDP port = 5001
 * Payload size = 18 bytes
 *
 * Only validated feedback frames are sent.
 * ============================================================ */

static void UDP_SendFeedback(void)
{
    SerialFeedback local_feedback;

    uint32_t primask;

    struct pbuf *p;

    err_t err;

    /* Check if new feedback is available */

    if (feedback_ready == 0U)
    {
        return;
    }

    /* Check if Ubuntu IP is known */

    if (ubuntu_ip_valid == 0U)
    {
        return;
    }

    if (udp_pcb_bridge == NULL)
    {
        return;
    }

    /*
     * Copy feedback atomically.
     * Prevent UART interrupt from modifying the structure
     * while main loop is copying it.
     */

    primask = __get_PRIMASK();

    __disable_irq();

    memcpy(
        &local_feedback,
        &feedback_pending,
        sizeof(SerialFeedback)
    );

    feedback_ready = 0U;

    if (primask == 0U)
    {
        __enable_irq();
    }

    /* Allocate LwIP packet buffer */

    p = pbuf_alloc(
        PBUF_TRANSPORT,
        sizeof(SerialFeedback),
        PBUF_RAM
    );

    if (p == NULL)
    {
        udp_tx_errors++;

        return;
    }

    /* Copy feedback into UDP packet */

    memcpy(
        p->payload,
        &local_feedback,
        sizeof(SerialFeedback)
    );

    /* Send feedback to Ubuntu port 5001 */

    err = udp_sendto(
        udp_pcb_bridge,
        p,
        &ubuntu_ip,
        UBUNTU_FEEDBACK_PORT
    );

    if (err == ERR_OK)
    {
        udp_tx_packets++;
    }
    else
    {
        udp_tx_errors++;
    }

    pbuf_free(p);
}

/* ============================================================
 * MAIN
 * ============================================================ */

int main(void)
{
    /*
     * MPU configuration must be called before HAL_Init(),
     * matching the CubeMX-generated STM32F767 project.
     */

    MPU_Config();

    HAL_Init();

    SystemClock_Config();

    MX_GPIO_Init();

    /* Initialize USART1 */

    USART1_Init();

    /*
     * Start UART receive interrupt before LwIP operation.
     */

    if (HAL_UART_Receive_IT(
            &huart1,
            &uart_rx_byte,
            1U
        ) != HAL_OK)
    {
        Error_Handler();
    }

    /* Initialize Ethernet and LwIP */

    MX_LWIP_Init();

    /* Initialize UDP bridge */

    UDP_Bridge_Init();

    /* Main loop */

    while (1)
    {
        /*
         * Process Ethernet and UDP packets.
         */

        MX_LWIP_Process();

        /*
         * Send validated UART feedback to Ubuntu.
         */

        UDP_SendFeedback();
    }
}

/* ============================================================
 * SYSTEM CLOCK CONFIGURATION
 *
 * HSI = 16 MHz
 * SYSCLK = 216 MHz
 * ============================================================ */

void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};

    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    __HAL_RCC_PWR_CLK_ENABLE();

    HAL_PWR_EnableBkUpAccess();

    __HAL_PWR_VOLTAGESCALING_CONFIG(
        PWR_REGULATOR_VOLTAGE_SCALE1
    );

    RCC_OscInitStruct.OscillatorType =
        RCC_OSCILLATORTYPE_HSI;

    RCC_OscInitStruct.HSIState =
        RCC_HSI_ON;

    RCC_OscInitStruct.HSICalibrationValue =
        RCC_HSICALIBRATION_DEFAULT;

    RCC_OscInitStruct.PLL.PLLState =
        RCC_PLL_ON;

    RCC_OscInitStruct.PLL.PLLSource =
        RCC_PLLSOURCE_HSI;

    RCC_OscInitStruct.PLL.PLLM = 8;

    RCC_OscInitStruct.PLL.PLLN = 216;

    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;

    RCC_OscInitStruct.PLL.PLLQ = 2;

    RCC_OscInitStruct.PLL.PLLR = 2;

    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }

    if (HAL_PWREx_EnableOverDrive() != HAL_OK)
    {
        Error_Handler();
    }

    RCC_ClkInitStruct.ClockType =
        RCC_CLOCKTYPE_HCLK |
        RCC_CLOCKTYPE_SYSCLK |
        RCC_CLOCKTYPE_PCLK1 |
        RCC_CLOCKTYPE_PCLK2;

    RCC_ClkInitStruct.SYSCLKSource =
        RCC_SYSCLKSOURCE_PLLCLK;

    RCC_ClkInitStruct.AHBCLKDivider =
        RCC_SYSCLK_DIV1;

    RCC_ClkInitStruct.APB1CLKDivider =
        RCC_HCLK_DIV4;

    RCC_ClkInitStruct.APB2CLKDivider =
        RCC_HCLK_DIV2;

    if (HAL_RCC_ClockConfig(
            &RCC_ClkInitStruct,
            FLASH_LATENCY_7
        ) != HAL_OK)
    {
        Error_Handler();
    }
}

/* ============================================================
 * MPU CONFIGURATION
 *
 * Copied from the CubeMX-generated configuration.
 * ============================================================ */

void MPU_Config(void)
{
    MPU_Region_InitTypeDef MPU_InitStruct = {0};

    /* Disable the MPU */

    HAL_MPU_Disable();

    MPU_InitStruct.Enable = MPU_REGION_ENABLE;

    MPU_InitStruct.Number = MPU_REGION_NUMBER0;

    MPU_InitStruct.BaseAddress = 0x0;

    MPU_InitStruct.Size = MPU_REGION_SIZE_4GB;

    MPU_InitStruct.SubRegionDisable = 0x87;

    MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;

    MPU_InitStruct.AccessPermission = MPU_REGION_NO_ACCESS;

    MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;

    MPU_InitStruct.IsShareable = MPU_ACCESS_SHAREABLE;

    MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;

    MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;

    HAL_MPU_ConfigRegion(&MPU_InitStruct);

    HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);
}

/* ============================================================
 * ERROR HANDLER
 * ============================================================ */

void Error_Handler(void)
{
    __disable_irq();

    while (1)
    {
    }
}

/* ============================================================
 * ASSERT FAILED
 * ============================================================ */

#ifdef USE_FULL_ASSERT

void assert_failed(uint8_t *file, uint32_t line)
{
    (void)file;

    (void)line;

    while (1)
    {
    }
}

#endif
