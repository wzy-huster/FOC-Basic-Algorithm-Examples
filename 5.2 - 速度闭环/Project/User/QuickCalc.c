#include "stm32g4xx.h"                  // Device header
#include "cordic.h"
#include <stdint.h>
#include <math.h>

#define PI 3.1415926f

void QuickCalc_Init(void) {
    CORDIC_ConfigTypeDef sConfig = {0};
    // 配置为计算正弦和余弦
    sConfig.Function = CORDIC_FUNCTION_SINE;
    // 精度设置：6次迭代，足以满足Q1.31的精度
    sConfig.Precision = CORDIC_PRECISION_6CYCLES;
    // 缩放因子为0，不缩放
    sConfig.Scale = 0;
    // 输入数据为Q1.31格式 (32位)
    sConfig.InSize = CORDIC_INSIZE_32BITS;
    // 输出数据为Q1.31格式 (32位)
    sConfig.OutSize = CORDIC_OUTSIZE_32BITS;
    // 写入1个数据(角度)，读出2个数据(sin和cos)
    sConfig.NbWrite = CORDIC_NBWRITE_1;
    sConfig.NbRead = CORDIC_NBREAD_2;
    
    // 将配置写入硬件
    HAL_CORDIC_Configure(&hcordic, &sConfig);
}


// 将浮点数转换为 Q1.31 格式 (int32_t)
int32_t float_to_q31(float x) {
    // 将浮点数x乘上2^31 (即2147483648)，并四舍五入取整
    return (int32_t)(x * 2147483648.0f);
}

// 将 Q1.31 格式 (int32_t) 转换为浮点数
float q31_to_float(int32_t qx) {
    // 将int32_t值除以2^31得到浮点数
    return (float)qx / 2147483648.0f;
}

float normalize_angle(float angle) {
    angle = fmodf(angle, 2.0f * PI);      // 先映射到 [0, 2π)
    if (angle > PI) angle -= 2.0f * PI; // 再移到 [-π, π]
    return angle;
}

void QuickCalc_Calc(float angle_rad, float *sin_val, float *cos_val) {
	// 化归角度到 [-π, π]
	float angle_rad_normalized = normalize_angle(angle_rad);
	
    // 1. 浮点角度 -> Q1.31 格式
    int32_t angle_q31 = float_to_q31(angle_rad_normalized);

    // 2. 启动CORDIC计算
    // 注意：如果使用零开销模式，HAL_CORDIC_Calculate 函数内部通常会处理好等待，
    //     您只需要检查返回值即可。
	int32_t *sin_cos_q31;
    if (HAL_CORDIC_Calculate(&hcordic, &angle_q31, sin_cos_q31, 0, HAL_MAX_DELAY) != HAL_OK) {
        // 错误处理：可以在这里设置默认值或返回
        *sin_val = 0.0f;
        *cos_val = 1.0f;
        return;
    }

    // 3. CORDIC输出的是 Q1.31 格式，需要转换回浮点数
    *sin_val = q31_to_float(sin_cos_q31[0]);
    *cos_val = q31_to_float(sin_cos_q31[1]);
}
