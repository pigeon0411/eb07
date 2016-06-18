#include "key.h"

//static void Delay(u32 nCount)
//{
//	unsigned int i;
//	
//  	for (i = 0; i < 2000; i++) {
//		for (; nCount > 0; nCount--)
//			;
//  	}
//}
// 
void Key_Init()
{
  //GPIO_MODE_IN_PU_NO_IT GPIO输入模式，带上拉电阻，未开中断 
  
  //GPIO_Init(GPIOC,GPIO_PIN_3, GPIO_MODE_IN_PU_NO_IT); //PC3
 // GPIO_Init(GPIOC,GPIO_PIN_5, GPIO_MODE_IN_PU_NO_IT); //Pc5
  GPIO_Init(GPIOC,GPIO_PIN_6, GPIO_MODE_IN_PU_NO_IT); //Pc6
  GPIO_Init(GPIOC,GPIO_PIN_7, GPIO_MODE_IN_PU_NO_IT); //Pc7
  
}

#define	Key_release_CHK		0

#define A11  GPIO_PIN_3
#define A13  GPIO_PIN_4
#define A15  GPIO_PIN_5
#define A17  GPIO_PIN_6
#define A19  GPIO_PIN_7


#if 1

#else
uint8_t Key_Scan()
{
    uint8_t key_value,k_v2;
    key_value=GPIO_ReadInputData(GPIOE)&0xFE;
    k_v2=(GPIO_ReadInputData(GPIOE)&0x20)>>5;
    //k_v2 = GPIO_ReadInputPin(GPIOE,GPIO_PIN_5);
    key_value = key_value|k_v2;
    
    return key_value;
} 
#endif

