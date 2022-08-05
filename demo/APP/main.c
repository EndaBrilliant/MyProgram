/***********************************************************************
 * Copyright (c)  2008 - 2016, Shanghai AisinoChip Co.,Ltd .
 * All rights reserved.
 * Filename    : main.c
 * Description : main source file
 * Author(s)   : Eric
 * version     : V1.0
 * Modify date : 2016-03-24
 ***********************************************************************/
#include  "common.h"
#include  "app.h"
#include	"timer0.h"
#include	"stdio.h"
#include  "stdlib.h"
#include  "string.h"
#include	"lcd.h"
#include	"iic_sx.h"
#include	"iic.h"


/***********************************************************************
 * main主函数
 * 输入参数 ：无
 * 返回值   ：无
 * 函数功能 ：主程序入口函数，各个模块初始化以及各个模块分支子函数的轮询
 ***********************************************************************/
int main(void)
{
	SystemInit();
	
	uart_init(UARTA, UART_BAUD_RATE);	//UARTA初始化
	uart_init(UARTB, UART_BAUD_RATE);	//UARTB初始化
	
	Timer0Init();			//定时器0初始化
	
// 	led_init();			  //LED灯初始化
 	
	iic_test();				//软件iic测试
//  iic_sx_test();		//滤波iic测试
	
// 	LCD_down(10);		  //LCD_计数器(计数值)

	while(1)
	{ 
		;
// 		led();			    //流水灯
	}	
}




