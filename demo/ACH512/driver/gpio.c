/***********************************************************************
 * Copyright (c)  2008 - 2016, Shanghai AisinoChip Co.,Ltd .
 * All rights reserved.
 * Filename    : gpio.c
 * Description : gpio driver source file
 * Author(s)   : Eric
 * version     : V1.0
 * Modify date : 2016-03-24
 ***********************************************************************/
#include "gpio.h"

volatile UINT8 flag_gpioa_int = 0;
volatile UINT8 flag_gpiob_int = 0;

void GPIOA_IRQHandler(void)
{
    UINT32 status;
    status = REG_GPIO_MIS(GPIOA);
    //具体IO中断处理代码
    flag_gpioa_int = 1;

    REG_GPIO_IC(GPIOA) = status;
}

void GPIOB_IRQHandler(void)
{
    UINT32 status;
    status = REG_GPIO_MIS(GPIOB);
    //具体IO中断处理代码
    flag_gpiob_int = 1;

    REG_GPIO_IC(GPIOB) = status;
}

/************************************************************************
 * function   : gpio_init
 * Description: gpio initial
 * input : none
 * return: none
 ************************************************************************/
void gpio_init(void)
{
#ifdef LOW_POWER
    enable_module(BIT_GPIO); //enable GPIO
#endif

    NVIC_ClearPendingIRQ(GPIOA_IRQn);
    NVIC_EnableIRQ(GPIOA_IRQn);

    NVIC_ClearPendingIRQ(GPIOB_IRQn);
    NVIC_EnableIRQ(GPIOB_IRQn);
}

