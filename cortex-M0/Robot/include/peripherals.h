#ifndef PERIPHERALS_H__
#define PERIPHERALS_H__

#include "nrfx_uart.h"
#include "nrfx_twi.h"
#include "nrfx_timer.h"
#include "nrfx_errors.h"
#include "nrf_gpio.h"
#include "serial.h"
#include "timing.h"
#include "../src/MPU6050/MPU6050.h"


#define PWM_ARR 750
#define UART_TX 9     // TX pin (P0.09 on PCA10028)
#define UART_RX 11    // RX pin (P0.11)
#define TWI_SCL 30
#define TWI_SDA 7
#define WHITE_LED_PIN 20
#define RED_LED_PIN 21
#define BLUE_LED_PIN 22


#define N_MOTORS 2
#define N_TIMERS 1


extern const nrfx_uart_t uart;
extern const nrfx_twi_t twi;
extern  nrfx_timer_t timers[N_TIMERS];

extern volatile uint16_t motors_duty[N_MOTORS];
extern volatile uint16_t motors_pin[N_MOTORS];
extern Mpu6050 mpu6050;
typedef struct Pwm_config Pwm_config;

struct Pwm_config {
    uint16_t pin1;
    uint16_t pin2;
    uint16_t motor_duty1;
    uint16_t motor_duty2;
    nrfx_timer_t* timer;

    void (*pin_config)(Pwm_config*);
    void (*timer_init)(Pwm_config*, nrfx_timer_event_handler_t, IRQn_Type);
};

void uart_init(void);
void twi_init(void);
void set_motor_duty(uint16_t duty_cycle, int motor_idx);
void toggleLED(int white, int red, int blue);
void LED_StartupSequence(void);
void Peripherals_Init(void);
void _Error_handler(char* file, int line);
void scheduling_timer_init();
void schedeuling_timer_handler(nrf_timer_event_t event_type, void* p_context);

#endif