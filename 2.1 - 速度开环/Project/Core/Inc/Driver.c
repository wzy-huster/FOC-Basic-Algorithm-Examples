#include "stm32g4xx.h"                  // Device header
#include "math.h"

#define HALF_VOLTAGE_POWER_SUPPLY 6
#define PAIR 7  // 极对数
#define PI 3.1415926
#define PI_2_3 2.0943951

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

void Driver_SetPhaseVoltage(float Ua, float Ub, float Uc) {
	Ua = __constrain(Ua, 0.0, 12.0);
	Ub = __constrain(Ub, 0.0, 12.0);
	Uc = __constrain(Uc, 0.0, 12.0);
	uint16_t duty_a, duty_b, duty_c;
	duty_a = (uint16_t)((Ua / HALF_VOLTAGE_POWER_SUPPLY) * 500) + 500;
	duty_b = (uint16_t)((Ub / HALF_VOLTAGE_POWER_SUPPLY) * 500) + 500;
	duty_c = (uint16_t)((Uc / HALF_VOLTAGE_POWER_SUPPLY) * 500) + 500;
	__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, duty_a);
	__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, duty_b);
	__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, duty_c);
}

void Driver_SetRightangledVoltage(float Uq, float Ud, float e_angle) {
	float Ua, Ub, Uc;
	Ua = cosf(e_angle) * Ud - sinf(e_angle) * Uq;
	Ub = cosf(e_angle - PI_2_3) * Ud - sinf(e_angle - PI_2_3) * Uq;
	Uc = cosf(e_angle + PI_2_3) * Ud - sinf(e_angle + PI_2_3) * Uq;
	Driver_SetPhaseVoltage(Ua, Ub, Uc);
}

void Driver_SetVelocityOL(float m_veloc, float intensity) {
	float e_angle = (float)HAL_GetTick() / 1000.0 * m_veloc;
	Driver_SetRightangledVoltage(intensity, 0.0, e_angle);	
}
