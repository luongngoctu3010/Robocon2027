#include "robot_control.h"

#include <string.h>
#include <math.h>

#include "lwip/udp.h"
#include "lwip/pbuf.h"
#include "lwip/ip_addr.h"

// ==========================================
// UART HANDLES FROM CUBEMX
// ==========================================

extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart6;


// ==========================================
// CONFIGURATION
// ==========================================

#define UDP_PORT               5000

#define WHEEL_DIAMETER_MM      160.0f

#define N_MOT_MAX              1000.0f

#define UDP_TIMEOUT_MS         200

#define CONTROL_PERIOD_MS      20

#define SERIAL_START_FRAME    0xABCD


// ==========================================
// HOVERBOARD COMMAND
// ==========================================

typedef struct __attribute__((packed))
{
    uint16_t start;
    int16_t steer;
    int16_t speed;
    uint16_t checksum;

} HoverboardCommand;

_Static_assert(sizeof(HoverboardCommand) == 8,
               "Hoverboard command must be 8 bytes");


// ==========================================
// HOVERBOARD FEEDBACK
// ENABLE_ODOMETRY = 1
// ==========================================

typedef struct __attribute__((packed))
{
    uint16_t start;

    int16_t cmd1;
    int16_t cmd2;

    int16_t speedR_meas;
    int16_t speedL_meas;

    int16_t wheelR_cnt;
    int16_t wheelL_cnt;

    int16_t batVoltage;
    int16_t boardTemp;

    uint16_t cmdLed;
    uint16_t checksum;

} HoverboardFeedback;

_Static_assert(sizeof(HoverboardFeedback) == 22,
               "Hoverboard feedback must be 22 bytes");


// ==========================================
// UDP DATA
// FL FR RL RR
// ==========================================

static volatile int16_t wheel[4] = {0};

static volatile uint32_t last_udp_tick = 0;

static struct udp_pcb *udp_pcb = NULL;


// ==========================================
// UART RX
// ==========================================

static uint8_t uart1_rx_byte;
static uint8_t uart6_rx_byte;

static uint8_t uart1_buffer[22];
static uint8_t uart6_buffer[22];

static uint8_t uart1_index = 0;
static uint8_t uart6_index = 0;

volatile HoverboardFeedback front_feedback;
volatile HoverboardFeedback rear_feedback;


// ==========================================
// LIMIT
// ==========================================

static int16_t LimitCommand(int32_t value)
{
    if (value > 1000)
        value = 1000;

    if (value < -1000)
        value = -1000;

    return (int16_t)value;
}


// ==========================================
// MM/S TO MOTOR RPM COMMAND
// ==========================================

static int16_t SpeedToCommand(int16_t speed_mm_s)
{
    float rpm;

    rpm = ((float)speed_mm_s * 60.0f) /
          (3.14159265f * WHEEL_DIAMETER_MM);

    float command = rpm * 1000.0f / N_MOT_MAX;

    return LimitCommand((int32_t)command);
}


// ==========================================
// HOVERBOARD CHECKSUM
// ==========================================

static uint16_t CommandChecksum(HoverboardCommand *cmd)
{
    return (uint16_t)(
        cmd->start ^
        (uint16_t)cmd->steer ^
        (uint16_t)cmd->speed
    );
}


// ==========================================
// SEND COMMAND TO ONE HOVERBOARD
// ==========================================

static void SendHoverboard(
    UART_HandleTypeDef *huart,
    int16_t left,
    int16_t right
)
{
    HoverboardCommand cmd;

    cmd.start = SERIAL_START_FRAME;

    // TANK_STEERING:
    // steer = left motor
    // speed = right motor

    cmd.steer = LimitCommand(left);
    cmd.speed = LimitCommand(right);

    cmd.checksum = CommandChecksum(&cmd);

    HAL_UART_Transmit(
        huart,
        (uint8_t *)&cmd,
        sizeof(cmd),
        5
    );
}


// ==========================================
// UDP CALLBACK
// ==========================================

static void UDP_Receive_Callback(
    void *arg,
    struct udp_pcb *pcb,
    struct pbuf *p,
    const ip_addr_t *addr,
    u16_t port
)
{
    (void)arg;
    (void)pcb;
    (void)addr;
    (void)port;

    if (p == NULL)
        return;

    if (p->tot_len == 8)
    {
        uint8_t data[8];

        if (pbuf_copy_partial(p, data, 8, 0) == 8)
        {
            int16_t values[4];

            for (int i = 0; i < 4; i++)
            {
                uint16_t raw =
                    (uint16_t)data[2 * i] |
                    ((uint16_t)data[2 * i + 1] << 8);

                values[i] = (int16_t)raw;
            }

            __disable_irq();

            wheel[0] = values[0];
            wheel[1] = values[1];
            wheel[2] = values[2];
            wheel[3] = values[3];

            last_udp_tick = HAL_GetTick();

            __enable_irq();
        }
    }

    pbuf_free(p);
}


// ==========================================
// UDP INITIALIZATION
// ==========================================

void RobotControl_UdpInit(void)
{
    udp_pcb = udp_new();

    if (udp_pcb == NULL)
        return;

    if (udp_bind(
        udp_pcb,
        IP_ADDR_ANY,
        UDP_PORT
    ) != ERR_OK)
    {
        udp_remove(udp_pcb);
        udp_pcb = NULL;
        return;
    }

    udp_recv(
        udp_pcb,
        UDP_Receive_Callback,
        NULL
    );
}


// ==========================================
// FEEDBACK PARSER
// ==========================================

static void ParseFeedbackByte(
    uint8_t data,
    uint8_t *buffer,
    uint8_t *index,
    volatile HoverboardFeedback *feedback
)
{
    if (*index == 0)
    {
        if (data == 0xCD)
        {
            buffer[0] = data;
            *index = 1;
        }

        return;
    }

    if (*index == 1)
    {
        if (data == 0xAB)
        {
            buffer[1] = data;
            *index = 2;
        }
        else if (data != 0xCD)
        {
            *index = 0;
        }

        return;
    }

    buffer[*index] = data;
    (*index)++;

    if (*index >= sizeof(HoverboardFeedback))
    {
        HoverboardFeedback temp;

        memcpy(
            &temp,
            buffer,
            sizeof(temp)
        );

        uint16_t checksum =
            temp.start ^
            (uint16_t)temp.cmd1 ^
            (uint16_t)temp.cmd2 ^
            (uint16_t)temp.speedR_meas ^
            (uint16_t)temp.speedL_meas ^
            (uint16_t)temp.wheelR_cnt ^
            (uint16_t)temp.wheelL_cnt ^
            (uint16_t)temp.batVoltage ^
            (uint16_t)temp.boardTemp ^
            temp.cmdLed;

        if (temp.start == SERIAL_START_FRAME &&
            temp.checksum == checksum)
        {
            memcpy(
                (void *)feedback,
                &temp,
                sizeof(temp)
            );
        }

        *index = 0;
    }
}


// ==========================================
// UART RX CALLBACK HANDLER
// CALL FROM HAL_UART_RxCpltCallback()
// ==========================================

void RobotControl_UART_RxCplt(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1)
    {
        ParseFeedbackByte(
            uart1_rx_byte,
            uart1_buffer,
            &uart1_index,
            &front_feedback
        );

        HAL_UART_Receive_IT(
            &huart1,
            &uart1_rx_byte,
            1
        );
    }

    else if (huart->Instance == USART6)
    {
        ParseFeedbackByte(
            uart6_rx_byte,
            uart6_buffer,
            &uart6_index,
            &rear_feedback
        );

        HAL_UART_Receive_IT(
            &huart6,
            &uart6_rx_byte,
            1
        );
    }
}


// ==========================================
// INITIALIZATION
// ==========================================

void RobotControl_Init(void)
{
    last_udp_tick = HAL_GetTick();

    HAL_UART_Receive_IT(
        &huart1,
        &uart1_rx_byte,
        1
    );

    HAL_UART_Receive_IT(
        &huart6,
        &uart6_rx_byte,
        1
    );
}


// ==========================================
// MAIN CONTROL PROCESS
// ==========================================

void RobotControl_Process(void)
{
    static uint32_t last_control_tick = 0;

    uint32_t now = HAL_GetTick();

    if (now - last_control_tick < CONTROL_PERIOD_MS)
        return;

    last_control_tick = now;

    int16_t fl, fr, rl, rr;

    __disable_irq();

    fl = wheel[0];
    fr = wheel[1];

    rl = wheel[2];
    rr = wheel[3];

    uint32_t last_packet = last_udp_tick;

    __enable_irq();

    // UDP watchdog

    if (now - last_packet > UDP_TIMEOUT_MS)
    {
        fl = 0;
        fr = 0;
        rl = 0;
        rr = 0;
    }

    // Convert mm/s to motor RPM command

    int16_t cmd_fl = SpeedToCommand(fl);
    int16_t cmd_fr = SpeedToCommand(fr);

    int16_t cmd_rl = SpeedToCommand(rl);
    int16_t cmd_rr = SpeedToCommand(rr);

    // FRONT HOVERBOARD
    // UART1: PA9 / PA10

    SendHoverboard(
        &huart1,
        cmd_fl,
        cmd_fr
    );

    // REAR HOVERBOARD
    // UART6: PC6 / PC7

    SendHoverboard(
        &huart6,
        cmd_rl,
        cmd_rr
    );
}
