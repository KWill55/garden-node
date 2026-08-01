#pragma once
#include <cstdint>  

#ifdef __cplusplus
extern "C" {
#endif

int32_t const BLINK_PERIOD = 500; //in ms  

void blink_init(void);
void blink_task(void *pvParameter);

#ifdef __cplusplus
}
#endif