#include "key.h"




extern u32 voltage_sys_error_overtime_cnt;


void Key_Init()
{
  //GPIO_MODE_IN_PU_NO_IT GPIO输入模式，带上拉电阻，未开中断 

	//IN 
	GPIO_Init(GPIOD,GPIO_PIN_5, GPIO_MODE_IN_PU_NO_IT); //Pc7
	GPIO_Init(GPIOB,GPIO_PIN_4, GPIO_MODE_IN_PU_NO_IT); //Pc7
	GPIO_Init(GPIOB,GPIO_PIN_5, GPIO_MODE_IN_PU_NO_IT); //Pc7
	GPIO_Init(GPIOA,GPIO_PIN_1, GPIO_MODE_IN_PU_NO_IT); //Pc7
	GPIO_Init(GPIOA,GPIO_PIN_2, GPIO_MODE_IN_PU_NO_IT); //Pc7
	GPIO_Init(GPIOA,GPIO_PIN_3, GPIO_MODE_IN_PU_NO_IT); //Pc7


	//out
	GPIO_Init(GPIOC,GPIO_PIN_4, GPIO_MODE_OUT_PP_HIGH_FAST); //
	GPIO_Init(GPIOC,GPIO_PIN_5, GPIO_MODE_OUT_PP_HIGH_FAST); //
	GPIO_Init(GPIOC,GPIO_PIN_6, GPIO_MODE_OUT_PP_HIGH_FAST); //
	GPIO_Init(GPIOC,GPIO_PIN_7, GPIO_MODE_OUT_PP_HIGH_FAST); //
	GPIO_Init(GPIOD,GPIO_PIN_2, GPIO_MODE_OUT_PP_HIGH_FAST); //
	GPIO_Init(GPIOD,GPIO_PIN_3, GPIO_MODE_OUT_PP_HIGH_FAST); //
	GPIO_Init(GPIOD,GPIO_PIN_4, GPIO_MODE_OUT_PP_HIGH_FAST); //


	GPIO_WriteHigh(GPIOD,GPIO_PIN_4);
	GPIO_WriteLow(GPIOD,GPIO_PIN_2);
	GPIO_WriteLow(GPIOD,GPIO_PIN_3);
	
	GPIO_WriteLow(GPIOC,GPIO_PIN_4);
	GPIO_WriteLow(GPIOC,GPIO_PIN_5);
	GPIO_WriteLow(GPIOC,GPIO_PIN_6);
	GPIO_WriteLow(GPIOC,GPIO_PIN_7);


	
  
}

enum key_in_type
{
	KEY_IN_NONE,

	KEY_IN_1_PD5,
	KEY_IN_2_PA1,
	KEY_IN_3_PA2,
	KEY_IN_4_PA3,
	KEY_IN_5_PB4,
	KEY_IN_6_PB5,
	KEY_IN_NORMAL
};

#define		KEY_IN_PINS_NUM			6
#define		KEY_OUT_PINS_NUM			7

GPIO_TypeDef * key_port_in[KEY_IN_PINS_NUM]={
	GPIOD,GPIOB,GPIOB,GPIOA,GPIOA,GPIOA,

};

GPIO_Pin_TypeDef key_pin_in[KEY_IN_PINS_NUM]=
{
	GPIO_PIN_5,GPIO_PIN_4,GPIO_PIN_5,GPIO_PIN_1,GPIO_PIN_2,GPIO_PIN_3,


};

enum out_pin_type
{
OUT_PC4,
	OUT_PC5,
	OUT_PC6,
	OUT_PC7,
	OUT_PD2,
	OUT_PD3,
	OUT_PD4,

};

GPIO_TypeDef * key_port_out[]={
	GPIOC,GPIOC,GPIOC,GPIOC,GPIOD,GPIOD,GPIOD,

};

GPIO_Pin_TypeDef key_pin_out[]=
{
	GPIO_PIN_4,GPIO_PIN_5,GPIO_PIN_6,GPIO_PIN_7,GPIO_PIN_2,GPIO_PIN_3,GPIO_PIN_4,


};



void out_port_set_all(u8 mode)
{
	u8 i;

	if(mode)
	{
		for(i=0;i<KEY_OUT_PINS_NUM;i++)
		{
			if(i != OUT_PD4)
			GPIO_WriteHigh(key_port_out[i],key_pin_out[i]);

		}
	}
	else
	{
		for(i=0;i<KEY_OUT_PINS_NUM;i++)
		{
			if(i != OUT_PD4)
			GPIO_WriteLow(key_port_out[i],key_pin_out[i]);

		}
	}

}

void out_port_set_pin(enum out_pin_type  val, u8 mode)
{
	u8 i;

	if(mode)
	{
		GPIO_WriteHigh(key_port_out[val],key_pin_out[val]);
	}
	else
	{
		GPIO_WriteLow(key_port_out[val],key_pin_out[val]);
	}

}

void out_port_reverse_pin(enum out_pin_type  val)
{

	GPIO_WriteReverse(key_port_out[val],key_pin_out[val]);


}



u16 key_pre = 0;



u8 long_key_state = 0;

#define		KEY_DELAY_CHECK_MS		40


#define	key_to_long(val)	(val|0x9000)
#define	key_to_release(val)	(val|0x8000)

//返回0为无按键，返回非0值，则为对应的按键号
static u32 key_ctl_check(void)
{
	u16 i;
	u32 key_tmp;
	static u32 long_press_cnt = 0;// 50ms
	
	for(i=0;i<KEY_IN_PINS_NUM;i++)
	{
		key_tmp = GPIO_ReadInputPin(key_port_in[i],key_pin_in[i]);

		if(key_tmp == 1)
		{

			Delay_ms(50);

			key_tmp = GPIO_ReadInputPin(key_port_in[i],key_pin_in[i]);

			if(key_tmp == 1)
			{
				if(key_pre == i+1)
				{
					if(long_press_cnt>20)
					{

						long_press_cnt=0;
						key_pre = (i+1)|0x9000;
						long_key_state = 1;
						return ((i+1)|0x9000);
					}

					if(long_key_state == 0)
						long_press_cnt++;
				}
				else if((key_pre&0x9000) == i+1)
				{

						//key_pre = (i+1)|0x9000;
						//long_key_state = 1;
						return (0);
				}
				key_pre = i+1;
				//return (i+1);
				break;
			}
		}
	}


	
	if((key_pre>0x9000)&& (key_pre&0x9000 == (i+1)) && i<20)
	{
		if(long_key_state)
		{
			long_key_state = 0;
		}

		key_pre = 0;
		return (i+1);

	}

	
	
	if((key_pre && key_pre!=(i+1))||(key_pre && i==20))
	{
		if(long_key_state)
		{
			long_key_state = 0;
			key_pre = 0;
			return 0;
		}
		
		i = key_pre|0x8000;
		key_pre = 0;
		return i;

	}
	return 0;
}




void key_handle(u32 val)
{
	u32 tmp;
	
	if(val>0x8000 && val<key_to_release(KEY_IN_NORMAL))
	{
		tmp = val&(~0x8000);


		if(tmp >=KEY_IN_2_PA1 && tmp<=KEY_IN_6_PB5)
		{
			voltage_sys_error_overtime_cnt = 0;
		}
		
		switch(tmp)
		{
		case KEY_IN_1_PD5:
			out_port_set_all(0);
			Delay_ms(50);
			out_port_reverse_pin(OUT_PD4);
			break;

		case KEY_IN_2_PA1:
			out_port_set_all(0);
			Delay_ms(50);
			out_port_set_pin(OUT_PD3,1);
			break;

		case KEY_IN_3_PA2:
			out_port_set_all(0);
			Delay_ms(50);
			out_port_set_pin(OUT_PD2,1);
			break;

			
		case KEY_IN_4_PA3:
			out_port_set_all(0);
			Delay_ms(50);
			out_port_set_pin(OUT_PC6,1);
			Delay_ms(50);

			out_port_set_all(0);
			Delay_ms(50);
			out_port_set_pin(OUT_PC4,1);

			
			break;

		case KEY_IN_5_PB4:
			out_port_set_all(0);
			Delay_ms(50);
			out_port_set_pin(OUT_PC7,1);
			Delay_ms(50);

			out_port_set_all(0);
			Delay_ms(50);
			out_port_set_pin(OUT_PC4,1);

			
			break;

		case KEY_IN_6_PB5:
			out_port_set_all(0);
			Delay_ms(50);
			out_port_set_pin(OUT_PC5,1);
			Delay_ms(50);

			out_port_set_all(0);
			Delay_ms(50);
			out_port_set_pin(OUT_PC4,1);

			
			break;			

			
		default:
			
			break;
		
		}



	}


}



uint8_t Key_Scan(void)
{
	u32 k;

	k = key_ctl_check();
	if(k)
	{
		
		key_handle(k);

	}

	if(voltage_sys_error_overtime_cnt > 1000*60*10)
	{
		out_port_set_pin(OUT_PD4,0);
	}
	else if(voltage_sys_error_overtime_cnt > 1000*60*3)
	{
		out_port_set_all(0);
	}
} 

