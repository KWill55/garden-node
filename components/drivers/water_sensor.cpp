#include "water_sensor.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

static const char *TAG = "water";

static adc_oneshot_unit_handle_t s_adc = nullptr;
static volatile int s_raw = 0;
static volatile int s_percent = 0;

esp_err_t water_sensor_init(void)
{
    adc_oneshot_unit_init_cfg_t unit_cfg = {};
    unit_cfg.unit_id = WATER_ADC_UNIT;
    esp_err_t err = adc_oneshot_new_unit(&unit_cfg, &s_adc);
    if (err != ESP_OK) return err;

    adc_oneshot_chan_cfg_t chan_cfg = {};
    chan_cfg.atten = WATER_ADC_ATTEN;
    chan_cfg.bitwidth = ADC_BITWIDTH_DEFAULT;  // 12-bit on ESP32
    return adc_oneshot_config_channel(s_adc, WATER_ADC_CHANNEL, &chan_cfg);
}

int water_sensor_get_raw(void)     { return s_raw; }
int water_sensor_get_percent(void) { return s_percent; }

void water_sensor_task(void *pvParameter)
{
    while (true) {
        int sum = 0;
        int good = 0;
        for (int i = 0; i < WATER_SAMPLES; i++) {
            int v = 0;
            if (adc_oneshot_read(s_adc, WATER_ADC_CHANNEL, &v) == ESP_OK) {
                sum += v;
                good++;
            }
        }

        if (good > 0) {
            int raw = sum / good;
            int pct = (raw - WATER_RAW_DRY) * 100 / (WATER_RAW_WET - WATER_RAW_DRY);
            if (pct < 0)   pct = 0;
            if (pct > 100) pct = 100;
            s_raw = raw;
            s_percent = pct;
        } else {
            ESP_LOGW(TAG, "ADC read failed");
        }

        vTaskDelay(pdMS_TO_TICKS(WATER_POLL_MS));
    }
}