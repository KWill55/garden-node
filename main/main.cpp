#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "blink.h"
#include "buttons.h"
#include "oled.h"
#include "oled_text.h"
#include "water_sensor.h"

// Layout (128x64):
//   top-left   : "WATER LEVEL" label
//   left       : big percent value
//   bottom-left: raw ADC reading
//   right      : small button circle (outline = released, filled = pressed) + "BTN" label
static void display_task(void *pvParameter)
{
    int  last_raw = -1;
    int  last_pct = -1;
    bool last_pressed = !button_is_pressed();  // force first draw

    char buf[24];

    while (true) {
        bool pressed = button_is_pressed();
        int  raw = water_sensor_get_raw();
        int  pct = water_sensor_get_percent();

        if (pressed != last_pressed || raw != last_raw || pct != last_pct) {
            last_pressed = pressed;
            last_raw = raw;
            last_pct = pct;

            oled_clear();

            oled_draw_string(0, 0, "WATER LEVEL", 1);

            snprintf(buf, sizeof(buf), "%d%%", pct);
            oled_draw_string(0, 16, buf, 3);

            snprintf(buf, sizeof(buf), "RAW: %d", raw);
            oled_draw_string(0, 50, buf, 1);

            const int cx = 108, cy = 30, r = 10;
            if (pressed) {
                oled_fill_circle(cx, cy, r, true);
            } else {
                oled_draw_circle(cx, cy, r, true);
            }
            oled_draw_string(96, 46, "BTN", 1);

            oled_flush();
        }
        vTaskDelay(pdMS_TO_TICKS(30));
    }
}

extern "C" void app_main(void)
{
    printf("Initializing Plant Node...\n");

    blink_init();
    button_init();

    if (oled_init() != ESP_OK) {
        printf("OLED init failed!\n");
    }
    if (water_sensor_init() != ESP_OK) {
        printf("Water sensor init failed!\n");
    }

    if (xTaskCreate(&blink_task, "blink_task", 2048, NULL, 5, NULL) != pdPASS) {
        printf("Failed to create blink_task!\n");
    }
    if (xTaskCreate(&button_task, "button_task", 2048, NULL, 5, NULL) != pdPASS) {
        printf("Failed to create button_task!\n");
    }
    if (xTaskCreate(&water_sensor_task, "water_task", 3072, NULL, 5, NULL) != pdPASS) {
        printf("Failed to create water_task!\n");
    }
    if (xTaskCreate(&display_task, "display_task", 4096, NULL, 4, NULL) != pdPASS) {
        printf("Failed to create display_task!\n");
    }
}