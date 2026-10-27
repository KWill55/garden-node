#pragma once
#include <cstdint>
#include <stdbool.h>
 
#ifdef __cplusplus
extern "C" {
#endif
 
// Button wired between this GPIO and GND (internal pull-up is enabled).
// Pressed = pin reads LOW.
#define BUTTON_GPIO        GPIO_NUM_4
#define BUTTON_POLL_MS     10   // sample period
#define BUTTON_DEBOUNCE_N  4    // consecutive identical samples required to accept a change
 
void button_init(void);
void button_task(void *pvParameter);
 
// Debounced state, safe to call from any task.
bool button_is_pressed(void);
 
#ifdef __cplusplus
}
#endif