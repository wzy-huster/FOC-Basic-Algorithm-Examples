#include "stm32g4xx.h"                  // Device header
#include "Encoder.h"
#include <math.h>
#include "QuickCalc.h"

extern ADC_HandleTypeDef hadc1;

#define D2A 0.000805664f  // / 2048.0f * 1.65f / 100.0f / 0.01f
#define PI 3.1415926f
uint32_t adc_buffer[3]; // 存储ADC结果
float phase_current[3];  // 保存abc三相电流数值
float rightangled_current[2];  // 保存dq轴的电流数值

void CurrentMeasure_Init(void) {
	HAL_ADC_Start_DMA(&hadc1, adc_buffer, 3);
}

void CurrentMeasure_CalcPhaseCurrent(void) {
	phase_current[0] = ((float)adc_buffer[0] - 2048.0f) * D2A;
	phase_current[1] = ((float)adc_buffer[1] - 2048.0f) * D2A;
	phase_current[2] = ((float)adc_buffer[2] - 2048.0f) * D2A;
}

float* CurrentMeasure_GetPhaseCurrent(void) {
	CurrentMeasure_CalcPhaseCurrent();
	return phase_current;
}
	
void CurrentMeasure_CalcRightangledCurrent(void) {
	CurrentMeasure_CalcPhaseCurrent();  // 易漏，必须提前调用计算abc三相电流
	float I_common = (phase_current[0] + phase_current[1] + phase_current[2]) / 3.0f;
	float I_A = phase_current[0] - I_common;
	float I_B = phase_current[1] - I_common;
	float I_C = phase_current[2] - I_common;
	float e_angle = Encoder_GetAngleNoTrack() * 7;
	rightangled_current[0] = 2.0f / 3.0f * (cosf(e_angle)*I_A 
		+ cosf(e_angle-PI*2.0f/3.0f)*I_B + cosf(e_angle+PI*2.0f/3.0f)*I_C);
	rightangled_current[1] = -2.0f / 3.0f * (sinf(e_angle)*I_A 
		+ sinf(e_angle-PI*2.0f/3.0f)*I_B + sinf(e_angle+PI*2.0f/3.0f)*I_C);

//	CurrentMeasure_CalcPhaseCurrent();  // 易漏，必须提前调用计算abc三相电流
//	// 1. Clarke 变换
//	float ialpha = phase_current[0];
//	float ibeta = (phase_current[0] + 2.0f * phase_current[1]) * 0.57735027f;   // sqrt(3) ≈ 1.7320508

//	// 2. 用 CORDIC 同时计算 cosθ 和 sinθ
//	float cos_theta, sin_theta;
//	float e_angle = Encoder_GetAngleNoTrack() * 7;
//	QuickCalc_Calc(e_angle, &sin_theta, &cos_theta);  // 你自己实现的快速函数

//	// 3. Park 变换
//	rightangled_current[0] = ialpha * cos_theta + ibeta * sin_theta;
//	rightangled_current[1] = -ialpha * sin_theta + ibeta * cos_theta;
}

float* CurrentMeasure_GetRightangledCurrent(void) {
	CurrentMeasure_CalcRightangledCurrent();
	return rightangled_current;
}
