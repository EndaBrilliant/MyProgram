/***********************************************************************
 * Copyright (c)  2008 - 2016, Shanghai AisinoChip Co.,Ltd .
 * All rights reserved.
 * Filename    : app.c
 * Description : application example source file
 * Author(s)   : Eric
 * version     : V1.0
 * Modify date : 2016-03-24
 ***********************************************************************/
#include  "app.h"
#include  "gpio.h"
#include	"timer.h"
int t = 1; 
int L = 10;

void gpio_output_test(void)
{
	REG_GPIO_DIR(GPIOA) |= 1<<3; //����GPIO3Ϊ�������
	printfS("gpio output test!\n");
	while(1)
	{
		REG_GPIO_CLR(GPIOA) = 1<<3;
		delay(100);
		REG_GPIO_SET(GPIOA) = 1<<3;
		delay(100);
	}
}

void gpio_input_test(void)
{

	REG_GPIO_DIR(GPIOA) &= ~(1<<3); //����GPIO3Ϊ���빦��

	printfS("gpio input test!\n");
	while(1)
	{
		printfS("GPIO3 is 0x%x!\n", (REG_GPIO_IDATA(GPIOA) >> 3) & 0x01);
		delay(50000);
	}
}

void gpio_edge_int_test(void)
{
	printfS("gpio edge int test start!\r\n");

	REG_GPIO_DIR(GPIOA) &= ~(1 << 3); //����GPIO3Ϊ���빦��
	REG_GPIO_IEN(GPIOA) |= (1 << 3); //ʹ���ж�
	REG_GPIO_IS(GPIOA) &= ~(1 << 3);   //���ؼ��
	REG_GPIO_IBE(GPIOA) &= ~(1 << 3);   //�������жϴ���

	while(1)
	{
		if(flag_gpioa_int == 1)
		{
			flag_gpioa_int = 0;
			printfS("gpioa_int is happened!\n");
		}
	}
}

void gpio_test(void)
{
	gpio_init();
	
	REG_SCU_MUXCTRLA &= ~(0x03 << 6);//���ùܽŸ���ΪGPIO3
	
	printfS("gpio test is start!\n");
	gpio_output_test();
//	gpio_input_test();
//	gpio_edge_int_test();
	printfS("gpio test is over!\n");
}

/************************************************************************
 * function   : led_init
 * Description: ��ʼ��led��
 * input : none
 * return: none
 ************************************************************************/
void led_init(void)
{
	REG_GPIO_DIR(GPIOB) |= 0xF << 11;
}

/************************************************************************
 * function   : led
 * Description: ��ˮ��
 * input : none
 * return: none
 ************************************************************************/
	void led(void)
{
	REG_GPIO_CLR(GPIOB) |= 1 << L;
	
	if( L == 14 ) L = 11;
	else L++;
	
	REG_GPIO_SET(GPIOB) |= 1 << L;
	
	timer0_set_us(5);		//��ʱ5us
// 	timer0_set_us(1000000);		//��ʱ1000000us
}



