/***********************************************
  �˲�IIC
 **********************************************/

#include "timer0.h"
#include "timer.h"
#include "iic_sx.h"
#include "common.h"

#define 	SCL		0x08
#define		SDA		0x10

#define		SCL_OUT()				{REG_GPIO_DIR(GPIOB) |= SCL;}							//����SCL:GPIO35Ϊ���ģʽ
#define		SDA_OUT()				{REG_GPIO_DIR(GPIOB) |= SDA;}							//����SDA:GPIO36Ϊ���ģʽ
#define		SDA_IN()				{REG_GPIO_DIR(GPIOB) &= ~(SDA);}					//����SDA:GPIO36Ϊ����ģʽ

#define  	SCL_SET()				{REG_GPIO_SET(GPIOB) |=	SCL;}							//SCL��λ
#define		SCL_CLR()				{REG_GPIO_CLR(GPIOB) |= SCL;} 						//SCL����

#define  	SDA_SET()				{REG_GPIO_SET(GPIOB) |=	SDA;}							//SDA��λ
#define		SDA_CLR()				{REG_GPIO_CLR(GPIOB) |= SDA;} 						//SDA����

#define		READ_SDA		  	(REG_GPIO_IDATA(GPIOB) &= SDA)						//��SDAֵ		

#define		delay_w_1				100
#define		delay_w_2				220
	
//���������ά���飬�涨ÿһ��Ϊһ�������0λΪ��ַ(����λ)������Ϊ����
UINT8 sx_dataBuf[21][10] = {{ 0x50, 0x00, 0xCA, 0x00, 0x00, 0x00, 0x00},
														{ 0x50, 0x00, 0xCA, 0x02, 0x00, 0x00, 0x00},
														{ 0x50, 0x00, 0xCA, 0x03, 0x00, 0x00, 0x00},
														{ 0x50, 0x00, 0xCA, 0x04, 0x00, 0x00, 0x00},
														{ 0x50, 0x00, 0xCA, 0x05, 0x00, 0x00, 0x00},
														{ 0x50, 0x00, 0xCA, 0x06, 0x00, 0x00, 0x00},
														{ 0x50, 0xA4, 0xCF, 0x01, 0x00, 0x00, 0x00, 0x00, 0x3C, 0x71},
														{ 0x50, 0xA4, 0xCF, 0x02, 0x00, 0x00, 0x00, 0x00, 0x21, 0xBD},
														{ 0x50, 0xA4, 0xCF, 0x03, 0x00, 0x00, 0x00, 0x00, 0x2A, 0xF9},
														{ 0x50, 0xA4, 0xCF, 0x04, 0x00, 0x00, 0x00, 0x00, 0x1A, 0x25},
														{ 0x50, 0xA4, 0xCF, 0x05, 0x00, 0x00, 0x00, 0x00, 0x11, 0x61},
														{ 0x50, 0xA4, 0xCF, 0x06, 0x00, 0x00, 0x00, 0x00, 0x0C, 0xAD},
														{ 0x50, 0xA4, 0xCF, 0x06, 0x01, 0x00, 0x00, 0x00, 0x10, 0x16},
														{ 0x50, 0xA4, 0xCF, 0x06, 0x02, 0x00, 0x00, 0x00, 0x35, 0xDB},
														{ 0x50, 0xA4, 0xCF, 0x06, 0x03, 0x00, 0x00, 0x00, 0x29, 0x60},
														{ 0x50, 0xA4, 0xCF, 0x06, 0x04, 0x00, 0x00, 0x00, 0x7E, 0x41},
														{ 0x50, 0xA4, 0xCF, 0x06, 0x05, 0x00, 0x00, 0x00, 0x62, 0xFA},
														{ 0x50, 0xA4, 0xCF, 0x06, 0x06, 0x00, 0x00, 0x00, 0x47, 0x37},
														{ 0x50, 0xA4, 0xCF, 0x06, 0x07, 0x00, 0x00, 0x00, 0x5B, 0x8C},
														{ 0x50, 0xA4, 0xCF, 0x06, 0x08, 0x00, 0x00, 0x00, 0xE9, 0x75},
														{ 0x50, 0xA4, 0xCF, 0x07, 0x00, 0x00, 0x00, 0x00, 0x07, 0xE9}};								

													
//iic�������ݳ���													
long sx_lengthBuf[] = {25, 10, 7, 6, 36, 12, 36, 68, 260, 260, 260, 260, 
											 260, 260, 260, 260, 260, 260, 260, 260, 260, 260};

//�����������
UINT8 sx_readBuf[300] = {0};			


/************************************************************************
 * function   : iic_sx_init
 * Description: iic��ʼ��
 * input 			: none
 * return			: none
 ************************************************************************/
void iic_sx_init(void)
{
//GPIO35:SCL	GPIO36:SDA  -------------------------------------------------------------------
	REG_SCU_MUXCTRLC = (REG_SCU_MUXCTRLC & ~(0xF << 6));
	REG_SCU_PUCRB |= 0x18; 		//�ܽ�35 36������Ч
	
	REG_SCU_CTRLA &= ~(0x01 << 22);		//ʹ��GPIOģ��ʱ��
	REG_GPIO_IEN(GPIOB) &= ~(0x03 << 3);	//����GPIO35��36�ж�����
	
	SCL_OUT();		//����SCL:GPIO35Ϊ���ģʽ
	SDA_OUT();		//����SDA:GPIO36Ϊ���ģʽ
	
	Timer0DelayMs(30);
	
	SDA_SET();		//SCL��λ
	Timer0DelayMs(7);
	SCL_SET();		//SDA��λ
	Timer0DelayMs(25);
	
}

/************************************************************************
 * function   : scl_shake
 * Description: SCL����״̬
 * input 			: count����������
 * return			: none
 ************************************************************************/
void scl_shake(int count)
{
	long i = 0;
	for(i=0; i<count; i++)
	{			
		delay(30);
		SCL_SET();
		delay(35);	
		SCL_CLR();			//һ�����ڽ���ʱSCLΪ�͵�ƽ
	}
}

/************************************************************************
 * function   : sda_shake
 * Description: SDA����״̬����λʱ���ã�
 * input 			: count����������
 * return			: none
 ************************************************************************/
void sda_shake(int count)
{
	long i = 0;
	for(i=0; i<count; i++)
	{
		delay(90);
		SDA_CLR();
		delay(90);	
		SDA_SET();			//һ�����ڽ���ʱSDAΪ�ߵ�ƽ
	}
}

/************************************************************************
 * function   : iic_sx_reset
 * Description: ��λ
 * input 			: none
 * return			: none
 ************************************************************************/
void iic_sx_reset(void)
{
	Timer0DelayUs(1);
	SCL_SET();
	SDA_SET();
	Timer0DelayUs(9);
	SCL_CLR();
	
	sda_shake(20);
	
	SCL_SET();
	Timer0DelayUs(5);
}

/************************************************************************
 * function   : iic_sx_start
 * Description: iic���ݴ�����ʼ����
 * input 			: none
 * return			: none
 ************************************************************************/
void iic_sx_start(void)
{
	SDA_OUT();
	scl_shake(98);
	
	delay(2);
	SCL_SET();
	delay(25);	
	SCL_CLR();
	
	delay(15);
	SCL_SET();
	
	SDA_CLR();
}

/************************************************************************
 * function   : iic_sx_stop
 * Description: iic���ݴ�����ֹ����
 * input 			: none
 * return			: none
 ************************************************************************/
void iic_sx_stop(void)
{	
	SDA_OUT();		//SDA���ģʽ
	SDA_CLR();		//SDA����
	
	scl_shake(50);
	delay(30);
	SCL_SET();
	
	Timer0DelayUs(8);
	SDA_SET();		//SCL�ߵ�ƽ�ڼ�SDA��0��Ϊ1
	Timer0DelayUs(100);
}

/************************************************************************
 * function   : iic_sx_wait_ack
 * Description: ��ȡACK��Ӧ�ź� 0���ʾ�ź���Ч��1��ʾ�ź���Ч
 * input 			: none
 * return			: 0��ʾ�ź���Ч��1��ʾ�ź���Ч
 ************************************************************************/
 int iic_sx_wait_ack(void)
{
	UINT8 i;
	delay(25);
  
	SDA_IN();			//SDA����ģʽ
	Timer0DelayUs(1);	
	
	SCL_SET();

	i=READ_SDA;
	while( i )		//�ж�SDA״̬
	{
		iic_sx_stop();		//��Ӧ������ֹͣλ
			return 1;
	}
	Timer0DelayUs(3);	
	delay(10);

	SCL_CLR();
	delay(33);
	SCL_SET();
	delay(50);	
	SCL_CLR();
	
	return 0;
}

/************************************************************************
 * function   : iic_sx_ack
 * Description: iic����Ӧ���ź�
 * input 			: none
 * return			: none
 ************************************************************************/
void iic_sx_ack(void)
{
	SDA_OUT();
	SDA_CLR();
	
	SCL_SET();
	Timer0DelayUs(3);	
	delay(20);
	SCL_CLR();
	
	delay(35);
	SCL_SET();
	delay(45);
	SCL_CLR();
	
	delay(10);
	SCL_CLR();
}

/************************************************************************
 * function   : iic_sx_nack
 * Description: iic���ͷ�Ӧ���ź�
 * input 			: none
 * return			: none
 ************************************************************************/
void iic_sx_nack(void)
{
	SCL_CLR();
	SDA_OUT();
	SDA_SET();
	
	SCL_SET();
	Timer0DelayUs(3);	
	delay(10);
	SCL_CLR();
	
	delay(40);
	SCL_SET();
	delay(50);
	SCL_CLR();
}

/************************************************************************
 * function   : iic_sx_write_byte
 * Description: iic���ֽ�д
 * input 			: UINT32 data��iicҪд����ֽ�
 * return			: none
 ************************************************************************/
int iic_sx_write_byte(UINT32 data)
{
	int i;
	UINT32 write_data_byte = data;
	SDA_OUT();
	for(i=7; i>=0; i--)
	{	
		SCL_CLR();		//SCL���Ϳ�ʼд����
		delay(20);
		Timer0DelayUs(1);
		if( (write_data_byte & 0x80) == 0x80 )		//�ж�д0����1
		{
			SDA_SET();
		}
		else
		{
			SDA_CLR();
		}
		write_data_byte <<= 1; 	//ÿ��ȡһλ������һλ	
		
		SCL_SET();
		delay(20);
		Timer0DelayUs(3);
		
		SCL_CLR();
	}
	
	if(iic_sx_wait_ack())	//��Ӧ�𷵻�0
		return 0;
	else
		return 1;
}

/******************************************************************************
 * function   : iic_sx_write
 * Description: iic�����鷢��
 * input 			: UINT8 *data��iicҪ���͵����飬 UINT8 data_len��iic�������鳤��
 * return			: ���ͳ�����1����ȷ����0
 ******************************************************************************/
void iic_sx_write(UINT8 *data, UINT8 data_len)
{
	int i,count=0;
	iic_sx_start();			//��ʼ����
	
	for(i=0; i < (data_len); i++)			//д����
	{
			while( iic_sx_write_byte(data[i]) == 0)				//��Ӧ��ѭ������5��
			{	
				i = 1;			// i=1 ��֤ÿ�����·��Ͷ��ӵ�һλ���ݿ�ʼ����
				if( count >= 8 )
				{
					iic_sx_stop();
					return ;
				}
				iic_sx_start();
				count++;
			}
	}
	
	iic_sx_stop();		//IIC��ֹ����
	
	for(i=0; i < data_len; i++)			//���ڴ�ӡ
	{
		printf("%.2x ",data[i]);
	}
	printf("\r\n");	

}

/************************************************************************
 * function   : iic_sx_read_byte
 * Description: iic���ֽڽ���
 * input 			: UINT8 receive��iic���յ��ֽ�
 * return			: none
 ************************************************************************/
UINT8 iic_sx_read_byte(UINT8 receive, int ack)
{
	int	SDA_Single = 0;
	UINT32 i;
	SDA_IN()					//����SDAΪ����
	
	delay(45);
	for(i=0; i < 8; i++)
	{
		delay(15);
		SCL_SET();			//ʱ���߸� �ӻ��ı�SDA
		Timer0DelayUs(3);
		delay(20);
		SCL_CLR();
		receive <<= 1;
		SDA_Single = READ_SDA;	//��ȡSDA��ֵ�仯
		
		Timer0DelayUs(1);
		
		delay(15);
		
		if(SDA_Single != 0)	
		{ 
			receive++; 
		}
	}
	if(!ack)
	{
		iic_sx_ack();		//����Ӧ���ź�
	}
	else
	{
		iic_sx_nack();	//���ͷ�Ӧ���ź�
	}
	
	return receive;
}

/************************************************************************
 * function   : iic_sx_read
 * Description: iic�������������
 * input 			: UINT8 *sx_readBuf:���������飬 long length���������ݳ���
 * return			: ���ͳ�����1�� ��ȷ���ؽ�������
 ************************************************************************/
UINT8 iic_sx_read(UINT8 *sx_readBuf, long length)
{
	long i=0;
	int count=0;
	scl_shake(50);
	iic_sx_start();
	
	while( iic_sx_write_byte(0x51) == 0)	//��Ӧ��ѭ������8��
	{	
		scl_shake(300);
		iic_sx_start();
		if( count>=8 )
		{
			iic_sx_stop();
			return 0;
		}
		count++;
	}
	
	delay(10);

	while(i<length)			//��ȡ����
	{
		if(i == length-1)
		{
			sx_readBuf[i] = iic_sx_read_byte(sx_readBuf[i], 1);		//ackΪ1����ȡ���һλ���ͷ�Ӧ��λ
		}
		else
		{
			sx_readBuf[i] = iic_sx_read_byte(sx_readBuf[i], 0);		//ackΪ0������Ӧ��λ
		}
		i++;
	}
	
	iic_sx_stop();								//IIC��ֹ����
		
	for(i=0; i < length; i++)			//���ڴ�ӡ
	{
		printf("%.2x ",sx_readBuf[i]);
	}
	printf("\r\n");	
	
	return *sx_readBuf;				  	//���ؽ��յ�������
}

/************************************************************************
 * function   : iic_sx_rx_txf
 * Description: iic�������������
 * input 			: UINT8 *sx_readBuf:���������飬 long length���������ݳ���
 * return			: ���ͻ���ճ�����1����ȷ����0
 ************************************************************************/
void iic_sx_rx_txf(UINT8 *data, UINT8 data_len, UINT8 *sx_readBuf, long length, UINT32 timer)
{
	memset((UINT8 *)sx_readBuf, 0, 300);					//��ս���buf
		
	if(timer < 150)
	{
		Timer0DelayMs(15);
		iic_sx_write((UINT8 *)data, data_len);			//дָ��
		
		Timer0DelayMs(timer);
		iic_sx_read((UINT8 *)sx_readBuf, length);		//����������
	}
	else
	{
		Timer0DelayMs(15);
		iic_sx_write((UINT8 *)data, data_len);			//дָ��
		
		Timer0DelayMs(150);
	 	scl_shake(timer*6);	
		iic_sx_read((UINT8 *)sx_readBuf, length);		//����������
	}
}	
	

/********************************************************************************************
*********************************************************************************************/
void iic_sx_test()
{
	iic_sx_init();

	scl_shake(1000);		//SCL����״̬
	iic_sx_reset();			//��λ
	
	iic_sx_read((UINT8 *)sx_readBuf, sx_lengthBuf[0]);	
	
	iic_sx_rx_txf((UINT8 *)sx_dataBuf[0],  7,  (UINT8 *)sx_readBuf,  sx_lengthBuf[1],   delay_w_1);
	iic_sx_rx_txf((UINT8 *)sx_dataBuf[1],  7,  (UINT8 *)sx_readBuf,  sx_lengthBuf[2],   delay_w_1);
	iic_sx_rx_txf((UINT8 *)sx_dataBuf[2],  7,  (UINT8 *)sx_readBuf,  sx_lengthBuf[3],   delay_w_1);
	iic_sx_rx_txf((UINT8 *)sx_dataBuf[3],  7,  (UINT8 *)sx_readBuf,  sx_lengthBuf[4],   delay_w_1);
	iic_sx_rx_txf((UINT8 *)sx_dataBuf[4],  7,  (UINT8 *)sx_readBuf,  sx_lengthBuf[5],   delay_w_1);
	
	iic_sx_rx_txf((UINT8 *)sx_dataBuf[5],  7,  (UINT8 *)sx_readBuf,  sx_lengthBuf[6],   delay_w_2);
	iic_sx_rx_txf((UINT8 *)sx_dataBuf[6],  10, (UINT8 *)sx_readBuf,  sx_lengthBuf[7],   delay_w_2);
	iic_sx_rx_txf((UINT8 *)sx_dataBuf[7],  10, (UINT8 *)sx_readBuf,  sx_lengthBuf[8],   delay_w_2);
	iic_sx_rx_txf((UINT8 *)sx_dataBuf[8],  10, (UINT8 *)sx_readBuf,  sx_lengthBuf[9],   delay_w_2);
	iic_sx_rx_txf((UINT8 *)sx_dataBuf[9],  10, (UINT8 *)sx_readBuf,  sx_lengthBuf[10],  delay_w_2);
	iic_sx_rx_txf((UINT8 *)sx_dataBuf[10], 10, (UINT8 *)sx_readBuf,  sx_lengthBuf[11],  delay_w_2);
	iic_sx_rx_txf((UINT8 *)sx_dataBuf[11], 10, (UINT8 *)sx_readBuf,  sx_lengthBuf[11],  delay_w_2);
	iic_sx_rx_txf((UINT8 *)sx_dataBuf[12], 10, (UINT8 *)sx_readBuf,  sx_lengthBuf[13],  delay_w_2);
	iic_sx_rx_txf((UINT8 *)sx_dataBuf[13], 10, (UINT8 *)sx_readBuf,  sx_lengthBuf[14],  delay_w_2);
	iic_sx_rx_txf((UINT8 *)sx_dataBuf[14], 10, (UINT8 *)sx_readBuf,  sx_lengthBuf[15],  delay_w_2);
	
	iic_sx_rx_txf((UINT8 *)sx_dataBuf[15], 10, (UINT8 *)sx_readBuf,  sx_lengthBuf[16],  delay_w_2);
	iic_sx_rx_txf((UINT8 *)sx_dataBuf[16], 10, (UINT8 *)sx_readBuf,  sx_lengthBuf[17],  delay_w_2);
	iic_sx_rx_txf((UINT8 *)sx_dataBuf[17], 10, (UINT8 *)sx_readBuf,  sx_lengthBuf[18],  delay_w_2);
	iic_sx_rx_txf((UINT8 *)sx_dataBuf[18], 10, (UINT8 *)sx_readBuf,  sx_lengthBuf[19],  delay_w_2);
	iic_sx_rx_txf((UINT8 *)sx_dataBuf[19], 10, (UINT8 *)sx_readBuf,  sx_lengthBuf[20],  delay_w_2);
	iic_sx_rx_txf((UINT8 *)sx_dataBuf[20], 10, (UINT8 *)sx_readBuf,  sx_lengthBuf[21],  delay_w_2);
}


















