# STM32F411CEU6 UART Bootloader


本项目基于 **STM32F411CEU6** 单片机实现了一个轻量级的 Bootloader，支持通过 UART（串口）与 Python 上位机通信进行固件升级（IAP）。

## 目录结构

项目主要包含以下三个核心部分：

```text
Project_Root
├── 📂 Bootloader      # Bootloader 底层驱动程序（引导加载程序）
├── 📂 Firmware        # 用于测试的 APP 固件（通过 LED 闪烁频率验证升级结果）
└── 📂 bootloader_app  # 基于 Python 编写的 PC 端上位机工具
```

## 开发环境与依赖

### 硬件
*   **MCU**: STM32F411CEU6(为主控的开发板)
*   **通信**: USB 转 TTL 串口模块 (连接 PA9/PA10)

### 软件
*   **IDE**: [EIDE](https://em-ide.com/) (Embedded IDE for VS Code)
*   **Python**: Python 3.13
*   **Python 库**: 需要安装 `pyserial` 等依赖库

## 注意事项

> **关于开发环境的兼容性说明：**
>
> 本项目主要使用 **VS Code + EIDE 插件** 进行开发和管理。
>
> *   如果您使用 **EIDE**：直接打开项目即可编译。
> *   如果您使用 **Keil MDK**：您需要**手动将源码文件添加到工程组中，同时自行配置头文件包含路径（Include Paths）**。

## Flash 内存布局 

建议的内存划分如下（请根据实际代码修改）：

| 区域 | 起始地址 | 说明 |
| :--- | :--- | :--- |
| **Bootloader** | `0x0800 0000` | 引导程序存放起始地址，大小:16KB，1个扇区                                                    |
| **Application**| `0x0800 4000` | APP 固件存放区起始地址，大小:剩下的496KB，在擦除的时候留了后面几个扇区，用于存放用户配置的可能 |

---

**Enjoy coding!** 如果觉得本项目对你有帮助，请给个 Star ⭐️。