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


    ICR1   = 640;                                   // valor TOP para f_pwm = 12 KHz
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
    control.D_raw = control.D_raw_target = control.D = 0;
    control.I_raw = control.I_raw_target = control.I = 0;
    VERBOSE_MSG_PWM(usart_send_string("PWM turned off!\n"));
}


/**
 * @brief computs duty-cycle for PWM
 */
/*
inline void pwm_compute(void)
{

    // it defines the arctan of D curve
    if(control.I_raw < control.I_raw_target){
        control.I_raw++;
    }else if(control.I_raw >= control.I_raw_target){
        control.I_raw = control.I_raw_target;
    }

    if(control.D_raw < control.D_raw_target){           //!< law for D increasing
        if((pwm_d_clk_div++ +8) >= ((control.I_raw >> 3))){
            control.D_raw += PWM_D_DELTA;
            pwm_d_clk_div = 0;
        }
    }else if(control.D_raw >= control.D_raw_target){    //!< law for D decreasing
        control.D_raw = control.D_raw_target;
    }

    // treats fault from ir2127
    if(control.fault){
        control.fault = 0;
        if(control.D_raw >= 2)     control.D_raw -= 2;
        else                        control.D_raw = 0;
    }
*/
/**
 * @brief P&O algorithm
 */
inline void mppt_task(void)
{
	// Computes power input
	pi_med = adc_voltage*adc_current;
	// Respects limits for duty Cycle
	if( pi_med < pi_med_old )	updown ^=1; // configurar updown como variavel global (talvez)

	// Apply a perturbation
	if(!updown) d -= D_STEP;		// (1) configurar d como variavel global; (2) configurar D_STEP como um DEFINE
	else d += D_STEP;

	// recycles
	pi_med_old = pi_med;
}
    // converts to OCR1A range.
    control.D = (control.D_raw*PWM_D_LIN_MULT) >> PWM_D_LIN_DIV;

    // apply some threshhold saturation limits
    if(control.D > PWM_D_MAX_THRESHHOLD)        control.D = PWM_D_MAX;
    else if(control.D < PWM_D_MIN_THRESHHOLD)   control.D = PWM_D_MIN;

    // apply dutycycle
    OCR1A = control.D;

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

/**
 *  @brief funcao conta o tempo em que o Duty cycle fica em zero
 *  @param registrador de 16 bits; nesse caso, do timer 1 (OCR1A)
 *  @ret  retorna 1 se registrador esta em zero por mais de um segundo (para
 *  freq = 35 Hz)
 */
uint8_t pwm_zero_width(uint16_t duty_cycle)
{
    //VERBOSE_MSG_PWM(usart_send_string("PWM zero witdh..."));

	static uint8_t times = 0;
	static uint8_t one_sec = 0;

	if(duty_cycle <= PWM_D_MIN_THRESHHOLD) times++;
	else times = 0;

	if(times >= MIN_ZERO_WIDTH_TIMES){
		one_sec = 1;
        VERBOSE_MSG_PWM(usart_send_string("OK!\n"));
	}else{
        one_sec = 0;
        VERBOSE_MSG_PWM(usart_send_string("NOK!\n"));
    }
	return one_sec;
}
