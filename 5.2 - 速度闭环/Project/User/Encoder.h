#ifndef __ENCODER_H__
#define __ENCODER_H__

void Encoder_Init(void);
uint16_t Encoder_GetData(I2C_HandleTypeDef *hi2c);
float Encoder_GetAngleNoTrack(void);
float Encoder_GetAngle(void);
float Encoder_GetVelocity(uint32_t interrupt_freq);


#endif
