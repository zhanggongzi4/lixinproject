# UART DMA 架构学习：完成环形缓冲区创建阶段

- 仓库：<https://github.com/zhanggongzi4/lixinproject>
- 分支：`uart_dma_arch_ope`
- 阶段标签：`uart-dma-stage-05-circular-buffer-create-saved`
- 阶段日期：2026-11-02
- Keil 工程：`MDK-ARM/BaseProject.uvprojx`
- 接收任务及回调：`BSP/USART/bsp_uart_driver.c`
- 解析任务：`APP/Uart_Parse/uart_parse_task.c`
- 环形缓冲区模块：`Middlewares/MID_circular_buffer/`

## 本阶段内容

环形缓冲区大小为 100 字节，结构体包含 `data`、`head` 和 `tail`。`create_empty_circular_buffer()` 申请内存，失败时返回 `NULL`，成功时将整个结构体清零并返回缓冲区指针。

已实现创建、初始化、判空、判满，以及 `insert_data()` 写入和 `get_data()` 读取。BSP 任务中包含写入数值 `15` 后读取的测试代码。本次保存用户重新保存后的源码。

BSP 任务创建缓冲区并打印判空、判满结果；创建失败时删除当前任务，避免继续打印成功。任务循环加入 `osDelay(1)`，使低优先级任务有机会运行。

头文件已统一为 `mid_circular_buffer.h`，定义 `data_type_t` 为 `uint8_t`，并补齐 Keil 头文件搜索路径。EasyLogger 的三个未实现信息接口返回空字符串，避免非 void 函数缺少返回值。

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
| `uart-dma-stage-05-circular-buffer-create-saved` | 完成缓冲区创建、初始化、空满判断、数据读写及对应测试代码 |

## 当前源码注意点

- 创建队列返回 `NULL` 表示失败；当前成功和失败日志的判断写反了。
- `queue_irc_rec_A` 当前声明为 `QueueSetHandle_t`；普通队列对应的句柄类型应为 `QueueHandle_t`。
- `buffer1`、`buffer2` 当前在头文件中定义；后续多个源文件包含该头文件时，应改为在头文件中 `extern` 声明，在一个 `.c` 文件中定义。
- 接收回调目前在中断中打印字符，没有把收到的数据发送到解析任务队列。解析任务中的队列接收代码仍然保留。

## 验证

2026-11-02 按用户要求直接上传已保存的源码。本次未重新编译，也未进行开发板运行测试。
