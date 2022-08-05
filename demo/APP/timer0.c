#include  "timer0.h"
#include	"types.h"

void Timer0Init(void)
{
	REG_SCU_CTRLA &= ~(1 << 27);    //开TIMER时钟
	
	REG_TIMER_CR(TIMER0) &= ~0x01;	//配置计数器前先关闭
	REG_TIMER_CR(TIMER0) = 0x1E;		//向下计数，中断屏蔽，循环模式, 关闭使能
	REG_TIMER_PSC &= ~0x07; 				//1分频
}

void Timer0DelayMs(u32 ms)        //毫秒定时器
{
	REG_TIMER_CR(TIMER0) &= ~0x01;	//关闭使能
	REG_TIMER_ARR(TIMER0) = 55000 * ms;
	REG_TIMER_CR(TIMER0) |= 0x01;		//使能计数器
	
	while(!REG_TIMER_IF(TIMER0));
	
	REG_TIMER_CIF(TIMER0) = 0xFF;  //中断清除
}

void Timer0DelayUs(u32 us)       //微秒定时器
{
	REG_TIMER_CR(TIMER0) &= ~0x01;
	REG_TIMER_ARR(TIMER0) = 55 * us;
	REG_TIMER_CR(TIMER0) |= 0x01;
	
	while(!REG_TIMER_IF(TIMER0));
	
	REG_TIMER_CIF(TIMER0) = 0xFF;  //中断清除
}


