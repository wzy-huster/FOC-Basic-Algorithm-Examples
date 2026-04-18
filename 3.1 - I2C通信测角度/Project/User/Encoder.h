#ifndef __ENCODER_H__
#define __ENCODER_H__

uint16_t Encoder_GetData(I2C_HandleTypeDef *hi2c);
float Encoder_GetAngleNoTrack(void);
float Encoder_GetAngle(void);


#endif
