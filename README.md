# ADC+DMA 阶段

基于 STM32F411CEUx 和 FreeRTOS 的学习工程，当前处于 ADC+DMA 阶段。

## 分支说明

- 分支：`adc-dma-task1-buffer`
- 本分支代码对应：第一个任务申请 buffer。
- 工程目录：`07_freeRTOS_uart1/`
- 任务代码：`07_freeRTOS_uart1/Core/Src/freertos.c`

当前快照已包含 ADC1、DMA2 Stream0 和 USART1 初始化配置；默认任务每秒打印 `hello world`。第一个任务申请 buffer 为本分支的任务目标，当前源码尚未实现 buffer 申请及 ADC DMA 采集启动。

## 打开工程

使用 Keil MDK 打开 `07_freeRTOS_uart1/MDK-ARM/07_freeRTOS_uart1.uvprojx`。
STM32CubeMX 配置文件为 `07_freeRTOS_uart1/07_freeRTOS_uart1.ioc`。
