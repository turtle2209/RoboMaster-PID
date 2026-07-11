#ifndef __VOFA_PID_H
#define __VOFA_PID_H

#include "usart.h"

void Vofa_Init(void); //初始化VOFA接收串口参数
void Vofa_Output(void); //输出VOFA接收的角度值到PID计算中


#endif