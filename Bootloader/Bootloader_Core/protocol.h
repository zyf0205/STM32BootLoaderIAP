#ifndef __PROTOCOL_H
#define __PROTOCOL_H

#include "stm32f4xx.h"

/* 协议定义 */
#define FRAME_HEAD_1 0x5A
#define FRAME_HEAD_2 0xA5

/* 命令码 */
#define CMD_CONNECT 0x01
#define CMD_DATA 0x02
#define CMD_FINISH 0x03

/* 应答码 */
#define ACK 0x06
#define NACK 0x15

/* 接收缓冲区 */
#define RX_BUF_SIZE 2048

void Protocol_Process(void);

#endif