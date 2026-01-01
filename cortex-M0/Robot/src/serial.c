/*
 * serial_new.c
 *
 *  Created on: Jun 12, 2017
 *      Author: Heethesh
 *
 * Transmit --> Blocking
 * Receive 	--> Non-Blocking (Interrupts) + FIFO
 */

#include "circular_buffer.h"
#include "serial.h"
#include <math.h>
#include <string.h>

extern const nrfx_uart_t uart;
extern CircularBuffer rxc;

unsigned char rx_buffer[2];
char tx_buffer[100];

// volatile bool tx_done = true; // There might be a problem here. Can the fuzz find it?

void UART0_IRQHandler(void)
{
    nrfx_uart_0_irq_handler(); // Let NRFX dispatch the `uart_event_handler`
}

/**********************************
 Function name	:	serialAvailable
 Functionality	:	To get receive buffer size
 Arguments		:	None
 Return Value	:	Error state
 Example Call	:	serialAvailable()
 ***********************************/
int serialAvailable(void)
{
	return (CB_Size(&rxc));
}

/**********************************
 Function name	:	serialWrite
 Functionality	:	To send a byte of data
 Arguments		:	Byte to send
 Return Value	:	None
 Example Call	:	serialWrite('H')
 ***********************************/
void serialWrite(unsigned char ch)
{
	// while (!tx_done);
	// tx_done = false;
	tx_buffer[0] = ch;
    nrfx_uart_tx(&uart,(uint8_t*) &tx_buffer, 1);
	// while (!tx_done);
}

/**********************************
 Function name	:	serialRead
 Functionality	:	To read a byte of data from the FIFO buffer
 Arguments		:	None
 Return Value	:	Read data
 Example Call	:	serialRead()
 ***********************************/
unsigned char serialRead()
{
	unsigned char data;
	CB_Read(&rxc, &data);
	return data;
}


void uart_event_handler(nrfx_uart_event_t const *p_event, void *p_context)
{
    if (p_event->type == NRFX_UART_EVT_RX_DONE)
    {
        // Store received byte (p_event->data.rxtx.p_data[0]) into the buffer
        CB_Write(&rxc, p_event->data.rxtx.p_data[0]);

        // Re-enable RX to receive the next byte
        nrfx_uart_rx(&uart, rx_buffer, 1);
    }

	/*
	if (p_event->type == NRFX_UART_EVT_TX_DONE)
	{
		tx_done = true;
	}
	*/

}

/**********************************
 Function name	:	serialFlush
 Functionality	:	To clear the FIFO buffers
 Arguments		:	None
 Return Value	:	None
 Example Call	:	serialFlush()
 ***********************************/
void serialFlush(void)
{
	CB_Init(&rxc);
}

/**********************************
 Function name	:	serialBegin
 Functionality	:	To reset FIFO buffer and enable UART interrupt flags
 Arguments		:	None
 Return Value	:	None
 Example Call	:	serialBegin()
 ***********************************/
void serialBegin(void)
{
	serialFlush();
	// Start the first receive
	nrfx_uart_rx(&uart, rx_buffer, 1);
}

/**********************************
 Function name	:	serialPrint
 Functionality	:	To send a stream of bytes
 Arguments		:	Data stream
 Return Value	:	None
 Example Call	:	serialPrint("Hola Monde!")
 ***********************************/
void serialPrint(char* data)
{
	for (int i = 0; i < strlen(data); i++)
		serialWrite(data[i]);
}


void int_to_string(int val, char *buf){
	char *p = buf;
    if (val < 0) {
        *p++ = '-';
        val = -val;
    }

    // Convert digits in reverse order
    char temp[12];
    int i = 0;
    do {
        temp[i++] = '0' + (val % 10);
        val /= 10;
    } while (val > 0);

    // Reverse into final buffer
    while (i--) {
        *p++ = temp[i];
    }

    *p = '\0'; // Null-terminate
}


void float_to_string(float val, char *buf, int precision){
	int int_part = (int)val;
    float frac_part = (val >= 0) ? (val - int_part) : (int_part - val);

    // Handle sign
    char *p = buf;
    if (val < 0) {
        *p++ = '-';
        int_part = -int_part;
    }

    // Convert integer part
    int i = 0;
    char int_buf[12]; // enough for 32-bit int
    do {
        int_buf[i++] = (int_part % 10) + '0';
        int_part /= 10;
    } while (int_part > 0);

    // Reverse integer part
    while (i--) {
        *p++ = int_buf[i];
    }

    *p++ = '.';

    // Convert fractional part
    for (int j = 0; j < precision; j++) {
        frac_part *= 10;
        int digit = (int)frac_part;
        *p++ = digit + '0';
        frac_part -= digit;
    }

    *p = '\0'; // null-terminate
}


/**********************************
 Function name	:	serialInt
 Functionality	:	To print a integer value on a serial terminal (digit-wise)
 Arguments		:	Integer value
 Return Value	:	None
 Example Call	:	serialInt()
 ***********************************/
void serialInt(int val)
{
	int_to_string(val, tx_buffer);
	serialPrint(tx_buffer);
}

/**********************************
 Function name	:	serialFloat
 Functionality	:	To print a float value on a serial terminal (digit-wise)
 Arguments		:	Float value
 Return Value	:	None
 Example Call	:	serialFloat()
 ***********************************/
void serialFloat(float val)
{
	float_to_string(val, tx_buffer, 4);
	serialPrint(tx_buffer);
}
