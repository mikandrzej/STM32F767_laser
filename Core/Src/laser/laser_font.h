#ifndef __LASER_FONT_H__
#define __LASER_FONT_H__

#include <stdint.h>

typedef struct {
    int16_t x;
    int16_t y;
    uint8_t blanking;
}laser_font_s;

typedef struct {
    uint16_t points;
    int16_t width;
}laser_font_meta_s;

extern int32_t laser_font_print_text(char *txt, int32_t x, int32_t y, int16_t divider, uint8_t r, uint8_t g, uint8_t b);

#endif