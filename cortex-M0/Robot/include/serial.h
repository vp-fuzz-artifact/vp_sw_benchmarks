/*
 * 
 *
 *  Created on: Jun 12, 2017
 *      Author: Heethesh
 */

#ifndef SERIAL_H_
#define SERIAL_H_

#include "peripherals.h"

void serialBegin();
int serialAvailable();
void serialWrite(unsigned char ch);
unsigned char serialRead();
void serialFlush();
void serialPrint(char* data);
void serialInt(int val);
void serialFloat(float val);
void float_to_string(float val, char *buf, int precision);
void int_to_string(int val, char *buf);
void uart_event_handler(nrfx_uart_event_t const *p_event, void *p_context);

#endif /* SERIAL_H_ */
