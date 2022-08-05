#include 	"timer.h"
#include	"uart.h"
#include	"iic.h"
#include	"app.h"
#include	"common.h"
#include	"stdio.h"
#include	"lcd.h"

/************************************************************************
 * function   : getlength
 * Description: 获取buff长度
 * input : UINT8 *buff
 * return: 数组长度count
 ************************************************************************/
int getlength(UINT8 *buff)
{
	int count = 0;
	while(buff[count] != '\0')
	{
		count++;
	}
	return count;
}

/****************************************************************************************
 * function   : type_change
 * Description: 格式转换，将int转换为UINT8(unsigned char)，存放在数组temp，length为长度
 * input : 要转换的数字num，存放数据的数组temp
 * return: none
 ****************************************************************************************/
int type_change(int num, UINT8 temp[256])
{
	UINT8 test[256];
	int i=0,j=0;
	do{
		test[i++] = num%10 + '0';
		num /= 10;
	}while( num != 0 );
	
	for(i--; i>=0; i--)
	{
		temp[j++] = test[i];
	}
	return j;
}

/************************************************************************
 * function   : splice
 * Description: 拼接字符串数组，按head、buf、tail顺序拼接并存放在tempBuf中
 * input : 三个待拼接的字符串 head middle tail
 * return: none
 ************************************************************************/
//----------------------此函数未调用------------------------------------
void splice(UINT8 *head, UINT8 *middle, UINT8 *tail,UINT8 tempBuf[100])
{
	int HL = getlength(head);		//获取head长度
	int ML = getlength(middle);	//获取middle长度
	int TL = getlength(tail);		//获取tail长度
		
	int i=0;
	
	for(i=0; i<HL; i++)			//将head写入tempBuf
	{	
		tempBuf[i] = head[i];		
	}

	for(i=0; i < ML; i++)		//将middle追加进tempBuf
	{
		tempBuf[HL+i] = middle[i];
	}
	
	for(i=0; i<TL; i++)			//将tail追加进tempBuf
	{
		tempBuf[HL+ML+i] = tail[i];
	}
}

/************************************************************************
 * function   : splice_order
 * Description: 拼接字符串数组，按head、X、Y、order、col、tail按顺序拼接并存放在Buf中
 *							splice_order((UINT8*)"SBC(15);dcv32(",120, 230, str, 55, (UINT8*)");\r\n", tmpBuf);
 *							Buf中得到 SBC(15);DCV32(120,230,str,55); 
 * input      : head：命令头部，X：横坐标，Y：纵坐标，str：显示的字符，
 *				      col：颜色，tail：命令尾部，buf：数据存储buf
 * return     : none
 ************************************************************************/
void splice_order(UINT8* head, int X, int Y, UINT8* order, int col, UINT8* tail, UINT8 buf[256])
{
	unsigned char temp[256];
	int length = 0;
	int i = 0,p = 0;
	for(i=0; i<getlength(head); i++)
	{
		buf[p++] = head[i];
	}
	
	length = type_change(X, temp);
	for(i=0; i<length; i++)
	{
		buf[p++] = temp[i];
	}
	buf[p++] = ',';
	
	length = type_change(Y, temp);
	for(i=0; i<length; i++)
	{
		buf[p++] = temp[i];
	}
	buf[p++] = ',';
	
	for(i=0; i<getlength(order); i++)
	{
		buf[p++] = order[i];
	}
	buf[p++] = ',';
	
	length = type_change(col, temp);
	for(i=0; i<length; i++)
	{
		buf[p++] = temp[i];
	}
	
	for(i=0; i<getlength(tail); i++)
	{
		buf[p++] = tail[i];
	}
}

/************************************************************************
 * function   : CheckBusy
 * Description: 忙检测
 * input 			: none
 * return			: none
 ************************************************************************/
void CheckBusy(void)
{
	int count = 0;
	while (1)
	{
		if((ok == 0x0f) || ( count++ > 400))	//带超时检测
		{
			break;
		}
	}
	ok = 0;
}

/************************************************************************
 * function   : uart_LCD
 * Description: 发送LCD显示字符串指令
 * input 			: head:命令头部，X:横坐标，Y：纵坐标，order：命令，col：颜色
 * return			: none
 ************************************************************************/
//示例：UartSend(T_DC32(176, 280, "全部通过", 23)); 
void uart_LCD(UINT8 *head, int X, int Y, UINT8* order, int col)
{	
	UINT8 *Buf;
	UINT8 tempBuf[256] = {0};
	
	splice_order(head, X, Y, order, col, ");\r\n", tempBuf);			//拼接字符串
	
	Buf = (UINT8 *)tempBuf;
	
	uart_send_bytes(UARTB, Buf, getlength(Buf));
}

/************************************************************************
 * function   : LCD_down
 * Description: 计时器
 * input 			: 最大几时 num
 * return			: none
 ************************************************************************/
void LCD_down(int num)
{
	int i=0;
	
	uart_send_bytes(UARTB, "CLR(0);\r\n", strlen("CLR(0);\r\n"));		//清屏
	CheckBusy();									//忙检测
	
	while( i <= num )
	{		
		UINT8 str[256];
		type_change(i++, str);			//格式转换

		uart_LCD("SBC(15);DCV32(", 0, 0, str, 1);
 		CheckBusy();								//忙检测
		timer0_set_ms(1000);				//定时1秒
 	}
}


