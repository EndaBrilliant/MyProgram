/***********************************************
	标准IIC
 **********************************************/

#include "timer0.h"
#include "timer.h"
#include "iic.h"

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

#define		GPIO27_SET()	  {REG_GPIO_SET(GPIOA) |=	(0x01 << 27);}		//GPIO27、28置位
#define		GPIO28_SET()	  {REG_GPIO_SET(GPIOA) |=	(0x01 << 28);}
#define		GPIO27_CLR()	  {REG_GPIO_CLR(GPIOA) |=	(0x01 << 27);}		//GPIO27、28清零
#define		GPIO28_CLR()	  {REG_GPIO_CLR(GPIOA) |=	(0x01 << 28);}

#define		IIC_POW_SET()		{REG_GPIO_SET(GPIOB) |=	(0x04);}					//iic_power(GPIO34)置位、清零
#define 	IIC_POW_CLR()		{REG_GPIO_CLR(GPIOB) |=	(0x04);}

#define		delay_w_r1			50		
#define		delay_w_r2			150

//发送命令二维数组，规定每一行为一条命令，第0位为地址(已移位)，其余为命令							
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
													
//iic返回数据长度													
long lengthBuf[] = {25, 10, 7, 6, 36, 12, 36, 68, 260, 260, 260, 260, 
										260, 260, 260, 260, 260, 260, 260, 260, 260, 260};

//定义接收数组
UINT8 readBuf[300] = {0};			


/************************************************************************
 * function   : iic_init
 * Description: iic初始化
 * input 			: none
 * return			: none
 ************************************************************************/
void iic_init(void)
{
//IIC_Power:GPIO34. GPIO27,GPIO28  ----------------------------------------------------------
	REG_SCU_MUXCTRLB = (REG_SCU_MUXCTRLB & ~(0xF << 22));	//配置GPIO 27、28复用管脚
	REG_SCU_MUXCTRLC = (REG_SCU_MUXCTRLC & ~(0x30));			//配置IIC_POWER IO（GPIO34）复用管脚
	
	REG_SCU_PUCRA |= (0x03 << 27);							//管脚上拉有效
	REG_SCU_PUCRB |= (0x04);
	
	REG_GPIO_IEN(GPIOA) &= ~(0x03 << 27);				//设置GPIO 27、28、34屏蔽中断
	REG_GPIO_IEN(GPIOB) &= ~(0x01 << 3);
	
	REG_GPIO_DIR(GPIOA)	|= (0x03 << 27);			  //设置GPIO 27、28、34输出有效
	REG_GPIO_DIR(GPIOB)	|= (0x01 << 3);
	
	
//GPIO35:SCL	GPIO36:SDA  -------------------------------------------------------------------
	REG_SCU_MUXCTRLC = (REG_SCU_MUXCTRLC & ~(0xF << 6));
	REG_SCU_PUCRB |= 0x18; 		//管脚35 36上拉有效
	
	REG_SCU_CTRLA &= ~(0x01 << 22);		//使能GPIO模块时钟
	REG_GPIO_IEN(GPIOB) &= ~(0x03 << 3);	//设置GPIO35、36中断屏蔽
	
	SCL_OUT();		//设置SCL:GPIO35为输出模式
	SDA_OUT();		//设置SDA:GPIO36为输出模式
	
	SDA_SET();		//SDA置位
	SCL_SET();		//SCL置位
	
	Timer0DelayUs(5);
}	

/************************************************************************
 * function   : iic_power
 * Description: iic供电使能
 * input 			: state：为1时使能，0时断电
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
 * Description: iic数据传输起始条件
 * input 			: none
 * return			: none
 ************************************************************************/
void iic_start(void)
{
	Timer0DelayUs(5);
	SDA_OUT();		//SDA输出模式
	
	SDA_SET();		//SDA SCL置位
	Timer0DelayUs(5);
	SCL_SET();
	
	Timer0DelayUs(5);
	
	SDA_CLR();		//SDA置零
	Timer0DelayUs(5);
}

/************************************************************************
 * function   : iic_stop
 * Description: iic数据传输终止条件
 * input 			: none
 * return			: none
 ************************************************************************/
void iic_stop(void)
{	
	SDA_OUT();		//SDA输出模式
	
	SCL_CLR();		//SCL SDA置零
	Timer0DelayUs(5);
	SDA_CLR();
	Timer0DelayUs(5);
	
	SCL_SET();		//在SCL高电平期间，SDA拉高
	Timer0DelayUs(5);
	SDA_SET();
	Timer0DelayUs(5);
}

/************************************************************************
 * function   : iic_wait_ack
 * Description: 读取ACK响应信号 0标表示信号有效，1表示信号无效
 * input 			: none
 * return			: 0表示信号有效，1表示信号无效
 ************************************************************************/
 int iic_wait_ack(void)
{
	SDA_IN();			//SDA输入模式
	Timer0DelayUs(1);
	SCL_SET();	
	Timer0DelayUs(2);
	
	while( READ_SDA )		//判断SDA状态
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
 * Description: iic发送应答信号
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
 * Description: iic发送非应答信号
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
 * Description: iic单字节写
 * input 			: UINT32 data：iic要写入的字节
 * return			: none
 ************************************************************************/
int iic_write_byte(UINT32 data)
{
	int i;
	UINT32 write_data_byte = data;
	SDA_OUT();
	
	SCL_CLR();		//SCL拉低开始写数据
	Timer0DelayUs(3);
	for(i=7; i>=0; i--)
	{	
		if( (write_data_byte & 0x80) == 0x80 )		//判断写0或者1
		{
			SDA_SET();
		}
		else
		{
			SDA_CLR();
		}
		write_data_byte <<= 1; 	//每读取一位就左移一位	
		Timer0DelayUs(2);
		
		SCL_SET();
		Timer0DelayUs(5);
		
		SCL_CLR();
		Timer0DelayUs(2);
	}
	
	if(iic_wait_ack())	//无应答返回0
		return 0;
	else
		return 1;
}

/******************************************************************************
 * function   : iic_write
 * Description: iic按数组发生
 * input 			: UINT8 *data：iic要发送的数组， UINT8 data_len：iic发送数组长度
 * return			: none
 ******************************************************************************/
void iic_write(UINT8 *data, UINT8 data_len)
{
	int i;
	iic_start();	//起始条件
	
	for(i=0; i<(data_len); i++)				//写数据 
	{
		if(iic_write_byte(data[i]) == 0)
			return;
	}
	
	iic_stop();		//IIC终止条件
	
	for(i=0; i<=(data_len); i++)			//串口打印
	{
		printf("%.2x ", data[i]);
	}
	printf("\r\n");	
}

/************************************************************************
 * function   : iic_read_byte
 * Description: iic按字节接收
 * input 			: UINT8 receive：iic接收的字节
 * return			: none
 ************************************************************************/
UINT8 iic_read_byte(UINT8 receive)
{
	int	SDA_Single = 0;
	UINT32 i;
	SDA_IN()					//设置SDA为输入
	Timer0DelayUs(5);
	
	for(i=0; i<8; i++)
	{
		SCL_CLR();
		Timer0DelayUs(5);
		SCL_SET();			//时钟线高 从机改变SDA
		Timer0DelayUs(5);
		
		receive <<= 1;
		SDA_Single = READ_SDA;	//读取SDA数值变化
		
		if(SDA_Single != 0)	
		{ 
			receive++; 
		}
	}
	  
	iic_ack();		    //发送应答信号
	
	return receive;
}

/************************************************************************
 * function   : iic_read
 * Description: iic按数组接收数据
 * input 			: UINT8 *readBuf:接收属数组， long length：接收数据长度
 * return			: none
 ************************************************************************/
UINT8 iic_read(UINT8 *readBuf, long length)
{
	long i=0;

	iic_start();
	
	if( !iic_write_byte( 0x51 ) )			//发送读数据地址
	{	return 0;	}
	
	while(i<length)
	{
		readBuf[i] = iic_read_byte(readBuf[i]);		//读取数据
		i++;
	}
	
	iic_nack();		//接收完一组数据后发生非应答信号
	iic_stop();		//IIC终止条件
	
	for(i=0; i<length; i++)			//串口打印
	{
		printf("%.2x ",readBuf[i]);
	}
	printf("\r\n");	
	
	return *readBuf;
}

/************************************************************************
 * function   : iic_rx_tx
 * Description: iic收发数据
 * input 			: UINT8 *data   ：发送的数据, UINT8 data_len：发送数据长度
								UINT8 *readBuf：接收的数据, long length   ：接收数据长度
								UINT32 timer_us 延时时间
 * return			: none
 ************************************************************************/
void iic_rx_tx(UINT8 *data, UINT8 data_len, UINT8 *readBuf, long length, UINT32 timer_ms)
{
	memset((UINT8 *)readBuf, 0, 300);		//清空接收buf
	
	iic_write((UINT8 *)data, data_len);	//写命令
	Timer0DelayMs(timer_ms);						//延时timer_us
	iic_read((UINT8 *)readBuf, length);	//读数据
}

/************************************************************************
 * function   : iic_test
 * Description: iic接收测试
 * input 			: none
 * return			: none
 ************************************************************************/
void iic_test(void)
{
	iic_init();		//iic初始化

	iic_power(1);	//iic上电
	Timer0DelayMs(30);
	memset((UINT8 *)readBuf, 0, 300);				//清空读取缓冲区
	
	iic_read((UINT8 *)readBuf, lengthBuf[0]);			//读取初始数据
	
	iic_rx_tx((UINT8 *)dataBuf[0],  7,  (UINT8 *)readBuf, lengthBuf[1],  delay_w_r1);		//依次收发21条数据
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
	
	iic_stop();		//结束条件
	iic_power(0);	//iic掉电	
}


