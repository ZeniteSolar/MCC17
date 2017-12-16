/**
 * @file pwm.h
 *
 * @defgroup PWM Module
 *
 * @brief This module implements a simple PWM
 *
 */

#ifndef PWM_H
#define PWM_H

#include <avr/io.h>
#include <avr/interrupt.h>

#include "conf.h"
#include "usart.h"
#include "dbg_vrb.h"
#include "machine.h"
#include "PeO.h"
#include "../lib/cbuf.h"

#ifndef INITIAL_D
#define INITIAL_D 0
#endif

// // pwm macros
#define set_pwm_duty_cycle(d)       OCR1A = d      //!< apply duty cycle 'd'
#define set_pwm_off()               set_pwm_duty_cycle(0)      //!< d = 0

// pwm functions
void pwm_init(void);
void pwm_reset(void);
void pwm_compute(void);
void pwm_treat_fault(void);
uint8_t pwm_zero_width(uint16_t duty_cycle);

// // pwm variables
uint8_t pwm_d_clk_div;

#endif /* ifndef PWM_H */
