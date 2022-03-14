#include "../global_defines.h"

#include "laser_frame.h"
#include "laser_point.h"
#include "laser_frame_queue.h"

#include <string.h>

typedef struct {
    laser_frame_s *act_frame;
    uint16_t act_point;
} laser_frame_processor_s;

laser_frame_processor_s laser_processor = {0};

static err_t prepare_frame_pointer(void)
{
    if(NULL != laser_processor.act_frame)
    {
        return ERR_OK;
    }
    
    laser_processor.act_frame = laser_frame_queue_get_empty();
    if(NULL == laser_processor.act_frame)
    {
        return ERR_GENERAL;
    }
    laser_processor.act_point = 0u;
    return ERR_OK;
}

err_t laser_frame_processor_add_point(laser_point_s *point)
{
    err_t status = prepare_frame_pointer();
    if(status != ERR_OK)
    {
        return status;
    }
    if(laser_processor.act_point >= POINTS_PER_FRAME)
    {
        return ERR_GENERAL;
    }

    memcpy(&laser_processor.act_frame->point[laser_processor.act_point], point, sizeof(laser_point_s));
    laser_processor.act_point++;

    return ERR_OK;
}

err_t laser_frame_processor_add_interp_point(laser_point_s *point)
{
    err_t status;

    status = prepare_frame_pointer();
    if(status != ERR_OK)
    {
        return status;
    }

    laser_point_s *last_point;
    if(laser_processor.act_point > 0)
    {
        last_point = &laser_processor.act_frame->point[laser_processor.act_point - 1u];
    }
    else
    {
        laser_point_s temp_point = {0};
        last_point = &temp_point;
    }
    
    int32_t len_x = (int32_t)point->x - (int32_t)last_point->x;
    if(len_x < 0)
    {
        len_x *= -1;
    }
    int32_t len_y = (int32_t)point->y - (int32_t)last_point->y;
    if(len_y < 0)
    {
        len_y *= -1;
    }


    int16_t parts = 0;
    if(len_x > POINT_TO_POINT_MAX_DIST)
    {
        parts = (len_x / POINT_TO_POINT_MAX_DIST) + 1;
    }
    if(len_y > POINT_TO_POINT_MAX_DIST)
    {
        if(len_y > len_x)
        {
            parts = (len_y / POINT_TO_POINT_MAX_DIST) + 1;
        }
    }

    if(parts > 0)
    {
        int16_t part_len_x = (point->x - last_point->x) / parts;
        int16_t part_len_y = (point->y - last_point->y) / parts;

        int16_t x = last_point->x;
        int16_t y = last_point->y;
        for(int16_t k = 1; k < parts; k++)
        {
            x += part_len_x;
            y += part_len_y;
            laser_point_s new_point;
            memcpy(&new_point, point, sizeof(laser_point_s));
            new_point.x = x;
            new_point.y = y;
            status = laser_frame_processor_add_point(&new_point);
            if(status != ERR_OK)
            {
                return status;
            }
        }
        status = laser_frame_processor_add_point(point);
    }
    else
    {
        status = laser_frame_processor_add_point(point);
    }

    return status;
}

int32_t laser_frame_processor_enqueue(void)
{
    uint16_t recopy_cnt = (POINTS_PER_FRAME / laser_processor.act_point) - 1u;
    size_t recopy_len = laser_processor.act_point * sizeof(laser_point_s);
    uint16_t temp_act_point = laser_processor.act_point;
    for(uint16_t k = 0; k < recopy_cnt; k++)
    {
        memcpy(&laser_processor.act_frame->point[laser_processor.act_point], &laser_processor.act_frame->point[0u], recopy_len);
        laser_processor.act_point += temp_act_point;
    }
    
    size_t len = (POINTS_PER_FRAME - laser_processor.act_point) * sizeof(laser_point_s);
    memset(&laser_processor.act_frame->point[laser_processor.act_point], 0, len);
    err_t status = laser_frame_queue_enqueue();
    laser_processor.act_frame = NULL;
    return status;
}