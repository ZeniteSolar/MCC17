#include "machine.h"

/*
 * to-do:
 *      - modularize state sinalization
 *      - 
 *
 */

/**
 * @brief 
 */
void machine_init(void)
{
    TCCR2A  =   (1 << WGM21) | (0 << WGM20)         // Timer 2 in Mode 2 = CTC (clear on compar  e)
            | (0 << COM2A1) | (0 << COM2A0)         // do nothing with OC2A
            | (0 << COM2B1) | (0 << COM2B0);        // do nothing with OC2B
    TCCR2B  =   (0 << WGM22)                        // Timer 0 in Mode 2 = CTC (clear on compar  e)
            | (0 << FOC0A) | (0 << FOC0B)           // dont force outputs
            | (1 << CS22)                           // clock enabled, prescaller = 1024
            | (1 << CS21)
            | (1 << CS20);
    OCR2A   =   240; //80                               // Valor para igualdade de comparacao A par  a frequencia de 150 Hz
    TIMSK2 |=   (1 << OCIE2A);                      // Ativa a interrupcao na igualdade de comp  aração do TC2 com OCR2A

	
} 

/**
 * @brief checks the quantity of the faults.
 */
inline void check_pwm_fault(void)
{
	#ifdef PWM_ON
    if(pwm_fault_count >= FAULT_COUNT_LIMIT){
        error_flags.fault = 1;
        check_pwm_fault_times = 0;
        set_state_error();
    }else if(check_pwm_fault_times++ > CHECKS_BEFORE_RESET_FAULT_COUNTER){
        pwm_fault_count = 0;    
    }
	#endif
}

/**
 * @brief checks the buffer and waits it fills up
 */
inline void check_buffers(void)
{
    //VERBOSE_MSG_MACHINE(usart_send_string("Checking buffers..."));
    while(!CBUF_IsFull(cbuf_adc0));
    while(!CBUF_IsFull(cbuf_adc1));
    while(!CBUF_IsFull(cbuf_adc2));
    //VERBOSE_MSG_ERROR(usart_send_string("<No buffers to check>"));
    //VERBOSE_MSG_MACHINE(usart_send_string(" \t\tdone.\n")); 
}

/**
 * @brief checks if the current level is ok for IDLE state
 */
inline void check_idle_panel_current(void)
{ 
    control.i_panel = ma_adc0() * CONVERSION_PANEL_CURRENT_VALUE;

	/*if(control.i_panel >= MAXIMUM_IDLE_PANEL_CURRENT ){		// MAXIMUM_RUNNING_PANEL_VOLTAGE sem valor em #define 
		error_flags.overcurrent = 1;
	}else if(control.i_panel <= MINIMUM_IDLE_PANEL_CURRENT){
		error_flags.undercurrent = 1;
	}else error_flags.overcurrent = 0;*/
 
}

/**
 * @brief checks if the voltage level is ok for IDLE state
 */
inline void check_idle_panel_voltage(void)
{
    control.v_panel = ma_adc1() * CONVERSION_PANEL_VOLTAGE_VALUE;
   	/*if(control.v_panel >= MAXIMUM_IDLE_PANEL_VOLTAGE){
	   	error_flags.overvolt_panel = 1;
   	}else if(control.v_panel <= MINIMUM_IDLE_PANEL_VOLTAGE){
		error_flags.undervol_panel = 1;	
	}else error_flags.overvolt_panel = 0; 
    */
}

/**
* @brief checks if the voltage of Battery level is ok for IDLE state
*/
inline void check_idle_battery_voltage(void)
{
    check_running_battery_voltage();
}
 
/**
 * @brief checks if the current level is ok for running state
 */
inline void check_running_panel_current(void)
{
   	control.i_panel = ma_adc0() * CONVERSION_PANEL_CURRENT_VALUE;
	
    if(control.i_panel >= MAXIMUM_RUNNING_PANEL_CURRENT ){		// MAXIMUM_RUNNING_PANEL_VOLTAGE sem valor em #define 
		error_flags.overcurrent = 1;
	/*}else if(control.i_panel <= MINIMUM_RUNNING_PANEL_CURRENT){
		error_flags.undercurrent = 1;*/
	}else error_flags.overcurrent = 0;
}

/**
 * @brief checks if the voltage level is ok for running state
 */
inline void check_running_panel_voltage(void)
{
   	control.v_panel = ma_adc1() * CONVERSION_PANEL_VOLTAGE_VALUE;
   	/*
    if(control.v_panel >= MAXIMUM_RUNNING_PANEL_VOLTAGE){		// MAXIMUM_RUNNING_PANEL_VOLTAGE sem valor em #define 
	   	error_flags.overvolt_panel = 1;
   	}else if(control.v_panel <= MINIMUM_RUNNING_PANEL_VOLTAGE){
		error_flags.undervol_panel = 1;	
	}else error_flags.overvolt_panel = 0;
    */
}

/**
 * @brief checks if the voltage of Battery level is ok for running state
 */
inline void check_running_battery_voltage(void) // sem panel
{
   	control.v_bat = ma_adc2() * CONVERSION_BATTERY_VOLTAGE_VALUE;
	   
   	if(control.v_bat >= MAXIMUM_BATTERY_VOLTAGE){		// MAXIMUM_RUNNING_PANEL_VOLTAGE sem valor em #define 
	   	error_flags.overvoltage = 1;
   	/*}else if(control.v_bat <= MINIMUM_BATTERY_VOLTAGE){
		error_flags.undervoltage = 1; */
	}else error_flags.overvoltage = 0; 
}

/**
 * @brief set error state
 */
inline void set_state_error(void)
{
    state_machine = STATE_ERROR;
}

/**
* @brief set initializing state
*/ 
inline void set_state_initializing(void)
{
    state_machine = STATE_INITIALIZING;
}

/**
* @brief set idle state
*/ 
inline void set_state_idle(void)
{
    state_machine = STATE_IDLE;
    pwm_reset();
}

/**
* @brief set running state
*/ 
inline void set_state_running(void)
{
    state_machine = STATE_RUNNING;
}

/**
* @brief prints the system flags
*/
inline void print_system_flags(void)
{
    VERBOSE_MSG_MACHINE(usart_send_string(" ON: "));
    VERBOSE_MSG_MACHINE(usart_send_char(48+system_flags.mppt_on));
    
    VERBOSE_MSG_MACHINE(usart_send_string(" EN "));
    VERBOSE_MSG_MACHINE(usart_send_char(48+system_flags.enable));

}

/**
* @brief prints the error flags
*/
inline void print_error_flags(void)
{
    VERBOSE_MSG_MACHINE(usart_send_string("\nOvrI: "));
    VERBOSE_MSG_MACHINE(usart_send_char(48+error_flags.overcurrent));
    
    VERBOSE_MSG_MACHINE(usart_send_string(" OvrV: "));
    VERBOSE_MSG_MACHINE(usart_send_char(48+error_flags.overvoltage));

    VERBOSE_MSG_MACHINE(usart_send_string(" OvrT: "));
    VERBOSE_MSG_MACHINE(usart_send_char(48+error_flags.overheat));

    VERBOSE_MSG_MACHINE(usart_send_string(" NOCAN: "));
    VERBOSE_MSG_MACHINE(usart_send_char(48+error_flags.no_canbus));
}
 
/**
* @brief prints the error flags
*/
inline void print_control(void)
{
    VERBOSE_MSG_MACHINE(usart_send_string(" D: "));
    VERBOSE_MSG_MACHINE(usart_send_uint16(control.D));
    VERBOSE_MSG_MACHINE(usart_send_char(' '));
    
    VERBOSE_MSG_MACHINE(usart_send_string(" Ip: "));
    VERBOSE_MSG_MACHINE(usart_send_uint16(control.i_panel));
    VERBOSE_MSG_MACHINE(usart_send_char(' '));

    VERBOSE_MSG_MACHINE(usart_send_string(" Vp: "));
    VERBOSE_MSG_MACHINE(usart_send_uint16(control.v_panel));
    VERBOSE_MSG_MACHINE(usart_send_char(' '));

    VERBOSE_MSG_MACHINE(usart_send_string(" Vb: "));
    VERBOSE_MSG_MACHINE(usart_send_uint16(control.v_bat));
    VERBOSE_MSG_MACHINE(usart_send_char(' '));

    VERBOSE_MSG_MACHINE(usart_send_string(" Pp: "));
    VERBOSE_MSG_MACHINE(usart_send_uint16(control.pi_med));
    VERBOSE_MSG_MACHINE(usart_send_char(' '));
    VERBOSE_MSG_MACHINE(usart_send_uint16(control.pi_med_old));
    VERBOSE_MSG_MACHINE(usart_send_char(' '));

}

/**
 * @brief Checks if the system is OK to run:
 *  - all ring_buffers needed to be full
 *  - checks the current
 *  - checks the voltage
 *
 */
inline void task_initializing(void)
{
    set_led();
    set_pwm_off();
    pwm_fault_count = 0;

    check_buffers();
    check_idle_panel_current();
    check_idle_panel_voltage();
    check_idle_battery_voltage();
    
    VERBOSE_MSG_INIT(usart_send_string("System initialized without errors.\n"));
    set_state_idle();
}

/**
 * @brief waits for commands while checking the system:
 *  - checks the deadman's switch state
 *  - checks the on_off_switch state
 *  - checks the potentiometer state
 *  then if its ok, enable the system to operate
 */
inline void task_idle(void)
{
    if(led_clk_div++ >= 50){
        cpl_led();
        led_clk_div = 0;
    }

    check_idle_panel_current();
    check_idle_panel_voltage();
    check_idle_battery_voltage();

    if(system_flags.mppt_on && system_flags.enable){
        if(init_pwm_increment_divider++){
            init_pwm_increment_divider = 0;
            if(control.D < PWM_INITIAL_D) set_pwm_duty_cycle(control.D++);
            else{
                if(!error_flags.all){
                    VERBOSE_MSG_MACHINE(usart_send_string("Enjoy, the system is at its RUNNING STATE!!\n"));
                    set_state_running();
                }
                else{
                    VERBOSE_MSG_ERROR(usart_send_string("Sorry. I have have found errors in the initialilation process. \n\nI will begin to process it...\n"));
                    set_state_error();
                }
            }
        }
    }
}


/**
 * @brief running task checks the system and apply the control action to pwm.
 */
inline void task_running(void)
{
    if(led_clk_div++ >= 10){
        cpl_led();
        led_clk_div = 0;
    }

    check_running_panel_current();
    check_running_panel_voltage();
	check_running_battery_voltage();

    // enable pwm to be computed
    adc_data_ready = 1;

    if(system_flags.mppt_on && system_flags.enable){
        pwm_compute();
    }else{
        set_state_idle();
    }
}


/**
 * @brief error task checks the system and tries to medicine it.
 */
inline void task_error(void)
{
    if(led_clk_div++ >= 5){
        cpl_led();
        led_clk_div = 0;
    }

    pwm_reset();

    total_errors++;         // incrementa a contagem de erros
    VERBOSE_MSG_ERROR(usart_send_string("The error code is: "));
    VERBOSE_MSG_ERROR(usart_send_uint16(error_flags.all));
    VERBOSE_MSG_ERROR(usart_send_char('\n'));

    if(error_flags.overcurrent)
        VERBOSE_MSG_ERROR(usart_send_string("\t - Panel over-current!\n"));
    if(error_flags.overvoltage)
        VERBOSE_MSG_ERROR(usart_send_string("\t - Battery over-voltage!\n"));
    if(error_flags.fault)
        VERBOSE_MSG_ERROR(usart_send_string("\t - FAULT!\n"));
    if(error_flags.no_canbus)
        VERBOSE_MSG_ERROR(usart_send_string("\t - No canbus communication with MIC17!\n"));
    if(!error_flags.all)
        VERBOSE_MSG_ERROR(usart_send_string("\t - Oh no, it was some unknown error.\n"));
 
    VERBOSE_MSG_ERROR(usart_send_string("The error level is: "));
    VERBOSE_MSG_ERROR(usart_send_uint16(total_errors));
    VERBOSE_MSG_ERROR(usart_send_char('\n'));
    
    if(total_errors < 2){
        VERBOSE_MSG_ERROR(usart_send_string("I will reset the machine state.\n"));
    }
    if(total_errors >= 20){
        VERBOSE_MSG_ERROR(usart_send_string("The watchdog will reset the whole system.\n"));
        for(;;);    // waits the watchdog to reset.
    }
    
    cpl_led();
    set_state_initializing();
}


/**
 * @brief this is the machine state itself.
 */
inline void machine_run(void)
{
	#ifdef CAN_ON
    can_app_task();
    #else
    system_flags.enable = system_flags.mppt_on = 1;
	#endif

    print_system_flags();
    print_error_flags();
    print_control();

    if(machine_clk){
        machine_clk = 0;
        switch(state_machine){
            case STATE_INITIALIZING:
                task_initializing();

                break;
            case STATE_IDLE:
                task_idle();

                break;
            case STATE_RUNNING:
                task_running();

                break;
            case STATE_ERROR:
                task_error();

            default:
                break;
        }
    }
}

/**
 * @brief Interrupcao das chaves: se alguma chave desligar, o motor desliga.
 */

ISR(INT0_vect) //overvoltage
{
	control.fault = 1;
}
ISR(INT1_vect) //enable
{    
    /*if(bit_is_clear(FAULT_PIN, FAULT)){
        pwm_treat_fault();
        cpl_led();
        pwm_fault_count++;
    }
    */
    system_flags.enable = 0;
    DEBUG1;
}

/**
* @brief ISR para ações de controle
*/
ISR(TIMER2_COMPA_vect)
{
    //VERBOSE_MSG_ERROR(if(machine_clk) usart_send_string("\nERROR: CLOCK CONFLICT!!!\n"));
	machine_clk = 1;
}

