# UART DMA 架构学习：创建 BSP 层 UART 任务阶段

- 仓库：<https://github.com/zhanggongzi4/lixinproject>
- 分支：`uart_dma_arch_ope`
- 阶段标签：`uart-dma-stage-02-bsp-uart-task`
- 阶段日期：2026-10-30
- Keil 工程：`MDK-ARM/BaseProject.uvprojx`
- 任务源码：`APP/Uart_Parse/uart_parse_task.c`

## 本阶段内容

新增 `BSP/USART/bsp_uart_driver.c` 和 `bsp_uart_driver.h`，在 `MX_FREERTOS_Init()` 中通过 `osThreadNew()` 创建 BSP 层 UART 任务，入口为 `uart_driver_fuc()`，优先级为 Normal，栈大小为 512 字节。Keil 工程已加入 BSP 文件，头文件名称和工程路径保持一致。

UART 接收完成回调仍保留在 `Core/Src/usart.c`，使用 `xQueueSendFromISR()` 发送测试值 `0x12`；`uart_rec_A_func()` 创建队列并尝试接收、打印数据。当前 BSP 任务入口为空函数，尚未实现 UART DMA 接收和 buffer 切换；空任务直接返回存在运行风险，后续需补充任务循环或显式删除任务。

## 阶段记录

| 标签 | 阶段 |
| --- | --- |
| `uart-dma-stage-01-queue-create` | 创建队列 |
| `uart-dma-stage-02-bsp-uart-task` | 创建 BSP 层 UART 任务，加入队列收发测试代码 |

## 当前源码注意点

- 创建队列返回 `NULL` 表示失败；当前成功和失败日志的判断写反了。
- `queue_irc_rec_A` 当前声明为 `QueueSetHandle_t`；普通队列对应的句柄类型应为 `QueueHandle_t`。
- `buffer1`、`buffer2` 当前在头文件中定义；后续多个源文件包含该头文件时，应改为在头文件中 `extern` 声明，在一个 `.c` 文件中定义。

## 验证

使用 Keil ARMCC 5.06 update 5 完整重新编译：0 个错误、5 个警告，已生成目标文件。警告来自 `usart.c` 中未使用的 `ret` 变量，以及 EasyLogger 移植文件中缺少返回值和末尾换行。未进行开发板运行测试。
