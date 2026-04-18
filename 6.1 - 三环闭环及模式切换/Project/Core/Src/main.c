/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  速度测量和滤波
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "cordic.h"
#include "dma.h"
#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <math.h>
#include "Driver.h"
#include "Encoder.h"
#include "Serial.h"
#include "PID.h"
#include "Beep.h"
#include "CurrentMeasure.h"
#include "Filter.h"
#include "QuickCalc.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

MAF filter_speed;  // 速度滤波器
uint8_t N = 1;  // 滑动平均滤波器点数

PID pid_I_d, pid_I_q;  // 电流环
PID pid_speed;  // 速度环
PID pid_angle;  // 位置环

// 位置环
float angle_desire = 0.0f;
float angle_actual = 0.0f;

// 速度环
float speed_desire = 0.0f;  // 期望速度
float speed = 0.0f;  // 测速变量
float speed_actual;  // 滤波后速度

// 电流环
float I_d_desire = 0.0f, I_q_desire = 0.0f;  // 期望电流值
float *rightangledCurrent, I_d_actual, I_q_actual;  // 测得的电流

uint8_t led_state = 0;  // 测试程序执行速度使用的LED，同时亮起指示初始化完成
uint8_t flg_mode = 0;  // 模式标志位：0位置闭环，1速度闭环，2力矩闭环

void Test_MultiMode(void) {
// ========== 上位机的指令变量及其接收
		static float data;
		static char type;
		VOFA_GetData(&data, &type);
// ========== 上位机发送角度（位置）指令
		if (type == 'a') {
			angle_desire = data;
			flg_mode = 0;
		}
// ========== 上位机发送speed指令
		if (type == 's') {
			speed_desire = data;
			flg_mode = 1;
		}
// ========== 上位机发送dq电流指令
		if (type == 'd') {
			I_d_desire = data;
			flg_mode = 2;
		}
		else if (type == 'q') {
			I_q_desire = data;
			flg_mode = 2;
		}
// ========== 发送位置和速度波形
		VOFA_SendData(angle_desire, angle_actual, speed_desire, speed_actual);
}

void Test_Sine(void) {  // 正弦振荡方式旋转
	angle_desire = sinf((float)HAL_GetTick() * 0.005f);
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_TIM1_Init();
  MX_I2C1_Init();
  MX_USART1_UART_Init();
  MX_TIM2_Init();
  MX_ADC1_Init();
  MX_TIM3_Init();
  MX_CORDIC_Init();
  /* USER CODE BEGIN 2 */
  Filter_MAF_Init(&filter_speed, N);
  
  Serial_Init();   // 初始化串口
  Driver_Init();
  Encoder_Init();  // 对轴操作
  CurrentMeasure_Init();
//  QuickCalc_Init();  // CORDIC模块初始化，加速三角函数计算
  
  PID_Init(&pid_I_d, 6.0f, 0.0f, 0.0f, 6.0f, 6.0f);
  PID_Init(&pid_I_q, 6.0f, 0.0f, 0.0f, 6.0f, 6.0f);
  PID_Init(&pid_speed, 0.01f, 0.001f, 0.0f, 0.5f, 0.5f);
  PID_Init(&pid_angle, 60.0f, 0.0f, 10.0f, 100.0f, 100.0f);  // 1rad的偏差，使用60rad/s速度纠偏
  HAL_TIM_Base_Start_IT(&htim3);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
//// ========== 正弦方式振荡旋转
//	  Test_Sine();
	  
// ========== 模式切换：力矩-速度-位置闭环、力矩-速度闭环、力矩闭环
	  // 在VOFA上发送什么指令就进入什么闭环模式
	  Test_MultiMode();
	  
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV1;
  RCC_OscInitStruct.PLL.PLLN = 15;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	static uint8_t cnt_velocityLoop = 0;
	static uint8_t cnt_positionLoop = 0;
	if (htim == &htim3) {  // TIM3定时中断，5kHz
		
		if (flg_mode == 0 && cnt_positionLoop == 25) {  // 位置环200Hz（位置闭环模式执行）
			cnt_positionLoop = 0;
			angle_actual = Encoder_GetAngle();  // 获得当期位置
// ========== 位置环输出给速度环
			PID_pos_Calc(&pid_angle, angle_desire, angle_actual);  // 这里适合使用位置式PID，因为速度和位置具有积分关系
			speed_desire = pid_angle.output;  // 更新到speed_desire
		}
		
		if ((flg_mode == 0 || flg_mode == 1) && cnt_velocityLoop == 5) {  // 速度环1kHz（位置闭环、速度闭环模式执行）
			cnt_velocityLoop = 0;
			speed = Encoder_GetVelocity(1000);
// ========== 测速和滤波
			speed_actual = Filter_MAF_Update(&filter_speed, speed);
// ========== 速度环输出给电流环
			PID_pos_Calc(&pid_speed, speed_desire, speed_actual);  // 这里适合使用位置式PID，因为力矩和速度具有积分关系
			I_q_desire = pid_speed.output;  // 更新到Iq
		}
		
		{  // 电流环5kHz（无条件执行）
// ========== AD采样
			rightangledCurrent = CurrentMeasure_GetRightangledCurrent();
			I_d_actual = rightangledCurrent[0];
			I_q_actual = rightangledCurrent[1];
		
// ========== 电流环PID
			PID_inc_Calc(&pid_I_d, I_d_desire, I_d_actual);  // 这里适合使用增量式PID，因为电压和电流不具有积分关系
			PID_inc_Calc(&pid_I_q, I_q_desire, I_q_actual);
			Driver_SetRightangledVoltage(pid_I_d.output, pid_I_q.output, Encoder_GetAngleNoTrack() * 7);
		}
			// 调试开始
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, led_state);
			led_state = !led_state;
			// 调试结束
		
		cnt_positionLoop++;
		cnt_velocityLoop++;
	}
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
