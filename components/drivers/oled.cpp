#include "oled.h"
#include <string.h>
#include "driver/i2c.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
 
static const char *TAG = "oled";
 
// 1 bit per pixel, organised in 8 pages of 128 columns (SSD1306 native layout)
static uint8_t s_fb[OLED_WIDTH * OLED_HEIGHT / 8];
 
static esp_err_t oled_write_cmds(const uint8_t *cmds, size_t len)
{
    i2c_cmd_handle_t h = i2c_cmd_link_create();
    i2c_master_start(h);
    i2c_master_write_byte(h, (OLED_I2C_ADDR << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(h, 0x00, true);              // control byte: commands
    i2c_master_write(h, cmds, len, true);
    i2c_master_stop(h);
    esp_err_t err = i2c_master_cmd_begin(OLED_I2C_PORT, h, pdMS_TO_TICKS(100));
    i2c_cmd_link_delete(h);
    return err;
}
 
esp_err_t oled_init(void)
{
    i2c_config_t conf = {};
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = OLED_SDA_GPIO;
    conf.scl_io_num = OLED_SCL_GPIO;
    conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
    conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
    conf.master.clk_speed = OLED_I2C_FREQ_HZ;
 
    esp_err_t err = i2c_param_config(OLED_I2C_PORT, &conf);
    if (err != ESP_OK) return err;
    err = i2c_driver_install(OLED_I2C_PORT, conf.mode, 0, 0, 0);
    if (err != ESP_OK) return err;
 
    vTaskDelay(pdMS_TO_TICKS(100));  // let the panel power up
 
    static const uint8_t init_seq[] = {
        0xAE,             // display off
        0xD5, 0x80,       // clock divide / oscillator
        0xA8, 0x3F,       // multiplex ratio (64 rows)
        0xD3, 0x00,       // display offset
        0x40,             // start line = 0
        0x8D, 0x14,       // charge pump on
        0x20, 0x00,       // horizontal addressing mode
        0xA1,             // segment remap (flip X)
        0xC8,             // COM scan direction remapped (flip Y)
        0xDA, 0x12,       // COM pins config
        0x81, 0xCF,       // contrast
        0xD9, 0xF1,       // pre-charge
        0xDB, 0x40,       // VCOMH deselect
        0xA4,             // follow RAM contents
        0xA6,             // normal (not inverted)
        0xAF              // display on
    };
    err = oled_write_cmds(init_seq, sizeof(init_seq));
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Init failed (%s). Check wiring and I2C address.", esp_err_to_name(err));
        return err;
    }
 
    oled_clear();
    return oled_flush();
}
 
void oled_clear(void)
{
    memset(s_fb, 0, sizeof(s_fb));
}
 
void oled_draw_pixel(int x, int y, bool on)
{
    if (x < 0 || x >= OLED_WIDTH || y < 0 || y >= OLED_HEIGHT) return;
    uint8_t mask = 1 << (y & 7);
    uint8_t &b = s_fb[(y >> 3) * OLED_WIDTH + x];
    if (on) b |= mask; else b &= ~mask;
}
 
void oled_fill_circle(int cx, int cy, int r, bool on)
{
    for (int dy = -r; dy <= r; dy++) {
        for (int dx = -r; dx <= r; dx++) {
            if (dx * dx + dy * dy <= r * r) {
                oled_draw_pixel(cx + dx, cy + dy, on);
            }
        }
    }
}
 
void oled_draw_circle(int cx, int cy, int r, bool on)
{
    int inner = (r - 1) * (r - 1);
    for (int dy = -r; dy <= r; dy++) {
        for (int dx = -r; dx <= r; dx++) {
            int d2 = dx * dx + dy * dy;
            if (d2 <= r * r && d2 >= inner) {
                oled_draw_pixel(cx + dx, cy + dy, on);
            }
        }
    }
}
 
esp_err_t oled_flush(void)
{
    static const uint8_t addr_cmds[] = {
        0x21, 0, OLED_WIDTH - 1,          // column range
        0x22, 0, (OLED_HEIGHT / 8) - 1    // page range
    };
    esp_err_t err = oled_write_cmds(addr_cmds, sizeof(addr_cmds));
    if (err != ESP_OK) return err;
 
    // Send one page (128 bytes) per transaction
    for (int page = 0; page < OLED_HEIGHT / 8; page++) {
        i2c_cmd_handle_t h = i2c_cmd_link_create();
        i2c_master_start(h);
        i2c_master_write_byte(h, (OLED_I2C_ADDR << 1) | I2C_MASTER_WRITE, true);
        i2c_master_write_byte(h, 0x40, true);          // control byte: data
        i2c_master_write(h, &s_fb[page * OLED_WIDTH], OLED_WIDTH, true);
        i2c_master_stop(h);
        err = i2c_master_cmd_begin(OLED_I2C_PORT, h, pdMS_TO_TICKS(100));
        i2c_cmd_link_delete(h);
        if (err != ESP_OK) return err;
    }
    return ESP_OK;
}