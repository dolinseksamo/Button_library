/*
 * button.c
 *
 *  Created on: Mar 5, 2025
 *      Author: samod
 */

#include "button.h"


/* Handles button press events and detects different types of presses
 *
 * This function should be called in the appropriate interrupt or polling routine.
 * It debounces the button input, and detects single press, double press, and hold events.
 *
 * For every button, check if (button.change) buttonCallback(button);
 *
 * Returns:
 * 0 - No event detected (debounce period)
 * 1 - Hold detected
 * 3 - Single press detected
 * 2 - Double press detected
 */
uint8_t buttonCallback(Button *btn) {

	btn->change = 0;						// acknowledge change
	uint8_t out = BUTTON_NOTHING;			// Set default output
	uint8_t pin_state = (uint8_t)HAL_GPIO_ReadPin(btn->GPIO_Port, btn->GPIO_Pin);	// Read pin state
	btn->pressed = btn->Active_High ^ pin_state;		// Check if pressed or released

	if (btn->pressed) {				// Check if button is pressed ...
#ifdef SINGLE
		btn->press_count++;
#else
		out = BUTTON_PRESSED;
#endif

#ifdef HOLD
		if (btn->hold) {
			out = BUTTON_HOLD;
			btn->hold = 0;
			btn->press_count = 0;
			btn->counter = 0;
			btn->pressed = 0;
		}
#endif
	}
#ifdef SINGLE
	else if (btn->press_count == 1) {
		out = BUTTON_SINGLE;	// Single press
		btn->press_count = 0;
		btn->counter = 0;
	}
#else
	else {
		out = BUTTON_RELEASED;
		btn->counter = 0;
	}
#endif

    return out;
}
/* Call this function in timer interrupt callback
 * Set HOLD_TIME in counter units
 * ex. Timer_freq = 1000Hz -> HOLD_TIME is in ms.
 */
void buttonIncrementCounter(Button *btn) {

	if (btn->input) {
		// Check for debounce
		if (++btn->debounce >= BUTTON_DEBOUNCE_TIME) {
			btn->change = 1;
			btn->input = 0;
			btn->debounce = 0;
		}
	}
	#ifdef HOLD
	// Check for hold requirement
	if (btn->pressed && ++btn->counter >= BUTTON_HOLD_TIME) {
		btn->change = 1;
		btn->hold = 1;
	}
	#endif
}

/* Call this function in external interrupt callback
 *
 */
void buttonInput(Button *btn) {
	
	btn->debounce = 0;
	btn->input = 1;
}
