// coding: utf-8

#include "main.h"

int main(void)
{
    #ifdef USART_ON
        usart_init(MYUBRR,1,1);                         // inicializa a usart
        VERBOSE_MSG_INIT(usart_send_string("\n\n\nUSART... OK!\n"));
    #endif

    _delay_ms(1000);

    #ifdef PWM_ON
        VERBOSE_MSG_INIT(usart_send_string("PWM..."));
        pwm_init();
        VERBOSE_MSG_INIT(usart_send_string(" OK!\n"));
    #endif

 /*   #ifdef TEST_PWM
        for(;;) OCR1A = 30;
    #endif
    */

    //for(;;) OCR1A = 60;

    #ifdef CAN_ON
        VERBOSE_MSG_INIT(usart_send_string("CAN (500kbps)..."));
        can_init(BITRATE_500_KBPS);
        VERBOSE_MSG_INIT(usart_send_string(" OK!\n"));
        VERBOSE_MSG_INIT(usart_send_string("CAN filters..."));
        can_static_filter(can_filter);
        VERBOSE_MSG_INIT(usart_send_string(" OK!\n"));
    #endif

    #ifdef ADC_ON
        VERBOSE_MSG_INIT(usart_send_string("ADC..."));
        adc_init();
        VERBOSE_MSG_INIT(usart_send_string(" OK!\n"));
    #endif


    #ifdef SLEEP_ON 
        VERBOSE_MSG_INIT(usart_send_string("SLEEP..."));
        sleep_init();
        VERBOSE_MSG_INIT(usart_send_string(" OK!\n"));
    #endif

    #ifdef WATCHDOG_ON
        VERBOSE_MSG_INIT(usart_send_string("WATCHDOG..."));
        wdt_init();
        VERBOSE_MSG_INIT(usart_send_string(" OK!\n"));
    #endif

 	#ifdef MACHINE_ON
        VERBOSE_MSG_INIT(usart_send_string("MACHINE..."));
		machine_init();
        VERBOSE_MSG_INIT(usart_send_string(" OK!\n"));
	#endif
	
    set_bit(LED_DDR, LED);                      // LED como saída
    #ifdef PWM_ON
	set_bit(PWM_DDR, PWM);                      // PWM como saida
    #endif
	/*
    // ------------------------------------------------------------------------
	clr_bit(BatOverVoltageInterrupt_DDR,BatOverVoltageInterrupt);	// BatOverVoltageInterrupt como entrada
	
	clr_bit(Enable_DDR,Enable);					// Enable como entrada
	set_bit(Enable_PORT,Enable);				// Enable com pull-up
	
	// interrupcao do Enable (Enable PD3)
    set_bit(EICRA, ISC11);                      // falling edge for int1
    set_bit(EIMSK, INT1);                       // enables int1 interrupt
    set_bit(EIFR, INTF1);                       // clears int1 interrupt
	
	// interrupcao Over voltage Battery (BatOvervoltage_interrupt PD2)
	set_bit(EICRA, ISC10);                      // rising edge for int1
	set_bit(EIMSK, INT0);                       // enables int1 interrupt
	set_bit(EIFR, INTF0);                       // clears int0 interrupt
	*/	
	
    sei();
	
	for(;;){
		#ifdef WATCHDOG_ON
            wdt_reset();
		#endif

        #ifdef MACHINE_ON
            machine_run();
        #endif

		#ifdef SLEEP_ON
            sleep_mode();
		#endif
	}
}


/**
 * @brief se em debug, fica chaveando os pinos de debugs até o reset do watchdog
 */
ISR(BADISR_vect)
{
    for(;;){
        VERBOSE_MSG_ERROR(usart_send_string("\nFATAL ERROR: BAD ISR."));
        #ifdef WATCHDOG_ON
            VERBOSE_MSG_ERROR(usart_send_string("WAITING FOR WATCHDOG TO RESET...\n"));
        #endif
        #ifdef DEBUG_ON
            DEBUG0;
            DEBUG1;
            _delay_ms(100);
        #endif
    }
}

