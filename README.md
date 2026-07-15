## 📋 项目更新日志

该项目循序渐进，不断规范工程结构，主要用于日常学习，核心内容是 PID 控制的学习代码与思路分享。

| 日期 | 项目 | 更新内容 |
|------|------|----------|
| 2026/07/15 | `test_ff` | ① 学习了simulink，仿真出较优的前馈PID参数 |
| 2026/07/13 | `test_freertos` | ① 新增前馈PID + 模糊PID控制器<br>② 裸机代码整体移植至 FreeRTOS |
| 2026/07/11 | `test_engine` | ① 重构完整工程结构<br>② 封装 PID 模块和 VOFA 调试输出 |
| 2026/07/10 | `test` | ① 上传电机基础测试代码<br>② 配置 VSCode 开发环境 |

以下图片有关前馈PID

<p align="center">
  <img width="600" alt="未加前馈" src="https://github.com/user-attachments/assets/986e1637-2c1e-41b6-8d3e-61c5e9b755c1" />
</p>
<p align="center"><b>图一：未加前馈</b> — 仅使用传统 PID 控制，存在明显的滞后与超调</p>

<p align="center">
  <img width="600" alt="加前馈" src="https://github.com/user-attachments/assets/d0ffcb9e-8303-458a-8d8e-bfee842e04dd" />
</p>
<p align="center"><b>图二：加前馈</b> — 引入前馈 PID 后，响应速度明显提升，跟随误差大幅减小</p>

<p align="center">
  <img width="600" alt="Simulink仿真" src="https://github.com/user-attachments/assets/c31af293-6d62-4df7-9066-d5125f57fd8d" />
</p>
<p align="center"><b>图三：Simulink 仿真</b> — 通过 Simulink 整定出较优的前馈 PID 参数</p>



