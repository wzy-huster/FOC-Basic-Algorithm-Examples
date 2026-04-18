#include "stm32g4xx.h"                  // Device header
#include "math.h"

#define HALF_VOLTAGE_POWER_SUPPLY 6.0f
#define PAIR 7  // 极对数
#define PI 3.1415926f
#define PI_2_3 2.0943951f

extern TIM_HandleTypeDef htim1;

float __constrain(float x, float max, float min) {
	if (x > max) {
		return max;
	}
	else if (x < min) {
		return min;
	}
	else {
		return x;
	}
}

void Driver_Init(void) {
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);
}

void Driver_SetPhaseVoltage(float Ua, float Ub, float Uc) {
	Ua = __constrain(Ua, 6.0f, -6.0f);
	Ub = __constrain(Ub, 6.0f, -6.0f);
	Uc = __constrain(Uc, 6.0f, -6.0f);
	uint16_t duty_a, duty_b, duty_c;
	duty_a = (int16_t)((Ua / HALF_VOLTAGE_POWER_SUPPLY) * 500) + 500;
	duty_b = (int16_t)((Ub / HALF_VOLTAGE_POWER_SUPPLY) * 500) + 500;
	duty_c = (int16_t)((Uc / HALF_VOLTAGE_POWER_SUPPLY) * 500) + 500;
	__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, duty_a);
	__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, duty_b);
	__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, duty_c);
}

void Driver_SetRightangledVoltage(float Ud, float Uq, float e_angle) {
	float Ua, Ub, Uc;
	Ua = cosf(e_angle) * Ud - sinf(e_angle) * Uq;
	Ub = cosf(e_angle - PI_2_3) * Ud - sinf(e_angle - PI_2_3) * Uq;
	Uc = cosf(e_angle + PI_2_3) * Ud - sinf(e_angle + PI_2_3) * Uq;
	Driver_SetPhaseVoltage(Ua, Ub, Uc);
}

void Driver_SetVelocityOL(float m_veloc, float intensity) {
	float e_angle = (float)HAL_GetTick() / 1000.0f * m_veloc * PAIR;
	Driver_SetRightangledVoltage(intensity, 0.0f, e_angle);	
}
