idf_component_register(
    SRCS "oled_text.cpp" "water_sensor.cpp" "buttons.cpp" "oled.cpp" "blink.cpp"
    INCLUDE_DIRS "include"
    REQUIRES esp_adc
    PRIV_REQUIRES driver
)