#ifndef __BUZZER_H
#define __BUZZER_H	 
#include "sys.h"

#define BUZZERPORT	GPIOA	//����IO�ӿ�
#define BUZZER	GPIO_Pin_0	//����IO�ӿ�

#define BUZZER1 PAout(0)// PA4

void BUZZER_Init(void);//��ʼ��
void BUZZER_BEEP1(void);//��һ��

		 				    
#endif
