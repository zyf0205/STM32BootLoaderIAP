#ifndef __BOOT_H
#define __BOOT_H

#include "stm32f4xx.h"

/* ================= Flash 地址规划 ================= */
/*
 * STM32F411 Flash 起始地址：0x08000000
 * Bootloader 使用 Sector 0 （共 16KB）
 * Application 从 Sector 1 开始
 */
#define APP_START_ADDR 0x08004000

/* ================= SRAM 范围 ================= */
/*
 * 用于判断 Application 是否有效
 * Cortex-M 启动时，向量表第 0 项是 MSP
 * MSP 必须指向 SRAM 区域
 */
#define SRAM_START_ADDR 0x20000000
#define SRAM_END_ADDR 0x20020000 // 128KB SRAM

/* ================= 串口协议定义 ================= */
// #define FRAME_HEAD 0x55

// #define CMD_GET_VERSION 0x01
// #define CMD_START_UPDATE 0x02
// #define CMD_WRITE_DATA 0x03
// #define CMD_FINISH 0x04
// #define CMD_JUMP_APP 0x05

void Boot_JumpToApp(void);

#endif
