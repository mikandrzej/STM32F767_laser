#ifndef __LASER_FRAME_QUEUE_H__
#define __LASER_FRAME_QUEUE_H__

#include "../global_defines.h"
#include "laser_frame.h"

extern laser_frame_s* laser_frame_queue_get_empty(void);
extern err_t laser_frame_queue_enqueue(void);
extern laser_frame_s* laser_frame_queue_get_frame(void);
extern err_t laser_frame_queue_dequeue(void);
extern err_t laser_frame_queue_is_next_frame(void);

#endif