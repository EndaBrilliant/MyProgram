#ifndef __TIMER0_H__
#define __TIMER0_H__
#include  "common.h"
#include	"types.h"

void Timer0Init(void);         //定时器初始化
void Timer0DelayMs(u32 ms);    //毫秒延时
void Timer0DelayUs(u32 us);    //微秒延时
#endif

