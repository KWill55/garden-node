#pragma once
#include <cstdint>
#include <stdbool.h>
#include "esp_err.h"
 
#ifdef __cplusplus
extern "C" {
#endif
 
// SSD1306 128x64 I2C OLED (the common 4-pin GND/VCC/SCL/SDA module)
#define OLED_WIDTH       128
#define OLED_HEIGHT      64
#define OLED_I2C_ADDR    0x3C        // some modules are 0x3D
#define OLED_I2C_PORT    I2C_NUM_0
#define OLED_SDA_GPIO    GPIO_NUM_21
#define OLED_SCL_GPIO    GPIO_NUM_22
#define OLED_I2C_FREQ_HZ 400000
 
esp_err_t oled_init(void);
 
// All drawing goes into a RAM framebuffer; call oled_flush() to push it to the panel.
void oled_clear(void);
void oled_draw_pixel(int x, int y, bool on);
void oled_fill_circle(int cx, int cy, int r, bool on);
void oled_draw_circle(int cx, int cy, int r, bool on);  // 1px outline
esp_err_t oled_flush(void);
 
#ifdef __cplusplus
}
#endif