#include <stdint.h>
#include "nrf51.h"
//#include "core_cm0.h"  // CMSIS header for Cortex-M0
#include "timing.h"

#undef SysTick
#define SystemCoreClock 16000000UL
#define MySysTick ((My_SysTick_Type *) 0xE000E010UL)

volatile uint32_t systick_millis = 0;

typedef struct
{
  __IOM uint32_t RESERVED[4];  // 0x00 - 0x0C
  __IOM uint32_t CTRL;         // 0x10 (SysTick Control and Status Register)
  __IOM uint32_t LOAD;         // 0x14 (SysTick Reload Value Register)
  __IOM uint32_t VAL;          // 0x18 (SysTick Current Value Register)
  __IM  uint32_t CALIB;        // 0x1C (SysTick Calibration Register)
} My_SysTick_Type;


// ISR (interrupt service routine) triggered automatically by the hardware SysTick interrupt every 1ms.
void SysTick_Handler(void) {
    systick_millis++;
}

/*
void systick_init(void) {

    SysTick->LOAD = (SystemCoreClock / 1000) - 1;  // Reload value for 1 ms (16 MHz / 1000)
    SysTick->VAL = 0;           // Clear current value
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk |  // Use processor clock
                    SysTick_CTRL_TICKINT_Msk   |  // Enable interrupt
                    SysTick_CTRL_ENABLE_Msk;      // Enable SysTick
}*/

void systick_init(void) {
    MySysTick->LOAD = (SystemCoreClock / 1000) - 1;
    MySysTick->VAL = 0;
    NVIC_SetPriority(SysTick_IRQn, 3);
    MySysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk |
                      SysTick_CTRL_TICKINT_Msk   |
                      SysTick_CTRL_ENABLE_Msk;
}


void systick_reset(void){
    MySysTick->VAL = 0;           // Clear current value
}


unsigned int millis(void) {
    return systick_millis;
}

/*
unsigned int micros(void) {
    // Not super precise, but can do rough micros by reading SysTick VAL
    uint32_t ms = systick_millis;
    uint32_t val = SysTick->VAL;
    // SysTick counts down from LOAD to 0
    uint32_t ticks = (SysTick->LOAD + 1) - val;
    // Convert ticks to microseconds: (ticks / CPU_freq) * 1,000,000
    uint32_t us = (ticks * 1000000) / 16000000;
    return ms * 1000 + us;
}*/

unsigned int micros(void) {
    uint32_t ms = systick_millis;
    uint32_t val = MySysTick->VAL;
    uint32_t load = MySysTick->LOAD;

    // SysTick counts down from LOAD to 0
    uint32_t ticks = (load + 1) - val;

    // Convert ticks to microseconds: (ticks / CPU_freq) * 1,000,000
    uint32_t us = (ticks * 1000000) / SystemCoreClock;

    return ms * 1000 + us;
}


// Not super accurate, but we cannot use timers (all taken)
void nrf_delay_ms(unsigned int delay_ms){

    uint32_t start = millis();
    // For now the irq is not triggered :(
    //while ((millis() - start) < delay_ms);
}
