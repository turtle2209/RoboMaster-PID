/* ============================================================
   文件名：fuzzy_PID.c
   功能：模糊PID控制器的初始化和计算
   日期：2026-07-13
   说明：
     1. 该文件实现了模糊PID控制器的初始化和计算功能。
     2. 使用了 GitHub 上的 fuzzy_pid 库。
     3. 控制器参数在 fuzzy_config.h 中定义。
     4.主要用于对模糊PID的学习以及在STM32上进行测试，方便后续在其他项目中使用。
   ============================================================ */



#include "fuzzy_PID.h"
#include "CAN_receive.h"  // CAN_cmd_gimbal()

/* ============================================================
   一行一行解释：
   ============================================================ */

// 全局变量：模糊PID控制器指针
// 用 static 是因为只有这个文件需要用它，不需要被外部看到
// 类比：你原来 PID.c 里的 PID、ECD、Output 也是全局变量
static struct PID *fuzzy_pid;


/* ========== 函数1：初始化 ========== */
void Fuzzy_PID_Init(void)
{
    /* --- 第1小步：把参数打包成数组 ---
       为什么要打包？因为 GitHub 的 fuzzy_pid_init() 函数
       接收的是一个 float 数组，不是一个个单独的变量。
       pid_params_count = 7，所以数组长度是 7。

       对照 fuzzyPID.h 第133行的注释：
       float params[pid_params_count] = {kp, ki, kd, integral_limit,
                                         dead_zone, feed_forward, linear_adaptive_kp};
    */
    float params[pid_params_count] = {
        FUZZY_KP,              // [0] 基础比例系数 —— 你原来 #define K_p 23.0f
        FUZZY_KI,              // [1] 基础积分系数 —— 你原来 #define K_i 0.05f
        FUZZY_KD,              // [2] 基础微分系数 —— 你原来 #define K_d 23.0f
        FUZZY_INTEGRAL_LIMIT,  // [3] 积分上限 —— 你原来写死的 10000
        FUZZY_DEAD_ZONE,       // [4] 死区 —— 误差多小就不调了
        FUZZY_FEED_FORWARD,    // [5] 前馈系数 —— 你原来有前馈，这里先填0
        1.0f                   // [6] 线性自适应Kp开关 —— 1.0=关，<1.0=开
    };

    /* --- 第2小步：调用 GitHub 的函数创建控制器 ---
       raw_fuzzy_pid_init 的参数非常多，但可以分为 4 组：

       第①组：PID基础参数（从 params 数组拆出来）
       第②组：误差归一化范围（告诉模糊引擎"你的误差大概是多大"）
       第③组：ΔK变化范围（告诉模糊引擎"参数最多能调多少"）
       第④组：模糊引擎配置（规则表、隶属度参数、用什么算法）
       第⑤组：输出限幅（和你原来 ±10000 一样的意思）
    */
    fuzzy_pid = raw_fuzzy_pid_init(
        // ------- 第①组：PID基础参数 -------
        params[0],  // kp      = 23.0  → 模糊PID的基础值
        params[1],  // ki      = 0.05  → 模糊PID会在这个基础上±ΔKi
        params[2],  // kd      = 23.0  → 模糊PID会在这个基础上±ΔKd
        params[3],  // integral_limit = 10000 → 积分上限
        params[4],  // dead_zone       = 0    → 死区
        params[5],  // feed_forward    = 0    → 前馈

        // ------- 第②组：误差归一化范围 -------
        // 为什么需要这两个值？
        // 三角形隶属度函数画在 [-3, 3] 上。
        // 但你的编码器误差范围是 [-4096, 4096]。
        // 所以需要做一个映射：error / error_max × 3
        // 比如误差=2048 → 2048/4096×3 = 1.5  → 对应到三角形上的1.5位置
        FUZZY_ERROR_MAX,         // 4096 —— 你编码器半圈的值
        FUZZY_DELTA_ERROR_MAX,   // 4096 —— 误差变化率的最大值

        // ------- 第③组：ΔK变化范围 -------
        // 这三个值决定了模糊PID的"调节力度"
        // ΔKp_max = 12：Kp 在 23±12 = [11, 35] 之间浮动
        // ΔKi_max = 0.02：Ki 在 0.05±0.02 = [0.03, 0.07] 之间浮动
        // ΔKd_max = 10：Kd 在 23±10 = [13, 33] 之间浮动
        // 调大 = 模糊PID更激进，调小 = 更保守
        DELTA_KP_MAX,
        DELTA_KI_MAX,
        DELTA_KD_MAX,

        // ------- 第④组：模糊引擎配置 -------
        FUZZY_MF_TYPE,   // 4 = 用三角形隶属度函数
        FUZZY_FO_TYPE,   // 1 = 用代数积模糊算子
        FUZZY_DF_TYPE,   // 0 = 用中心平均法解模糊
        mf_params,       // 7个三角形的 {a,b,c} 参数（在 fuzzy_config.h 里定义）
        rule_base,       // 21×7 规则表（在 fuzzy_config.h 里定义）

        // ------- 第⑤组：输出限幅 -------
        FUZZY_OUTPUT_MIN,   // -10000
        FUZZY_OUTPUT_MID,   // 0
        FUZZY_OUTPUT_MAX    // 10000
    );
    // 到这，fuzzy_pid 指针指向了一个配置好的模糊PID控制器
    // 以后每次调用 fuzzy_pid_control() 时传这个指针就行
}


/* ========== 函数2：控制计算 ========== */
void Fuzzy_PID_Calculate_Send(ECD_TypeDef *ecd)
{
    // --- 第1步：计算误差（和你原来一模一样）---
    ecd->error = ecd->target_ecd - ecd->current_ecd;

    // --- 第2步：过零点处理（和你原来一模一样）---
    // 编码器是环形的：0 和 8192 代表同一个位置
    // 如果从 8000 到 0（实际只差 192），误差写成 -8000 就错了
    if (ecd->error > 4096)  ecd->error -= 8192;
    if (ecd->error < -4096) ecd->error += 8192;

    // --- 第3步：调用模糊PID（⭐ 这行替代了你原来手写的PID公式）---
    // 原来你是这样写的：
    //   Output.Output_1 = PID.kp * error + PID.ki * error_sum + PID.kd * delta_error + u_ff;
    //
    // 现在只需要一行：把实际值和目标值传进去，函数内部自动：
    //   ① 算误差变化率 de
    //   ② 把 e 和 de 归一化到 [-3,3]
    //   ③ 查三角形隶属度
    //   ④ 查规则表
    //   ⑤ 加权平均解模糊 → ΔKp, ΔKi, ΔKd
    //   ⑥ 用 (Kp+ΔKp) 代替原来的 Kp 做PID计算
    //   ⑦ 返回输出值
    float real = (float)ecd->current_ecd;   // 实际值（编码器读数）
    float idea = (float)ecd->target_ecd;    // 目标值（想让它转到哪）
    float output = fuzzy_pid_control(real, idea, fuzzy_pid);
    //                                    ↑     ↑       ↑
    //                                 实际值 目标值  控制器指针

    // --- 第4步：输出限幅（和你原来一模一样）---
    if (output > FUZZY_OUTPUT_MAX)  output = FUZZY_OUTPUT_MAX;
    if (output < FUZZY_OUTPUT_MIN)  output = FUZZY_OUTPUT_MIN;

    // --- 第5步：通过CAN发给电机（和你原来一模一样）---
    int16_t control_current = (int16_t)output;
    CAN_cmd_gimbal(control_current, 0, 0, 0);

    // --- 第6步：记录上一次的值（和你原来一模一样）---
    ecd->last_error = ecd->error;
    ecd->last_target_ecd = ecd->target_ecd;
}
