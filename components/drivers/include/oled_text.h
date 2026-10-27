#pragma once

#ifdef __cplusplus
extern "C" {
#endif

// Draws text into the oled.h framebuffer (call oled_flush() afterwards).
// Built-in 5x7 font: digits, A-Z (lowercase is shown as uppercase), and  space ! " # $ % & ' ( ) * + , - . / : ; < = > ? @
// Each character cell is (6*scale) wide and (8*scale) tall.
void oled_draw_char(int x, int y, char c, int scale);
void oled_draw_string(int x, int y, const char *s, int scale);

#ifdef __cplusplus
}
#endif