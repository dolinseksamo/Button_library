/*
 * button.h
 *
 *  Created on: Mar 5, 2025
 *      Author: samod
 */

#ifndef INC_BUTTON_H_
#define INC_BUTTON_H_

#include "main.h"

//#define HOLD    // Unomment this if you need hold
//#define SINGLE  // Uncomment to detect only button press (if this is not active the function return button pressed or released)

#define BUTTON_DEBOUNCE_TIME 10      // Debounce time in milliseconds
#define BUTTON_HOLD_TIME 1000       // Hold time in milliseconds

#define BUTTON_NOTHING 0

#ifdef HOLD
#define BUTTON_HOLD 1
#endif

#ifdef SINGLE
#define BUTTON_SINGLE 2
#endif

#ifndef SINGLE
#define BUTTON_PRESSED 3    
#define BUTTON_RELEASED 4
#endif

#define BUTTON_ACTIVE_HIGH 0
#define BUTTON_ACTIVE_LOW 1

typedef struct {
    GPIO_TypeDef *GPIO_Port;
    uint16_t GPIO_Pin;
    uint8_t Active_High;  // 0 = Active High, 1 = Active Low

    uint8_t press_count;
    uint8_t hold;
    volatile uint8_t input;
    volatile uint8_t debounce;
    volatile uint8_t change;
    volatile uint8_t pressed;
    volatile uint16_t counter;
} Button;

void buttonInit(Button *btn);
uint8_t buttonCallback(Button *btn);
void buttonIncrementCounter(Button *btn);
void buttonInput(Button *btn);

#endif /* INC_BUTTON_H_ */
