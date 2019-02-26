#include "sys.h"
#include "usart.h"
#include "delay.h"
#include "led.h"
#include "serial.h"
#include "stdlib.h"

u16 t;  
u16 len;
u16 times=0; //不需要

extern int coords[2];

void recieveData(void)
{
	u8 flag=0;
	char strX[4], strY[4];
	u8 cnt_x=0;
	u8 cnt_y=0;
	u8 *adress = NULL;
	
	if(USART_RX_STA&0x8000)		//如果接收完数据
		{	
			LED1=!LED1;//1号指示灯变更状态
			
			len=USART_RX_STA&0x3fff;//得到此次接收到的数据长度
			
			adress = &USART_RX_BUF[0];	//指针adress储存字符地址，从0-len过一遍
			
			//根据协议取出坐标的字符形式到数strX和strY中
			for(t=0;t<len;t++)
			{
				if(*adress>='0' && *adress<='9')
				{
					if(flag==1)
					{
						strX[cnt_x] = *adress;
						cnt_x++;
					}
					else
					{
						strY[cnt_y] = *adress;
						cnt_y++;
					}	
				}
				else
				{
					if(*adress=='#')
						flag = 1;
					if(*adress=='$')
						flag = 2;
				}
				adress++;	
			}
			
			//转换字符串为整型，并存储到全局变量coords中
			coords[0] = atoi(strX);
			coords[1] = atoi(strY);
			//标志位清零
			USART_RX_STA=0;
		}
	else
		{
			times++;
			if(times%30==0)LED0=!LED0;//闪烁0号指示灯,提示系统正在运行.
			delay_ms(10); 
		}
}
