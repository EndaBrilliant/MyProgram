#ifndef __LCD_H__
#define __LCD_H__

#include	"common.h"

int getlength(UINT8 *buff);
int type_change(int num, UINT8 temp[256]);

void splice(UINT8 *head, UINT8 *middle, UINT8 *tail,UINT8 tempBuf[100]);
void splice_order(UINT8* head, int X, int Y, UINT8* order, int col, UINT8* tail, UINT8 buf[256]);

void CheckBusy(void);

void uart_LCD(UINT8 *head, int X, int Y, UINT8* order, int col);

void LCD_down(int num);

#endif
