
#include "peripherals.h"
#include "serial.h"
#include "i2c.h"
#include "timing.h"
#include "circular_buffer.h"
#include <stdlib.h>
#include "MPU6050/MPU6050.h"

#define MULTIWII_CONF

volatile unsigned int last_tick1 = 0, last_tick2 = 0, last_tick3 = 0;
extern struct txFrame txf;
extern CircularBuffer rxc, txc;

void _i2c_write_reg(uint8_t addr, uint8_t reg,  uint8_t *data, uint16_t len)
{
	if (len > 31) _Error_handler(__FILE__, __LINE__);
	uint8_t tx_buf[32];
	tx_buf[0] = reg;  
	for (int i = 0; i < len; i++) {
		tx_buf[i + 1] = data[0];
	}
 
	nrfx_twi_xfer_desc_t xfer = {
		.address = addr,
		.type = NRFX_TWI_XFER_TX,
		.p_primary_buf = tx_buf,
		.primary_length = len + 1,
	};
 
	if (nrfx_twi_xfer(&twi, &xfer, 0) != NRFX_SUCCESS)
		_Error_handler(__FILE__, __LINE__);
}

 
void _i2c_read_reg(uint8_t addr, uint8_t reg, uint8_t *data, uint16_t len) {
	uint8_t tx_buf;
	tx_buf = reg;
 
	nrfx_twi_xfer_desc_t txfer = {
		.address = addr,
		.type = NRFX_TWI_XFER_TX,
		.p_primary_buf = &tx_buf,
		.primary_length = sizeof(tx_buf),
	};
 
	if (nrfx_twi_xfer(&twi, &txfer, 0) != NRFX_SUCCESS)
		_Error_handler(__FILE__, __LINE__);

	uint8_t len1 = I2C_ReadByte(MPU6050, ACCEL_XOUT_H, __FILE__, __LINE__);
	len1 = fmax(len1, 1);
    //uint8_t len_lsb = I2C_ReadByte(MPU6050, ACCEL_XOUT_H, __FILE__, __LINE__);
    //uint16_t reported_len = ((uint16_t)len_msb << 8) | (uint16_t)len_lsb;
 
 	nrfx_twi_xfer_desc_t rxfer = {
		.address = addr,
		.type = NRFX_TWI_XFER_RX,
		.p_primary_buf = data,
		//.primary_length = len+ 50,
		.primary_length = len1,
	};
 
	if (nrfx_twi_xfer(&twi, &rxfer, 0) != NRFX_SUCCESS)
		_Error_handler(__FILE__, __LINE__);
}
 
uint32_t _get_ms_tick(void) {
	return millis();
}

Mpu6050 mpu6050;
void setup(void)
{
    systick_init();
	Peripherals_Init();
	I2C_Write_Reg = _i2c_write_reg;
	I2C_Read_Reg = _i2c_read_reg;
	Get_ms_tick = _get_ms_tick;
	__disable_irq();
	mpu6050_init(&mpu6050);
	__enable_irq();
	scheduling_timer_init();
	serialBegin();
}

int main(void)
{
	setup();
    for(int i = 0; i < 10; ++i) {
		nrf_delay_ms(1);
	}
	exit(0);
}


