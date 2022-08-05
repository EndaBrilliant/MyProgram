#ifndef __IIC_SX_H__
#define __IIC_SX_H__

#include  "common.h"

void iic_sx_init(void);
void iic_sx_power(int state);

void iic_sx_start(void);
void iic_sx_stop(void);

int iic_sx_wait_ack(void);
void iic_sx_ack(void);
void iic_sx_nack(void);

int iic_sx_write_byte(UINT32 data);
void iic_sx_write(UINT8 *data, UINT8 data_len);

void iic_sx_rx_txf(UINT8 *data, UINT8 data_len, UINT8 *sx_readBuf, long length, UINT32 timer);

UINT8 iic_sx_read_byte(UINT8 receive, int ack);
UINT8 iic_sx_read(UINT8 *readBuf, long length);

void iic_sx_test(void);

#endif

