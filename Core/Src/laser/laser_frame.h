#ifndef __LASER_FRAME_H__
#define __LASER_FRAME_H__

#include <stdint.h>

#include "laser_config.h"
#include "laser_point.h"

typedef struct {
    laser_point_s point[POINTS_PER_FRAME];
} laser_frame_s;

#endif