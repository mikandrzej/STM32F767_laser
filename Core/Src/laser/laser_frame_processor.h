#ifndef __LASER_FRAME_PROCESSOR_H__
#define __LASER_FRAME_PROCESSOR_H__

#include "../global_defines.h"
#include "laser_point.h"

extern err_t laser_frame_processor_add_point(laser_point_s *point);
extern err_t laser_frame_processor_add_interp_point(laser_point_s *point);
extern int32_t laser_frame_processor_enqueue(void);

#endif