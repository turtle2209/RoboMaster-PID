#include "stdint.h"
#include "CAN_receive.h"


#ifndef __PID_H
#define __PID_H

//pid参数
#define K_p 23.0f
#define K_i 0.05f
#define K_d 23.0f


//**************结构体***************//

//创建电机输出结构体
typedef struct
{
    float Output_1 ; //1号电机输出
    float Output_2 ; //2号电机输出
    float Output_3 ; //3号电机输出
    float Output_4 ; //4号电机输出
    ///可添加更多电机
}   Output_TypeDef;

//创建编码器结构体
typedef struct
{
    int16_t target_ecd ;  //目标编码器数值
    int16_t current_ecd ; //当前编码器数值
    int16_t error ; //编码器误差
    int16_t last_error ; //上一次编码器误差
    int16_t error_sum ; //编码器误差积分
}   ECD_TypeDef ;


//创建角度结构体
typedef struct
{
    float target_angle ;
    float current_angle ;
    float debug_rx_value ;
}   Angle_TypeDef ;

// 全局角度实例（在 Vofa.c 中定义）
extern Angle_TypeDef Angle;

// 全局编码器与输出实例（在 PID.c 中定义）
extern ECD_TypeDef ECD;
extern Output_TypeDef Output;

//创建pid结构体
typedef struct
{
    float kp; //比例系数
    float ki; //积分系数
    float kd; //微分系数
}   PID_TypeDef;

// 全局 PID 实例（在 PID.c 中定义）
extern PID_TypeDef PID;

//***************结构体****************//



//**************函数声明***************//

void PID_Init(void); //初始化pid参数
void PID_Calculate_send(ECD_TypeDef *ecd); //计算pid输出
void PID_SetTarget(ECD_TypeDef *ecd, int16_t target); //设置目标编码器数值

int16_t PID_GetCurrent(ECD_TypeDef *ecd); //获取当前编码器数值

//**************函数声明***************//


#endif

