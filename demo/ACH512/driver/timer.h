/***********************************************************************
 * Copyright (c)  2008 - 2016, Shanghai AisinoChip Co.,Ltd .
 * All rights reserved.
 * Filename    : timer.h
 * Description : timer driver header file
 * Author(s)   : Eric
 * version     : V1.0
 * Modify date : 2016-03-24
 ***********************************************************************/

#ifndef __TIMER_H__
#define __TIMER_H__

#include  "common.h"

/*----------------------TIMER BIT------------------------*/
#define VAL_TIMER_CONTROL_MOD_FREE     0
#define VAL_TIMER_CONTROL_MOD_CYC      2
#define VAL_TIMER_CONTROL_MOD_SINGLE   3

#define VAL_TIMER_PRES_DIVISOR_1       0
#define VAL_TIMER_PRES_DIVISOR_2       1
#define VAL_TIMER_PRES_DIVISOR_4       2
#define VAL_TIMER_PRES_DIVISOR_8       3
#define VAL_TIMER_PRES_DIVISOR_16      4
#define VAL_TIMER_PRES_DIVISOR_32      5
#define VAL_TIMER_PRES_DIVISOR_64      6
#define VAL_TIMER_PRES_DIVISOR_128     7

#define CAPTURE_TRIGGER_RISING         0
#define CAPTURE_TRIGGER_FALLING        1

extern volatile UINT8 flag_capture_int[];
extern volatile UINT32 CaptureCounter[];
extern volatile UINT8 flag_timer_int[];

/************************************************************************
 * function   : timer_init
 * Description: timer initial
 * input : none
 * return: none
 ************************************************************************/
void timer_init(void);

/************************************************************************
 * function   : timer_set_us
 * Description: timer set_us
 * input :
 *         UINT8 num: TIMER0,1,2,3
 *         UINT32 timer_us: delay timer_us
 *         void(*pFunc)() pFunc: processing function
 * return: none
 ************************************************************************/
void timer_set_us(UINT8 num, UINT32 timer_us, void (*pFunc)());

/************************************************************************
 * function   : timer_set_ms
 * Description: timer set_ms
 * input :
 *         UINT8 num: TIMER0,1,2,3
 *         UINT32 timer_us: delay timer_ms
 *         void(*pFunc)() pFunc: processing function
 * return: none
 ************************************************************************/
void timer_set_ms(UINT8 num, UINT32 timer_ms, void (*pFunc)());

/************************************************************************
 * function   : timer_start
 * Description: timer start
 * input :
 *         UINT8 num: TIMER0,1,2,3
 * return:
 ************************************************************************/
void timer_start(UINT8 num);

/************************************************************************
 * function   : timer_stop
 * Description: timer stop
 * input :
 *         UINT8 num: TIMER0,1,2,3
 * return:
 ************************************************************************/
void timer_stop(UINT8 num);

/************************************************************************
 * function   : capture_set
 * Description: capture set
 * input :
 *         UINT8 num: only TIMER0,2
 *         UINT8 trigger: 0 -- CAPTURE_TRIGGER_RISING
 *                        1 -- CAPTURE_TRIGGER_FALLING
 * return: none
 ************************************************************************/
void capture_set(UINT8 num, UINT8 trigger);

/************************************************************************
 * function   : capture_start
 * Description: capture start
 * input :
 *         UINT8 num: only TIMER0,2
 * return: none
 ************************************************************************/
void capture_start(UINT8  num);

/************************************************************************
 * function   : capture_stop
 * Description: capture stop
 * input :
 *         UINT8 num: only TIMER0,2
 * return: none
 ************************************************************************/
void capture_stop(UINT8  num);

/************************************************************************
 * function   : pwm_set
 * Description: pwm set frequence
 * input :
 *         UINT8 num: TIMER0,1,2,3
 *         UINT32 timer_freq: timer frequence
 *         UINT32 pwm_freq: pwm frequence
 * return: none
 ************************************************************************/
void pwm_set(UINT8 num, UINT32 timer_frep, UINT32 pwm_freq);

/************************************************************************
 * function   : pwm_start
 * Description: pwm start
 * input :
 *         UINT8 num: TIMER0,1,2,3
 * return: none
 ************************************************************************/
void pwm_start(UINT8 num);

/************************************************************************
 * function   : pwm_stop
 * Description: pwm stop
 * input :
 *         UINT8 num: TIMER0,1,2,3
 * return: none
 ************************************************************************/
void pwm_stop(UINT8 num);

void timer0_init(void);
void timer0_set_ms(UINT32 timer_ms);
void timer0_set_us(UINT32 timer_ms);

#endif

