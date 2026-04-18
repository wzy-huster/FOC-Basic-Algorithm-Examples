#include "Serial.h"
#include <stdio.h>
#include <string.h>
#include <math.h>

extern UART_HandleTypeDef huart1;

/* ====================== 接收缓冲区（DMA + 空闲中断） ====================== */
#define RX_BUF_SIZE    256
static uint8_t rx_dma_buf[RX_BUF_SIZE];
static uint8_t rx_overflow = 0;

// 兼容原接口的全局变量
uint8_t RxData = 0;
uint8_t RxFlag = 0;
uint8_t dataReceived[100];
uint8_t ptr_dataReceived = 0;
float VOFA_Data = 0.0f;
char VOFA_DataType = 0;

/* ====================== 初始化 ====================== */
void Serial_Init(void) {
    // 启动 DMA 循环接收
    HAL_UART_Receive_DMA(&huart1, rx_dma_buf, RX_BUF_SIZE);
    // 使能空闲中断
    __HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE);
}

/* ====================== 阻塞发送（可靠，无 DMA 冲突） ====================== */
void Serial_SendByte(uint8_t data) {
    HAL_UART_Transmit(&huart1, &data, 1, 100);
}

void Serial_SendArray(uint8_t *array, uint16_t length) {
    HAL_UART_Transmit(&huart1, array, length, 100);
}

void Serial_SendString(char *string) {
    Serial_SendArray((uint8_t*)string, strlen(string));
}

void Serial_SendNum(uint32_t num, uint8_t length) {
    char buf[16];
    for (uint8_t i = 0; i < length; i++) {
        buf[i] = (num / (uint32_t)pow(10, length - 1 - i)) % 10 + '0';
    }
    Serial_SendArray((uint8_t*)buf, length);
}

//// 重定向 printf（可选）
//int fputc(int ch, FILE *f) {
//    Serial_SendByte((uint8_t)ch);
//    return ch;
//}

/* ====================== 接收处理（空闲中断） ====================== */
static void Process_Received_Byte(uint8_t data) {
    if (RxFlag == 0) {
        if (ptr_dataReceived < sizeof(dataReceived) - 1) {
            dataReceived[ptr_dataReceived++] = data;
            if (data == '!') {
                RxFlag = 1;
                VOFA_ProcessData(&VOFA_Data, &VOFA_DataType);
            }
        } else {
            rx_overflow = 1;
        }
    }
}

void Serial_UART_IdleCallback(UART_HandleTypeDef *huart) {
    if (huart->Instance == USART1) {
        HAL_UART_DMAStop(huart);
        uint32_t remain = __HAL_DMA_GET_COUNTER(huart->hdmarx);
        uint32_t recv_len = RX_BUF_SIZE - remain;

        if (recv_len > 0 && !rx_overflow) {
            for (uint32_t i = 0; i < recv_len; i++) {
                Process_Received_Byte(rx_dma_buf[i]);
            }
        }

        HAL_UART_Receive_DMA(huart, rx_dma_buf, RX_BUF_SIZE);
        __HAL_UART_CLEAR_IDLEFLAG(huart);
    }
}

/* ====================== 协议解析（与原逻辑一致，类型改为 float） ====================== */
void VOFA_ProcessData(float *data, char *type) {
    uint8_t ptr = 0;
    uint8_t pod = 0;
    uint8_t flg_positive = 1;
    *data = 0.0f;
    for (int i = 0; i < 100; i++) {
        char c = dataReceived[i];
        if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) {
            *type = c;
        }
        if (c == '-') flg_positive = 0;
        if (c >= '0' && c <= '9') {
            *data += powf(10.0f, -((float)ptr)) * (float)(c - '0');
            ptr++;
        }
        if (c == '.') pod = ptr - 1;
        if (c == '!') break;
    }
    if (flg_positive)
        *data *= powf(10.0f, (float)pod);
    else
        *data *= -powf(10.0f, (float)pod);
    RxFlag = 0;
    ptr_dataReceived = 0;
}

void VOFA_GetData(float *data, char *type) {
    *data = VOFA_Data;
    *type = VOFA_DataType;
}

/* ====================== 发送协议（保持原格式） ====================== */
void ANODT_SendF1(int16_t _a, int16_t _b, int32_t _c) {
    uint8_t dataToSend[100];
    uint8_t _cnt = 0;
    dataToSend[_cnt++] = 0xAA;
    dataToSend[_cnt++] = 0xFF;
    dataToSend[_cnt++] = 0xF1;
    dataToSend[_cnt++] = 8;

    dataToSend[_cnt++] = _a & 0xFF;
    dataToSend[_cnt++] = (_a >> 8) & 0xFF;
    dataToSend[_cnt++] = _b & 0xFF;
    dataToSend[_cnt++] = (_b >> 8) & 0xFF;
    dataToSend[_cnt++] = _c & 0xFF;
    dataToSend[_cnt++] = (_c >> 8) & 0xFF;
    dataToSend[_cnt++] = (_c >> 16) & 0xFF;
    dataToSend[_cnt++] = (_c >> 24) & 0xFF;

    uint8_t sc = 0, ac = 0;
    for (uint8_t i = 0; i < dataToSend[3] + 4; i++) {
        sc += dataToSend[i];
        ac += sc;
    }
    dataToSend[_cnt++] = sc;
    dataToSend[_cnt++] = ac;

    Serial_SendArray(dataToSend, _cnt);
}

void VOFA_SendData(float d0, float d1, float d2, float d3) {
    char buf[128];
    snprintf(buf, sizeof(buf), "%f,%f,%f,%f\r\n", d0, d1, d2, d3);
    Serial_SendString(buf);
}

/* ====================== 兼容原查询接口 ====================== */
uint8_t Serial_GetFlag(void) {
    if (RxFlag) {
        RxFlag = 0;
        return 1;
    }
    return 0;
}

uint8_t Serial_GetData(void) {
    return RxData;
}
