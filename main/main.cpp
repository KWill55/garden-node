#include <stdio.h>
#include <iostream>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "blink.h"


extern "C" void app_main(void)
{
    printf("Initializing Plant Node...\n");

    blink_init();
    BaseType_t result = xTaskCreate(&blink_task, "blink_task", 2048, NULL, 5, NULL);
    if (result != pdPASS) {
        printf("Failed to create blink_task!\n");
    }

}