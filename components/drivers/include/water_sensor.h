#pragma once
#include <cstdint>
#include <stdbool.h>
#include "esp_err.h"
#include "esp_idf_version.h"
#include "esp_adc/adc_oneshot.h"

#ifdef __cplusplus
extern "C" {
#endif

// S pin -> GPIO34 (ADC1 channel 6, input-only). + -> 3.3V, - -> GND.
#define WATER_ADC_UNIT      ADC_UNIT_1
#define WATER_ADC_CHANNEL   ADC_CHANNEL_6

// ~0-3.1V input range. DB_11 was renamed DB_12 in IDF 5.2.
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 2, 0)
#define WATER_ADC_ATTEN     ADC_ATTEN_DB_12
#else
#define WATER_ADC_ATTEN     ADC_ATTEN_DB_11
#endif

#define WATER_SAMPLES       16     // readings averaged per update
#define WATER_POLL_MS       100

// Calibration: measure these on your sensor and edit.
// RAW_DRY = reading in air, RAW_WET = reading when submerged to the line you call "full".
#define WATER_RAW_DRY       0
#define WATER_RAW_WET       400

esp_err_t water_sensor_init(void);
void water_sensor_task(void *pvParameter);

int water_sensor_get_raw(void);       // 0-4095, averaged
int water_sensor_get_percent(void);   // 0-100, from the calibration above

#ifdef __cplusplus
}
#endif