# Understand 配置与分析步骤

本文记录在 SciTools Understand Offline（Build 1206）中分析 `07_freeRTOS_uart1/Core/Src/freertos.c` 的实际操作。工程使用 STM32F411、Keil ARMCC 5、HAL 和 FreeRTOS。

## 1. 打开工程

用 Understand 打开 `07_freeRTOS_uart1/07_freeRTOS_uart1.und`，然后选择 **Project → Configure Project**。

如果在新电脑上重新创建 Understand 工程，可以先导入 `07_freeRTOS_uart1/MDK-ARM/07_freeRTOS_uart1.uvprojx`，再按下面步骤核对配置。

## 2. 设置分析语言和文件范围

1. 在 **Languages** 页面勾选 **C/C++**，取消 **Assembly**。本次只需要分析 C 代码；此前把其他芯片的启动汇编文件一起分析，产生了大量无关错误。
2. 在 **Files** 页面保留 `Core/Src` 和 FreeRTOS 的相关源码。如果此前将整个 `Drivers` 目录设为递归监视，右键该目录，选择 **Remove Selected Files**。这只会从 Understand 的分析列表中移除文件，不会删除磁盘上的源码。
3. 点击文件列表上方的文件夹图标（**Add Directory**），添加 `07_freeRTOS_uart1/Drivers/STM32F4xx_HAL_Driver/Src`，保留 **Include subdirectories** 和 **Watch this directory**。这样可以追踪 `HAL_ADC_Start_DMA()` 等 HAL 函数，又不会把 CMSIS 的示例、测试和其他芯片启动文件全部加入分析。

## 3. 设置头文件路径和宏

1. 展开左侧 **C++ → Includes**，点击 **Import**，选择仓库根目录的 `understand-includes.txt`。导入后确认列表中有 `Core/Inc`、HAL `Inc`、CMSIS、FreeRTOS 和 `C:\keil5\ARM\ARMCC\include` 等路径。
2. 打开 **C++ → Macros**，点击 **Import**，选择仓库根目录的 `understand-macros.txt`。导入后确认至少有 `USE_HAL_DRIVER=1`、`STM32F411xE=1`、`__CC_ARM=1`、`__ARMCC_VERSION=5060750` 和 `__TARGET_FPU_VFP=1`。

`understand-includes.txt` 使用的是本机绝对路径。换电脑或移动工程后，需要修改其中的路径并重新导入；Keil 安装位置不同也要同步修改。

## 4. 选择分析模式并重新分析

在 **C++ → Options** 中选择 **Fuzzy**，编译器保持 **Keil arm**。点击 **OK** 保存；弹出“Project settings have changed and all files need to be analyzed. Analyze now?”时点击 **Yes**。

本次重新分析的日志显示 **Errors: 0、Warnings: 0**。Fuzzy 模式用于阅读源码和追踪调用关系；它对不完整或编译器特有的语法比较宽容，因此这个结果不能代替 Keil 编译或开发板运行测试。

## 5. 查看线程与 ADC 调用过程

打开 `freertos.c`，在函数名上右键，选择 **Graphical Views → Calls**：

- 对 `MX_FREERTOS_Init()` 查看 `osThreadNew()`，了解 `StartDefaultTask` 和 `ADCTask` 在哪里被创建。
- 对 `StartDefaultTask()` 查看队列创建、发送、接收以及 `HAL_ADC_Start_DMA()` 的调用。点击图中 `HAL_ADC_Start_DMA` 右侧的小箭头，可以继续展开其内部调用，包括 `HAL_DMA_Start_IT()`。
- 对 `ADCTask()` 查看 `xQueueReceive()`；收到 buffer 就绪消息后，任务继续处理对应数据。

DMA 完成回调由硬件中断触发，不是 `StartDefaultTask()` 直接调用的函数，因此普通的 **Calls** 图不会把完整的“任务 → 中断 → 回调”运行时顺序画成一条连续调用链。需要结合 `HAL_ADC_ConvCpltCallback()`、DMA IRQ 入口和任务中的队列收发代码阅读。
