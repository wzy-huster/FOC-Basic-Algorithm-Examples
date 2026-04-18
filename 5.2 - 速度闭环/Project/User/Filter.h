#ifndef __FILTER_H__
#define __FILTER_H__

#include <stdint.h>

#define MAX_LEN 100

typedef struct {
    float buffer[MAX_LEN];       // 用于存储历史数据的数组
    uint16_t window_size; // 滤波器的窗口大小
    uint16_t index;       // 当前索引位置
    uint16_t count;       // 当前窗口内的数据数量
} MAF;

void Filter_MAF_Init(MAF *maf, uint16_t window_size);
float Filter_MAF_Update(MAF *maf, float new_value);


#endif
