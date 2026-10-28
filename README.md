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

本次直接从初始化阶段保存到 DMA 传输及中断阶段，未单独保存仅完成 buffer 申请的版本，因此没有 `stage-01-buffer` 标签。

## 当前代码状态

- `StartDefaultTask()` 使用 `malloc()` 申请 `buffer1`、`buffer2`，每个 buffer 包含一个 `uint32_t` 元素。
- 调用 `HAL_ADC_Start_DMA(&hadc1, buffer1, BUFFER_SIZE)`，当前仅使用 `buffer1` 接收数据，尚未实现双缓冲切换。
- ADC 为单次转换、DMA 为 Normal 模式，任务启动时调用一次采集；循环打印 buffer 不表示持续重新采样。
- 完成回调和错误回调中保留当前调试用 `printf()`；后续可改为中断通知任务，由任务处理和打印。
- 用户文件中的两个回调当前仍带 `__weak`，与 HAL 默认弱实现并存，建议后续改为普通函数以明确覆盖默认回调。

本标签保存当前学习代码及用户反馈的板上结果，本次归档未重新编译或进行硬件测试。

## 打开工程

使用 Keil MDK 打开 `07_freeRTOS_uart1/MDK-ARM/07_freeRTOS_uart1.uvprojx`。
STM32CubeMX 配置文件为 `07_freeRTOS_uart1/07_freeRTOS_uart1.ioc`。
