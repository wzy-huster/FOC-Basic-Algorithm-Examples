#ifndef __SERIAL_H__
#define __SERIAL_H__

#include "stm32g4xx_hal.h"
#include <stdint.h>

// 初始化（启动 DMA 接收）
void Serial_Init(void);

// 发送接口（阻塞模式，可靠）
void Serial_SendByte(uint8_t data);
void Serial_SendArray(uint8_t *array, uint16_t length);
void Serial_SendString(char *string);
void Serial_SendNum(uint32_t num, uint8_t length);

// 接收查询（与原接口兼容）
//static void WaitForTxComplete(void);
uint8_t Serial_GetFlag(void);
uint8_t Serial_GetData(void);

// 应用层协议
void ANODT_SendF1(int16_t _a, int16_t _b, int32_t _c);
void VOFA_ProcessData(float *data, char *type);
void VOFA_SendData(float d0, float d1, float d2, float d3);
void VOFA_GetData(float *data, char *type);

// 空闲中断回调（需在 stm32g4xx_it.c 中调用）
void Serial_UART_IdleCallback(UART_HandleTypeDef *huart);


#endif
