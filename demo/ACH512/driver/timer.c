/***********************************************************************
 * Copyright (c)  2008 - 2016, Shanghai AisinoChip Co.,Ltd .
 * All rights reserved.
 * Filename    : timer.c
 * Description : timer driver source file
 * Author(s)   : Eric
 * version     : V1.0
 * Modify date : 2016-03-24
 ***********************************************************************/
#include  "timer.h"

void (*TimerFunc[4])(void) = { 0 };		//数组长度为4对应4个timer处理函数

//仅Timer0,Timer2的Capture有效
volatile UINT8 flag_capture_int[4] = { 0 };
volatile UINT32 CaptureCounter[4] = { 0 };

volatile UINT8 flag_timer_int[4] = { 0 };

/*TIMER int handler*/
void TIMER_IRQHandler(void)
{
    UINT8 i;
    for(i = 0; i < 4; i++)
    {
        if(REG_TIMER_IF(i))
        {
            flag_timer_int[i] = 1;
			REG_TIMER_CIF(i) = 0xff; //清中断timer

            if(TimerFunc[i] != NULL)
            {
                ((void (*)())(TimerFunc[i]))(); //timer的处理函数
            }

        }
    }
}

void CCPWM_IRQHandler(void)
{
    UINT8 i;
//PWM
    for(i = 0; i < 4; i++)
    {
        if(REG_TIMER_CPIF & (0x01 << i))
        {
            REG_TIMER_CPIF = 0x01 << i; //清PWM中断,写1清中断
        }
    }
//CC
    if(REG_TIMER_CCIF & 0x01)
    {
        REG_TIMER_CCIF = 0x01; //清CC0中断，写1清中断
        CaptureCounter[TIMER0] = REG_TIMER_C0_CR;
        flag_capture_int[TIMER0] = 1;
    }
    if((REG_TIMER_CCIF & 0x02))
    {

        REG_TIMER_CCIF = 0x02; //清CC2中断，写1清中断
        CaptureCounter[TIMER2] = REG_TIMER_C2_CR;
        flag_capture_int[TIMER2] = 1;
    }
}
/************************************************************************
 * function   : timer_init
 * Description: timer initial
 * input : none
 * return: none
 ************************************************************************/
void timer_init(void)
{
#ifdef LOW_POWER
    enable_module(BIT_TIMER);
#endif

    NVIC_ClearPendingIRQ(TIMER_IRQn); 
    NVIC_EnableIRQ(TIMER_IRQn);
}
/************************************************************************
 * function   : timer_set_us
 * Description: timer set_us
 * input :
 *         UINT8 num: TIMER0,1,2,3
 *         UINT32 timer_us: delay timer_us
 *         void(*pFunc)() pFunc: processing function
 * return: none
 ************************************************************************/
void timer_set_us(UINT8 num, UINT32 timer_us, void (*pFunc)())
{
    UINT32 timer_clk_hz;

    UINT32 tmp;

    timer_clk_hz = PClock;

    TimerFunc[num] = pFunc;
	
	REG_TIMER_CR(num) = 0x01 << 4 | 0x02 << 1; //up counter,interrupt not masked,Cyclic mode,close

    //divided by 8,timer clk = (timer_clk_hz/8)

    REG_TIMER_PSC = (REG_TIMER_PSC & ~(0x07 << (3 * num))) | (0x00 << (3 * num)); //不分频，提高精度

    tmp = timer_clk_hz / (1 * 1000000); //定时器的真实时钟
    tmp *= timer_us;

    REG_TIMER_ARR(num) = tmp - 1; //延时tmp us，真实定时器计数值会比ARR多1个


}

/************************************************************************
 * function   : timer_set_ms
 * Description: timer set_ms
 * input :
 *         UINT8 num: TIMER0,1,2,3		//定时器号数
 *         UINT32 timer_us: delay timer_ms		//延时时间
 *         void(*pFunc)() pFunc: processing function		//timer的处理函数  
 * return: none
 ************************************************************************/
void timer_set_ms(UINT8 num, UINT32 timer_ms, void (*pFunc)())
{
    UINT32 timer_clk_hz;

    UINT32 tmp;

    timer_clk_hz = PClock;		//APB时钟/PCLK  , (uint:Hz)

		TimerFunc[num] = pFunc;
	
		REG_TIMER_CR(num) = 0x01 << 4 | 0x02 << 1; //down counter,interrupt not masked,Cyclic mode,close
// 		REG_TIMER_CR(num) = 0x01 << 4 | 0x0E; //down counter,interrupt masked,Single mode,close
    //divided by 8,timer clk = (timer_clk_hz/8)			TIMER_CR：timer0控制寄存器	0x14

    REG_TIMER_PSC = (REG_TIMER_PSC & ~(0x07 << (3 * num))) | (0x03 << (3 * num)); //8分频
		// TIM_PSC 定时器预分频寄存器
 	
    tmp = timer_clk_hz / (8 * 1000); //定时器的真实时钟
    tmp *= timer_ms;

    REG_TIMER_ARR(num) = tmp - 1; //延时tmp us,真实定时器计数值会比ARR多1个
		// timer加载寄存器	技计数周期为寄存器值加+1
	
	//使能
		REG_TIMER_CR(num) |= 0x01; 

	//判断是否产生中断
		while(!REG_TIMER_IF(num));
	//清除中断
		REG_TIMER_CIF(num) = 0xff;
}

/************************************************************************
 * function   : timer_start
 * Description: timer start
 * input :
 *         UINT8 num: TIMER0,1,2,3
 * return:
 ************************************************************************/
void timer_start(UINT8 num)
{
    REG_TIMER_CR(num) |= 0x01;      //enable timer   //使用定时器
}

/************************************************************************
 * function   : timer_stop
 * Description: timer stop
 * input :
 *         UINT8 num: TIMER0,1,2,3
 * return:
 ************************************************************************/
void timer_stop(UINT8 num)
{
    REG_TIMER_CR(num) &= ~0x01;    //close timer
    REG_TIMER_CIF(num) = 0xff; //清中断timer
}

/************************************************************************
 * function   : capture_set
 * Description: capture set
 * input :
 *         UINT8 num: only TIMER0,2
 *         UINT8 trigger: 0 -- CAPTURE_TRIGGER_RISING
 *                        1 -- CAPTURE_TRIGGER_FALLING
 * return: none
 ************************************************************************/
void capture_set(UINT8 num, UINT8 trigger)
{

    NVIC_ClearPendingIRQ(CCPWM_IRQn); //CCPWM
    NVIC_EnableIRQ(CCPWM_IRQn);
    if(num == TIMER0)
    {
        REG_SCU_MUXCTRLD = (REG_SCU_MUXCTRLD & ~(0x03 << 2)) | (0x02 << 2); //Capture0
    }
    else if(num == TIMER2)
    {
        REG_SCU_MUXCTRLD = (REG_SCU_MUXCTRLD & ~(0x03 << 0)) | (0x02 << 0); //Capture2
    }

    REG_TIMER_PSC = (REG_TIMER_PSC & ~(0x07 << (3 * num))) | (0x03 << (3 * num)); //8分频
    REG_TIMER_CR(num) = 0x01 << 4 | 0x01 << 3 | 0x02 << 1; //up counter,interrupt masked,Cyclic mode,close
    REG_TIMER_ICMODE &= ~(1 << num);
    REG_TIMER_ARR(TIMER0) = 0xffffffff;

    if(num == TIMER0)
    {
        REG_TIMER_CCR = (REG_TIMER_CCR & ~0x07) | (trigger << 2); //中断使能,关闭cc0模块

    }
    else if(num == TIMER2)
    {
        REG_TIMER_CCR = (REG_TIMER_CCR & ~(0x07 << 3)) | (trigger << 5); //中断使能,关闭cc0模块
    }
}

/************************************************************************
 * function   : capture_start
 * Description: capture start
 * input :
 *         UINT8 num: only TIMER0,2
 * return: none
 ************************************************************************/
void capture_start(UINT8 num)
{

    if(num == TIMER0)
    {
        REG_TIMER_CCR |= 0x01; //Start Capture0

    }
    else if(num == TIMER2)
    {
        REG_TIMER_CCR |= 0x01 << 3; //Start Capture2
    }
}

/************************************************************************
 * function   : capture_stop
 * Description: capture stop
 * input :
 *         UINT8 num: only TIMER0,2
 * return: none
 ************************************************************************/
void capture_stop(UINT8 num)
{

    if(num == TIMER0)
    {
        REG_TIMER_CCR &= ~0x01; //Start Capture0

    }
    else if(num == TIMER2)
    {
        REG_TIMER_CCR &= ~(0x01 << 3); //Start Capture2
    }
}

/************************************************************************
 * function   : pwm_set
 * Description: pwm set frequence
 * input :
 *         UINT8 num: TIMER0,1,2,3
 *         UINT32 timer_freq: timer frequence
 *         UINT32 pwm_freq: pwm frequence
 * return: none
 ************************************************************************/
void pwm_set(UINT8 num, UINT32 timer_frep, UINT32 pwm_freq)
{

    NVIC_ClearPendingIRQ(CCPWM_IRQn); //CCPWM
    NVIC_EnableIRQ(CCPWM_IRQn);
    if(num == TIMER0)
    {
        REG_SCU_MUXCTRLD = (REG_SCU_MUXCTRLD & ~(0x03 << 2)) | (0x02 << 2); //PWM0
    }
    else if(num == TIMER1)
    {
        REG_SCU_MUXCTRLD = (REG_SCU_MUXCTRLD & ~(0x03 << 22)) | (0x02 << 22); //PWM1
    }
		else if(num == TIMER2)
		{
			REG_SCU_MUXCTRLD = (REG_SCU_MUXCTRLD & ~(0x03 << 0)) | (0x02 << 0); //PWM2
		}
		else
		{
			REG_SCU_MUXCTRLA = (REG_SCU_MUXCTRLA & ~(0x03 << 18)) | (0x02 << 18); //PWM3
		
		}

    REG_TIMER_PSC = (REG_TIMER_PSC & ~(0x07 << (3 * num))) | (0x03 << (3 * num)); //8分频
    REG_TIMER_CR(num) = 0x01 << 4 | 0x01 << 3 | 0x02 << 1; //up counter,interrupt masked,Cyclic mode,close
    REG_TIMER_ICMODE |= 1 << num;

    REG_TIMER_ARR(num) = timer_frep;
    REG_TIMER_CX_PR(num) = pwm_freq;

    REG_TIMER_PCR &= 1 << (2 * num + 1);  //enable pwm interrupt

}

/************************************************************************
 * function   : pwm_start
 * Description: pwm start
 * input :
 *         UINT8 num: TIMER0,1,2,3
 * return: none
 ************************************************************************/
void pwm_start(UINT8 num)
{
    REG_TIMER_PCR |= 1 << 2 * num;  //使能
}

/************************************************************************
 * function   : pwm_stop
 * Description: pwm stop
 * input :
 *         UINT8 num: TIMER0,1,2,3
 * return: none
 ************************************************************************/
void pwm_stop(UINT8 num)
{
    REG_TIMER_PCR &= ~(1 << 2 * num);
}


/************************************************************************
 * function   : timer0_init
 * Description: timer0 GPIO initial, without NVIC
 * input : none
 * return: none
 ************************************************************************/
 void timer0_init(void)
{
	REG_SCU_CTRLA &= ~(0x01 << 27);		//使能TIMER模块时钟
	REG_SCU_MUXCTRLD	&= ~(0x04);	//设置管脚复用为GPIO49 TIMER0
	REG_SCU_PUCRB	|= 0x01 << 17;			//管脚49上拉控制有效
}

/************************************************************************
 * function   : timer0_set_us
 * Description: timer0 set_us
 * input :
 *         UINT32 timer_us: delay timer_us
 * return: none
 ************************************************************************/
void timer0_set_us(UINT32 timer_us)
{
    UINT32 timer_clk_hz;

    UINT32 tmp;

		REG_TIMER_CR(TIMER0) &= 0xfe;
	
    timer_clk_hz = PClock;
	
		REG_TIMER_CR(TIMER0) = 0x01 << 4 | 0x0E; //down counter,interrupt masked,Single mode,close
    //divided by 8,timer clk = (timer_clk_hz/8)

    REG_TIMER_PSC = (REG_TIMER_PSC & ~(0x07 << (3 *TIMER0))) | (0x00 << (3 * TIMER0)); //不分频，提高精度

    tmp = timer_clk_hz / (1 * 1000000); //定时器的真实时钟
    tmp *= timer_us;

    REG_TIMER_ARR(TIMER0) = tmp - 1; //延时tmp us，真实定时器计数值会比ARR多1个

	
		//使能
		REG_TIMER_CR(TIMER0) |= 0x01; 

		//判断是否产生中断
		while(!REG_TIMER_IF(TIMER0));
		//清除中断
		REG_TIMER_CIF(TIMER0) = 0xff;
}

/************************************************************************
 * function   : timer0_set_ms
 * Description: timer0 set_ms
 * input :
 *         UINT32 timer_us: delay timer_ms		//延时时间
 * return: none
 ************************************************************************/
void timer0_set_ms(UINT32 timer_ms)
{
    UINT32 timer_clk_hz;

    UINT32 tmp;

    timer_clk_hz = PClock;		//APB时钟/PCLK  , (uint:Hz)

		REG_TIMER_CR(TIMER0) = 0x01 << 4 | 0x0E; //down counter,interrupt masked,Single mode,close
    //divided by 8,timer clk = (timer_clk_hz/8)			TIMER_CR：timer0控制寄存器	0x14

    REG_TIMER_PSC = (REG_TIMER_PSC & ~(0x07 << (3 * TIMER0))) | (0x03 << (3 * TIMER0)); //8分频
		// TIM_PSC 定时器预分频寄存器
 	
    tmp = timer_clk_hz / (8 * 1000); //定时器的真实时钟
    tmp *= timer_ms;

    REG_TIMER_ARR(TIMER0) = tmp - 1; //延时tmp us,真实定时器计数值会比ARR多1个
		// timer加载寄存器	技计数周期为寄存器值加+1
	
	//使能
		REG_TIMER_CR(TIMER0) |= 0x01; 

	//判断是否产生中断
		while( !REG_TIMER_IF(TIMER0) );
	//清除中断
		REG_TIMER_CIF(TIMER0) = 0xff;
}

