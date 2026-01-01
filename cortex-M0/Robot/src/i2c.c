#include "peripherals.h"
#include "i2c.h"
#include "stdint.h"

void I2C_WriteByte(uint16_t device_add, uint16_t register_add, uint8_t register_val, uint8_t delay_mode)
{   
    uint8_t tx_buf[3];
    tx_buf[0] = (register_add >> 8) & 0xFF;  // MSB
    tx_buf[1] = register_add & 0xFF;         // LSB
    tx_buf[2] = register_val;

    nrfx_twi_xfer_desc_t xfer = {
        .address = device_add,
        .type = NRFX_TWI_XFER_TX,
        .p_primary_buf = tx_buf,
        .primary_length = sizeof(tx_buf),
    };

    if (nrfx_twi_xfer(&twi, &xfer, 0) != NRFX_SUCCESS)
        _Error_handler(__FILE__, __LINE__);

    if(delay_mode) nrf_delay_ms(50);
}


// The usual rx sequence is:
// Write the register address to the sensor (telling it the register).
// Read the data from that register.
uint8_t I2C_ReadByte(uint16_t device_add, uint16_t register_add, char* file, int line){
    uint8_t rx_buf = 0;
    uint8_t tx_buf[2];

    tx_buf[0] = (register_add >> 8) & 0xFF;  // MSB
    tx_buf[1] = register_add & 0xFF;         // LSB

    nrfx_twi_xfer_desc_t txfer = {
        .address = device_add,
        .type = NRFX_TWI_XFER_TX,
        .p_primary_buf = tx_buf,
        .primary_length = sizeof(tx_buf),
    };

    if (nrfx_twi_xfer(&twi, &txfer, 0) != NRFX_SUCCESS)
        _Error_handler(__FILE__, __LINE__);


    nrfx_twi_xfer_desc_t rxfer = {
        .address = device_add,
        .type = NRFX_TWI_XFER_RX,
        .p_primary_buf = &rx_buf,
        .primary_length = 1,
    };

    if (nrfx_twi_xfer(&twi, &rxfer, 0) != NRFX_SUCCESS)
        _Error_handler(__FILE__, __LINE__);

    return rx_buf;
}


void I2C_ReadBytes(uint16_t device_add, uint16_t register_add, uint8_t* bytes, uint16_t size, char* file, int line){
    uint8_t tx_buf[2];

    tx_buf[0] = (register_add >> 8) & 0xFF;  // MSB
    tx_buf[1] = register_add & 0xFF;         // LSB

    nrfx_twi_xfer_desc_t txfer = {
        .address = device_add,
        .type = NRFX_TWI_XFER_TX,
        .p_primary_buf = tx_buf,
        .primary_length = sizeof(tx_buf),
    };

    if (nrfx_twi_xfer(&twi, &txfer, 0) != NRFX_SUCCESS)
        _Error_handler(__FILE__, __LINE__);

    /**********************************************
     * BUG INJECTION: misinterpret length field  *
     * Instead of using the 'size' argument, read *
     * two bytes from the device and use them as  *
     * a 16-bit length (MSB first). No bounds     *
     * checking is performed on the resulting len.*
     **********************************************/
    //uint8_t len_msb = I2C_ReadByte(device_add, register_add, __FILE__, __LINE__);
    //uint8_t len_lsb = I2C_ReadByte(device_add, register_add + 1, __FILE__, __LINE__);
    //uint16_t reported_len = ((uint16_t)len_msb << 8) | (uint16_t)len_lsb;


    nrfx_twi_xfer_desc_t rxfer = {
        .address = device_add,
        .type = NRFX_TWI_XFER_RX,
        .p_primary_buf = bytes,
        .primary_length = (size+50),
        /* BUG: use reported_len (fuzzer-controlled) instead of caller 'size' */
        //.primary_length = reported_len,
    };

    if (nrfx_twi_xfer(&twi, &rxfer, 0) != NRFX_SUCCESS)
        _Error_handler(__FILE__, __LINE__);
}


void I2C_ReadByteArray(uint16_t device_add, uint16_t register_add, uint8_t* byte_array, uint16_t size, char* file, int line){
    int i = 0;
	while (i<size)
	{
		byte_array[i++] = I2C_ReadByte(device_add, register_add++, __FILE__, __LINE__);
	}
}