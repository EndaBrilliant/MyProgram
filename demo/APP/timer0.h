#ifndef __TIMER0_H__
#define __TIMER0_H__
#include  "common.h"
#include	"types.h"

void Timer0Init(void);         //��ʱ����ʼ��
void Timer0DelayMs(u32 ms);    //������ʱ
void Timer0DelayUs(u32 us);    //΢����ʱ
#endif

