#ifndef __LASER_POINT_H__
#define __LASER_POINT_H__

#include <stdint.h>

typedef struct {
    int16_t x;
    int16_t y;
    uint16_t r : 5;
    uint16_t g : 5;
    uint16_t b : 5;
    uint16_t on : 1;
}laser_point_s;

#endif