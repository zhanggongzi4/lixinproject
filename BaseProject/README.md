# UART DMA 架构学习：完成环形缓冲区创建阶段

- 仓库：<https://github.com/zhanggongzi4/lixinproject>
- 分支：`uart_dma_arch_ope`
- 阶段标签：`uart-dma-stage-04-circular-buffer-create`
- 阶段日期：2026-10-31
- Keil 工程：`MDK-ARM/BaseProject.uvprojx`
- 接收任务及回调：`BSP/USART/bsp_uart_driver.c`
- 解析任务：`APP/Uart_Parse/uart_parse_task.c`
- 环形缓冲区模块：`Middlewares/MID_circular_buffer/`

## 本阶段内容

新增环形缓冲区模块及 Keil 工程分组。头文件定义 `CIRCULAR_BUFFER_SIZE = 100`，以及包含字节数组 `data`、写位置 `head` 和读位置 `tail` 的 `circular_buffer_t`；声明创建缓冲区、判空、判满和数据操作接口。

本阶段名称按学习进度记录为“完成环形缓冲区创建”。当前磁盘源码仅有结构体与接口声明，`mid_circular_buffer.c` 仍为空，尚未实现创建、判空、判满或读写函数。

## 已保留的 UART 接收代码

`uart_driver_fuc()` 首先调用 `HAL_UART_Receive_IT(&huart1, g_data_bufferA, 1)`，启动 USART1 单字节中断接收。`g_data_bufferA` 和 `g_data_bufferB` 各包含 1 个字节，使用 `flag_AB` 记录当前接收目标。

UART 接收完成回调已迁入 BSP。A 接收完成后打印 A 中的字符并启动 B 接收；B 接收完成后打印 B 中的字符并重新启动 A 接收。每次重新接收都会检查 HAL 返回值，失败时输出日志。

当前实现使用 UART 中断接收，尚未使用 DMA；此处的 A/B 切换通过回调重新启动接收实现。`Core/Src/usart.c` 中旧的测试回调已注释，避免重复定义。

## 阶段记录

| 标签 | 阶段 |
| --- | --- |
| `uart-dma-stage-01-queue-create` | 创建队列 |
| `uart-dma-stage-02-bsp-uart-task` | 创建 BSP 层 UART 任务，加入队列收发测试代码 |
| `uart-dma-stage-03-uart-ab-buffer` | UART 数据接收与 A/B buffer 切换 |
| `uart-dma-stage-04-circular-buffer-create` | 完成环形缓冲区创建阶段：新增模块文件、结构体及接口声明 |

## 当前源码注意点

- 环形缓冲区头文件实际名为 `mid_circular_buffer .h`（扩展名前有空格）；源码中的 `data_type_t` 尚未定义，两个 `insert_data` 声明参数类型不同，后续需明确类型与读写接口名称。
- 创建队列返回 `NULL` 表示失败；当前成功和失败日志的判断写反了。
- `queue_irc_rec_A` 当前声明为 `QueueSetHandle_t`；普通队列对应的句柄类型应为 `QueueHandle_t`。
- `buffer1`、`buffer2` 当前在头文件中定义；后续多个源文件包含该头文件时，应改为在头文件中 `extern` 声明，在一个 `.c` 文件中定义。
- BSP 接收任务启动 UART 接收后进入空循环，没有延时或阻塞；该任务优先级为 Normal，可能使优先级为 Low 的解析任务无法运行。
- 接收回调目前在中断中打印字符，没有把收到的数据发送到解析任务队列。解析任务中的队列接收代码仍然保留。

## 验证

本次按用户要求直接归档当前源码，未重新编译，也未进行开发板运行测试。
