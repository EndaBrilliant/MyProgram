/***********************************************************************
 * Copyright (c)  2008 - 2016, Shanghai AisinoChip Co.,Ltd .
 * All rights reserved.
 * Filename    : config.h
 * Description : config header file
 * Author(s)   : Eric  
 * version     : V1.0
 * Modify date : 2016-03-24
 ***********************************************************************/
#ifndef __CONFIG_H__
#define __CONFIG_H__

#define DEBUG    //printf���Խӿ�ʹ��

#define DEBUG_UART    UARTA  //���Դ�ӡ�Ķ˿�
 
#ifdef DEBUG
#define printfS     printf
#define printfB8    printf_buff_byte
#define printfB32   printf_buff_word	 
#else
#define	printfS(format, ...)	     ((void)0)
#define	printfB8(buff, byte_len)	 ((void)0)
#define	printfB32(buff, word_len)	 ((void)0)
#endif

/*--------------- ʱ������ ----------------------- */
#define FCLK 110  //����coreʱ��/HCLK ,������Ϊ��6 12 30 48 50 60 70 80 90 100 110 �� (uint:MHz)
  
/*--------------- uart���� ----------------------- */
//ͨ�Ÿ�ʽ���ã�8λ����λ��1λֹͣλ����У��λ

#define UART_BAUD_RATE	115200
//#define UART_Tx_INT_MODE   // Tx�����жϷ�ʽ  ��������Rxʼ�ղ����жϷ�ʽ��
//#define UARTB_USE_CTSMODE    //CTS mode
//#define UARTB_USE_RTSMODE	 //RTS mode

#endif
