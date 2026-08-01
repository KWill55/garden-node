#include "blink.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver_config.h"

void blink_init(void)
{
    gpio_reset_pin(BLINK_GPIO);
    gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT);
}

void blink_task(void *pvParameter)
{
    bool led_on = false;

    while (true) {
        led_on = !led_on;
        gpio_set_level(BLINK_GPIO, led_on);
        vTaskDelay(pdMS_TO_TICKS(BLINK_PERIOD)); 
    }
}