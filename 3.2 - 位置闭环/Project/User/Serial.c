//#include "stm32g4xx.h"                  // Device header
//#include "math.h"
//#include <stdio.h>

//extern UART_HandleTypeDef huart1;

//uint8_t RxData, RxFlag;
//uint8_t dataToSend[100];
//uint8_t dataReceived[100];
//uint8_t ptr_dataReceived;
//double VOFA_Data;
//char VOFA_DataType;

//void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {
//	if (huart->Instance == USART1) {  // 判断串口1接收到数据
//		
//	}
//}

//void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
//	if (huart->Instance == USART1) {  // 判断串口1发送完数据
//		
//	}
//}

//void Serial_SendByte(uint8_t data) {
//	HAL_UART_Transmit_DMA(&huart1, data);
//	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
//}

//void Serial_SendArray(uint8_t *array, uint16_t length) {
//	for (uint16_t i = 0; i < length; i++) {
//		Serial_SendByte(array[i]);
//	}
//}

//void Serial_SendString(char *string) {
//	for (uint8_t i = 0; string[i] != '\0'; i++) {
//		Serial_SendByte(string[i]);
//	}
//}

//void Serial_SendNum(uint32_t num, uint8_t length) {
//	for (uint8_t i = length; i > 0; i--) {
//		Serial_SendByte(num / (int)pow(10, i - 1) % 10 + '0');
//	}
//}

//int fputc(int ch, FILE *f) {
//	Serial_SendByte(ch);
//	return ch;
//}

//uint8_t Serial_GetFlag(void) {
//	if (RxFlag == 1) {
//		RxFlag = 0;
//		return 1;
//	}
//	return 0;
//}

//uint8_t Serial_GetData(void) {
//	return RxData;
//}

////// 原版串口中断函数
////void USART1_IRQHandler(void) {
////	if (USART_GetITStatus(USART1, USART_IT_RXNE) == SET) {
////		RxData = USART_ReceiveData(USART1);
////		RxFlag = 1;
////		// -----
////		Serial_SendByte(RxData);
////		// -----
////		USART_ClearITPendingBit(USART1, USART_IT_RXNE);
////	}
////}

//// DS修改后的串口中断函数
//void USART1_IRQHandler(void) {
//    if (USART_GetITStatus(USART1, USART_IT_RXNE) == SET) {
//        uint8_t data = USART_ReceiveData(USART1);

//        // 仅当没有未处理的完整数据包时，才存储新数据
//        if (RxFlag == 0) {
//            // 防止缓冲区溢出（留一个字节位置，但实际也可存满）
//            if (ptr_dataReceived < sizeof(dataReceived) - 1) {
//                dataReceived[ptr_dataReceived++] = data;
//                if (data == '!') {
//                    RxFlag = 1;          // 标记一个浮点数数据包接收完成
//					VOFA_ProcessData(&VOFA_Data, &VOFA_DataType);
//                }
//            } else {
//                // 缓冲区已满：可根据需要处理，例如丢弃该字节或重置指针
//                // 这里简单忽略该字节
//            }
//        }
//        // 若 RxFlag == 1，则丢弃后续数据，避免覆盖未处理的包

//        USART_ClearITPendingBit(USART1, USART_IT_RXNE);
//    }
//}

//void ANODT_SendF1(int16_t _a, int16_t _b, int32_t _c) {
//	uint8_t _cnt = 0;
//	dataToSend[_cnt++] = 0xAA;
//	dataToSend[_cnt++] = 0xFF;
//	dataToSend[_cnt++] = 0xF1;
//	dataToSend[_cnt++] = 8;
//	
//	dataToSend[_cnt++] = _a & 0x00FF;
//	dataToSend[_cnt++] = (_a & 0xFF00) >> 8;
//	
//	dataToSend[_cnt++] = _b & 0x00FF;
//	dataToSend[_cnt++] = (_b & 0xFF00) >> 8;
//	 
//	dataToSend[_cnt++] = _c & 0x000000FF;
//	dataToSend[_cnt++] = (_c & 0x0000FF00) >> 8;
//	dataToSend[_cnt++] = (_c & 0x00FF0000) >> 16;
//	dataToSend[_cnt++] = (_c & 0xFF000000) >> 24;
//	
//	uint8_t sc = 0;
//	uint8_t ac = 0;
//	
//	for (uint8_t i = 0; i < dataToSend[3] + 4; i++) {
//		sc += dataToSend[i];
//		ac += sc;
//	}
//	dataToSend[_cnt++] = sc;
//	dataToSend[_cnt++] = ac;
//	
//	Serial_SendArray(dataToSend, _cnt);
//}


//void VOFA_SendData(double d0, double d1, double d2, double d3) {
////	"channels: 1.386578,0.977929,-0.628913,-0.942729\n"
//	char DataToSend[100];
//	sprintf(DataToSend, "%f,%f,%f,%f\r\n", d0, d1, d2, d3);
//	Serial_SendString(DataToSend);
//}

//void VOFA_ProcessData(double *data, char *type) {
//	uint8_t ptr = 0;
//	uint8_t pod = 0;
//	uint8_t flg_positive = 1;
//	*data = 0;
//	for (int i = 0; i < 100; i++) {
//		char data_c = dataReceived[i];
//		if ((data_c >= 'a' && data_c <= 'z') || (data_c >= 'A' && data_c <= 'Z')) {
//			*type = data_c;
//		}
//		if (data_c == '-') {
//			flg_positive = 0;
//		}
//		if (data_c <= '9' && data_c >= '0') {
//			*data += pow(10.0, -((double)ptr)) * (double)(data_c - '0');
//			ptr++;
//		}
//		if (data_c == '.') {
//			pod = ptr - 1;
//		}
//		if (data_c == '!') {
//			break;
//		}
//	}
//	if (flg_positive) {
//		*data *= pow(10.0, (double)pod);
//	}
//	else {
//		*data *= -pow(10.0, (double)pod);
//	}
//	// 处理接收标志位和缓存指针
//	RxFlag = 0;
//	ptr_dataReceived = 0;
//}

//void VOFA_GetData(double *data, char *type) {
//	*data = VOFA_Data;
//	*type = VOFA_DataType;
//}





///*
// * serial_comm.c
// * 重构说明：
// * 1. 发送：使用 DMA 模式（HAL_UART_Transmit_DMA）
// * 2. 接收：使用 DMA 循环模式 + 空闲中断，自动缓存数据包
// * 3. 保持原协议：ANODT 二进制帧、VOFA 浮点数据（'!' 结束）
// * 4. 删除冗余：移除标准库 USART1_IRQHandler，改用 HAL 回调
// */

//#include "stm32g4xx_hal.h"
//#include "math.h"
//#include <stdio.h>
//#include <string.h>

///* 外部句柄 */
//extern UART_HandleTypeDef huart1;

///* ========================= 接收缓冲区与标志 ========================= */
//#define RX_BUF_SIZE   256          // DMA 接收缓冲区大小（循环模式）
//static uint8_t rx_dma_buf[RX_BUF_SIZE];
//static uint8_t rx_overflow = 0;    // 溢出标志（缓冲区满或数据包过长）
////static uint8_t rx_packet_ready = 0; // 完整数据包就绪标志

//// 原协议解析使用的全局变量（保持接口兼容）
//uint8_t RxData = 0;
//uint8_t RxFlag = 0;
//uint8_t dataReceived[100];
//uint8_t ptr_dataReceived = 0;
//float VOFA_Data = 0.0f;
//char VOFA_DataType = 0;

///* ========================= 发送缓冲区 ========================= */
//// 保留原发送数组（用于 ANODT 组包）
//uint8_t dataToSend[100];

///* ========================= 辅助函数声明 ========================= */
//static void Process_Received_Byte(uint8_t data);
//static void VOFA_ProcessData(float *data, char *type); // 原函数声明

///* ========================= 1. 初始化 DMA 接收（在 main 中调用一次） ========================= */
//void Serial_Init_DMA(void)
//{
//    // 启动 DMA 循环接收，数据持续写入 rx_dma_buf
//    HAL_UART_Receive_DMA(&huart1, rx_dma_buf, RX_BUF_SIZE);
//    // 使能 UART 空闲中断
//    __HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE);
//}

///* ========================= 2. 发送函数（全部改为 DMA 模式） ========================= */
//void Serial_SendByte(uint8_t data)
//{
//    // 使用 DMA 发送单个字节（注意：频繁单字节发送效率不高，但保持接口兼容）
//    HAL_UART_Transmit_DMA(&huart1, &data, 1);
//    // 可选：等待发送完成，避免连续调用覆盖缓冲区
//    // 实际使用时建议用回调或环形队列，此处为简单实现保持原行为
//    while (HAL_UART_GetState(&huart1) == HAL_UART_STATE_BUSY_TX);
//}

//void Serial_SendArray(uint8_t *array, uint16_t length)
//{
//    for (uint16_t i = 0; i < length; i++) {
//        Serial_SendByte(array[i]);
//    }
//}

//void Serial_SendString(char *string)
//{
//    while (*string) {
//        Serial_SendByte(*string++);
//    }
//}

//void Serial_SendNum(uint32_t num, uint8_t length)
//{
//    for (uint8_t i = length; i > 0; i--) {
//        uint8_t digit = (num / (uint32_t)pow(10, i - 1)) % 10;
//        Serial_SendByte(digit + '0');
//    }
//}

////int fputc(int ch, FILE *f)
////{
////    Serial_SendByte((uint8_t)ch);
////    return ch;
////}

///* ========================= 3. 接收处理：空闲中断 + DMA ========================= */
//// 在 UART 空闲中断服务函数中调用此函数（需在 stm32g4xx_it.c 中添加）
//void Serial_UART_IdleCallback(UART_HandleTypeDef *huart)
//{
//    if (huart->Instance == USART1) {
//        // 停止 DMA 接收，获取已接收字节数
//        HAL_UART_DMAStop(huart);
//        uint32_t remain = __HAL_DMA_GET_COUNTER(huart->hdmarx);
//        uint32_t recv_len = RX_BUF_SIZE - remain;

//        if (recv_len > 0 && !rx_overflow) {
//            // 将 DMA 缓冲区内的字节逐个解析
//            for (uint32_t i = 0; i < recv_len; i++) {
//                Process_Received_Byte(rx_dma_buf[i]);
//            }
//        }

//        // 重新启动 DMA 循环接收
//        HAL_UART_Receive_DMA(huart, rx_dma_buf, RX_BUF_SIZE);
//        // 清除空闲中断标志
//        __HAL_UART_CLEAR_IDLEFLAG(huart);
//    }
//}

//// 逐字节处理函数（与原中断服务函数逻辑一致）
//static void Process_Received_Byte(uint8_t data)
//{
//    // 仅当没有未处理的完整数据包时，才存储新数据
//    if (RxFlag == 0) {
//        if (ptr_dataReceived < sizeof(dataReceived) - 1) {
//            dataReceived[ptr_dataReceived++] = data;
//            if (data == '!') {
//                RxFlag = 1;          // 标记数据包接收完成
//                VOFA_ProcessData(&VOFA_Data, &VOFA_DataType);
//            }
//        } else {
//            // 缓冲区溢出：丢弃该字节，并置溢出标志
//            rx_overflow = 1;
//        }
//    }
//    // 若 RxFlag == 1，则丢弃后续数据，避免覆盖未处理的包
//}

///* ========================= 4. 原协议解析函数（保持原样） ========================= */
//void VOFA_ProcessData(float *data, char *type)
//{
//    uint8_t ptr = 0;
//    uint8_t pod = 0;
//    uint8_t flg_positive = 1;
//    *data = 0;
//    for (int i = 0; i < 100; i++) {
//        char data_c = dataReceived[i];
//        if ((data_c >= 'a' && data_c <= 'z') || (data_c >= 'A' && data_c <= 'Z')) {
//            *type = data_c;
//        }
//        if (data_c == '-') {
//            flg_positive = 0;
//        }
//        if (data_c <= '9' && data_c >= '0') {
//            *data += powf(10.0f, -((float)ptr)) * (float)(data_c - '0');
//            ptr++;
//        }
//        if (data_c == '.') {
//            pod = ptr - 1;
//        }
//        if (data_c == '!') {
//            break;
//        }
//    }
//    if (flg_positive) {
//        *data *= powf(10.0, (float)pod);
//    } else {
//        *data *= -powf(10.0, (float)pod);
//    }
//    // 处理接收标志位和缓存指针
//    RxFlag = 0;
//    ptr_dataReceived = 0;
//}

//void VOFA_GetData(float *data, char *type)
//{
//    *data = VOFA_Data;
//    *type = VOFA_DataType;
//}

///* ========================= 5. ANODT 协议发送函数（保持原样） ========================= */
//void ANODT_SendF1(int16_t _a, int16_t _b, int32_t _c)
//{
//    uint8_t _cnt = 0;
//    dataToSend[_cnt++] = 0xAA;
//    dataToSend[_cnt++] = 0xFF;
//    dataToSend[_cnt++] = 0xF1;
//    dataToSend[_cnt++] = 8;

//    dataToSend[_cnt++] = _a & 0x00FF;
//    dataToSend[_cnt++] = (_a & 0xFF00) >> 8;

//    dataToSend[_cnt++] = _b & 0x00FF;
//    dataToSend[_cnt++] = (_b & 0xFF00) >> 8;

//    dataToSend[_cnt++] = _c & 0x000000FF;
//    dataToSend[_cnt++] = (_c & 0x0000FF00) >> 8;
//    dataToSend[_cnt++] = (_c & 0x00FF0000) >> 16;
//    dataToSend[_cnt++] = (_c & 0xFF000000) >> 24;

//    uint8_t sc = 0;
//    uint8_t ac = 0;
//    for (uint8_t i = 0; i < dataToSend[3] + 4; i++) {
//        sc += dataToSend[i];
//        ac += sc;
//    }
//    dataToSend[_cnt++] = sc;
//    dataToSend[_cnt++] = ac;

//    Serial_SendArray(dataToSend, _cnt);
//}

///* ========================= 6. VOFA 格式发送（保持原样） ========================= */
//void VOFA_SendData(float d0, float d1, float d2, float d3)
//{
//    char DataToSend[100];
//    sprintf(DataToSend, "%f,%f,%f,%f\r\n", d0, d1, d2, d3);
//    Serial_SendString(DataToSend);
//}

///* ========================= 7. 兼容原查询函数 ========================= */
//uint8_t Serial_GetFlag(void)
//{
//    if (RxFlag == 1) {
//        RxFlag = 0;
//        return 1;
//    }
//    return 0;
//}

//uint8_t Serial_GetData(void)
//{
//    return RxData;
//}






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

//// 发送时用于等待的绝对时间（HAL_Delay 在中断中不安全，改用阻塞轮询）
//static void WaitForTxComplete(void) {
//    uint32_t tickstart = HAL_GetTick();
//    while (HAL_UART_GetState(&huart1) == HAL_UART_STATE_BUSY_TX) {
//        if ((HAL_GetTick() - tickstart) > 100) break; // 超时100ms
//    }
//}

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
