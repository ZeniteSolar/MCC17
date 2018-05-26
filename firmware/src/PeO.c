/*
 * PeO.c
 *
 * Created: 15/12/2017 19:18:18
 *  Author: jack
 */ 
#include "PeO.h"
/**
 * @brief P&O algorithm
 */
inline void pertub_and_observe(void)
{	
	// Computes power input
	control.pi_med = (control.v_panel)*(control.i_panel);
	// Respects limits for duty Cycle
	if( control.pi_med < control.pi_med_old )	control.updown ^=1; // configurar updown como variavel global (talvez)

	// Apply a perturbation
	if(!control.updown) control.D -= D_STEP;		// D_STEP nao tem valor em #define 
	else control.D += D_STEP;

    // WARNING: DEFINITIONS FOR TEST THE CONVERTER WITH FIXED DUTY CYCLE!!!
#ifdef CONVERTER_TEST_WITH_FIXED_DUTYCYCLE
    control.D = CONVERTER_TEST_WITH_FIXED_DUTYCYCLE_DT_VALUE;
#endif

	// recycles
	control.pi_med_old = control.pi_med;
}
