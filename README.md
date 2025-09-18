# Button_library
 
# Button Handling Module

This module provides functionality to handle button press events, including debouncing and detecting single press, double press, and hold events.

## Files

- `button.c`: Implementation of button handling functions.
- `button.h`: Header file containing the declarations and definitions for the button handling module.

## Defines
```c
#define HOLD
#define SINGLE
```
Uncomment any of these two to change how button inputs are handled

## Functions

### `buttonCallback`

```c
uint8_t buttonCallback(Button *btn);
```

Handles button press events and detects different types of presses. This function should be called in the appropriate interrupt or polling routine.

**Parameters:**
- `btn`: Pointer to the `Button` structure.

**Returns:**
- `0`: No event detected (debounce period).
- `1`: Hold detected.
- `2`: Single press detected.
- `3`: Press detected.
- `4`: release detected.


### `buttonIncrementCounter`

```c
void buttonIncrementCounter(Button *btn);
```

Call this function in the timer interrupt callback. It increments the button counter and checks for hold events.

**Parameters:**
- `btn`: Pointer to the `Button` structure.

### `buttonInput`

```c
void buttonInput(Button *btn);
```

Call this function in the external interrupt callback. It sets the button input state for debouncing.

**Parameters:**
- `btn`: Pointer to the `Button` structure.

## Structures

### `Button`

The `Button` structure should be defined in `button.h` and contains the following fields:

- `GPIO_Port`: GPIO port of the button.
- `GPIO_Pin`: GPIO pin of the button.
- `Active_High`: Indicates if the button is active high.
- `pressed`: Indicates if the button is currently pressed.
- `change`: Indicates if there is a change in the button state.
- `input`: Input state for debouncing.
- `debounce`: Debounce counter.
- `counter`: Counter for hold detection.
- `hold`: Indicates if the button is held.
- `press_count`: Counter for press detection.

## Usage

1. Include `button.h` in your source file.
2. Initialize the `Button` structure with the appropriate GPIO port, pin, and active high state.
3. Call `buttonInit` to initialize button
3. Call `buttonInput` in the external interrupt callback.
4. Call `buttonIncrementCounter` in the timer interrupt callback.
5. Call `buttonCallback` in main function to handle button events.

## Example

```c
#include "button.h"

Button myButton = {
    .GPIO_Port = GPIOA,
    .GPIO_Pin = GPIO_PIN_0,
    .Active_High = BUTTON_ACTIVE_HIGH,
};


void main() {
    while(1) {
        if (myButton.change) buttonCallback(myButton);
    }
}

void EXTI0_IRQHandler(void) {
    buttonInput(&myButton);
}

void TIM2_IRQHandler(void) {
    buttonIncrementCounter(&myButton);
}
```