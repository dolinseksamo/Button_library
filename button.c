/*
 * button.c
 *
 *  Created on: Mar 5, 2025
 *      Author: samod
 */

#include "button.h"

/* -------------------------------------------------------------------------
 * buttonCallback()
 *
 * Main function to process button state changes.
 * - Should be called when (btn->vars.change == 1).
 * - Reads the GPIO pin and updates the pressed/released state.
 * - Handles pressed, released, and hold events.
 *
 * Returns one of:
 *  - BUTTON_NOTHING   → no event detected
 *  - BUTTON_PRESSED   → button was pressed
 *  - BUTTON_RELEASED  → button was released
 *  - BUTTON_HOLD      → hold time exceeded (if enabled)
 * ------------------------------------------------------------------------- */
uint8_t buttonCallback(Button_t *btn) {
    btn->vars.change = 0;  // Clear change flag (event acknowledged)
    uint8_t out = BUTTON_NOTHING;

    // Read the current GPIO pin state
    uint8_t pin_state = (uint8_t)HAL_GPIO_ReadPin(btn->gpio.GPIO_Port, btn->gpio.GPIO_Pin);

    // Apply polarity (Active High / Active Low) to determine if pressed
    btn->vars.pressed = btn->gpio.Active_High ^ pin_state;

    if (btn->vars.pressed) {
        // Button is physically pressed
        out = BUTTON_PRESSED;

        // Check hold detection
        if (btn->vars.hold_enable && btn->vars.hold) {
            out = BUTTON_HOLD;
            btn->vars.hold = 0;       // Reset hold flag
            btn->vars.counter = 0;    // Reset hold counter
            btn->vars.pressed = 0;    // Clear pressed state
        }
    } else {
        // Button is physically released
        out = BUTTON_RELEASED;
        btn->vars.counter = 0;        // Reset hold counter on release
    }

    return out;
}

/* -------------------------------------------------------------------------
 * buttonIncrementCounter()
 *
 * Called in a periodic timer interrupt (example: 1kHz SysTick).
 * Handles:
 *  - Debouncing (filters out noise when button toggles)
 *  - Hold timing (increments counter while pressed)
 * ------------------------------------------------------------------------- */
inline void buttonIncrementCounter(ButtonVariables_t *btn) {
    if (btn->input) {
        // Debounce in progress
        if (++btn->debounce >= BUTTON_DEBOUNCE_TIME) {
            btn->change = 1;          // Signal state change ready
            btn->input = 0;           // Clear input trigger
            btn->debounce = 0;        // Reset debounce counter
        }
    }

    // Hold detection (if enabled and still pressed)
    if (btn->hold_enable && btn->pressed && ++btn->counter >= BUTTON_HOLD_TIME) {
        btn->change = 1;              // Signal hold event ready
        btn->hold = 1;                // Mark hold detected
    }
}

/* -------------------------------------------------------------------------
 * buttonInput()
 *
 * Called in EXTI interrupt when the button pin changes.
 * Simply flags input for debounce processing.
 * ------------------------------------------------------------------------- */
inline void buttonInput(ButtonVariables_t *btn) {
    btn->debounce = 0;  // Reset debounce counter
    btn->input = 1;     // Mark input change detected
}
