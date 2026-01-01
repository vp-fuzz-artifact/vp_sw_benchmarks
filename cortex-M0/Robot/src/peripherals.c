#include "peripherals.h"
#include "nrf51.h"
#include <stdio.h>   
#include <string.h>  

#define MOTOR_A_IN1   1
#define MOTOR_A_IN2   2
#define MOTOR_B_IN1   3
#define MOTOR_B_IN2   4

#define SCHEDULER_INTERVAL_MS   20
#define SCHEDULER_TIMER_TICKS   (SCHEDULER_INTERVAL_MS * 1000)

#define PWM_INTERVAL_MS 5
#define PWM_TIMER_TICKS   (PWM_INTERVAL_MS * 1000)

uint8_t maxPwm = 100;
uint8_t minPwm = 0;

float setpoint = 90.00;
float error = 0;
float lastError = 0;

float Kp = 0.0;
float Ki = 0.0;
float Kd = 0.0;

float P;
float I;
float D;

float pitch, roll;
char strbuf[64];

const nrfx_uart_t uart = NRFX_UART_INSTANCE(0);
const nrfx_twi_t twi = NRFX_TWI_INSTANCE(0);
nrfx_timer_t timers[N_TIMERS] = {NRFX_TIMER_INSTANCE(1)};

volatile uint16_t motors_duty[N_MOTORS] = {0};  // Range: 0 .. PWM_ARR
volatile uint16_t motors_pin[N_MOTORS] = {16,17}; // are those valid pin assignments?


void TIMER0_IRQHandler(void)
{
    nrfx_timer_0_irq_handler(); 
}

void TIMER1_IRQHandler(void)
{
    nrfx_timer_1_irq_handler(); 
}

void uart_init(void) {
    nrfx_uart_config_t config = {
        .pseltxd = UART_TX,  
        .pselrxd = UART_RX,  
        .pselcts = NRF_UART_PSEL_DISCONNECTED,
        .pselrts = NRF_UART_PSEL_DISCONNECTED,
        .p_context = NULL,
        .baudrate = NRF_UART_BAUDRATE_115200,
        .interrupt_priority = NRFX_UART_DEFAULT_CONFIG_IRQ_PRIORITY,
        .hal_cfg            = {                                                       
            .hwfc           = NRF_UART_HWFC_DISABLED,                                 
            .parity         = NRF_UART_PARITY_EXCLUDED,                               
        }  
    };

    if (nrfx_uart_init(&uart, &config, uart_event_handler) != NRFX_SUCCESS)
        _Error_handler(__FILE__, __LINE__);

    NVIC_EnableIRQ(UART0_IRQn);
}


void twi_init(void) {
    nrfx_twi_config_t config = {
        .scl = TWI_SCL, // clock
        .sda = TWI_SDA, // data
        .frequency = NRF_TWI_FREQ_400K,
        .interrupt_priority = NRFX_TWI_DEFAULT_CONFIG_IRQ_PRIORITY,
        .hold_bus_uninit = false,
    };

    if(nrfx_twi_init(&twi, &config, NULL, NULL) != NRFX_SUCCESS) // should the event handler be null? Will we be using interrupts?
        _Error_handler(__FILE__, __LINE__);
    
    nrfx_twi_enable(&twi); // The init() function in this case doesnt enable the twi :(
}

void timer1_handler(nrf_timer_event_t event_type, void* p_context){
    switch (event_type) {
        case NRF_TIMER_EVENT_COMPARE0:
            nrf_gpio_pin_clear(motors_pin[0]);
            break;
        case NRF_TIMER_EVENT_COMPARE1:
            nrf_gpio_pin_clear(motors_pin[1]);
            break;
        case NRF_TIMER_EVENT_COMPARE2:
            nrf_gpio_pin_set(motors_pin[0]);
            nrf_gpio_pin_set(motors_pin[1]);
            break;
        default:
            break;
    }
}


// Set the PWM for the motor. PWM values: (0 - 750)
void set_motor_duty(uint16_t duty_cycle, int motor_idx){
    if((motor_idx < N_MOTORS) && (duty_cycle < PWM_ARR))
    {
        int timer_idx = motor_idx / 2;
        int channel = motor_idx % 2;
        
        motors_duty[motor_idx] = duty_cycle;
        nrfx_timer_compare(&timers[timer_idx], channel, duty_cycle, false);
    }
}

void pwm_pin_config(Pwm_config* pwm_cfg){
    // Configure motors' GPIOs
    nrf_gpio_cfg_output(pwm_cfg->pin1);
    nrf_gpio_cfg_output(pwm_cfg->pin2);
    nrf_gpio_pin_clear(pwm_cfg->pin1);
    nrf_gpio_pin_clear(pwm_cfg->pin2);
}
void scheduling_timer_init()
{
    nrfx_timer_t scheduler_timer = NRFX_TIMER_INSTANCE(0);
	// Sets the mode, the frequency, the priority and the bitmode
    nrfx_timer_config_t config = {
        .frequency          = NRF_TIMER_FREQ_1MHz, // Sets the timer clock to 1 MHz
        .mode               = NRF_TIMER_MODE_TIMER,
        .interrupt_priority = NRFX_TIMER_DEFAULT_CONFIG_IRQ_PRIORITY,
        .p_context          = NULL
    };
    nrfx_timer_init(&scheduler_timer, &config, schedeuling_timer_handler);
    scheduler_timer.p_reg->SHORTS |= TIMER_SHORTS_COMPARE2_CLEAR_Msk;
    nrfx_timer_compare(&scheduler_timer, NRF_TIMER_CC_CHANNEL2, SCHEDULER_TIMER_TICKS, true);
    NVIC_EnableIRQ(TIMER0_IRQn);
    nrfx_timer_enable(&scheduler_timer);
}

void pwm_timer_init(Pwm_config* pwm_cfg, nrfx_timer_event_handler_t event_handler, IRQn_Type irq_num) 
{
    // Sets the mode, the frequency, the priority and the bitmode
    nrfx_timer_config_t config = {
        .frequency          = NRF_TIMER_FREQ_1MHz, // Sets the timer clock to 1 MHz
        .mode               = NRF_TIMER_MODE_TIMER,
        .interrupt_priority = NRFX_TIMER_DEFAULT_CONFIG_IRQ_PRIORITY,
        .p_context          = NULL
    };
    nrfx_timer_init(pwm_cfg->timer, &config, event_handler);


    // When the timer reaches the CC[2] value, clear the timer counter
    pwm_cfg->timer->p_reg->SHORTS |= TIMER_SHORTS_COMPARE2_CLEAR_Msk;

    // It enables 3 interrupts, on CC[0], CC[1] and CC[3] and sets the register values
    nrfx_timer_compare(pwm_cfg->timer, NRF_TIMER_CC_CHANNEL0, pwm_cfg->motor_duty1, true);
    nrfx_timer_compare(pwm_cfg->timer, NRF_TIMER_CC_CHANNEL1, pwm_cfg->motor_duty2, true);
    nrfx_timer_compare(pwm_cfg->timer, NRF_TIMER_CC_CHANNEL2, PWM_TIMER_TICKS, true);
    NVIC_EnableIRQ(irq_num);

    // Start timer 
    nrfx_timer_enable(pwm_cfg->timer);
}

void Peripherals_Init(void){
    twi_init();
    uart_init();

    nrf_gpio_cfg_output(MOTOR_A_IN1);
    nrf_gpio_cfg_output(MOTOR_A_IN2);
    nrf_gpio_cfg_output(MOTOR_B_IN1);
    nrf_gpio_cfg_output(MOTOR_B_IN2);

    nrf_gpio_pin_clear(MOTOR_A_IN1);
    nrf_gpio_pin_clear(MOTOR_A_IN2);
    nrf_gpio_pin_clear(MOTOR_B_IN1);
    nrf_gpio_pin_clear(MOTOR_B_IN2);


    nrfx_timer_event_handler_t irq_handlers[N_TIMERS] = {timer1_handler};
    IRQn_Type irq_num[N_TIMERS] = {TIMER1_IRQn};
    
    Pwm_config pwm_cfg = {
        .pin1 = motors_pin[0],
        .pin2 = motors_pin[1], 
        .motor_duty1 = motors_duty[0],
        .motor_duty2 = motors_duty[1],
        .pin_config = pwm_pin_config,
        .timer = &timers[0], // Since one timer handles 2 motors
        .timer_init = pwm_timer_init
    };
    pwm_cfg.pin_config(&pwm_cfg);
    pwm_timer_init(&pwm_cfg, irq_handlers[0], irq_num[0]);
}

void flush_strbuf() {
	memset(strbuf, 0, 64);
}



void schedeuling_timer_handler(nrf_timer_event_t event_type, void* p_context)
{
    if (event_type == NRF_TIMER_EVENT_COMPARE2) {
        //update IMU, and calculate pitch/roll angles
        
        mpu6050_update(&mpu6050);
        mpu6050_calc_pitch_roll(&mpu6050, &pitch, &roll);
        mpu6050_calc_pitch(&mpu6050, &pitch);
        
        flush_strbuf();
        sprintf(strbuf, "Pitch: %f\n", pitch);
        serialPrint(strbuf);

        /********************* PID CALCULATION *************************/

        //calculate error
        error = setpoint - pitch;

        //calculate Proportional term
        P = Kp * error;

        //calculate Integral term. Account for wind-up
        I += (Ki * error);
        if (I > maxPwm)
            I = maxPwm;
        else if (I < minPwm)
            I = minPwm;

        ////calculate Derivative term
        D = -Kd * (error - lastError);

        //total PID value
        float pid_pwm = P + I + D;

        //max sure pwm is bound between allowed min/max thresholds
        uint16_t out_pwm;
        if (pid_pwm > maxPwm)
            out_pwm = maxPwm;
        else if (pid_pwm < minPwm)
            out_pwm = minPwm;
        else
            out_pwm = (uint8_t) pid_pwm;

        lastError = error;
        
        if (error > 0.0) {
            // Drive motors FORWARD to correct the tilt
            nrf_gpio_pin_set(MOTOR_A_IN1);
            nrf_gpio_pin_clear(MOTOR_A_IN2);

            nrf_gpio_pin_set(MOTOR_B_IN1);
            nrf_gpio_pin_clear(MOTOR_B_IN2);
        }
        else
        {
            // Drive motors BACKWARD to correct the tilt
            nrf_gpio_pin_clear(MOTOR_A_IN1);
            nrf_gpio_pin_set(MOTOR_A_IN2);

            nrf_gpio_pin_clear(MOTOR_B_IN1);
            nrf_gpio_pin_set(MOTOR_B_IN2);
        }
        set_motor_duty(out_pwm, 0);
        set_motor_duty(out_pwm, 1);
    }
}

void _Error_handler(char* file, int line){
    
    while (true) {
        serialPrint("Error with ");
		serialPrint(file);
		serialPrint(" at line: ");
		serialInt(line);
		serialWrite('\n');

		// Delay to avoid terminal from hanging
        nrf_delay_ms(500);
    }
        
}