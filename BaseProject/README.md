# UART DMA 架构学习：创建队列阶段

- 仓库：<https://github.com/zhanggongzi4/lixinproject>
- 分支：`uart_dma_arch_ope`
- 阶段标签：`uart-dma-stage-01-queue-create`
- 阶段日期：2026-10-29
- Keil 工程：`MDK-ARM/BaseProject.uvprojx`
- 任务源码：`APP/Uart_Parse/uart_parse_task.c`

## 本阶段内容

FreeRTOS 创建 `Task_uart_rec_A` 任务，入口为 `uart_rec_A_func()`。任务调用 `xQueueCreate(1, 4)`，申请一个容量为 1、每项 4 字节的队列，并通过日志观察创建结果，随后循环调用 `osDelay(1)`。

本次保存当前学习源码；阶段内容为创建队列，尚未实现队列发送、接收或 UART DMA 接收流程。

## 当前源码注意点

- 创建队列返回 `NULL` 表示失败；当前成功和失败日志的判断写反了。
- `queue_irc_rec_A` 当前声明为 `QueueSetHandle_t`；普通队列对应的句柄类型应为 `QueueHandle_t`。
- `buffer1`、`buffer2` 当前在头文件中定义；后续多个源文件包含该头文件时，应改为在头文件中 `extern` 声明，在一个 `.c` 文件中定义。

## 验证

HAL 头文件已改为通过 `stm32f4xx_hal.h` 引入。上传前使用 Keil ARMCC 5.06 update 5 构建：0 个错误、0 个警告，已生成目标文件。未进行开发板运行测试。
