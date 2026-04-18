#include "stm32g4xx.h"                  // Device header

extern TIM_HandleTypeDef htim2;

void Beep_Init(void) {
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_4);
}

void Beep_Set(uint8_t state) {
	if (state) {
	__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_4, 500);
	}
	else {
	__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_4, 0);
	}
}
