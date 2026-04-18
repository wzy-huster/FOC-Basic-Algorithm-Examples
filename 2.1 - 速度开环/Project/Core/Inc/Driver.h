#ifndef __DRIVER_H__
#define __DRIVER_H__

void Driver_SetPhaseVoltage(float Ua, float Ub, float Uc);
void Driver_SetRightangledVoltage(float Uq, float Ud, float e_angle);
void Driver_SetVelocityOL(float m_veloc, float intensity);


#endif
