#include <REGX52.H>
#include "LCD1602.h"
void Data_update();

unsigned int min=59;
unsigned int hr=11;

unsigned int day=31;
unsigned int mon=12;
unsigned int year=2026;

unsigned int am_pm=1;
unsigned int T0Count=0;

unsigned int sec=50;

void Timer0_Init()
{
	TMOD&=0xF0;
	TMOD|=0x01;

	TF0=0;

	//晶振為11.0592MHz;MCU：12T mode;Timer 1次週期約1.085(us)(12/11.0592)
	//1(ms)/1.085(us)=922(次),所以16位Timer初值為65536-922=64614
	TH0=64614/256; //65536-922,高8位
	TL0=64614%256; //低8位

	TR0=1;

	ET0=1;
	EA=1;
	PT0=0;
}

void main()
{	
	LCD_Init();
	Timer0_Init();

	while(1)
	{	
		if(am_pm%2==0)
		{
			LCD_ShowString(1,1,"AM");
		}else if(am_pm%2==1)
		{
			LCD_ShowString(1,1,"PM");
		}
		LCD_ShowNum(1,9,hr,2);
		LCD_ShowChar(1,11,':');

		LCD_ShowNum(1,12,min,2);
		LCD_ShowChar(1,14,':');

		LCD_ShowNum(1,15,sec,2);

		LCD_ShowNum(2,7,year,4);
		LCD_ShowChar(2,11,'/');

		LCD_ShowNum(2,12,mon,2);
		LCD_ShowChar(2,14,'/');

		LCD_ShowNum(2,15,day,2);
	}
}

void Timer0_RoCount() interrupt 1
{
	TH0=64614/256;
	TL0=64614%256;

	T0Count++;

	if(T0Count>=1000)
	{
		T0Count=0;
		sec++;
	}
		//  秒
	if(sec==60)
	{
		sec=0;
		min++;
	}
		
	//  分
	if(min==60)
	{
		min=0;
		hr++;
		//  時
		if(hr%12==0)
		{
			am_pm++;
			if(am_pm%2==0)
			{
				day++;
				Data_update();
			}
		}
		if(hr==13)
		{
			hr=1;
		}
	}
}

void Data_update(){
	//  日(小月)
	if(mon==1||mon==3||mon==5||mon==7||mon==8||mon==10||mon==12)
	{
		if(day==32)
		{
			day=1;
			mon++;
		}

	//  日(大月)
	}else if(mon==4||mon==6||mon==9||mon==11)
	{
		if(day==31)
		{
			day=1;
			mon++;
		}
			
	//  日(閏年2月)
	}else if(mon==2&&((year%4==0&&year%100!=0)||(year%100==0&&year%400==0)))
	{
		if(day==30)
		{
			day=1;
			mon++;
		}
		
	//  日(平年2月)
	}else if((mon==2&&!(year%4==0&&year%100!=0))||(mon==2&&!(year%100==0&&year%400==0)))
	{
		if(day==29)
		{
			day=1;
			mon++;
		}
	}
	//  月
	if(mon==13)
	{
		mon=1;
		year++;
	}
}