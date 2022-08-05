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
 * main������
 * ������� ����
 * ����ֵ   ����
 * �������� ����������ں���������ģ���ʼ���Լ�����ģ���֧�Ӻ�������ѯ
 ***********************************************************************/
int main(void)
{
	SystemInit();
	
	uart_init(UARTA, UART_BAUD_RATE);	//UARTA��ʼ��
	uart_init(UARTB, UART_BAUD_RATE);	//UARTB��ʼ��
	
	Timer0Init();			//��ʱ��0��ʼ��
	
// 	led_init();			  //LED�Ƴ�ʼ��
 	
	iic_test();				//���iic����
//  iic_sx_test();		//�˲�iic����
	
// 	LCD_down(10);		  //LCD_������(����ֵ)

	while(1)
	{ 
		;
// 		led();			    //��ˮ��
	}	
}




