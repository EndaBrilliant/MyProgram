/***********************************************
  滤波IIC
 **********************************************/

#include "timer0.h"
#include "timer.h"
#include "iic_sx.h"
#include "common.h"

#define 	SCL		0x08
#define		SDA		0x10

#define		SCL_OUT()				{REG_GPIO_DIR(GPIOB) |= SCL;}							//设置SCL:GPIO35为输出模式
#define		SDA_OUT()				{REG_GPIO_DIR(GPIOB) |= SDA;}							//设置SDA:GPIO36为输出模式
#define		SDA_IN()				{REG_GPIO_DIR(GPIOB) &= ~(SDA);}					//设置SDA:GPIO36为输入模式

#define  	SCL_SET()				{REG_GPIO_SET(GPIOB) |=	SCL;}							//SCL置位
#define		SCL_CLR()				{REG_GPIO_CLR(GPIOB) |= SCL;} 						//SCL清零

#define  	SDA_SET()				{REG_GPIO_SET(GPIOB) |=	SDA;}							//SDA置位
#define		SDA_CLR()				{REG_GPIO_CLR(GPIOB) |= SDA;} 						//SDA清零

#define		READ_SDA		  	(REG_GPIO_IDATA(GPIOB) &= SDA)						//读SDA值		

#define		delay_w_1				100
#define		delay_w_2				220
	
//发送命令二维数组，规定每一行为一条命令，第0位为地址(已移位)，其余为命令
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

													
//iic返回数据长度													
long sx_lengthBuf[] = {25, 10, 7, 6, 36, 12, 36, 68, 260, 260, 260, 260, 
											 260, 260, 260, 260, 260, 260, 260, 260, 260, 260};

//定义接收数组
UINT8 sx_readBuf[300] = {0};			


/************************************************************************
 * function   : iic_sx_init
 * Description: iic初始化
 * input 			: none
 * return			: none
 ************************************************************************/
void iic_sx_init(void)
{
//GPIO35:SCL	GPIO36:SDA  -------------------------------------------------------------------
	REG_SCU_MUXCTRLC = (REG_SCU_MUXCTRLC & ~(0xF << 6));
	REG_SCU_PUCRB |= 0x18; 		//管脚35 36上拉有效
	
	REG_SCU_CTRLA &= ~(0x01 << 22);		//使能GPIO模块时钟
	REG_GPIO_IEN(GPIOB) &= ~(0x03 << 3);	//设置GPIO35、36中断屏蔽
	
	SCL_OUT();		//设置SCL:GPIO35为输出模式
	SDA_OUT();		//设置SDA:GPIO36为输出模式
	
	Timer0DelayMs(30);
	
	SDA_SET();		//SCL置位
	Timer0DelayMs(7);
	SCL_SET();		//SDA置位
	Timer0DelayMs(25);
	
}

/************************************************************************
 * function   : scl_shake
 * Description: SCL空闲状态
 * input 			: count：波动次数
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
		SCL_CLR();			//一个周期结束时SCL为低电平
	}
}

/************************************************************************
 * function   : sda_shake
 * Description: SDA空闲状态（复位时调用）
 * input 			: count：波动次数
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
		SDA_SET();			//一个周期结束时SDA为高电平
	}
}

/************************************************************************
 * function   : iic_sx_reset
 * Description: 复位
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
 * Description: iic数据传输起始条件
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
 * Description: iic数据传输终止条件
 * input 			: none
 * return			: none
 ************************************************************************/
void iic_sx_stop(void)
{	
	SDA_OUT();		//SDA输出模式
	SDA_CLR();		//SDA置零
	
	scl_shake(50);
	delay(30);
	SCL_SET();
	
	Timer0DelayUs(8);
	SDA_SET();		//SCL高电平期间SDA从0变为1
	Timer0DelayUs(100);
}

/************************************************************************
 * function   : iic_sx_wait_ack
 * Description: 读取ACK响应信号 0标表示信号有效，1表示信号无效
 * input 			: none
 * return			: 0表示信号有效，1表示信号无效
 ************************************************************************/
 int iic_sx_wait_ack(void)
{
	UINT8 i;
	delay(25);
  
	SDA_IN();			//SDA输入模式
	Timer0DelayUs(1);	
	
	SCL_SET();

	i=READ_SDA;
	while( i )		//判断SDA状态
	{
		iic_sx_stop();		//无应答则发送停止位
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
 * Description: iic发送应答信号
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
 * Description: iic发送非应答信号
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
 * Description: iic单字节写
 * input 			: UINT32 data：iic要写入的字节
 * return			: none
 ************************************************************************/
int iic_sx_write_byte(UINT32 data)
{
	int i;
	UINT32 write_data_byte = data;
	SDA_OUT();
	for(i=7; i>=0; i--)
	{	
		SCL_CLR();		//SCL拉低开始写数据
		delay(20);
		Timer0DelayUs(1);
		if( (write_data_byte & 0x80) == 0x80 )		//判断写0或者1
		{
			SDA_SET();
		}
		else
		{
			SDA_CLR();
		}
		write_data_byte <<= 1; 	//每读取一位就左移一位	
		
		SCL_SET();
		delay(20);
		Timer0DelayUs(3);
		
		SCL_CLR();
	}
	
	if(iic_sx_wait_ack())	//无应答返回0
		return 0;
	else
		return 1;
}

/******************************************************************************
 * function   : iic_sx_write
 * Description: iic按数组发生
 * input 			: UINT8 *data：iic要发送的数组， UINT8 data_len：iic发送数组长度
 * return			: 发送出错返回1，正确返回0
 ******************************************************************************/
void iic_sx_write(UINT8 *data, UINT8 data_len)
{
	int i,count=0;
	iic_sx_start();			//起始条件
	
	for(i=0; i < (data_len); i++)			//写数据
	{
			while( iic_sx_write_byte(data[i]) == 0)				//无应答循环发送5次
			{	
				i = 1;			// i=1 保证每次重新发送都从第一位数据开始发送
				if( count >= 8 )
				{
					iic_sx_stop();
					return ;
				}
				iic_sx_start();
				count++;
			}
	}
	
	iic_sx_stop();		//IIC终止条件
	
	for(i=0; i < data_len; i++)			//串口打印
	{
		printf("%.2x ",data[i]);
	}
	printf("\r\n");	

}

/************************************************************************
 * function   : iic_sx_read_byte
 * Description: iic按字节接收
 * input 			: UINT8 receive：iic接收的字节
 * return			: none
 ************************************************************************/
UINT8 iic_sx_read_byte(UINT8 receive, int ack)
{
	int	SDA_Single = 0;
	UINT32 i;
	SDA_IN()					//设置SDA为输入
	
	delay(45);
	for(i=0; i < 8; i++)
	{
		delay(15);
		SCL_SET();			//时钟线高 从机改变SDA
		Timer0DelayUs(3);
		delay(20);
		SCL_CLR();
		receive <<= 1;
		SDA_Single = READ_SDA;	//读取SDA数值变化
		
		Timer0DelayUs(1);
		
		delay(15);
		
		if(SDA_Single != 0)	
		{ 
			receive++; 
		}
	}
	if(!ack)
	{
		iic_sx_ack();		//发送应答信号
	}
	else
	{
		iic_sx_nack();	//发送非应答信号
	}
	
	return receive;
}

/************************************************************************
 * function   : iic_sx_read
 * Description: iic按数组接收数据
 * input 			: UINT8 *sx_readBuf:接收属数组， long length：接收数据长度
 * return			: 发送出错返回1， 正确返回接收数组
 ************************************************************************/
UINT8 iic_sx_read(UINT8 *sx_readBuf, long length)
{
	long i=0;
	int count=0;
	scl_shake(50);
	iic_sx_start();
	
	while( iic_sx_write_byte(0x51) == 0)	//无应答循环发送8次
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

	while(i<length)			//读取数据
	{
		if(i == length-1)
		{
			sx_readBuf[i] = iic_sx_read_byte(sx_readBuf[i], 1);		//ack为1，读取最后一位后发送非应答位
		}
		else
		{
			sx_readBuf[i] = iic_sx_read_byte(sx_readBuf[i], 0);		//ack为0，发送应答位
		}
		i++;
	}
	
	iic_sx_stop();								//IIC终止条件
		
	for(i=0; i < length; i++)			//串口打印
	{
		printf("%.2x ",sx_readBuf[i]);
	}
	printf("\r\n");	
	
	return *sx_readBuf;				  	//返回接收到的数据
}

/************************************************************************
 * function   : iic_sx_rx_txf
 * Description: iic按数组接收数据
 * input 			: UINT8 *sx_readBuf:接收属数组， long length：接收数据长度
 * return			: 发送或接收出错返回1，正确返回0
 ************************************************************************/
void iic_sx_rx_txf(UINT8 *data, UINT8 data_len, UINT8 *sx_readBuf, long length, UINT32 timer)
{
	memset((UINT8 *)sx_readBuf, 0, 300);					//清空接收buf
		
	if(timer < 150)
	{
		Timer0DelayMs(15);
		iic_sx_write((UINT8 *)data, data_len);			//写指令
		
		Timer0DelayMs(timer);
		iic_sx_read((UINT8 *)sx_readBuf, length);		//读返回数据
	}
	else
	{
		Timer0DelayMs(15);
		iic_sx_write((UINT8 *)data, data_len);			//写指令
		
		Timer0DelayMs(150);
	 	scl_shake(timer*6);	
		iic_sx_read((UINT8 *)sx_readBuf, length);		//读返回数据
	}
}	
	

/********************************************************************************************
*********************************************************************************************/
void iic_sx_test()
{
	iic_sx_init();

	scl_shake(1000);		//SCL空闲状态
	iic_sx_reset();			//复位
	
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


















