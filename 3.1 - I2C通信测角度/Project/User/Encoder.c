#include "stm32g4xx.h"                  // Device header
#include "math.h"

extern I2C_HandleTypeDef hi2c1;

// === 定义常量 ===
#define AS5600_ADDR       (0x36 << 1) // 7位地址0x36左移一位，得到8位写地址
#define RAW_ANGLE_REG     0x0C        // 原始角度寄存器起始地址
#define RAW_TO_RAD (2.0f * 3.1415926535f / 4096.0f)  // 0.0015339807f

// 角度跟踪所需变量
static int32_t total_rounds = 0;       // 圈数计数器（有符号）
static uint16_t last_raw_angle = 0;    // 上一次的原始角度值 (0-4095)
static uint16_t last_raw_angle_no_track = 0;
static uint8_t is_first_read = 1;      // 首次读取标志，用于初始化


// === 读取AS5600的原始角度数据 ===
// 返回值: 0-4095 的原始角度值，若读取失败返回 0xFFFF
uint16_t Encoder_GetData(I2C_HandleTypeDef *hi2c) {
    uint8_t buf[2] = {0};
    uint8_t reg = RAW_ANGLE_REG;

    // 步骤1: 发送寄存器地址 (先写)
    // 参数: hi2c, 设备8位地址, 发送的数据, 数据长度, 超时时间(ms)
    if (HAL_I2C_Master_Transmit(hi2c, AS5600_ADDR, &reg, 1, HAL_MAX_DELAY) != HAL_OK) {
        return 0xFFFF; // 发送失败
    }

    // 步骤2: 接收2字节数据 (再读)
    if (HAL_I2C_Master_Receive(hi2c, AS5600_ADDR, buf, 2, HAL_MAX_DELAY) != HAL_OK) {
        return 0xFFFF; // 接收失败
    }

    // 步骤3: 组合数据
    // 注意：需要根据数据手册确认高8位和低4位的具体组合方式
    uint16_t raw_angle = (buf[0] << 8) | buf[1];
    return raw_angle;
}

// 获取角度（弧度），不含圈数
float Encoder_GetAngleNoTrack(void) {
	uint16_t current_raw_no_track = Encoder_GetData(&hi2c1);
    if (current_raw_no_track == 0xFFFF) {
        // I2C 读取失败，返回上一次的有效角度
        return last_raw_angle_no_track * RAW_TO_RAD;
    }
    // 更新上一次的原始角度值
    last_raw_angle_no_track = current_raw_no_track;
	float current_rad_no_track = current_raw_no_track * RAW_TO_RAD;
    return current_rad_no_track;
}

// 获取总角度（弧度），支持多圈计数
float Encoder_GetAngle(void) {
    uint16_t current_raw = Encoder_GetData(&hi2c1);
    if (current_raw == 0xFFFF) {
        // I2C 读取失败，返回上一次的有效角度
        return (total_rounds * 2.0f * 3.1415926535f) + (last_raw_angle * RAW_TO_RAD);
    }

    if (is_first_read) {
        // 首次读取，初始化状态，不计入圈数变化
        last_raw_angle = current_raw;
        is_first_read = 0;
        return 0.0f;
    }

    // 计算角度变化量（考虑0点跳变）
    int16_t delta = (int16_t)(current_raw - last_raw_angle);
    
    // 检测过零点并更新圈数
    if (delta > 2048) {          // 反向过零：从低值跳变到高值（如100 -> 4000）
        total_rounds--;          // 反转一圈，圈数减1
    } else if (delta < -2048) {  // 正向过零：从高值跳变到低值（如4000 -> 100）
        total_rounds++;          // 正转一圈，圈数加1
    }

    // 更新上一次的原始角度值
    last_raw_angle = current_raw;

    // 计算总弧度：圈数*2PI + 当前圈内弧度
    float total_rad = (total_rounds * 2.0f * 3.1415926535f) + (current_raw * RAW_TO_RAD);
    return total_rad;
}
