/**
 * @file watchdog.h
 *
 * @defgroup WATCHDOG Watchdog Module
 *
 * @brief A simple watchdog
 *
 */

#ifndef WATCHDOG_H
#define WATCHDOG_H 

#include <avr/io.h>
#include <avr/wdt.h>

/**
 * @brief This function is called upon a HARDWARE RESET:
 */
void wdt_first(void) __attribute__((naked)) __attribute__((section(".init3")));

/**
 * @brief Clear SREG_I on hardware reset.
 */
void wdt_first(void)
{
    MCUSR = 0; // clear reset flags
    wdt_disable();
    //http://www.atmel.com/webdoc/AVRLibcReferenceManual/FAQ_1faq_softreset.html
}

/**	
 * @brief inicializa o watchdog em 8s 
 */
void wdt_init(void)
{
    wdt_enable(WDTO_2S);
}

#endif /* ifndef WATCHDOG_H */
