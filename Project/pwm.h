#ifndef __PWM_H
#define __PWM_H

#include "stm8s.h"

#define PWM_Channel1  1
#define PWM_Channel2  2
#define PWM_Channel3  3

#define PWM_Channel   PWM_Channel1


#define	PWM_PERIOD_VAL	 320 //ÖÜÆÚ 50KHZ   //3200//5000HZ  //320 //10khz    //  //800//20KHZ   //     //
#define	Duty2TimerVal(duty)	(u16)((u32)duty*PWM_PERIOD_VAL/100)


void PWM_Init(void);

void SetTIM2_PWM_Frequency(uint16_t TIM2_Period);
void SetTIM2_PWM_DutyCycle( uint16_t TIM2_Pulse);
void TestPWM_LED(void);
void TestPWM_Beep(void);


#define		__ResetPeriodTIM(TIMx,TIM_Period,TIM_Pulse)	\
	TIMx->ARR = TIM_Period; \
	TIMx->CCR1 = TIM_Pulse;	


#endif


