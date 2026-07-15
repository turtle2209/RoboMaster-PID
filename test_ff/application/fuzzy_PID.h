#ifndef __FUZZY_PID_APP_H
#define __FUZZY_PID_APP_H

#include "fuzzyPID.h"        // 需要用到 struct PID 的定义
#include "PID.h"             // 需要用到 ECD_TypeDef 的定义
#include "fuzzy_config.h"    // 需要用到 FUZZY_KP 等宏

void Fuzzy_PID_Init(void);                      // 初始化（创建控制器）
void Fuzzy_PID_Calculate_Send(ECD_TypeDef *ecd); // 控制计算（替代原来的）

#endif
