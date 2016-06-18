#include "pwm.h"

//static void Delay(u16 nCount);


void PWM_Init(void)
{
	
//定义PD4 为PWM输出脚
    GPIO_Init(GPIOD,GPIO_PIN_4,GPIO_MODE_OUT_PP_HIGH_FAST);

  #if PWM_Channel==PWM_Channel1
    /*TIM2 Frequency = TIM2 counter clock/(ARR + 1) */
  
    /*TIM2 Frequency=16M/16/(49+1)=20K*/
    //TIM2_TimeBaseInit(TIM2_PRESCALER_16, PWM_PERIOD_VAL-1);  //设置为2K 的频率
    TIM2_TimeBaseInit(TIM2_PRESCALER_1, PWM_PERIOD_VAL-1);  //设置为2K 的频率

    /* PWM1 Mode configuration: Channel1
    TIM2 Channel1 duty cycle = [TIM2_CCR1/(TIM2_ARR + 1)] * 100 = 50%*/ 
    TIM2_OC1Init(TIM2_OCMODE_PWM1, TIM2_OUTPUTSTATE_ENABLE,Duty2TimerVal(0), TIM2_OCPOLARITY_HIGH);
    TIM2_OC1PreloadConfig(ENABLE);
    
  #elif PWM_Channel==PWM_Channel2
    TIM2_TimeBaseInit(TIM2_PRESCALER_16, 999);
    TIM2_OC2Init(TIM2_OCMODE_PWM2, TIM2_OUTPUTSTATE_ENABLE,500, TIM2_OCPOLARITY_HIGH);
    TIM2_OC2PreloadConfig(ENABLE);
    
  #elif PWM_Channel==PWM_Channel3
    TIM2_TimeBaseInit(TIM2_PRESCALER_16, 499);
    TIM2_OC3Init(TIM2_OCMODE_PWM1, TIM2_OUTPUTSTATE_ENABLE,250, TIM2_OCPOLARITY_HIGH);
    TIM2_OC3PreloadConfig(ENABLE);
  #endif
    
  TIM2_Cmd(ENABLE);

  
  
}

void SetTIM2_PWM_Frequency(uint16_t TIM2_Period)
{
      /* Set the Autoreload value */
  
    TIM2->ARRH = (uint8_t)(TIM2_Period >> 8);
    TIM2->ARRL = (uint8_t)(TIM2_Period);
}

void SetTIM2_PWM_DutyCycle( uint16_t TIM2_Pulse)
{
  
    
  #if PWM_Channel==PWM_Channel1
    /* Set the Pulse value */
    TIM2->CCR1H = (uint8_t)(TIM2_Pulse >> 8);
    TIM2->CCR1L = (uint8_t)(TIM2_Pulse);
    
  #elif PWM_Channel==PWM_Channel2
    TIM2->CCR2H = (uint8_t)(TIM2_Pulse >> 8);
    TIM2->CCR2L = (uint8_t)(TIM2_Pulse);
    
  #elif PWM_Channel==PWM_Channel3
    TIM2->CCR3H = (uint8_t)(TIM2_Pulse >> 8);
    TIM2->CCR3L = (uint8_t)(TIM2_Pulse);
    
  #endif
}

#if 0

static void Delay(u16 nCount)
{
  /* Decrement nCount value */
  while (nCount != 0)
  {
    nCount--;
  }
}
#endif
