#ifndef __IIC_H__
#define __IIC_H__

#include  "common.h"

void iic_init(void);
void iic_power(int state);

void iic_start(void);
void iic_stop(void);

int iic_wait_ack(void);
void iic_ack(void);
void iic_nack(void);

int iic_write_byte(UINT32 data);
void iic_write(UINT8 *data, UINT8 data_len);

UINT8 iic_read_byte(UINT8 receive);
UINT8 iic_read(UINT8 *readBuf, long length);

void iic_rx_tx(UINT8 *data, UINT8 data_len, UINT8 *readBuf, long length, UINT32 timer_us);

void iic_test(void);

#endif

