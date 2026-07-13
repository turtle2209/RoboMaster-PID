#ifndef __FUZZY_CONFIG_H
#define __FUZZY_CONFIG_H

#include "fuzzyPID.h"

/* ============================================================
   模糊PID配置文件 —— 你需要调的东西全在这里
   ============================================================ */

// ---------- 第1部分：隶属度函数参数 ----------
// 7个模糊集合，每个4个参数（三角形用前3个，第4个对齐占位）
// 格式：{a, b, c, 0}  三角形顶点=b，左右零点=a和c
static int mf_params[4 * qf_default] = {
    {-3, -3, -2, 0},   // NB: 负大
    {-3, -2, -1, 0},   // NM: 负中
    {-2, -1,  0, 0},   // NS: 负小
    {-1,  0,  1, 0},   // ZO: 零
    { 0,  1,  2, 0},   // PS: 正小
    { 1,  2,  3, 0},   // PM: 正中
    { 2,  3,  3, 0},   // PB: 正大
};

// ---------- 第2部分：模糊规则表 ----------
// 21行 × 7列：前7行=ΔKp，中7行=ΔKi，后7行=ΔKd
// 行 = 误差e的模糊集(NB→PB)，列 = 误差变化率de的模糊集(NB→PB)
static int rule_base[][qf_default] = {

    // --- ΔKp 规则表（行0-6）---
    {PB, PB, PM, PM, PS, ZO, ZO},
    {PB, PB, PM, PS, PS, ZO, NS},
    {PM, PM, PM, PS, ZO, NS, NS},
    {PM, PM, PS, ZO, NS, NM, NM},
    {PS, PS, ZO, NS, NS, NM, NM},
    {PS, ZO, NS, NM, NM, NM, NB},
    {ZO, ZO, NM, NM, NM, NB, NB},

    // --- ΔKi 规则表（行7-13）---
    {NB, NB, NM, NM, NS, ZO, ZO},
    {NB, NB, NM, NS, NS, ZO, ZO},
    {NB, NM, NS, NS, ZO, PS, PS},
    {NM, NM, NS, ZO, PS, PM, PM},
    {NM, NS, ZO, PS, PS, PM, PB},
    {ZO, ZO, PS, PS, PM, PB, PB},
    {ZO, ZO, PS, PM, PM, PB, PB},

    // --- ΔKd 规则表（行14-20）---
    {PS, NS, NB, NB, NB, NM, PS},
    {PS, NS, NB, NM, NM, NS, ZO},
    {ZO, NS, NM, NM, NS, NS, ZO},
    {ZO, NS, NS, NS, NS, NS, ZO},
    {ZO, ZO, ZO, ZO, ZO, ZO, ZO},
    {PB, PS, PS, PS, PS, PS, PB},
    {PB, PM, PM, PM, PS, PS, PB},
};

// ---------- 第3部分：你的系统参数（需要改的！）----------

// 基础PID参数 —— 用你现在调好的值
#define FUZZY_KP    23.0f
#define FUZZY_KI    0.05f
#define FUZZY_KD    23.0f

// ΔK的最大变化幅度 —— 决定了模糊PID的"调节力度"
// Kp最大浮动到 23±12=11~35，Ki浮动±0.02，Kd浮动±10
#define DELTA_KP_MAX    12.0f
#define DELTA_KI_MAX    0.02f
#define DELTA_KD_MAX    10.0f

// 误差归一化范围 —— 改成你编码器的量程！
// 编码器 0~8192，半圈是4096，所以最大误差≈4096
#define FUZZY_ERROR_MAX        4096.0f
#define FUZZY_DELTA_ERROR_MAX  4096.0f

// 积分限幅（对应你原来 error_sum 的 ±10000）
#define FUZZY_INTEGRAL_LIMIT   10000.0f

// 死区（误差小于这个值就不调了，防止抖动）
#define FUZZY_DEAD_ZONE        0.0f

// 前馈系数（你原来有前馈，保留）
#define FUZZY_FEED_FORWARD     0.0f

// 输出限幅（对应你原来的 ±10000）
#define FUZZY_OUTPUT_MAX    10000
#define FUZZY_OUTPUT_MID    0
#define FUZZY_OUTPUT_MIN   -10000

// ---------- 第4部分：模糊引擎配置（一般不改）----------
#define FUZZY_MF_TYPE   4    // 隶属度函数类型：4=三角形
#define FUZZY_FO_TYPE   1    // 模糊算子：1=代数积
#define FUZZY_DF_TYPE   0    // 解模糊方法：0=中心平均法
#define FUZZY_DELTA_K   2.0f // ΔK变化系数

#endif
