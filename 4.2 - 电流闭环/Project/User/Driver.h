#ifndef __DRIVER_H__
#define __DRIVER_H__

void Driver_Init(void);
void Driver_SetPhaseVoltage(float Ua, float Ub, float Uc);
void Driver_SetRightangledVoltage(float Ud, float Uq, float e_angle);
void Driver_SetVelocityOL(float m_veloc, float intensity);


#endif
