#include "hoverboard.h"

#include "lwip/udp.h"
#include "lwip/pbuf.h"
#include "lwip/ip_addr.h"

#include <stdint.h>
#include <string.h>


/* ==========================================
   CONFIGURATION
   ========================================== */

#define HOVERBOARD_UDP_PORT       5000

#define HOVERBOARD_START          0xABCD

#define HOVERBOARD_PACKET_SIZE    8

#define HOVERBOARD_TIMEOUT_MS     200

#define HOVERBOARD_UART_PERIOD    20

#define HOVERBOARD_UART_TIMEOUT   10


/* ==========================================
   UART HANDLE
   ========================================== */

extern UART_HandleTypeDef huart1;


/* ==========================================
   VARIABLES
   ========================================== */

static struct udp_pcb *udp_control_pcb = NULL;

static uint8_t hoverboard_rx_packet[8];

static volatile uint8_t packet_received = 0;

static uint32_t last_udp_tick = 0;

static uint32_t last_uart_tick = 0;


/* ==========================================
   READ LITTLE ENDIAN
   ========================================== */

static uint16_t Read_U16_LE(const uint8_t *data)
{
    return
        (uint16_t)data[0] |
        ((uint16_t)data[1] << 8);
}


/* ==========================================
   VALIDATE PACKET
   ========================================== */

static uint8_t Validate_Packet(const uint8_t *data)
{
    uint16_t start;
    uint16_t steer;
    uint16_t speed;
    uint16_t checksum;

    start = Read_U16_LE(&data[0]);

    steer = Read_U16_LE(&data[2]);

    speed = Read_U16_LE(&data[4]);

    checksum = Read_U16_LE(&data[6]);

    if (start != HOVERBOARD_START)
    {
        return 0;
    }

    if ((uint16_t)(start ^ steer ^ speed) != checksum)
    {
        return 0;
    }

    return 1;
}


/* ==========================================
   UDP RECEIVE CALLBACK
   ========================================== */

static void UDP_Receive_Callback(
    void *arg,
    struct udp_pcb *pcb,
    struct pbuf *p,
    const ip_addr_t *addr,
    u16_t port
)
{
    uint8_t temp[HOVERBOARD_PACKET_SIZE];

    (void)arg;
    (void)pcb;
    (void)addr;
    (void)port;

    if (p == NULL)
    {
        return;
    }

    if (p->tot_len == HOVERBOARD_PACKET_SIZE)
    {
        if (pbuf_copy_partial(
                p,
                temp,
                HOVERBOARD_PACKET_SIZE,
                0
            ) == HOVERBOARD_PACKET_SIZE)
        {
            if (Validate_Packet(temp))
            {
                memcpy(
                    hoverboard_rx_packet,
                    temp,
                    HOVERBOARD_PACKET_SIZE
                );

                last_udp_tick = HAL_GetTick();

                packet_received = 1;
            }
        }
    }

    pbuf_free(p);
}


/* ==========================================
   UDP SERVER INIT
   ========================================== */

void Hoverboard_Init(void)
{
    udp_control_pcb = udp_new();

    if (udp_control_pcb == NULL)
    {
        Error_Handler();
    }

    if (udp_bind(
            udp_control_pcb,
            IP_ADDR_ANY,
            HOVERBOARD_UDP_PORT
        ) != ERR_OK)
    {
        udp_remove(udp_control_pcb);

        udp_control_pcb = NULL;

        Error_Handler();
    }

    udp_recv(
        udp_control_pcb,
        UDP_Receive_Callback,
        NULL
    );

    memset(
        hoverboard_rx_packet,
        0,
        sizeof(hoverboard_rx_packet)
    );

    packet_received = 0;

    last_udp_tick = HAL_GetTick();

    last_uart_tick = HAL_GetTick();
}


/* ==========================================
   SEND STOP COMMAND
   ========================================== */

static void Hoverboard_Send_Stop(void)
{
    uint8_t stop_packet[8] =
    {
        0xCD, 0xAB,
        0x00, 0x00,
        0x00, 0x00,
        0xCD, 0xAB
    };

    HAL_UART_Transmit(
        &huart1,
        stop_packet,
        sizeof(stop_packet),
        HOVERBOARD_UART_TIMEOUT
    );
}


/* ==========================================
   SEND COMMAND TO FRONT HOVERBOARD
   ========================================== */

static void Hoverboard_Send_Command(void)
{
    HAL_UART_Transmit(
        &huart1,
        hoverboard_rx_packet,
        HOVERBOARD_PACKET_SIZE,
        HOVERBOARD_UART_TIMEOUT
    );
}


/* ==========================================
   MAIN PROCESS
   ========================================== */

void Hoverboard_Process(void)
{
    uint32_t now = HAL_GetTick();

    if ((now - last_uart_tick) < HOVERBOARD_UART_PERIOD)
    {
        return;
    }

    last_uart_tick = now;

    /*
       Watchdog:
       Neu mat UDP qua 200 ms -> STOP
    */

    if ((!packet_received) ||
        ((now - last_udp_tick) > HOVERBOARD_TIMEOUT_MS))
    {
        Hoverboard_Send_Stop();

        return;
    }

    /*
       Forward UDP packet to USART1
    */

    Hoverboard_Send_Command();
}
