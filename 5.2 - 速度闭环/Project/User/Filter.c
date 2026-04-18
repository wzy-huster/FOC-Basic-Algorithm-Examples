#include "Filter.h"

// 编写一个滑动平均滤波器（MAF）的函数的.c文件，使用数组缓存历史数据，并且可以指定滤波器的窗口大小。
// 该函数接受一个新的输入值，并返回当前窗口内的平均值。
// 要求可以给多个数据流复用滤波（使用结构体实例化），并且每个数据流可以有不同的窗口大小。
// 使用基本的循环而不是标准库的函数来计算平均值。


// 初始化MAF结构体
void Filter_MAF_Init(MAF *maf, uint16_t window_size) {
	if (window_size > MAX_LEN) {
		maf->window_size = MAX_LEN;
	}
	else {
		maf->window_size = window_size;
	}
    for (uint16_t i = 0; i < maf->window_size; i++) {
        maf->buffer[i] = 0.0f; // 初始化缓冲区
    }
    maf->index = 0;
    maf->count = 0;
}

// 更新MAF滤波器并返回当前窗口内的平均值
float Filter_MAF_Update(MAF *maf, float new_value) {
    // 将新值添加到缓冲区
    maf->buffer[maf->index] = new_value;
    maf->index = (maf->index + 1) % maf->window_size; // 循环索引

    // 更新窗口内的数据数量
    if (maf->count < maf->window_size) {
        maf->count++;
    }

    // 计算当前窗口内的平均值
    float sum = 0.0f;
    for (uint8_t i = 0; i < maf->count; i++) {
        sum += maf->buffer[i];
    }
    
    return sum / maf->count; // 返回平均值
}
