#ifndef __CURRENTMEASURE_H__
#define __CURRENTMEASURE_H__

void CurrentMeasure_Init(void);
void CurrentMeasure_CalcPhaseCurrent(void);
float* CurrentMeasure_GetPhaseCurrent(void);
void CurrentMeasure_CalcRightangledCurrent(void);
float* CurrentMeasure_GetRightangledCurrent(void);


#endif
