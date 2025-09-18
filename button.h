/*
 * button.h
 *
 *  Created on: Mar 5, 2025
 *      Author: samod
 */

#ifndef INC_BUTTON_H_
#define INC_BUTTON_H_

#include "main.h"

// === User Configuration ===
#define BUTTON_DEBOUNCE_TIME 10      // Debounce time in timer ticks (example: 1ms tick → 10ms debounce)
#define BUTTON_HOLD_TIME     1000    // Hold time in timer ticks (example: 1ms tick → 1000ms hold)

// === Event Return Codes ===
#define BUTTON_NOTHING   0           // No valid event detected
#define BUTTON_PRESSED   1           // Button pressed (down event)
#define BUTTON_RELEASED  2           // Button released (up event)
#define BUTTON_HOLD      4           // Button held long enough (if hold is enabled)

// === GPIO Configuration Struct ===
typedef struct {
    GPIO_TypeDef *GPIO_Port;         // GPIO port of the button
    uint16_t GPIO_Pin;               // GPIO pin of the button
    uint8_t Active_High;             // 0 = Active High, 1 = Active Low (button logic)
} ButtonGpio_t;

// === Runtime State Variables ===
typedef struct {
    uint8_t hold_enable;             // Enable/disable hold detection (0 = off, 1 = on)

    volatile uint8_t hold;           // Flag: set when hold is detected
    volatile uint8_t input;          // Flag: set when EXTI detects input change
    volatile uint8_t debounce;       // Debounce counter
    volatile uint8_t change;         // Flag: event ready for processing
    volatile uint8_t pressed;        // Current state (1 = pressed, 0 = released)
    volatile uint16_t counter;       // Counter used for hold timing
} ButtonVariables_t;

// === Main Button Object ===
typedef struct {
    ButtonGpio_t gpio;               // Hardware configuration (pin + polarity)
    ButtonVariables_t vars;          // Runtime variables (state, debounce, hold detection)

    void (*onPress)(void);    // Callback for press
    void (*onRelease)(void);  // Callback for release
    void (*onHold)(void);     // Callback for hold
} Button_t;

// === API Functions ===
void buttonInit(Button_t *btn, GPIO_TypeDef *port, uint16_t pin, uint8_t active_high, uint8_t hold_enable);
uint8_t buttonCallback(Button_t *btn);                     // Handles button logic and returns event
inline void buttonIncrementCounter(ButtonVariables_t *btn); // Called in timer ISR → debounce & hold logic
inline void buttonInput(ButtonVariables_t *btn);            // Called in EXTI ISR → marks input change


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

#endif /* INC_BUTTON_H_ */
