#ifndef __BUZZER_H
#define __BUZZER_H	 
#include "sys.h"

#define BUZZERPORT	GPIOA	//定义IO接口
#define BUZZER	GPIO_Pin_0	//定义IO接口

#define BUZZER1 PAout(0)// PA4

void BUZZER_Init(void);//初始化
void BUZZER_BEEP1(void);//响一声

		 				    
#endif
