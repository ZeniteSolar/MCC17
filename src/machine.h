/**
 * @file machine.h
 *
 * @defgroup MACHINE State Machine Module
 *
 * @brief Implements the main state machine of the system.
 *
 */

#ifndef MACHINE_H
#define MACHINE_H 

#include <avr/io.h>
#include <avr/wdt.h>

#include "conf.h"
#ifdef ADC_ON
#include "adc.h"
#endif
#ifdef USART_ON
#include "usart.h"
#endif
#include "dbg_vrb.h"
#ifdef PWM_ON
#include "pwm.h"
#endif
#ifdef CAN_ON
#include "can.h"
#include "can_app.h"
extern const uint8_t can_filter[];
#endif

typedef enum state_machine{
    STATE_INITIALIZING,
    STATE_IDLE,
    STATE_RUNNING,
    STATE_ERROR,
} state_machine_t;

typedef union system_flags{
    struct{
        uint8_t     mppt_on         :1;
        uint8_t     enable          :1;
    };
    uint8_t   all__;
} system_flags_t;

typedef union error_flags{
    struct{
        uint8_t     overcurrent :1;
        uint8_t     overvoltage :1;
        uint8_t     overheat    :1;
        uint8_t     fault       :1;
        uint8_t     no_canbus   :1;
    };
    uint8_t   all;
}error_flags_t;

typedef struct control{
    uint8_t     D_raw;          // value from 0 to 255
    uint8_t     D_raw_target;   // value for target pwm, from 0 to 255
    uint16_t    D;              // value converted from 0 to TOP
    uint8_t     I_raw;          // value from 0 to 255
    uint8_t     I_raw_target;   // value for target pwm, from 0 to 255
    uint8_t     I;              // value of current in AMPS
    uint8_t     V;              // value of voltage in VOLTS
    uint8_t     R;              // value of angular velocity in RPMS
    uint8_t     T;              // value of temperature in CELCIUS DEGREES
    uint8_t     fault;          // counts the faults from ir2127

}control_t;

control_t control;

// machine checks
void check_idle_current(void);
void check_idle_voltage(void);
void check_running_current(void);
void check_running_voltage(void);
//void check_can(void);         // transfered to can_app.h
void check_pwm_fault(void);

// debug functions
void print_system_flags(void);
void print_error_flags(void);
void print_control(void);

// machine tasks
void task_initializing(void);
void task_idle(void);
void task_running(void);
void task_error(void);

// the machine itself
void machine_init(void);
void machine_run(void);
void set_state_error(void);
void set_state_initializing(void);
void set_state_idle(void);
void set_state_running(void);

// machine variables
state_machine_t state_machine;
system_flags_t system_flags;
error_flags_t error_flags;
volatile uint8_t machine_clk;
uint8_t total_errors;   // Contagem de ERROS

// pwm variables
uint8_t pwm_fault_count;
uint8_t check_pwm_fault_times;

// other variables
uint8_t led_clk_div;

// ISRs
ISR(TIMER2_COMPA_vect);
ISR(PCINT2_vect);

#endif /* ifndef MACHINE_H */
