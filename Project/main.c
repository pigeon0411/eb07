
/******************** (C) COPYRIGHT  ********************
 * 文件名  ：main.c
 * 描述    ：    
 * 库版本  ：V2.1.0
 *修改时间 ：2014-3-25
**********************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "stm8s.h"
#include "stm8s_clk.h"
#include "pwm.h"
#include "key.h"
#include "common.h"
#include "stm8s_wwdg.h"
#include <stdio.h>
#include <stdlib.h>




extern uint8_t Key_Scan(void);


u16 check_pd5_cnt = 0;

uint16_t Conversion_Value = 0;
__IO uint32_t TimingDelay = 0;
__IO uint32_t TimingDelayNoBlock_Cnt = 0;


u32 voltage_sys_error_overtime_cnt = 0;


/* Private function prototypes -----------------------------------------------*/


/* Private function prototypes -----------------------------------------------*/
void Delay_ms(__IO uint32_t nTime);
void TimingDelay_Decrement(void);
static void TIM4_Config(void);

/* Private functions ---------------------------------------------------------*/



void Key_init(void)
{
	//初始化按键对应的IO
	/* 按键初始化 */
	Key_Init(); 

}





void my_device_init(void)
{
	  /*设置内部高速时钟16M为主时钟*/ 
	CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);

	GPIO_DeInit(GPIOA);
	GPIO_DeInit(GPIOB);
	GPIO_DeInit(GPIOC);
	GPIO_DeInit(GPIOD);
	GPIO_DeInit(GPIOE);

	//作为系统时基
	TIM4_Config();


	Key_init();



	
}	



#define TIM4_PERIOD       124

/**
  * @brief  Configure TIM4 to generate an update interrupt each 1ms 
  * @param  None
  * @retval None
  */
static void TIM4_Config(void)
{
  /* TIM4 configuration:
   - TIM4CLK is set to 16 MHz, the TIM4 Prescaler is equal to 128 so the TIM1 counter
   clock used is 16 MHz / 128 = 125 000 Hz
  - With 125 000 Hz we can generate time base:
      max time base is 2.048 ms if TIM4_PERIOD = 255 --> (255 + 1) / 125000 = 2.048 ms
      min time base is 0.016 ms if TIM4_PERIOD = 1   --> (  1 + 1) / 125000 = 0.016 ms
  - In this example we need to generate a time base equal to 1 ms
   so TIM4_PERIOD = (0.001 * 125000 - 1) = 124 */

  /* Time base configuration */
  TIM4_TimeBaseInit(TIM4_PRESCALER_128, TIM4_PERIOD);
  /* Clear TIM4 update flag */
  TIM4_ClearFlag(TIM4_FLAG_UPDATE);
  /* Enable update interrupt */
  TIM4_ITConfig(TIM4_IT_UPDATE, ENABLE);
  
  /* enable interrupts */
  enableInterrupts();

  /* Enable TIM4 */
  TIM4_Cmd(ENABLE);
}

/**
  * @brief  Inserts a delay time.
  * @param  nTime: specifies the delay time length, in milliseconds.
  * @retval None
  */
void Delay_ms(__IO uint32_t nTime)
{
  TimingDelay = nTime;

  while (TimingDelay != 0);
}

void delay_second(__IO uint32_t nTime)
{
	u16 i=0;
	for(i=0;i<nTime;i++)
		Delay_ms(1000);

}





//static u32 led_TimingCounter_ms_bak = 0;

/**
  * @brief  Decrements the TimingDelay variable.
  * @param  None
  * @retval None
  */
void TimingDelay_Decrement(void)
{
    static u8 axis_data_get_time=0;

	
	if(voltage_sys_error_overtime_cnt < 0xFFFFFFFF)
	{
		voltage_sys_error_overtime_cnt++;
	}

	
    if (TimingDelay != 0x00)
    {
        TimingDelay--;
    } 



    if (TimingDelayNoBlock_Cnt != 0x00)
    {
        TimingDelayNoBlock_Cnt--;
    }



	
}


main(void)
{ 
	my_device_init();

	Delay_ms(1200);


	voltage_sys_error_overtime_cnt = 0;//用于检测按键
	while (1)
	{
		Key_Scan();

	}	

}





#ifdef USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param file: pointer to the source file name
  * @param line: assert_param error line source number
  * @retval : None
  */
void assert_failed(u8* file, u32 line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
	//printf("%s\n",file);
  /* Infinite loop */
  while (1)
  {
	
	
  }
}
#endif

