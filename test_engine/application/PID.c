#include "PID.h"

PID_TypeDef PID;
ECD_TypeDef ECD;
Output_TypeDef Output;
Angle_TypeDef Angle;


void PID_Init(void)
{
    //初始化PID参数
    PID.kp = K_p;
    PID.ki = K_i;
    PID.kd = K_d;

    //初始化编码器参数
    ECD.target_ecd = 0;
    ECD.current_ecd = 0;
    ECD.error = 0;
    ECD.last_error = 0;
    ECD.error_sum = 0;

    //初始化角度参数
    Angle.target_angle = 0.0f;
    Angle.current_angle = 0.0f;
    Angle.debug_rx_value = 0.0f;

    //初始化电机输出参数
    Output.Output_1 = 0.0f;
    Output.Output_2 = 0.0f;
    Output.Output_3 = 0.0f;
    Output.Output_4 = 0.0f;

}


void PID_Calculate_send(ECD_TypeDef *ecd)
{
    //计算编码器误差
    ecd->error = ecd->target_ecd - ecd->current_ecd;

    //过零点处理
    int16_t error = ecd->target_ecd - ecd->current_ecd;
    if (error > 4096) error -= 8192;
    if (error < -4096) error += 8192;

    //积分误差累加
    ecd->error_sum = ecd->error_sum + error;
    if (ecd->error_sum > 10000) ecd->error_sum = 10000;
    if (ecd->error_sum < -10000) ecd->error_sum = -10000;

    //计算PID输出
    Output.Output_1 = PID.kp * ecd->error + PID.ki * ecd->error_sum + PID.kd * (ecd->error - ecd->last_error);

    //限制输出范围
    if (Output.Output_1 > 10000) Output.Output_1 = 10000;
    if (Output.Output_1 < -10000) Output.Output_1 = -10000;

    //将输出转换为电机控制电流
    int16_t control_current = (int16_t)Output.Output_1;

    //发送控制电流到电机
    CAN_cmd_gimbal(control_current, 0, 0, 0);
    ecd->last_error = ecd->error; //前一次误差记录
}



void PID_SetTarget(ECD_TypeDef *ecd, int16_t target)
{
    //设置目标编码器数值
    ecd->target_ecd = target;
}



int16_t PID_GetCurrent(ECD_TypeDef *ecd)
{
    //读取当前编码器的值
    ecd->current_ecd = get_yaw_gimbal_motor_measure_point()->ecd;//读取当前编码器值

    //编码器值换算角度值,以便用于VOFA输出更直观的角度显示
    Angle.current_angle = (float)ecd->current_ecd * 360.0f / 8192.0f;
    Angle.target_angle = (float)ecd->target_ecd * 360.0f / 8192.0f;


    return ecd->current_ecd;

}