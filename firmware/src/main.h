/**
 * @file main.h
 *
 * @defgroup MAIN Main Module
 *
 * @brief Headers for main.c
 *
 */
#ifndef MAIN_H
#define MAIN_H 

#ifndef F_CPU
#define F_CPU 16000000UL    ///< Frequency of CPU
#endif /* ifndef F_CPU */

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <util/delay.h>

// MODULOS DO SISTEMA
#include "conf.h"
#include "dbg_vrb.h"

#ifdef USART_ON
#include "usart.h"
#else
#pragma message("USART: OFF!")
#endif /*ifdef USART_ON*/

#ifdef CAN_ON
#include "can.h"
#include "can_filters.h"
#else
#pragma message("CAN: OFF!")
#endif /*ifdef CAN_ON*/

#ifdef ADC_ON 
#include "adc.h"
#else
#pragma message("ADC: OFF!")
#endif  /*ifdef ADC_ON*/

#ifdef MACHINE_ON
#include "machine.h"
#else
#pragma message("MACHINE: OFF!")
#endif /*ifdef MACHINE_ON*/

#ifdef PWM_ON
#include "pwm.h"
#else
#pragma message("PWM: OFF!")
#endif /*ifdef PWM*/

#ifdef WATCHDOG_ON
#include "watchdog.h"
#else
#pragma message("WATCHDOG: OFF!")
#endif /*ifdef WATCHDOG_ON*/

#ifdef SLEEP_ON
#include "sleep.h"
#else
#pragma message("SLEEP: OFF!")
#endif /*ifdef SLEEP_ON*/

#endif /* ifndef MAIN_H */
