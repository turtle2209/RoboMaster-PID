#include "Vofa_pid.h"
#include "PID.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// 接收缓冲区和索引（在 Vofa.c 中定义）
static uint8_t rx_buf[32];
static uint8_t rx_index = 0;
static uint8_t rx_byte = 0;

// 不在这里定义 Angle（在 PID.c 中已有定义）

void Vofa_Init(void)
{
    // 初始化VOFA接收串口参数
    HAL_UART_Receive_IT(&huart1, &rx_byte, 1);
}
    
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1)
    {
        
        if (rx_byte == '\n' || rx_byte == '\r')
        {
            rx_buf[rx_index] = '\0';
            if (rx_index > 0)
            {
                // 支持命令格式：P=23.0 或 I=0.05 或 D=23.0（首字母区分大小写）
                char *p = (char *)rx_buf;
                char cmd = p[0];
                char *num = p + 1;
                if (*num == '=') num++;
                float val = atof(num);
                switch (cmd)
                {
                    case 'P':
                        PID.kp = val;
                        break;
                    case 'I':
                        PID.ki = val;
                        break;
                    case 'D':
                        PID.kd = val;
                        break;
                    default:
                        break;
                }
            }
            rx_index = 0;
        }
        else
        {
            if (rx_index < 31)
                rx_buf[rx_index++] = rx_byte;
        }

        HAL_UART_Receive_IT(&huart1, &rx_byte, 1);
    }
}


//VOFA输出
void Vofa_Output(void)
{
    char buf[64]; 
    sprintf(buf, "%.1f,%.1f\n", Angle.target_angle, Angle.current_angle);
    HAL_UART_Transmit(&huart1, (uint8_t*)buf, strlen(buf), 5);

}