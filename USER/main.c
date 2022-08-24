#include "stm32f10x.h" //STM32ͷ�ļ�
#include "sys.h"
#include "delay.h"
#include "key.h"
#include "led.h"
#include "oled0561.h"
#include "dht11.h"
#include "usart.h"
#include "buzzer.h"
#include "exti.h"
#include "timer.h"
#include "esp8266.h"
#include "onenet.h"
#include <string.h>
#include <stdio.h>
#include "BH1750.h"

 const char *devSubTopic[]={"/v_new/sub"};
 const char devPubTopic[]="/v_new/pub";
 
 u8 LED_flag=0;
 u8 Buzzer_flag=0;
 
 char PUB_BUF[256];//�ϴ����ݵ�buf
 u8 user_set_flag=0;
 u8 alarm_free=10;
 //main����
 int main(void)
 {
	//float light_f;
	u16 light = 0;
	u8 j;
	u8 b[2];                        //��ʪ�ȴ�������
	unsigned short timeCount = 0;	//���ͼ������
	unsigned char *dataPtr = NULL;
	 
	delay_init();	                //��ʱ������ʼ��	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	delay_ms(3000);                 //�ϵ�ʱ�ȴ�������������
	
	BUZZER_Init();
	LED_Init();
	
	 
	Usart1_Init(115200);			//debug����
	Usart2_Init(115200);			//stm32-8266����
	 
	TIM3_Int_Init(4999,7199);
	
	 
	I2C_Configuration();			//I2C��ʼ��
	OLED0561_Init(); 				//OLED��ʼ��
	 
	OLED_DISPLAY_8x16_BUFFER(0,"   DHT11 TEST   "); 	//��ʾ�ַ���
	
	if(DHT11_Init()==0){ 								//DHT11��ʼ��	����0�ɹ���1ʧ��
		OLED_DISPLAY_8x16_BUFFER(4,"Humidity:   %   "); //��ʾ�ַ���
		OLED_DISPLAY_8x16_BUFFER(6,"Temperature:   C"); //��ʾ�ַ���
	}else{
		OLED_DISPLAY_8x16_BUFFER(4,"DHT11INIT ERROR!"); //��ʾ�ַ���
	}
	delay_ms(1000);//DHT11��ʼ�����Ҫ����ʱ������С��1�룩
	
	ESP8266_Init();//��ʼ��8266
	EXTIX_Init();
	Init_BH1750();       //��ʼ��BH1750
	while(OneNet_DevLink())
	delay_ms(500);
	
	OneNet_Subscribe(devSubTopic,1);
	UsartPrintf(USART1, " Hardware init ok\r\n");
	BUZZER_BEEP1();
	while(1)
	{
		if(++j> 40 )//1000ms  1000/25=40
		{	
			light=read_BH1750();  //��ȡBH1750�Ĺ�ǿ����
			if(DHT11_ReadData(b)==0){							//������ʪ��ֵ  ָ��1��ʪ�� 20��90%��ָ��2���¶� 0��50C������Ϊʮ����
				//UsartPrintf(USART_DEBUG, "ʪ��:%d %%,�¶�:%d C\r\n",b[0],b[1]);
				OLED_DISPLAY_8x16(4,9*8,b[0]/10 +0x30);			//��ʾʪ��ֵ
				OLED_DISPLAY_8x16(4,10*8,b[0]%10 +0x30);		//
				OLED_DISPLAY_8x16(6,12*8,b[1]/10 +0x30);		//��ʾ�¶�ֵ
				OLED_DISPLAY_8x16(6,13*8,b[1]%10 +0x30);		//
			}else{
				OLED_DISPLAY_8x16_BUFFER(6,"DHT11READ ERROR!"); //��ʾ�ַ���
			}
			

			
			UsartPrintf(USART_DEBUG, "ʪ��:%d %%,�¶�:%d C,���ն�:%d\r\n",b[0],b[1],light);
			j=0;
			if(alarm_free==10)
			{
				if(b[0]>50||b[1]>35||light>6000){
					user_set_flag=1;
				}
				else{
					user_set_flag=0;
					BUZZER1=1;
					LED0=1;
				}
			}
			if(alarm_free<10){
				alarm_free++;
				user_set_flag=0;
				BUZZER1=1;
				LED0=1;
			}
		
		}
		
		if(++timeCount >= 40)									//5000ms/25=200���ͼ��5s
		{
			LED_flag=GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_1);//��ȡ״̬
			//Buzzer_flag=GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0);
			
			UsartPrintf(USART_DEBUG, "OneNet_Publish\r\n");
			sprintf(PUB_BUF,"{\"Hump\":%d,\"Temp\":%d,\"Light\":%d,\"Led\":%d,\"Beep\":%d}",b[0],b[1],light,LED_flag?0:1,user_set_flag);
			OneNet_Publish(devPubTopic, PUB_BUF);
			
			timeCount = 0;
			ESP8266_Clear();
		}
		
		dataPtr = ESP8266_GetIPD(3);
		if(dataPtr != NULL)
			OneNet_RevPro(dataPtr);
		
		delay_ms(10);										//��ʱ��ˢ�����ݵ�Ƶ�ʣ�����С��1�룩
	}
 }



