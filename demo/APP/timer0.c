#include  "timer0.h"
#include	"types.h"

void Timer0Init(void)
{
	REG_SCU_CTRLA &= ~(1 << 27);    //��TIMERʱ��
	
	REG_TIMER_CR(TIMER0) &= ~0x01;	//���ü�����ǰ�ȹر�
	REG_TIMER_CR(TIMER0) = 0x1E;		//���¼������ж����Σ�ѭ��ģʽ, �ر�ʹ��
	REG_TIMER_PSC &= ~0x07; 				//1��Ƶ
}

void Timer0DelayMs(u32 ms)        //���붨ʱ��
{
	REG_TIMER_CR(TIMER0) &= ~0x01;	//�ر�ʹ��
	REG_TIMER_ARR(TIMER0) = 55000 * ms;
	REG_TIMER_CR(TIMER0) |= 0x01;		//ʹ�ܼ�����
	
	while(!REG_TIMER_IF(TIMER0));
	
	REG_TIMER_CIF(TIMER0) = 0xFF;  //�ж����
}

void Timer0DelayUs(u32 us)       //΢�붨ʱ��
{
	REG_TIMER_CR(TIMER0) &= ~0x01;
	REG_TIMER_ARR(TIMER0) = 55 * us;
	REG_TIMER_CR(TIMER0) |= 0x01;
	
	while(!REG_TIMER_IF(TIMER0));
	
	REG_TIMER_CIF(TIMER0) = 0xFF;  //�ж����
}


