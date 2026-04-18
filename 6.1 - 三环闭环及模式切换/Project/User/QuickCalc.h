#ifndef __QUICKCALC_H__
#define __QUICKCALC_H__

void QuickCalc_Init(void);
int32_t float_to_q31(float x);
float q31_to_float(int32_t qx);
float normalize_angle(float angle);
void QuickCalc_Calc(float angle_rad, float *sin_val, float *cos_val);


#endif
