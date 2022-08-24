#include "stm32f10x.h" //STM32头文件
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
 
 char PUB_BUF[256];//上传数据的buf
 u8 user_set_flag=0;
 u8 alarm_free=10;
 //main函数
 int main(void)
 {
	//float light_f;
	u16 light = 0;
	u8 j;
	u8 b[2];                        //温湿度储存数组
	unsigned short timeCount = 0;	//发送间隔变量
	unsigned char *dataPtr = NULL;
	 
	delay_init();	                //延时函数初始化	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	delay_ms(3000);                 //上电时等待其他器件就绪
	
	BUZZER_Init();
	LED_Init();
	
	 
	Usart1_Init(115200);			//debug串口
	Usart2_Init(115200);			//stm32-8266串口
	 
	TIM3_Int_Init(4999,7199);
	
	 
	I2C_Configuration();			//I2C初始化
	OLED0561_Init(); 				//OLED初始化
	 
	OLED_DISPLAY_8x16_BUFFER(0,"   DHT11 TEST   "); 	//显示字符串
	
	if(DHT11_Init()==0){ 								//DHT11初始化	返回0成功，1失败
		OLED_DISPLAY_8x16_BUFFER(4,"Humidity:   %   "); //显示字符串
		OLED_DISPLAY_8x16_BUFFER(6,"Temperature:   C"); //显示字符串
	}else{
		OLED_DISPLAY_8x16_BUFFER(4,"DHT11INIT ERROR!"); //显示字符串
	}
	delay_ms(1000);//DHT11初始化后必要的延时（不得小于1秒）
	
	ESP8266_Init();//初始化8266
	EXTIX_Init();
	Init_BH1750();       //初始化BH1750
	while(OneNet_DevLink())
	delay_ms(500);
	
	OneNet_Subscribe(devSubTopic,1);
	UsartPrintf(USART1, " Hardware init ok\r\n");
	BUZZER_BEEP1();
	while(1)
	{
		if(++j> 40 )//1000ms  1000/25=40
		{	
			light=read_BH1750();  //读取BH1750的光强数据
			if(DHT11_ReadData(b)==0){							//读出温湿度值  指针1是湿度 20～90%，指针2是温度 0～50C，数据为十进制
				//UsartPrintf(USART_DEBUG, "湿度:%d %%,温度:%d C\r\n",b[0],b[1]);
				OLED_DISPLAY_8x16(4,9*8,b[0]/10 +0x30);			//显示湿度值
				OLED_DISPLAY_8x16(4,10*8,b[0]%10 +0x30);		//
				OLED_DISPLAY_8x16(6,12*8,b[1]/10 +0x30);		//显示温度值
				OLED_DISPLAY_8x16(6,13*8,b[1]%10 +0x30);		//
			}else{
				OLED_DISPLAY_8x16_BUFFER(6,"DHT11READ ERROR!"); //显示字符串
			}
			

			
			UsartPrintf(USART_DEBUG, "湿度:%d %%,温度:%d C,光照度:%d\r\n",b[0],b[1],light);
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
		
		if(++timeCount >= 40)									//5000ms/25=200发送间隔5s
		{
			LED_flag=GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_1);//读取状态
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
		
		delay_ms(10);										//延时，刷新数据的频率（不得小于1秒）
	}
 }



