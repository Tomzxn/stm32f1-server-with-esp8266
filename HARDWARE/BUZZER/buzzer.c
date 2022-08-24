
#include "buzzer.h"
#include "delay.h"

void BUZZER_Init(void){ //�������Ľӿڳ�ʼ��
	GPIO_InitTypeDef  GPIO_InitStructure; 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA , ENABLE);	 //ʹ��PB,PE�˿�ʱ��
	
		
    GPIO_InitStructure.GPIO_Pin = BUZZER; //ѡ��˿ں�                        
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //ѡ��IO�ӿڹ�����ʽ       
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; //����IO�ӿ��ٶȣ�2/10/50MHz��    
	GPIO_Init(BUZZERPORT, &GPIO_InitStructure);	
	
	GPIO_WriteBit(BUZZERPORT,BUZZER,(BitAction)(1)); //�������ӿ�����ߵ�ƽ1		
}

void BUZZER_BEEP1(void){ //��������һ��
	GPIO_WriteBit(BUZZERPORT,BUZZER,(BitAction)(0));
	delay_ms(500);
	GPIO_WriteBit(BUZZERPORT,BUZZER,(BitAction)(1));
}

 
/*********************************************************************************************
 * �������� www.DoYoung.net
 * ���ҵ��� www.DoYoung.net/YT 
*********************************************************************************************/


/*
ѡ��IO�ӿڹ�����ʽ��
GPIO_Mode_AIN ģ������
GPIO_Mode_IN_FLOATING ��������
GPIO_Mode_IPD ��������
GPIO_Mode_IPU ��������
GPIO_Mode_Out_PP �������
GPIO_Mode_Out_OD ��©���
GPIO_Mode_AF_PP �����������
GPIO_Mode_AF_OD ���ÿ�©���
*/