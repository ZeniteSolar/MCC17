/**
 * @file adc.h
 *
 * @defgroup ADC    ADC Module
 *
 * @brief This module implements a simple ADC using a state machine to mux
 * between the adc channels.
 *
 */

#ifndef ADC_H
#define ADC_H

#include <avr/io.h>
#include <avr/interrupt.h>

#include "../lib/bit_utils.h"
#include "../lib/cbuf.h"
#include "../lib/log2.h"
#include "conf.h"

#include "dbg_vrb.h"

#define ADC_LAST_CHANNEL 4                  //*< quantity of channels used

typedef volatile enum adc_channels{ 
    ADC0, ADC1 ,ADC2, ADC3, ADC4, ADC5  
} adc_channels_t;                           //*< the adc_channel type 
static adc_channels_t ADC_CHANNEL = ADC0;   //*< current chosen adc channel
uint8_t raw_adc[ADC_LAST_CHANNEL];          //*< an array for raw measurements

void adc_select_channel(adc_channels_t _ch);
void adc_init(void);

// MOVING AVERAGE BELOW //
//
void init_buffers(void);

#define cbuf_adc0_SIZE          128       // size of buffers
#define cbuf_adc0_SIZE_2        7         // size of buffers in 2^n
volatile struct cbuf{
    uint8_t     m_getIdx;
    uint8_t     m_putIdx;
    uint8_t     m_entry[cbuf_adc0_SIZE];
} cbuf_adc0;

uint8_t ma_adc0(void);
uint8_t avg_adc0;

#endif /* ifndef ADC_H */
