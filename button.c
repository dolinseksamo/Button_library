/*
 * button.c
 *
 *  Created on: Mar 5, 2025
 *      Author: samod
 *
 *  Implementation of button handling library.
 *  Features:
 *   - Debouncing
 *   - Press, Release, Hold events
 *   - Safe callbacks (default empty functions)
 */

#include "button.h"

// ==================================================================
// Internal empty handler
// Used as default callback to prevent NULL pointer crashes
// ==================================================================
static void emptyHandler(void) { }

// ==================================================================
// buttonInit()
// Initializes button configuration, state variables, and callbacks
// ==================================================================
void buttonInit(Button_t *btn, GPIO_TypeDef *port, uint16_t pin, uint8_t active_high, uint8_t hold_enable) {
    // Assign GPIO config
    btn->gpio.GPIO_Port   = port;
    btn->gpio.GPIO_Pin    = pin;
    btn->gpio.Active_High = active_high;

    // Reset runtime variables
    btn->vars.hold_enable = hold_enable;
    btn->vars.hold        = 0;
    btn->vars.input       = 0;
    btn->vars.debounce    = 0;
    btn->vars.change      = 0;
    btn->vars.pressed     = 0;
    btn->vars.counter     = 0;

    // Assign safe default callbacks
    btn->onPress   = emptyHandler;
    btn->onRelease = emptyHandler;
    btn->onHold    = emptyHandler;
}

// ==================================================================
// buttonCallback()
// Should be called when (btn->vars.change) is set
// Reads GPIO, determines press/release/hold, and calls handlers
// ==================================================================
void buttonCallback(Button_t *btn) {
    btn->vars.change = 0;      // Reset change flag

    // Read GPIO pin state
    uint8_t pin_state = (uint8_t)HAL_GPIO_ReadPin(btn->gpio.GPIO_Port, btn->gpio.GPIO_Pin);

    // Apply polarity (active high/low)
    btn->vars.pressed = btn->gpio.Active_High ^ pin_state;

    if (btn->vars.pressed) {
        // Button is pressed
        btn->onPress();        // Safe to call (defaults to empty)

        // Handle hold event
        if (btn->vars.hold_enable && btn->vars.hold) {
            btn->vars.hold = 0;
            btn->vars.counter = 0;
            btn->vars.pressed = 0;
            btn->onHold();     // Safe to call
        }
    } else {
        // Button is released
        btn->vars.counter = 0; // Reset hold counter
        btn->onRelease();      // Safe to call
    }
}

// ==================================================================
// buttonIncrementCounter()
// Call from periodic timer ISR (e.g., SysTick at 1kHz)
// Handles debouncing + hold timing
// ==================================================================
inline void buttonIncrementCounter(ButtonVariables_t *btn) {
    if (btn->input) {
        // Debouncing in progress
        if (++btn->debounce >= BUTTON_DEBOUNCE_TIME) {
            btn->change = 1;   // Event ready to be processed
            btn->input = 0;    // Clear EXTI input flag
            btn->debounce = 0; // Reset debounce counter
        }
    }

    // Hold detection
    if (btn->hold_enable && btn->pressed && ++btn->counter >= BUTTON_HOLD_TIME) {
        btn->change = 1;       // Trigger event
        btn->hold = 1;         // Mark hold detected
    }
}

// ==================================================================
// buttonInput()
// Call from EXTI ISR when GPIO pin changes
// Marks input for debounce handling
// ==================================================================
inline void buttonInput(ButtonVariables_t *btn) {
    btn->debounce = 0;  // Reset debounce
    btn->input = 1;     // Mark state change
}
