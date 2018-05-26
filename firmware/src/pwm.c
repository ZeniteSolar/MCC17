#include "pwm.h"
#include <util/delay.h>

/**
 * @brief configura o PWM usando o timer TC1
 */
void pwm_init()
{
    // configuracao do Timer TC1 --> TIMER DO PWM
    //TCCR1B |= ((0<<ICNC1) | (0<<ICES1));
    
    /*TCCR1A = TCCR1B = 0;
    
    TCCR1B |= ((0<<CS12) | (0<<CS11) | (1<<CS10));
    TCCR1A |= ((1<<COM1A1) |    (0<<COM1A0));
    TCCR1A |= ((0<<COM1B1) |    (0<<COM1B0)); 
    TCCR1A |= ((1<<WGM11) | (0<<WGM10));
    TCCR1B |= ((0<<WGM13) | (0<<WGM12));
    
    */
  	TCCR1A |= 0b10000010;
  	TCCR1B |= 0b00010001;


    ICR1   = 80;                                    // valor TOP para f_pwm = 100kHz
    OCR1A  = INITIAL_D;                             // D = %*ICR1

    set_bit(PWM_DDR, PWM);                          // PWM como saida
      
    // Equacao para Frequencia do PWM:       ICR1 = (f_osc)/(2*f_pwm);

}

/**
 * @brief reset pwm and its control buffers.
 */
inline void pwm_reset(void)
{
    set_pwm_off();
    control.D = 0;
    VERBOSE_MSG_PWM(usart_send_string("PWM turned off!\n"));
}


/**
 * @brief computs duty-cycle for PWM
 */

inline void pwm_compute(void)
{	
    if(adc_data_ready){
	    pertub_and_observe();
        adc_data_ready = 0;
    }
	
	// treats faults
    if(error_flags.overvoltage || error_flags.overcurrent){
        error_flags.overvoltage = error_flags.overcurrent = 0;
        if(control.D >= (D_STEP+1))      control.D -= (D_STEP+1);
        else                    control.D = 0;
    }

    // apply some threshhold saturation limits
    if(control.D > PWM_D_MAX_THRESHHOLD)        control.D = PWM_D_MAX;
    else if(control.D < PWM_D_MIN_THRESHHOLD)   control.D = PWM_D_MIN;

    // apply dutycycle
    if(adc_data_ready){
        OCR1A = control.D;
        adc_data_ready = 0;
    }

    VERBOSE_MSG_PWM(usart_send_string("PWM computed as: "));
    VERBOSE_MSG_PWM(usart_send_uint16(OCR1A));
    VERBOSE_MSG_PWM(usart_send_char('\n'));
	
}

/**
 * @brief decreases pwm by 10% in case of mosfet fault detected by IR2127.
 */
inline void pwm_treat_fault(void)
{
    /*if(control.D_raw_target > 10)
        control.D_raw_target -= 6;      // -10%
        */
    if(OCR1A > 10)
        OCR1A -= 6;
    VERBOSE_MSG_PWM(usart_send_string("PWM fault treated\n"));
}

