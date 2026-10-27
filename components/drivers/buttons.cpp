#include "buttons.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
 
static volatile bool s_pressed = false;
 
void button_init(void)
{
    gpio_reset_pin(BUTTON_GPIO);
    gpio_set_direction(BUTTON_GPIO, GPIO_MODE_INPUT);
    gpio_set_pull_mode(BUTTON_GPIO, GPIO_PULLUP_ONLY);
}
 
bool button_is_pressed(void)
{
    return s_pressed;
}
 
void button_task(void *pvParameter)
{
    bool candidate = false;
    int  count = 0;
 
    while (true) {
        bool raw = (gpio_get_level(BUTTON_GPIO) == 0);  // active low
 
        if (raw == candidate) {
            if (count < BUTTON_DEBOUNCE_N) {
                count++;
            }
            if (count >= BUTTON_DEBOUNCE_N) {
                s_pressed = candidate;
            }
        } else {
            candidate = raw;
            count = 1;
        }
 
        vTaskDelay(pdMS_TO_TICKS(BUTTON_POLL_MS));
    }
}