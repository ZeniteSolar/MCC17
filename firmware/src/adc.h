/**
 * @file adc.h
 *
 * @defgroup ADC    ADC Module
 *
 * @brief This module implements a simple ADC using a state machine to mux
 * between the adc channels.
 *
 */

#ifndef _ADC_H_
#define _ADC_H_

#include "avr/io.h"
#include "avr/interrupt.h"
#include "../lib/bit_utils.h"
#include "dbg_vrb.h"
#include "../lib/cbuf.h"
#include "../lib/log2.h"
#include "usart.h"

#define ADC_LAST_CHANNEL 3                  //*< quantity of channels used

typedef volatile enum adc_channels{ 
    ADC0, ADC1 ,ADC2, ADC3, ADC4, ADC5  
} adc_channels_t;                           //*< the adc_channel type 
static adc_channels_t ADC_CHANNEL = ADC0;   //*< current chosen adc channel

uint8_t adc_select_channel(adc_channels_t __ch);
void adc_init(void);

// MOVING AVERAGE BELOW //
//
void init_buffers(void);

#define cbuf_adc0_SIZE          32        // size of buffers
#define cbuf_adc0_SIZE_2        5         // size of buffers in 2^n
volatile struct cbuf_adc0{
    uint8_t     m_getIdx;
    uint8_t     m_putIdx;
    uint8_t     m_entry[cbuf_adc0_SIZE];
} cbuf_adc0;
uint8_t ma_adc0(void);
uint8_t avg_adc0;

#define cbuf_adc1_SIZE          32        // size of buffers
#define cbuf_adc1_SIZE_2        5         // size of buffers in 2^n
volatile struct cbuf_adc1{
    uint8_t     m_getIdx;
    uint8_t     m_putIdx;
    uint8_t     m_entry[cbuf_adc1_SIZE];
} cbuf_adc1; 
uint8_t ma_adc1(void);
uint8_t avg_adc1;

#define cbuf_adc2_SIZE          32        // size of buffers
#define cbuf_adc2_SIZE_2        5         // size of buffers in 2^n
volatile struct cbuf_adc2{
    uint8_t     m_getIdx;
    uint8_t     m_putIdx;
    uint8_t     m_entry[cbuf_adc2_SIZE];
} cbuf_adc2;  
uint8_t ma_adc2(void);
uint8_t avg_adc2;

#endif /* ifndef _ADC_H_ */
