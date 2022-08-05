/***********************************************
	��׼IIC
 **********************************************/

#include "timer0.h"
#include "timer.h"
#include "iic.h"

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

#define		GPIO27_SET()	  {REG_GPIO_SET(GPIOA) |=	(0x01 << 27);}		//GPIO27��28��λ
#define		GPIO28_SET()	  {REG_GPIO_SET(GPIOA) |=	(0x01 << 28);}
#define		GPIO27_CLR()	  {REG_GPIO_CLR(GPIOA) |=	(0x01 << 27);}		//GPIO27��28����
#define		GPIO28_CLR()	  {REG_GPIO_CLR(GPIOA) |=	(0x01 << 28);}

#define		IIC_POW_SET()		{REG_GPIO_SET(GPIOB) |=	(0x04);}					//iic_power(GPIO34)��λ������
#define 	IIC_POW_CLR()		{REG_GPIO_CLR(GPIOB) |=	(0x04);}

#define		delay_w_r1			50		
#define		delay_w_r2			150

//���������ά���飬�涨ÿһ��Ϊһ�������0λΪ��ַ(����λ)������Ϊ����							
UINT8 dataBuf[21][10] = { { 0x50, 0x00, 0xCA, 0x00, 0x00, 0x00, 0x00},
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
													{ 0x50, 0xA4, 0xCF, 0x07, 0x00, 0x00, 0x00, 0x00, 0x07, 0xE9} };		
													
//iic�������ݳ���													
long lengthBuf[] = {25, 10, 7, 6, 36, 12, 36, 68, 260, 260, 260, 260, 
										260, 260, 260, 260, 260, 260, 260, 260, 260, 260};

//�����������
UINT8 readBuf[300] = {0};			


/************************************************************************
 * function   : iic_init
 * Description: iic��ʼ��
 * input 			: none
 * return			: none
 ************************************************************************/
void iic_init(void)
{
//IIC_Power:GPIO34. GPIO27,GPIO28  ----------------------------------------------------------
	REG_SCU_MUXCTRLB = (REG_SCU_MUXCTRLB & ~(0xF << 22));	//����GPIO 27��28���ùܽ�
	REG_SCU_MUXCTRLC = (REG_SCU_MUXCTRLC & ~(0x30));			//����IIC_POWER IO��GPIO34�����ùܽ�
	
	REG_SCU_PUCRA |= (0x03 << 27);							//�ܽ�������Ч
	REG_SCU_PUCRB |= (0x04);
	
	REG_GPIO_IEN(GPIOA) &= ~(0x03 << 27);				//����GPIO 27��28��34�����ж�
	REG_GPIO_IEN(GPIOB) &= ~(0x01 << 3);
	
	REG_GPIO_DIR(GPIOA)	|= (0x03 << 27);			  //����GPIO 27��28��34�����Ч
	REG_GPIO_DIR(GPIOB)	|= (0x01 << 3);
	
	
//GPIO35:SCL	GPIO36:SDA  -------------------------------------------------------------------
	REG_SCU_MUXCTRLC = (REG_SCU_MUXCTRLC & ~(0xF << 6));
	REG_SCU_PUCRB |= 0x18; 		//�ܽ�35 36������Ч
	
	REG_SCU_CTRLA &= ~(0x01 << 22);		//ʹ��GPIOģ��ʱ��
	REG_GPIO_IEN(GPIOB) &= ~(0x03 << 3);	//����GPIO35��36�ж�����
	
	SCL_OUT();		//����SCL:GPIO35Ϊ���ģʽ
	SDA_OUT();		//����SDA:GPIO36Ϊ���ģʽ
	
	SDA_SET();		//SDA��λ
	SCL_SET();		//SCL��λ
	
	Timer0DelayUs(5);
}	

/************************************************************************
 * function   : iic_power
 * Description: iic����ʹ��
 * input 			: state��Ϊ1ʱʹ�ܣ�0ʱ�ϵ�
 * return			: none
 ************************************************************************/
void iic_power(int state)
{
	if(state == 1)
	{
		GPIO27_SET();
		GPIO28_SET();
		IIC_POW_SET();
	}
	else
	{
		GPIO27_CLR();
		GPIO28_CLR();
		IIC_POW_CLR();
	}
}

/************************************************************************
 * function   : iic_start
 * Description: iic���ݴ�����ʼ����
 * input 			: none
 * return			: none
 ************************************************************************/
void iic_start(void)
{
	Timer0DelayUs(5);
	SDA_OUT();		//SDA���ģʽ
	
	SDA_SET();		//SDA SCL��λ
	Timer0DelayUs(5);
	SCL_SET();
	
	Timer0DelayUs(5);
	
	SDA_CLR();		//SDA����
	Timer0DelayUs(5);
}

/************************************************************************
 * function   : iic_stop
 * Description: iic���ݴ�����ֹ����
 * input 			: none
 * return			: none
 ************************************************************************/
void iic_stop(void)
{	
	SDA_OUT();		//SDA���ģʽ
	
	SCL_CLR();		//SCL SDA����
	Timer0DelayUs(5);
	SDA_CLR();
	Timer0DelayUs(5);
	
	SCL_SET();		//��SCL�ߵ�ƽ�ڼ䣬SDA����
	Timer0DelayUs(5);
	SDA_SET();
	Timer0DelayUs(5);
}

/************************************************************************
 * function   : iic_wait_ack
 * Description: ��ȡACK��Ӧ�ź� 0���ʾ�ź���Ч��1��ʾ�ź���Ч
 * input 			: none
 * return			: 0��ʾ�ź���Ч��1��ʾ�ź���Ч
 ************************************************************************/
 int iic_wait_ack(void)
{
	SDA_IN();			//SDA����ģʽ
	Timer0DelayUs(1);
	SCL_SET();	
	Timer0DelayUs(2);
	
	while( READ_SDA )		//�ж�SDA״̬
	{
		iic_stop();
			return 1;
	}
	
	Timer0DelayUs(3);
	SCL_CLR();
	Timer0DelayUs(2);
	return 0;
}

/************************************************************************
 * function   : iic_ack
 * Description: iic����Ӧ���ź�
 * input 			: none
 * return			: none
 ************************************************************************/
void iic_ack(void)
{
	SCL_CLR();
	SDA_OUT();
	SDA_CLR();
	Timer0DelayUs(2);
	SCL_SET();
	Timer0DelayUs(5);
	SCL_CLR();
	Timer0DelayUs(5);
}

/************************************************************************
 * function   : iic_nack
 * Description: iic���ͷ�Ӧ���ź�
 * input 			: none
 * return			: none
 ************************************************************************/
void iic_nack(void)
{
	SCL_CLR();
	SDA_OUT();
	SDA_SET();
	Timer0DelayUs(2);
	SCL_SET();
	Timer0DelayUs(5);
	SCL_CLR();
	Timer0DelayUs(5);
}

/************************************************************************
 * function   : iic_write_byte
 * Description: iic���ֽ�д
 * input 			: UINT32 data��iicҪд����ֽ�
 * return			: none
 ************************************************************************/
int iic_write_byte(UINT32 data)
{
	int i;
	UINT32 write_data_byte = data;
	SDA_OUT();
	
	SCL_CLR();		//SCL���Ϳ�ʼд����
	Timer0DelayUs(3);
	for(i=7; i>=0; i--)
	{	
		if( (write_data_byte & 0x80) == 0x80 )		//�ж�д0����1
		{
			SDA_SET();
		}
		else
		{
			SDA_CLR();
		}
		write_data_byte <<= 1; 	//ÿ��ȡһλ������һλ	
		Timer0DelayUs(2);
		
		SCL_SET();
		Timer0DelayUs(5);
		
		SCL_CLR();
		Timer0DelayUs(2);
	}
	
	if(iic_wait_ack())	//��Ӧ�𷵻�0
		return 0;
	else
		return 1;
}

/******************************************************************************
 * function   : iic_write
 * Description: iic�����鷢��
 * input 			: UINT8 *data��iicҪ���͵����飬 UINT8 data_len��iic�������鳤��
 * return			: none
 ******************************************************************************/
void iic_write(UINT8 *data, UINT8 data_len)
{
	int i;
	iic_start();	//��ʼ����
	
	for(i=0; i<(data_len); i++)				//д���� 
	{
		if(iic_write_byte(data[i]) == 0)
			return;
	}
	
	iic_stop();		//IIC��ֹ����
	
	for(i=0; i<=(data_len); i++)			//���ڴ�ӡ
	{
		printf("%.2x ", data[i]);
	}
	printf("\r\n");	
}

/************************************************************************
 * function   : iic_read_byte
 * Description: iic���ֽڽ���
 * input 			: UINT8 receive��iic���յ��ֽ�
 * return			: none
 ************************************************************************/
UINT8 iic_read_byte(UINT8 receive)
{
	int	SDA_Single = 0;
	UINT32 i;
	SDA_IN()					//����SDAΪ����
	Timer0DelayUs(5);
	
	for(i=0; i<8; i++)
	{
		SCL_CLR();
		Timer0DelayUs(5);
		SCL_SET();			//ʱ���߸� �ӻ��ı�SDA
		Timer0DelayUs(5);
		
		receive <<= 1;
		SDA_Single = READ_SDA;	//��ȡSDA��ֵ�仯
		
		if(SDA_Single != 0)	
		{ 
			receive++; 
		}
	}
	  
	iic_ack();		    //����Ӧ���ź�
	
	return receive;
}

/************************************************************************
 * function   : iic_read
 * Description: iic�������������
 * input 			: UINT8 *readBuf:���������飬 long length���������ݳ���
 * return			: none
 ************************************************************************/
UINT8 iic_read(UINT8 *readBuf, long length)
{
	long i=0;

	iic_start();
	
	if( !iic_write_byte( 0x51 ) )			//���Ͷ����ݵ�ַ
	{	return 0;	}
	
	while(i<length)
	{
		readBuf[i] = iic_read_byte(readBuf[i]);		//��ȡ����
		i++;
	}
	
	iic_nack();		//������һ�����ݺ�����Ӧ���ź�
	iic_stop();		//IIC��ֹ����
	
	for(i=0; i<length; i++)			//���ڴ�ӡ
	{
		printf("%.2x ",readBuf[i]);
	}
	printf("\r\n");	
	
	return *readBuf;
}

/************************************************************************
 * function   : iic_rx_tx
 * Description: iic�շ�����
 * input 			: UINT8 *data   �����͵�����, UINT8 data_len���������ݳ���
								UINT8 *readBuf�����յ�����, long length   ���������ݳ���
								UINT32 timer_us ��ʱʱ��
 * return			: none
 ************************************************************************/
void iic_rx_tx(UINT8 *data, UINT8 data_len, UINT8 *readBuf, long length, UINT32 timer_ms)
{
	memset((UINT8 *)readBuf, 0, 300);		//��ս���buf
	
	iic_write((UINT8 *)data, data_len);	//д����
	Timer0DelayMs(timer_ms);						//��ʱtimer_us
	iic_read((UINT8 *)readBuf, length);	//������
}

/************************************************************************
 * function   : iic_test
 * Description: iic���ղ���
 * input 			: none
 * return			: none
 ************************************************************************/
void iic_test(void)
{
	iic_init();		//iic��ʼ��

	iic_power(1);	//iic�ϵ�
	Timer0DelayMs(30);
	memset((UINT8 *)readBuf, 0, 300);				//��ն�ȡ������
	
	iic_read((UINT8 *)readBuf, lengthBuf[0]);			//��ȡ��ʼ����
	
	iic_rx_tx((UINT8 *)dataBuf[0],  7,  (UINT8 *)readBuf, lengthBuf[1],  delay_w_r1);		//�����շ�21������
	iic_rx_tx((UINT8 *)dataBuf[1],  7,  (UINT8 *)readBuf, lengthBuf[2],  delay_w_r1);
	iic_rx_tx((UINT8 *)dataBuf[2],  7,  (UINT8 *)readBuf, lengthBuf[3],  delay_w_r1);
	iic_rx_tx((UINT8 *)dataBuf[3],  7,  (UINT8 *)readBuf, lengthBuf[4],  delay_w_r1);
	iic_rx_tx((UINT8 *)dataBuf[4],  7,  (UINT8 *)readBuf, lengthBuf[5],  delay_w_r1);
	iic_rx_tx((UINT8 *)dataBuf[5],  7,  (UINT8 *)readBuf, lengthBuf[6],  delay_w_r1);
	iic_rx_tx((UINT8 *)dataBuf[6],  10, (UINT8 *)readBuf, lengthBuf[7],  delay_w_r1);
	iic_rx_tx((UINT8 *)dataBuf[7],  10, (UINT8 *)readBuf, lengthBuf[8],  delay_w_r1);
	iic_rx_tx((UINT8 *)dataBuf[8],  10, (UINT8 *)readBuf, lengthBuf[9],  delay_w_r1);
	iic_rx_tx((UINT8 *)dataBuf[9],  10, (UINT8 *)readBuf, lengthBuf[10], delay_w_r1);
	iic_rx_tx((UINT8 *)dataBuf[10], 10, (UINT8 *)readBuf, lengthBuf[11], delay_w_r2);
	iic_rx_tx((UINT8 *)dataBuf[11], 10, (UINT8 *)readBuf, lengthBuf[11], delay_w_r2);
	iic_rx_tx((UINT8 *)dataBuf[12], 10, (UINT8 *)readBuf, lengthBuf[13], delay_w_r2);
	iic_rx_tx((UINT8 *)dataBuf[13], 10, (UINT8 *)readBuf, lengthBuf[14], delay_w_r2);
	iic_rx_tx((UINT8 *)dataBuf[14], 10, (UINT8 *)readBuf, lengthBuf[15], delay_w_r2);
	iic_rx_tx((UINT8 *)dataBuf[15], 10, (UINT8 *)readBuf, lengthBuf[16], delay_w_r2);
	iic_rx_tx((UINT8 *)dataBuf[16], 10, (UINT8 *)readBuf, lengthBuf[17], delay_w_r2);
	iic_rx_tx((UINT8 *)dataBuf[17], 10, (UINT8 *)readBuf, lengthBuf[18], delay_w_r2);
	iic_rx_tx((UINT8 *)dataBuf[18], 10, (UINT8 *)readBuf, lengthBuf[19], delay_w_r2);
	iic_rx_tx((UINT8 *)dataBuf[19], 10, (UINT8 *)readBuf, lengthBuf[20], delay_w_r2);
	iic_rx_tx((UINT8 *)dataBuf[20], 10, (UINT8 *)readBuf, lengthBuf[21], delay_w_r2);
	
	iic_stop();		//��������
	iic_power(0);	//iic����	
}


