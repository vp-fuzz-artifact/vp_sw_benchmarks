/*
 * timing.h
 *
 *  Created on: Jun 19, 2017
 *      Author: Heethesh
 */

#ifndef TIMING_H_
#define TIMING_H_

void systick_init(void); 
unsigned int millis();
unsigned int micros();
void systick_reset(void);
void nrf_delay_ms(unsigned int delay);

#endif /* TIMING_H_ */
