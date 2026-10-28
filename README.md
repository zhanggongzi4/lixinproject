# ADC+DMA 阶段

基于 STM32F411CEUx 和 FreeRTOS 的学习工程，当前处于 ADC+DMA 阶段。

## 分支说明

- 分支：`adc-dma-task1-buffer`
- 本分支持续记录 ADC+DMA 开发进度，各阶段通过标签保存。
- 工程目录：`07_freeRTOS_uart1/`
- 任务代码：`07_freeRTOS_uart1/Core/Src/freertos.c`

## 阶段记录

| 标签 | 阶段内容 |
| --- | --- |
| [stage-00-adc-dma-init](https://github.com/zhanggongzi4/lixinproject/tree/stage-00-adc-dma-init) | ADC1、DMA2 Stream0、USART1 初始化；默认任务打印 `hello world`，尚未申请 buffer 或启动 ADC DMA。 |
| [stage-02-adc-dma](https://github.com/zhanggongzi4/lixinproject/tree/stage-02-adc-dma) | 第一个任务申请 buffer，启动 ADC DMA 传输，并添加完成及错误回调；用户已在开发板验证 DMA 传输和进入中断。 |
| [stage-03-queue-test](https://github.com/zhanggongzi4/lixinproject/tree/stage-03-queue-test) | 新增 FreeRTOS 队列创建、发送和接收测试：在同一个任务中发送数值 `123`，再接收并打印返回值及数据。 |
| [stage-04-queue-from-isr](https://github.com/zhanggongzi4/lixinproject/tree/stage-04-queue-from-isr) | 在中断中发送队列：ADC DMA 完成回调通过 `xQueueSendFromISR()` 发送完成标记 `0xA1`，任务尝试接收。 |

本次直接从初始化阶段保存到 DMA 传输及中断阶段，未单独保存仅完成 buffer 申请的版本，因此没有 `stage-01-buffer` 标签。

## 当前代码状态

- `StartDefaultTask()` 在启动 DMA 前使用 `xQueueCreate(10, 4)` 创建全局队列，容量为 10、每项 4 字节。
- DMA 完成回调通过 `xQueueSendFromISR()` 发送 `DMA_CPLT_INT`（`0xA1`）；若唤醒更高优先级任务，当前代码通过 `taskYIELD()` 请求 PendSV，在中断退出后进行任务切换。
- 任务仅调用一次 `xQueueReceive(..., 0)`，不阻塞等待；若接收成功，消息为 `161`（`0xA1`）。若回调尚未发送，接收会失败，接收变量仍为初始值 `255`，后续循环不会再次接收。
- `StartDefaultTask()` 使用 `malloc()` 申请 `buffer1`、`buffer2`，每个 buffer 包含一个 `uint32_t` 元素。
- 调用 `HAL_ADC_Start_DMA(&hadc1, buffer1, BUFFER_SIZE)`，当前仅使用 `buffer1` 接收数据，尚未实现双缓冲切换。
- ADC 为单次转换、DMA 为 Normal 模式，任务启动时调用一次采集；循环打印 buffer 不表示持续重新采样。
- 完成回调和错误回调中保留当前调试用 `printf()`；后续可改为中断通知任务，由任务处理和打印。
- 用户文件中的两个回调当前仍带 `__weak`，与 HAL 默认弱实现并存，建议后续改为普通函数以明确覆盖默认回调。
- 本阶段保留学习代码现状，尚未修复单次非阻塞接收、中断内打印、用户弱回调及任务失败分支直接 `return` 等审查问题。

各标签保存对应阶段的学习代码。当前在中断中发送队列阶段仅进行源码检查，本次归档未重新编译或进行硬件测试。

## 打开工程

使用 Keil MDK 打开 `07_freeRTOS_uart1/MDK-ARM/07_freeRTOS_uart1.uvprojx`。
STM32CubeMX 配置文件为 `07_freeRTOS_uart1/07_freeRTOS_uart1.ioc`。
