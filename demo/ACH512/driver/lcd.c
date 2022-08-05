#include 	"timer.h"
#include	"uart.h"
#include	"iic.h"
#include	"app.h"
#include	"common.h"
#include	"stdio.h"
#include	"lcd.h"

/************************************************************************
 * function   : getlength
 * Description: ��ȡbuff����
 * input : UINT8 *buff
 * return: ���鳤��count
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
 * Description: ��ʽת������intת��ΪUINT8(unsigned char)�����������temp��lengthΪ����
 * input : Ҫת��������num��������ݵ�����temp
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
 * Description: ƴ���ַ������飬��head��buf��tail˳��ƴ�Ӳ������tempBuf��
 * input : ������ƴ�ӵ��ַ��� head middle tail
 * return: none
 ************************************************************************/
//----------------------�˺���δ����------------------------------------
void splice(UINT8 *head, UINT8 *middle, UINT8 *tail,UINT8 tempBuf[100])
{
	int HL = getlength(head);		//��ȡhead����
	int ML = getlength(middle);	//��ȡmiddle����
	int TL = getlength(tail);		//��ȡtail����
		
	int i=0;
	
	for(i=0; i<HL; i++)			//��headд��tempBuf
	{	
		tempBuf[i] = head[i];		
	}

	for(i=0; i < ML; i++)		//��middle׷�ӽ�tempBuf
	{
		tempBuf[HL+i] = middle[i];
	}
	
	for(i=0; i<TL; i++)			//��tail׷�ӽ�tempBuf
	{
		tempBuf[HL+ML+i] = tail[i];
	}
}

/************************************************************************
 * function   : splice_order
 * Description: ƴ���ַ������飬��head��X��Y��order��col��tail��˳��ƴ�Ӳ������Buf��
 *							splice_order((UINT8*)"SBC(15);dcv32(",120, 230, str, 55, (UINT8*)");\r\n", tmpBuf);
 *							Buf�еõ� SBC(15);DCV32(120,230,str,55); 
 * input      : head������ͷ����X�������꣬Y�������꣬str����ʾ���ַ���
 *				      col����ɫ��tail������β����buf�����ݴ洢buf
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
 * Description: æ���
 * input 			: none
 * return			: none
 ************************************************************************/
void CheckBusy(void)
{
	int count = 0;
	while (1)
	{
		if((ok == 0x0f) || ( count++ > 400))	//����ʱ���
		{
			break;
		}
	}
	ok = 0;
}

/************************************************************************
 * function   : uart_LCD
 * Description: ����LCD��ʾ�ַ���ָ��
 * input 			: head:����ͷ����X:�����꣬Y�������꣬order�����col����ɫ
 * return			: none
 ************************************************************************/
//ʾ����UartSend(T_DC32(176, 280, "ȫ��ͨ��", 23)); 
void uart_LCD(UINT8 *head, int X, int Y, UINT8* order, int col)
{	
	UINT8 *Buf;
	UINT8 tempBuf[256] = {0};
	
	splice_order(head, X, Y, order, col, ");\r\n", tempBuf);			//ƴ���ַ���
	
	Buf = (UINT8 *)tempBuf;
	
	uart_send_bytes(UARTB, Buf, getlength(Buf));
}

/************************************************************************
 * function   : LCD_down
 * Description: ��ʱ��
 * input 			: ���ʱ num
 * return			: none
 ************************************************************************/
void LCD_down(int num)
{
	int i=0;
	
	uart_send_bytes(UARTB, "CLR(0);\r\n", strlen("CLR(0);\r\n"));		//����
	CheckBusy();									//æ���
	
	while( i <= num )
	{		
		UINT8 str[256];
		type_change(i++, str);			//��ʽת��

		uart_LCD("SBC(15);DCV32(", 0, 0, str, 1);
 		CheckBusy();								//æ���
		timer0_set_ms(1000);				//��ʱ1��
 	}
}


