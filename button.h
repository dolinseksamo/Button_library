/*
 * button.h
 *
 *  Created on: Mar 5, 2025
 *      Author: samod
 */

/*
 * button.h
 *
 *  Created on: Mar 5, 2025
 *      Author: samod
 *
 *  Button handling library with:
 *   - Debouncing
 *   - Press / Release events
 *   - Optional Hold detection
 *   - Optional user-defined callbacks (safe defaults provided)
 */

#ifndef INC_BUTTON_H_
#define INC_BUTTON_H_

#include "main.h"

// ======================== User Config ========================

// Debounce filter time in timer ticks
// Example: if timer = 1kHz, 10 ticks = 10 ms debounce
#define BUTTON_DEBOUNCE_TIME 10      

// Hold detection time in timer ticks
// Example: if timer = 1kHz, 1000 ticks = 1 second hold
#define BUTTON_HOLD_TIME     1000    

// ======================== Event Codes ========================

#define BUTTON_NOTHING   0   // No event
#define BUTTON_PRESSED   1   // Button was pressed (down)
#define BUTTON_RELEASED  2   // Button was released (up)
#define BUTTON_HOLD      3   // Button held for BUTTON_HOLD_TIME

// ======================== Data Types ========================

// Hardware configuration for one button
typedef struct {
    GPIO_TypeDef *GPIO_Port;  // Port, e.g. GPIOA, GPIOB
    uint16_t GPIO_Pin;        // Pin number, e.g. GPIO_PIN_0
    uint8_t Active_High;      // 0 = active-high button, 1 = active-low button
} ButtonGpio_t;

// Runtime state variables (internal library use)
typedef struct {
    uint8_t hold_enable;      // Enable/disable hold detection

    volatile uint8_t hold;    // Flag: hold event detected
    volatile uint8_t input;   // Flag: EXTI detected state change
    volatile uint8_t debounce;// Debounce counter
    volatile uint8_t change;  // Flag: event ready to process
    volatile uint8_t pressed; // Current button state (1 = pressed)
    volatile uint16_t counter;// Counter used for hold detection
} ButtonVariables_t;

// Main button object: GPIO config + state + callbacks
typedef struct {
    ButtonGpio_t gpio;        // GPIO settings
    ButtonVariables_t vars;   // Runtime state

    // User callbacks (always safe to call — default to empty functions)
    void (*onPress)(void);    
    void (*onRelease)(void);  
    void (*onHold)(void);     
} Button_t;

// ======================== API Functions ========================

// Initialize button handle with GPIO + options
void buttonInit(Button_t *btn, GPIO_TypeDef *port, uint16_t pin, uint8_t active_high, uint8_t hold_enable);

// Process button state and run callbacks
// Call this when (btn->vars.change) is set
uint8_t buttonCallback(Button_t *btn);

// Run in timer ISR → updates debounce + hold logic
inline void buttonIncrementCounter(ButtonVariables_t *btn);

// Run in EXTI ISR → marks button input as changed
inline void buttonInput(ButtonVariables_t *btn);

#endif /* INC_BUTTON_H_ */


/* Example usage of the API functions:

#define BUTTON_NUMBER 4

Button_t buttons[BUTTON_NUMBER];   // Array of button handles


int main(void) {
 // === Assign custom handlers ===
    buttons[0].onPress   = button1Pressed;
    buttons[0].onRelease = button1Released;
    buttons[0].onHold    = button1Hold;

    buttons[1].onPress   = button2Pressed;
    buttons[1].onRelease = button2Released;
    buttons[1].onHold    = button2Hold;

    buttons[2].onPress   = button3Pressed;
    buttons[2].onRelease = button3Released;
    // no hold assigned → defaults to empty handler

    buttons[3].onPress   = button4Pressed;
    buttons[3].onRelease = button4Released;
    // no hold assigned → defaults to empty handler

    // === Main loop ===
    while (1) {
        for (int i = 0; i < BUTTON_NUMBER; i++) {
            if (buttons[i].vars.change) {
                buttonCallback(&buttons[i]);  
                // Callbacks are executed inside buttonCallback()
            }
        }
    }
}

// === Timer ISR (e.g. SysTick at 1kHz) ===
void HAL_SYSTICK_Callback(void) {
    for (int i = 0; i < BUTTON_NUMBER; i++) {
        buttonIncrementCounter(&buttons[i].vars);
    }
}

// === EXTI ISR (button pin change) ===
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    for (int i = 0; i < BUTTON_NUMBER; i++) {
        if (GPIO_Pin == buttons[i].gpio.GPIO_Pin) {
            buttonInput(&buttons[i].vars);
        }
    }
}
*/