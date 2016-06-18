
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


#define		VERSION_CTL_MODE		1


#define     ADC_MY_ENABLE       1

#if VERSION_CTL_MODE==1
#define	ADC1_REF_MV_MIN_VALUE		2200
#define	ADC3_REF_MV_MIN_VALUE		800
#else
#define	ADC1_REF_MV_MIN_VALUE		2250
#define	ADC3_REF_MV_MIN_VALUE		500
#endif

#define		ADC3_STANDARD_MV		(ADC3_REF_MV_MIN_VALUE)  //mv

#define		ADC2_MODE_A				1
#define		ADC2_MODE_B				2

#define		ADC2_MODE_TYPE			ADC2_MODE_B

#if ADC2_MODE_TYPE==ADC2_MODE_A
#define	ADC2_REF_MIN_MV_VALUE		1000
#define	ADC2_REF_MAX_MV_VALUE		2000

#else
#define	ADC2_REF_MIN_MV_VALUE		2800
#define	ADC2_REF_MAX_MV_VALUE		3450

#endif


/* Private defines -----------------------------------------------------------*/
#define	ADC_INT		0
#define	LED_PA3_PIN		GPIO_PIN_3
#define	LED_PA2_PIN		GPIO_PIN_2

#define	PWM_TIME_CNT_ENABLE		0
#define	KEY_TIME_CNT_ENABLE		0
#define	ADC_TIME_CNT_ENABLE		0

/* Private variables defines -----------------------------------------------------------*/

u16 led_blink_cnt = 0; //unit: ms
u8 led_blink_mode = 0;

u16 pwm_adjust_first_time_cnt = 0;
u16 pd2_check_delay_cnt = 0;

u16 check_pd5_cnt = 0;

uint16_t Conversion_Value = 0;
__IO uint32_t TimingDelay = 0;
__IO uint32_t TimingDelayNoBlock_Cnt = 0;

u16 power_on_u3_delay_cnt = 0; //unit: ms
u16 power_off_u3_delay_cnt = 0; //unit: ms

u16 voltage_sys_error_overtime_cnt = 0;

u16 monitor_voltage_delay_cnt = 0; //unit: ms

__IO uint32_t ACC_TimingCounter_ms = 0; //accelerate get 


__IO uint32_t ADC_TimingCounter_ms = 0;
#if PWM_TIME_CNT_ENABLE
__IO uint32_t PWM_TimingCounter_ms = 0;
#endif

#if KEY_TIME_CNT_ENABLE

__IO uint32_t KEY_TimingCounter_ms = 0;
#endif

__IO uint32_t led_TimingCounter_ms = 0;

u16 pwm_duty_val=0,pwm_duty_pre_val = 0;

u8 led_state = 0;
#define	LED_BLINK_PER_1S		1
#define	LED_BLINK_PER_60MS		2
#define	LED_BLINK_PER_200MS		3
#define	LED_BLINK_PER_1S_THEN_PER_200MS		4

#define	LED_BLINK_NO_CHANGE		0
#define	LED_BLINK_STOP			0x0f


uint16_t adc_val;
u8 adc1_pd5_check_state = 0;
u8 voltage_system_type = 0;//12---->12V; 24---->24V


/* Private function prototypes -----------------------------------------------*/


/* Private function prototypes -----------------------------------------------*/
void Delay_ms(__IO uint32_t nTime);
void TimingDelay_Decrement(void);
static void TIM4_Config(void);
void ADC_check(void);
void led_blink_handle(void);
void pwm_power_off(void);
void PD2_temperature_monitor(void);
void current_led_blink_set(u8 mode);
void PD5_voltage_monitor(void);
void pwm_power_off_1(void);
void pwm_firt_adjust(void);
void adc_pd3_monitor(u8 type);
void power_on_check_first(void);
void PD2_temperature_check(void);
void PD5_voltage_monitor(void);
u8 current_get(void);
void Fan_power_on_off_set(u8 mode);

void heater_power_on_off_set(u8 mode);

/* Private functions ---------------------------------------------------------*/


void ADC_init(void)
{
//使用PD3作为ADC输入脚
#if 0
    GPIO_Init(GPIOB,GPIO_PIN_0,GPIO_MODE_IN_FL_NO_IT);
#else
    GPIO_Init(GPIOD,GPIO_PIN_2,GPIO_MODE_IN_FL_NO_IT);
 //   GPIO_Init(GPIOD,GPIO_PIN_2,GPIO_MODE_IN_FL_NO_IT);

#endif

	ADC1_PrescalerConfig(ADC1_PRESSEL_FCPU_D2);
	
	/* De-Init ADC peripheral*/
	ADC1_DeInit();

	/* Init ADC2 peripheral */
	ADC1_Init(ADC1_CONVERSIONMODE_SINGLE, ADC1_CHANNEL_3, ADC1_PRESSEL_FCPU_D2, \
	        ADC1_EXTTRIG_TIM, DISABLE, ADC1_ALIGN_RIGHT, ADC1_SCHMITTTRIG_CHANNEL4,\
	        DISABLE);


    ADC1_ScanModeCmd(DISABLE);
    
	/* Enable EOC interrupt */
#if ADC_INT
	ADC1_ITConfig(ADC1_IT_EOCIE,ENABLE);
#else
	ADC1_ITConfig(ADC1_IT_EOCIE,DISABLE);
#endif

	//使能缓冲区 共7个
	//ADC1_DataBufferCmd(ENABLE);

	/* Enable general interrupts */  
	enableInterrupts();

	ADC1_Cmd(ENABLE);

	/*Start Conversion */
	ADC1_StartConversion();
	
}

void Key_init(void)
{
	//初始化按键对应的IO
	/* 按键初始化 */
	Key_Init(); 

}



#define MY_GPIO_OUT_PORT_1        GPIOC
#define MY_GPIO_OUT_PORT_2        GPIOD

#define     GPIO_OUT_MODE_MY      GPIO_MODE_OUT_PP_HIGH_FAST  
void GPIO_Output_init(void)
{
	//初始化输出IO

    GPIO_Init(GPIOD,GPIO_PIN_4,GPIO_OUT_MODE_MY);

	GPIO_Init(GPIOA,GPIO_PIN_1,GPIO_OUT_MODE_MY);

//初始化低电平
    GPIO_WriteLow(GPIOD,GPIO_PIN_4);
    GPIO_WriteLow(GPIOA,GPIO_PIN_1);

//初始化高电平
   // GPIO_WriteHigh(MY_GPIO_OUT_PORT_1,GPIO_PIN_7);

}

void heater_pin_init(void)
{
	//初始化输出IO

    GPIO_Init(GPIOB,GPIO_PIN_4,GPIO_OUT_MODE_MY);
    GPIO_Init(GPIOB,GPIO_PIN_5,GPIO_OUT_MODE_MY);
    GPIO_Init(GPIOC,GPIO_PIN_3,GPIO_OUT_MODE_MY);
    GPIO_Init(GPIOC,GPIO_PIN_4,GPIO_OUT_MODE_MY);


//初始化低电平
    GPIO_WriteLow(GPIOB,GPIO_PIN_4);
    GPIO_WriteLow(GPIOB,GPIO_PIN_5);
    GPIO_WriteLow(GPIOC,GPIO_PIN_3);
    GPIO_WriteLow(GPIOC,GPIO_PIN_4);

//初始化高电平
   // GPIO_WriteHigh(MY_GPIO_OUT_PORT_1,GPIO_PIN_7);

}

void GPIO_Input_init(void)
{
	GPIO_Init(GPIOB,GPIO_PIN_5, GPIO_MODE_IN_PU_NO_IT); //
	GPIO_Init(GPIOA,GPIO_PIN_3, GPIO_MODE_IN_PU_NO_IT); //

}

void fan_pin_init(void)
{
	GPIO_Init(GPIOA,GPIO_PIN_1,GPIO_OUT_MODE_MY);
	GPIO_Init(GPIOA,GPIO_PIN_2,GPIO_OUT_MODE_MY);
    GPIO_WriteLow(GPIOA,GPIO_PIN_1);
    GPIO_WriteLow(GPIOA,GPIO_PIN_2);

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


 #if ADC_MY_ENABLE
    // ADC 引脚初始化
    ADC_init();
#endif
	PWM_Init();

	heater_pin_init();
	fan_pin_init();
	
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


#if 0
//return: 0,not reac the count; 1,the count is over
u8 delay_ms_no_block(__IO uint32_t nTime)
{
	static u8 me_state = 0;

	if(me_state == 0)
	{
		if(TimingDelayNoBlock_Cnt)
		{
			TimingDelayNoBlock_Cnt = nTime;
		}
		else
		{
			TimingDelayNoBlock_Cnt = nTime;
			me_state = 1;
		}
		return 0;
	}
	else
	{
		if(TimingDelayNoBlock_Cnt)
		{
			return 0;
		}
		else
		{
			me_state = 0;
			return 1;
		}
	}
}
#endif


//static u32 led_TimingCounter_ms_bak = 0;

/**
  * @brief  Decrements the TimingDelay variable.
  * @param  None
  * @retval None
  */
void TimingDelay_Decrement(void)
{
    static u8 axis_data_get_time=0;

	
	if(voltage_sys_error_overtime_cnt < 0xFFFF)
	{
		voltage_sys_error_overtime_cnt++;
	}

	if(check_pd5_cnt > 0)
		check_pd5_cnt--;


	if(pd2_check_delay_cnt > 0)
		pd2_check_delay_cnt--;

	
	if(pwm_adjust_first_time_cnt > 0)
		pwm_adjust_first_time_cnt--;

	if(monitor_voltage_delay_cnt > 0)
		monitor_voltage_delay_cnt--;
	
    if (TimingDelay != 0x00)
    {
        TimingDelay--;
    } 

    if (ADC_TimingCounter_ms != 0x00)
    {
        ADC_TimingCounter_ms--;
    }


    if (ACC_TimingCounter_ms != 0x00)
    {
        ACC_TimingCounter_ms--;
    }
	
#if PWM_TIME_CNT_ENABLE

    if (PWM_TimingCounter_ms != 0x00)
    {
        PWM_TimingCounter_ms--;
    }
#endif

#if KEY_TIME_CNT_ENABLE

    if (KEY_TimingCounter_ms != 0x00)
    {
        KEY_TimingCounter_ms--;
    }
#endif

    if (led_TimingCounter_ms != 0x00)
    {
        led_TimingCounter_ms--;
    }

    if (TimingDelayNoBlock_Cnt != 0x00)
    {
        TimingDelayNoBlock_Cnt--;
    }

   if(axis_data_get_time >= 10)
    {

        axis_data_get_time = 0;
   }
   else
    axis_data_get_time++;

	if(power_on_u3_delay_cnt != 0x00)
		power_on_u3_delay_cnt--;

	
	if(power_off_u3_delay_cnt != 0x00)
		power_off_u3_delay_cnt--;

	if(led_blink_cnt > 0)
		led_blink_cnt--;
	led_blink_handle();
	
}

//====================================================================

/**************************************************************
** 函数名:DigitFilter
** 功能:软件滤波
** 注意事项:取NO 的2/5 作为头尾忽略值,注意N 要大于5,否则不会去头尾
***************************************************************/
#define	ADC_SAMPLE_NUM	16	//ADC值抽样数	
#define	ADC_MIDDLE_START_NUM	(ADC_SAMPLE_NUM/5)
#define	ADC_MIDDLE_END_NUM		(ADC_SAMPLE_NUM - ADC_MIDDLE_START_NUM)
#define	ADC_SAMPLE_VALID_SIZE	(ADC_SAMPLE_NUM - ADC_MIDDLE_START_NUM * 2)	

u16 DigitFilter(u16* buf,u8 no)
{
	u8 i,j;
	u32 tmp;
	u16 Pravite_ADC_buf[16];
	
	for(i=0;i<no;i++)
	{
		Pravite_ADC_buf[i] = buf[i];
	 	buf[i] = 0;
	}	
	//排序，将buf[0]到buf[no-1]从大到小排列
	for(i=0;i<no;i++)
	{
		for(j=0;j<no-i-1;j++)
		{
			if(Pravite_ADC_buf[j]>Pravite_ADC_buf[j+1])
			{
				tmp=Pravite_ADC_buf[j];
				Pravite_ADC_buf[j]=Pravite_ADC_buf[j+1];
				Pravite_ADC_buf[j+1]=tmp;
			}
		}
	}


	//平均
	tmp=0;
	//for(i=cut_no;i<no-cut_no;i++) //只取中间n-2*cut_no 个求平均
	for(i=ADC_MIDDLE_START_NUM;i<ADC_MIDDLE_END_NUM;i++) //只取中间n-2*cut_no 个求平均
		tmp+=Pravite_ADC_buf[i];
	return(tmp/ADC_SAMPLE_VALID_SIZE);
}

//ADC采样处理
//1A -> 40mv


#define	reference_mV	(u32)5100//5000//5060 //参考电压

volatile u16 Photoreg_ADC1_ConvertedValue[ADC_SAMPLE_NUM];

//#if ADC2_MODE_TYPE==ADC2_MODE_A

////mv 
//const u16 adc_ch1_out_mv_table[] = 
//{
// // 1A  7A  9A  11A 13A   15A
//    //40,280,360,440, 520,  600
////  2527,2742,2814,2887,2957,3030
////	0, 1030,1150,1270,1390,1510,//1630,1770	

////  NC   11A  13A   15A    17A     19A  
//    0,	1260,1380, 1500,  1620,  1740,//1630,1770		

//};
//#else
//const u16 adc_ch1_out_mv_table[] = 
//{
// // 1A  7A  9A  11A 13A   15A
//    //40,280,360,440, 520,  600
////  2527,2742,2814,2887,2957,3030
////	0, 1030,1150,1270,1390,1510,//1630,1770	

////  NC   11A  13A   15A    17A     19A  
//    0,	2960,3040, 3120,  3200,  3280,//1630,1770		

//};

//#endif



#if 1
// return mv 返回的是mv
u16 adc1_read_by_chanel(u8 chn)
{
  u32 adctmp;
	
#if 1
	
	ADC1_Init(ADC1_CONVERSIONMODE_SINGLE,(ADC1_Channel_TypeDef)chn, ADC1_PRESSEL_FCPU_D2,\
     ADC1_EXTTRIG_TIM, DISABLE, ADC1_ALIGN_RIGHT, ADC1_SCHMITTTRIG_ALL, DISABLE);

	ADC1_ConversionConfig(ADC1_CONVERSIONMODE_SINGLE, (ADC1_Channel_TypeDef)chn,ADC1_ALIGN_RIGHT);
	
    ADC1->CSR |= (ADC1_CSR_CH & chn); //选择通道       

#endif
    //start ADC
    ADC1->CR1 |= ADC1_CR1_ADON;                //ADC powered up
    ADC1->CR1 |= ADC1_CR1_ADON;                //begin conversion        
    u8 no_of_sample_vals = 0;
    while( no_of_sample_vals < ADC_SAMPLE_NUM )        
    {   
        //check for the EOC 等待转换完成
        while (!(ADC1->CSR & ADC1_CSR_EOC)){}; 
        
        //must read LSBs first
        Photoreg_ADC1_ConvertedValue[no_of_sample_vals] = ADC1_GetConversionValue();                                       

        no_of_sample_vals++;
    }

        //ADC powered down
    ADC1->CR1 |= ~ADC1_CR1_ADON;        

	adctmp = (u32)(DigitFilter((u16 *)Photoreg_ADC1_ConvertedValue,ADC_SAMPLE_NUM));

	adctmp = (u32)(adctmp) * reference_mV/1023u;

    return((u16)adctmp);
}

#else
u16 adc1_read_by_chanel(u8 chn)
{
	
	//ADC1_Cmd(ENABLE);//若设置的ADC转换方式为连续转换，则不需要每次都调用该开启函数
 //   while(!(ADC1->CSR & 0X80));//等转换结束

    ADC1->CSR |= (ADC1_CSR_CH & chn); //选择通道       
    Delay_ms(1);

    u8 no_of_sample_vals = 0;
    while( no_of_sample_vals < ADC_SAMPLE_NUM )        
    {    
        //start ADC
        ADC1->CR1 |= ADC1_CR1_ADON;                //ADC powered up
        ADC1->CR1 |= ADC1_CR1_ADON;                //begin conversion        

        //check for the EOC 等待转换完成
        while (!(ADC1->CSR & ADC1_CSR_EOC)){}; 
        
        //must read LSBs first
        Photoreg_ADC1_ConvertedValue[no_of_sample_vals] = ADC1_GetConversionValue();                                       

        //ADC powered down
        ADC1->CR1 |= ~ADC1_CR1_ADON;        

        no_of_sample_vals++;
    }

    return(DigitFilter((u16 *)Photoreg_ADC1_ConvertedValue,ADC_SAMPLE_NUM));
}
#endif

#if 0
//读取缓冲并进行软件滤波
u16 adc1_buffer_read_with_filter(void)
{
	//ADC1_Cmd(ENABLE);

	u8 i;
	
	while(!(ADC1->CSR & 0X80));//等转换结束
	for(i=0;i<7;i++)
	{
		Photoreg_ADC1_ConvertedValue[i] = ADC1_GetBufferValue(i);
	}

	while(!(ADC1->CSR & 0X80));//等转换结束
	for(;i<14;i++)
	{
		Photoreg_ADC1_ConvertedValue[i] = ADC1_GetBufferValue(i/8);
	}

	return(DigitFilter((u16 *)Photoreg_ADC1_ConvertedValue,ADC_SAMPLE_NUM));
}
#endif

#if 0
u16 ADC1_milivoiltage_read(void)
{
	uint16_t adc_val;
	u32	mV_val;
	
#if ADC_INT	
	adc_val = Conversion_Value;
#else
	adc_val = adc1_buffer_read_with_filter();
#endif

	//mV_val = (u32)(adc_val/1023u) * reference_mV;//adc_val/1024 //10位精度 
	mV_val = (u32)(adc_val) * reference_mV/1023u;//adc_val/1024 //10位精度 

	return (u16)mV_val;
}
#endif

#if 0
void ADC_sample_handle(void)
{
	uint16_t adc_val;
	u32	mV_val;
	
#if ADC_INT	
	adc_val = Conversion_Value;
#else
	adc_val = adc1_buffer_read_with_filter();
#endif

	//mV_val = (u32)(adc_val/1023u) * reference_mV;//adc_val/1024 //10位精度 
	mV_val = (u32)(adc_val) * reference_mV/1023u;//adc_val/1024 //10位精度 

	if(mV_val > 40)
	{
		SetTIM2_PWM_DutyCycle(Duty2TimerVal(100));
        pwm_duty_val = 100;
	}
}
#endif



//value,为占空比 0-100 (%)的占空比
void pwm_handle_div_100(u16 value)
{
	pwm_duty_val = value;
	SetTIM2_PWM_DutyCycle(Duty2TimerVal(value));
}


#if 1
//value,为占空比 
void pwm_handle(u16 value)
{
	if(value > PWM_PERIOD_VAL)
		value = PWM_PERIOD_VAL;
	pwm_duty_val = value;
	SetTIM2_PWM_DutyCycle(value);
}

#else
//value,为占空比 
void pwm_handle(u16 value)
{
	pwm_duty_val = value;
	SetTIM2_PWM_DutyCycle(Duty2TimerVal(value));
}
#endif

#define PWM_DUTY_DELAY  1000

u8 target_current_val = 0;


////mv 
//const u16 adc_ch1_out_mv_table[] = 
//{
// // 1A  7A  9A  11A 13A   15A
//    40,280,360,440, 520,  600
//};

#define	OutMvErrorRange		10 //mv




u16 mv_val,mv_val3;

//val 为占空比 0-100 (%)的占空比
void pwm_up_set_div_100(u16 val)
{
	if (val >100)
		val = 100;
	for(u8 i=0;i<=val;i++)
	{
		Delay_ms(200);
		pwm_handle_div_100(i);
	
	}

}

void pwm_up_set(u16 val)
{
	if (val >100)
		val = 100;
	for(u8 i=0;i<=val;i++)
	{
		Delay_ms(200);
		pwm_handle(i);
	
	}

}


void alarm_handle(u8 AINchanel)
{

      GPIO_WriteLow(GPIOA,GPIO_PIN_1);
      
      GPIO_WriteHigh(GPIOA,GPIO_PIN_1);

}

u32	adc1_mV_val,adc3_mV_val;


#define     ADC_CHECK_DELAY     1

void ADC_check(void)
{
	
    adc1_pd5_check_state = 0;

    	adc1_mV_val = adc1_read_by_chanel(ADC2_PD3_CHN);
    	if(adc1_mV_val < 2180)//
    	{
			Delay_ms(ADC_CHECK_DELAY);
        	adc1_mV_val = adc1_read_by_chanel(ADC2_PD3_CHN);
        	if(adc1_mV_val < 2180)//
            {
                GPIO_WriteLow(MY_GPIO_OUT_PORT_2,GPIO_PIN_4);
                GPIO_WriteLow(MY_GPIO_OUT_PORT_2,GPIO_PIN_5);
                GPIO_WriteLow(MY_GPIO_OUT_PORT_2,GPIO_PIN_6);
                GPIO_WriteLow(MY_GPIO_OUT_PORT_2,GPIO_PIN_7);
            }   
			adc1_pd5_check_state = 0;
    	}
    	else if((adc1_mV_val >= 2180) && (adc1_mV_val <= 2250))//
    	{
			Delay_ms(ADC_CHECK_DELAY);
        	adc1_mV_val = adc1_read_by_chanel(ADC2_PD3_CHN);
        	if((adc1_mV_val >= 2180) && (adc1_mV_val <= 2250))
            {
                GPIO_WriteHigh(MY_GPIO_OUT_PORT_2,GPIO_PIN_4);
                GPIO_WriteLow(MY_GPIO_OUT_PORT_2,GPIO_PIN_5);
                GPIO_WriteLow(MY_GPIO_OUT_PORT_2,GPIO_PIN_6);
                GPIO_WriteLow(MY_GPIO_OUT_PORT_2,GPIO_PIN_7);
            }   
			adc1_pd5_check_state = 0;
    	}
    	else if((adc1_mV_val >= 2250) && (adc1_mV_val <= 2320))//
    	{
			Delay_ms(ADC_CHECK_DELAY);
        	adc1_mV_val = adc1_read_by_chanel(ADC2_PD3_CHN);
        	if((adc1_mV_val >= 2250) && (adc1_mV_val <= 2320))
            {
                GPIO_WriteHigh(MY_GPIO_OUT_PORT_2,GPIO_PIN_4);
                GPIO_WriteHigh(MY_GPIO_OUT_PORT_2,GPIO_PIN_5);
                GPIO_WriteLow(MY_GPIO_OUT_PORT_2,GPIO_PIN_6);
                GPIO_WriteLow(MY_GPIO_OUT_PORT_2,GPIO_PIN_7);
            }   
			adc1_pd5_check_state = 0;
    	}
    	else if((adc1_mV_val >= 2320) && (adc1_mV_val <= 2370))//
    	{
			Delay_ms(ADC_CHECK_DELAY);
        	adc1_mV_val = adc1_read_by_chanel(ADC2_PD3_CHN);
        	if((adc1_mV_val >= 2320) && (adc1_mV_val <= 2370))
            {
                GPIO_WriteHigh(MY_GPIO_OUT_PORT_2,GPIO_PIN_4);
                GPIO_WriteHigh(MY_GPIO_OUT_PORT_2,GPIO_PIN_5);
                GPIO_WriteHigh(MY_GPIO_OUT_PORT_2,GPIO_PIN_6);
                GPIO_WriteLow(MY_GPIO_OUT_PORT_2,GPIO_PIN_7);
            }   
			adc1_pd5_check_state = 0;
    	}
    	else if((adc1_mV_val > 2370))//
    	{
			Delay_ms(ADC_CHECK_DELAY);
        	adc1_mV_val = adc1_read_by_chanel(ADC2_PD3_CHN);
        	if((adc1_mV_val >2370))
            {
                GPIO_WriteHigh(MY_GPIO_OUT_PORT_2,GPIO_PIN_4);
                GPIO_WriteHigh(MY_GPIO_OUT_PORT_2,GPIO_PIN_5);
                GPIO_WriteHigh(MY_GPIO_OUT_PORT_2,GPIO_PIN_6);
                GPIO_WriteHigh(MY_GPIO_OUT_PORT_2,GPIO_PIN_7);
            }   
			adc1_pd5_check_state = 0;
    	}        
        
}

signed short adxl335_x_data=0;
//signed short adxl335_x_data_buffer[10];

//  Rx= 100*((((sstatex/1023.0)*5)-1.645)/0.329);
s16 Axis_adxl335_read(u8 dir)
{
	s16 acctemp;
	
    acctemp = adc1_read_by_chanel(dir);
    // 0-5v 对应 0-1024  
		// 3.3v/2  = 1.65v（0g时候值）
		// 1.65v ->= 338   (AD值即0X0152)
	acctemp -= 1650; //
	return acctemp;
}


void led_on_off_set(u8 state)
{
    {
        if(state == ON)
        {
			TIM2_Cmd(ENABLE);

        	GPIO_WriteHigh(GPIOD,GPIO_PIN_4);
    	}
        else
        {
		  	TIM2_Cmd(DISABLE);

        	GPIO_WriteLow(GPIOD,GPIO_PIN_4);
    	}
    }

}


const u16 blink_fast_4_array[] = {800,800,300,300,300,300,300,300,300,300,300,300};

// called by TimingDelay_Decrement()
void led_blink_handle(void)
{
    static u8 i=0,ii = 0;
    //u8 re_time = 0;
    
    switch(led_blink_mode)
    {
    case BLINK_NORMAL:
        if(!led_blink_cnt)
        {
			
			if(i++>100)
			{
				ii = 1;
				i = 100;

			}

			if(ii)
			{
				if(i-- == 0)
				{
					ii = 0;
				}
				
			}

			pwm_handle_div_100(i--);

			if(i)
            	led_blink_cnt = 15;
			else
				led_blink_cnt = 15;
        }
        else
        {
            ;
        }
        break;
    case BLINK_SLOW:
        if(!led_blink_cnt)
        {
            i = !i;
            led_on_off_set(i);
            led_blink_cnt = 1000;
        }
        else
        {
            ;
        }
        break;
    case BLINK_FAST:
        if(!led_blink_cnt)
        {
            i = !i;
            led_on_off_set(i);
            led_blink_cnt = 300;
        }
        else
        {
            ;
        }
        break;    
    case BLINK_FAST_4:
        if(!led_blink_cnt)
        {
        
            i = !i;
            led_on_off_set(i);

            led_blink_cnt = blink_fast_4_array[ii];
            if(ii >= 5)//if(ii >= 9)
            {
                ii = 0;
                
            }
            else
            {
                ii++;
            }
            
        }
        else
        {
            ;
        }
        break;  
    case BLINK_FAST_5:
        if(!led_blink_cnt)
        {
            i = !i;
            led_on_off_set(i);
            led_blink_cnt = blink_fast_4_array[ii];
            if(ii >= 11)
            {
                ii = 0;
                
            }
            else
            {
                ii++;
            }
        }
        else
        {
            ;
        }
        break;  
	case BLINK_NONE:
	case BLINK_STOP:
		led_blink_mode = BLINK_NONE;
		led_on_off_set(ON);
		break;
	case BLINK_OFF:
		led_blink_mode = BLINK_NONE;
		led_on_off_set(OFF);
		break;
    default:
        break;
    }
}

void led_blink_set(enum BLINK_MODE mode)
{
	led_blink_mode = mode;

	if(mode == BLINK_STOP)
	{
		TIM2_Cmd(DISABLE);
	}
	else
	{
		TIM2_Cmd(ENABLE);
	}
}

//return: 1,high; 0,low
u8 power_onoff_switch_detect(void)
{
	if(!GPIO_ReadInputPin(GPIOA,GPIO_PIN_3))
	{
		Delay_ms(5);
		if(!GPIO_ReadInputPin(GPIOA,GPIO_PIN_3))
		{
			return 0;
		}
	}

	return 1;
}

void system_voltage_error(void)
{
	
	while(1);
}

void power_on_U3_set(void)
{
    GPIO_WriteHigh(GPIOD,GPIO_PIN_4);
}

void power_off_U3_set(void)
{
    GPIO_WriteLow(GPIOD,GPIO_PIN_4);
}

#if 1
void power_off_U2_set(void)
{
    //GPIO_WriteHigh(GPIOA,GPIO_PIN_3);
}

#endif

void power_off_monitor(void)
{
	if(!power_off_u3_delay_cnt)
	{
		if(GPIO_ReadInputPin(GPIOB,GPIO_PIN_5))
		{
			power_off_U3_set();
		}
		power_off_u3_delay_cnt = 1000;  //  1s
	}
}

void power_on_U3_handle(void)
{
	if(voltage_system_type == VOLTAGE_SYSTEM_12V_TYPE || voltage_system_type == VOLTAGE_SYSTEM_24V_TYPE)
	{
		

	}
}

//check the power on delay time,then initial the counter
void power_on_delay_U3_init(void)
{
	if(GPIO_ReadInputPin(GPIOC,GPIO_PIN_7))
	{
		power_on_u3_delay_cnt = 10000;
	}
	else
	{
		power_on_u3_delay_cnt = 0;
	}
}

void voltage_system_pin_set(void)
{
	if(voltage_system_type == VOLTAGE_SYSTEM_12V_TYPE)
	{
		GPIO_WriteHigh(GPIOA, GPIO_PIN_1);
		GPIO_WriteLow(GPIOA, GPIO_PIN_2);
	}
	else if(voltage_system_type == VOLTAGE_SYSTEM_24V_TYPE)
	{
		GPIO_WriteLow(GPIOA, GPIO_PIN_1);
		GPIO_WriteHigh(GPIOA, GPIO_PIN_2);
	}
	else
	{
		GPIO_WriteLow(GPIOA, GPIO_PIN_2);
		GPIO_WriteLow(GPIOA, GPIO_PIN_1);
	}
}

#define	ACC_DATA_GET_PERIOD		10 //MS
#define	ACC_DATA_GET_PERIOD_UNIT_SECOND		(double)(0.01) //S


//u16 system_voltage_power_on_monitor = 0;
u8 system_voltage_power_off_num = 0;
#define	SYSTEM_VOLTAGE_POWER_OFF_TIMES		5

u8 system_current_type = 6;


#if 0
void pwm_power_off(void)
{
	u8 i;
	
	for( i = pwm_duty_val;i > 0;i--)
	{
		pwm_handle(i);
		Delay_ms(50);
	}
}
#else
void pwm_power_off(void)
{
	u16 i;

	
		for( i = pwm_duty_val;i > 0;i--)
		{
			pwm_handle(i);
			Delay_ms(20);
		}
}

#endif

void power_on_with_delay(void)
{


	WWDG_SWReset();



}


void pwm_off_and_system_reboot(u16 delayseconds)
{
	pwm_power_off();

	if(GPIO_ReadInputPin(GPIOB,GPIO_PIN_5))
		delay_second(delayseconds);
	
	power_on_with_delay();
}




#if 1
void pwm_firt_adjust(void)
{
	u16 mv_temp;
	u16 i;
	//u8 Key_val;

	
	while(1)
	{
		//Key_val=Key_Scan();  

		//current_led_blink_set(Key_val);//led_blink_set(BLINK_SLOW);
			
		//if(power_onoff_switch_detect())
		{
			led_blink_set(BLINK_SLOW);
			
			mv_temp = adc1_read_by_chanel(CURRENT_VERIFY_ADC_PORT);
			i = pwm_duty_val;


			pwm_adjust_first_time_cnt = 30000;
			while(1)
			{
				if(!pwm_adjust_first_time_cnt)
					return;

				if(i >= PWM_PERIOD_VAL)
					return;
				
				if(mv_temp < CURRENT_VOL_STANDARD_VAL)
				{
					if(i < PWM_PERIOD_VAL)
						i++;
					else
					{
						;
					}
				}
				else
				{
					if(i > 0)
						i--;
					else
					{
						;
					}
				}

				//current_led_blink_set(Key_val);
				

				pwm_handle(i);
				Delay_ms(40);
				mv_temp = adc1_read_by_chanel(CURRENT_VERIFY_ADC_PORT);
				if(abs(mv_temp - CURRENT_VOL_STANDARD_VAL) < OutMvErrorRange)
				{// ok
					break;
				}

				//system_voltage_power_on_monitor_1(1);

				//PD2_temperature_monitor();
				
			}
		
			//led_blink_set(BLINK_STOP);
			return;
		}

	}
}
#else
void pwm_firt_adjust(void)
{
	u16 mv_temp;
	u8 i;

	led_blink_set(BLINK_SLOW);
	
	mv_temp = adc1_read_by_chanel(CURRENT_VERIFY_ADC_PORT);

	if(mv_temp < CURRENT_VOL_STANDARD_VAL)
	{
		for( i = pwm_duty_val;i < PWM_PERIOD_VAL;i++)
		{
			pwm_handle(i);
			Delay_ms(80);

			mv_temp = adc1_read_by_chanel(CURRENT_VERIFY_ADC_PORT);
			if(abs(mv_temp - CURRENT_VOL_STANDARD_VAL) < OutMvErrorRange)
			{// ok
				break;
			}
			
		}
	}
	else
	{
		for( i=pwm_duty_val;i >= 0;i--)
		{
			pwm_handle(i);
			Delay_ms(80);

			mv_temp = adc1_read_by_chanel(CURRENT_VERIFY_ADC_PORT);
			if(abs(mv_temp - CURRENT_VOL_STANDARD_VAL) < OutMvErrorRange)
			{// ok
				break;
			}
			else
			{
				
			}
		}
	}

	led_blink_set(BLINK_STOP);

}
#endif


//u16 current_to_voltage_table[] = {0,2680,2920,3040,3160};

void current_led_blink_set(u8 mode)
{


	if(mode == 1)
		{
		led_blink_set(BLINK_SLOW);

	}
	else
		led_blink_set(BLINK_NORMAL);

}



#if 0
void pwm_adjust_current(void)
{
	u16 mv_temp;
	u16 i;
    u8 Key_val=0xff;

	//while(1)
	{
		if(power_onoff_switch_detect())
		{
						
			Key_val=Key_Scan();  

			current_led_blink_set(Key_val);//led_blink_set(BLINK_SLOW);

			
			pwm_duty_pre_val = pwm_duty_val;
			mv_temp = adc1_read_by_chanel(CURRENT_VERIFY_ADC_PORT);
			i = pwm_duty_val;
			
			while(1)
			{
				Key_val=Key_Scan();  
				pwm_duty_pre_val = pwm_duty_val;
				//mv_temp = adc1_read_by_chanel(CURRENT_VERIFY_ADC_PORT);

				if(mv_temp < current_to_voltage_table[Key_val])
				{
					if(i < PWM_PERIOD_VAL)
						i++;
					else
					{
						;
					}
				}
				else
				{
					if(i > 0)
						i--;
					else
					{
						;
					}
				}

				if(mv_temp < 2600)
					led_blink_set(BLINK_STOP);
				else
					current_led_blink_set(Key_val);
				pwm_handle(i);
				//Delay_ms(5);
				mv_temp = adc1_read_by_chanel(CURRENT_VERIFY_ADC_PORT);
				if(abs(mv_temp - current_to_voltage_table[Key_val]) < OutMvErrorRange)
				{// ok
					//speed_warning_out(0);
					break;
				}

				system_voltage_power_on_monitor(1);
				#if 0
				if(!GPIO_ReadInputPin(GPIOD,GPIO_PIN_1))
					{
							pwm_power_off();
							led_blink_set( BLINK_OFF);
						}
				#endif
			}
		
			//led_blink_set(BLINK_STOP);
			return;
		}
		else
		{
			if(!power_onoff_switch_detect())
			{
				pwm_power_off();//continue;
				led_blink_set( BLINK_OFF);

			}
		}
	}
}
#else
void pwm_adjust_current(void)
{
	u16 mv_temp;
	u16 i;
	u8 Key_val=0xff;


			//current_led_blink_set(Key_val);//led_blink_set(BLINK_SLOW);

			//led_blink_set(BLINK_NORMAL);

							
			pwm_duty_pre_val = pwm_duty_val;
			mv_temp = adc1_read_by_chanel(CURRENT_VERIFY_ADC_PORT);
			i = pwm_duty_val;
			
			while(1)
			{
				Key_val=current_get();//Key_Scan();  

				if(abs(mv_temp - current_to_voltage_table[Key_val]) < OutMvErrorRange)
				{// ok
				
					return;
				}


				if(mv_temp < VOLTAGE_ADC_PD6_PA1PA3_HIGH_VAL)
					led_blink_set(BLINK_STOP);
				else
				{
					if(system_current_type == 1)
						led_blink_set( BLINK_SLOW);
					else if(system_current_type == 12)
						led_blink_set( BLINK_NORMAL);
				}

				
				pwm_duty_pre_val = pwm_duty_val;
				//mv_temp = adc1_read_by_chanel(CURRENT_VERIFY_ADC_PORT);

				if(mv_temp < current_to_voltage_table[Key_val])
				{
					if(i < PWM_PERIOD_VAL)
					{
						i++;
						if(i >= PWM_PERIOD_VAL)
							break;
					}
					else
					{
						break;
					}
				}
				else
				{
					if(i > 0)
					{
						i--;
						if(!i)
							break;
					}
					else
					{
						break;
					}
					
				}

				
				pwm_handle(i);
				//Delay_ms(5);
				mv_temp = adc1_read_by_chanel(CURRENT_VERIFY_ADC_PORT);


				//adc_pd3_monitor(1);

			}
		
			//led_blink_set(BLINK_STOP);
			return;
}

#endif

uint8_t Key_Scan()
{
    uint8_t key_value;

	
    key_value=GPIO_ReadInputData(GPIOC);
    if( (key_value & GPIO_PIN_6) && (key_value & GPIO_PIN_7))
    {
		key_value = 3;//key_value = 2;
	}
	else if(!(key_value & GPIO_PIN_6) && (key_value & GPIO_PIN_7))
	{
		key_value = 2;//key_value = 3;
	}
	else if((key_value & GPIO_PIN_6)&&!(key_value & GPIO_PIN_7))
	{
		key_value = 4;
	}
	else
		key_value = 2;

		
    return key_value;
} 

u8 current_get(void)
{
	if(system_current_type == 1)
		return 1;
	else
	{

		return Key_Scan();
	}
}

u8 temperature_poweroff_retry_cnt =0;




void PD2_temperature_monitor(void)
{
	u16 temp;

	if(!pd2_check_delay_cnt)
	{
		temp = adc1_read_by_chanel(TEMPERATURE_ADC_PORT);
		if(temp < VOLTAGE_ADC_PD2_MIN || temp > VOLTAGE_ADC_PD2_MAX)
		{
			if(temp < 2850)// 超过40度 ADC值越小，温度越高
			{
				Fan_power_on_off_set(ON);

				led_blink_set(BLINK_FAST);
			}
			else if(temp > 3100)
			{
				Fan_power_on_off_set(OFF);

			}
			
			if(temperature_poweroff_retry_cnt > 5 )
			{
				
				
			}
			else
				temperature_poweroff_retry_cnt++;
		}
		else if(temp < VOLTAGE_ADC_PD2_HEATER_MIN || temp > VOLTAGE_ADC_PD2_HEATER_MAX)
		{
			if(temp < 2850)// 超过40度 ADC值越小，温度越高
			{
				heater_power_on_off_set(ON);
			}
			else if(temp > 3100)
			{
				heater_power_on_off_set(OFF);

			}
			
			if(temperature_poweroff_retry_cnt > 5 )
			{
				led_blink_set(BLINK_STOP);
				
			}
			else
				temperature_poweroff_retry_cnt++;
		}
		else
		{
			temperature_poweroff_retry_cnt = 0;

			led_blink_set(BLINK_NORMAL);
		}
		pd2_check_delay_cnt = 1000;
	}
}

//6: 500mv,12A:


u8 pd5_poweroff_check_retry_cnt = 0;


void PD5_voltage_monitor(void)
{
	u16 temp;
	//u8 Key_val = 0;
	//static u8 key_val_pre = 0;
	
	u16 voltemp = VOLTAGE_ADC_PD5_NOT6A_POWEROFF_VAL;

	
	if(system_current_type == 1)
	{
		voltemp = VOLTAGE_ADC_PD5_6A_POWEROFF_VAL;
	}

	if(!check_pd5_cnt)
	{
		temp = adc1_read_by_chanel(VOLTAGE_SYSTEM_ADC_PORT);

		if(temp < voltemp)
		{
			if(pd5_poweroff_check_retry_cnt > 10)
			{
				led_blink_set(BLINK_FAST);
				pwm_power_off();
				while(1)
				{

					temp = adc1_read_by_chanel(VOLTAGE_SYSTEM_ADC_PORT);
					if(temp > voltemp)
						break;
				}
			}
			else
			{
				temp = adc1_read_by_chanel(VOLTAGE_SYSTEM_ADC_PORT);
				if(temp < voltemp)
					pd5_poweroff_check_retry_cnt++;
			}
		}
		else
			pd5_poweroff_check_retry_cnt = 0;

		check_pd5_cnt = 200;
	}
}



void adc_pd3_check(void)
{
	u16 temp;
	u8 detect_system_verify_times = 0;
	
	while(1)
	{	 
		Delay_ms(100);
		temp = adc1_read_by_chanel(VOLTAGE_SYSTEM_VERIFY_ADC_PORT);

		
		
		if(temp>=VOLTAGE_SYSTEM_12V_MIN && temp<=VOLTAGE_SYSTEM_12V_MAX)
		{

			if(detect_system_verify_times >= 5)
			{
				voltage_system_type = VOLTAGE_SYSTEM_12V_TYPE;
				break;

			}
			else
				detect_system_verify_times++;
			
			
		}
		else
		{
			//system_voltage_error();
			detect_system_verify_times = 0;
			continue;
		}
	}
	
}


void Fan_power_on_off_set(u8 mode)
{
	if(mode == OFF)
	{
		GPIO_WriteLow(GPIOA,GPIO_PIN_1);
		GPIO_WriteLow(GPIOA,GPIO_PIN_2);

	}
	else
	{

	    GPIO_WriteHigh(GPIOA,GPIO_PIN_1);
	    GPIO_WriteHigh(GPIOA,GPIO_PIN_2);

	}
}

void heater_power_on_off_set(u8 mode)
{
	if(mode == OFF)
	{
		GPIO_WriteLow(GPIOB,GPIO_PIN_4);
		GPIO_WriteLow(GPIOB,GPIO_PIN_5);
		GPIO_WriteLow(GPIOC,GPIO_PIN_4);
		GPIO_WriteLow(GPIOC,GPIO_PIN_3);

	}
	else
	{
		GPIO_WriteHigh(GPIOB,GPIO_PIN_4);
		GPIO_WriteHigh(GPIOB,GPIO_PIN_5);
		GPIO_WriteHigh(GPIOC,GPIO_PIN_4);
		GPIO_WriteHigh(GPIOC,GPIO_PIN_3);
	}
}

void PD2_temperature_check(void)
{
	u16 temp;
	while(1)
	{
		Delay_ms(200);
		temp = adc1_read_by_chanel(TEMPERATURE_ADC_PORT);
		if(temp >= 800 && temp <= 4500)
		{
			if(temp < 2900)// 超过40度 ADC值越小，温度越高
			{
				Fan_power_on_off_set(ON);
			}
			else if(temp > 3000)
			{
				Fan_power_on_off_set(OFF);

			}
			break;
		}
		else
			continue;
	}

}

void adc_pc4_check(void)
{
	u16 temp;
	u8 detect_system_verify_times = 0;
	
	while(1)
	{	 
		Delay_ms(100);
		temp = adc1_read_by_chanel(ADC_PC4_CHN);

		
		
		if(temp>=VOLTAGE_ADC_PC4_MIN && temp<=VOLTAGE_ADC_PC4_MAX)
		{

			if(detect_system_verify_times >= 5)
			{
				voltage_system_type = VOLTAGE_SYSTEM_12V_TYPE;
				break;

			}
			else
				detect_system_verify_times++;
			
			
		}
		else
		{
			//system_voltage_error();
			detect_system_verify_times = 0;
			continue;
		}
	}
	
}


void pb5_poweron_delay_check(void)
{
	u8 temp = 0;

	temp = GPIO_ReadInputPin(GPIOB,GPIO_PIN_5);

	if(temp)
	{
		Delay_ms(10);
		temp = GPIO_ReadInputPin(GPIOB,GPIO_PIN_5);
		if(temp)
			delay_second(20);

	}
}

void power_on_check_first(void)
{


	adc_pd3_check();
	
	adc_pc4_check();
	pb5_poweron_delay_check();

	PD2_temperature_check();

}


void adc_pd3_monitor(u8 type)
{
	u16 temp;
	if(!monitor_voltage_delay_cnt)
	{
			if(voltage_sys_error_overtime_cnt >= 60000)
			{
				;
			}
						
			//Delay_ms(10);
			temp = adc1_read_by_chanel(VOLTAGE_SYSTEM_VERIFY_ADC_PORT);
			if(temp>=VOLTAGE_SYSTEM_12V_POWERON_MIN&& temp<=VOLTAGE_SYSTEM_12V_POWERON_MAX)
			{
				voltage_sys_error_overtime_cnt = 0;

				if(system_current_type == 12)
				{
					if(temp >= VOLTAGE_ADC_PD3_6A_MIN && temp <= VOLTAGE_ADC_PD3_12A_TO_6A_VALUE)
					{
						system_current_type = 1;
						//led_blink_set( BLINK_SLOW);
					}
				}
				else
				{

					if(temp >= VOLTAGE_ADC_PD3_6A_MIN && temp <= VOLTAGE_ADC_PD3_6A_MAX)
					{
						system_current_type = 1;
						//led_blink_set( BLINK_SLOW);
					}
					else
					{	
						if(temp >= VOLTAGE_ADC_PD3_12A_MIN)
						{	
							system_current_type = 12;//= Key_Scan();
							//led_blink_set( BLINK_NORMAL);

						}
						
					}
				}
			}
			else 
			{
					temp = adc1_read_by_chanel(VOLTAGE_SYSTEM_VERIFY_ADC_PORT);
					if(system_voltage_power_off_num >= SYSTEM_VOLTAGE_POWER_OFF_TIMES)
					{
						if(temp<VOLTAGE_SYSTEM_12V_POWEROFF_MIN || temp>VOLTAGE_SYSTEM_12V_POWERON_MAX)
						{
							led_blink_set( BLINK_OFF);
							pwm_off_and_system_reboot(1);
							
						}

						system_voltage_power_off_num = 0;
					}
					else
					{
						if(temp<VOLTAGE_SYSTEM_12V_POWEROFF_MIN || temp>VOLTAGE_SYSTEM_12V_POWERON_MAX+10)
						{
							system_voltage_power_off_num++;
							

						}
					}
					
			}
			
		monitor_voltage_delay_cnt = 50; //check one time per 20 ms
	}
}

void PA3_check(void)
{
	
	u8 temp = 0;

	while(1)
	{
		temp = GPIO_ReadInputPin(GPIOA,GPIO_PIN_3);

		if(temp)
		{
			Delay_ms(10);

			temp = GPIO_ReadInputPin(GPIOA,GPIO_PIN_3);
			if(temp)
			{
				break;

			}
			
		}
		
		Delay_ms(10);
		pwm_off_and_system_reboot(1);
	}

}

void main(void)
{ 
	my_device_init();

	Delay_ms(500);
				
	while (1)
	{
		PD2_temperature_monitor();

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

