
#include "laser_point.h"
#include "laser_frame.h"
#include "laser_frame_queue.h"
#include "laser_config.h"

#include "laser_irq.h"

static laser_frame_s *actual_frame;
static uint16_t actual_point = 0u;

laser_point_s *laser_irq_get_laser_point(void)
{
    if(NULL == actual_frame)
    {
        actual_frame = laser_frame_queue_get_frame();
        if(NULL == actual_frame)
        {
            return NULL;
        }
    }
    laser_point_s *point = &actual_frame->point[actual_point];
    actual_point++;
    if(actual_point >= POINTS_PER_FRAME)
    {
        if(ERR_OK == laser_frame_queue_is_next_frame())
        {
            laser_frame_queue_dequeue();
            actual_frame = laser_frame_queue_get_frame();
        }
        actual_point = 0u;
    }

    return point;
}
