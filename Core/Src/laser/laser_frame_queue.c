#include "../global_defines.h"
#include "laser_config.h"
#include "laser_frame.h"

#include "laser_frame_queue.h"

#define QUEUE_MODULO_MASK (FRAMES_IN_QUEUE - 1u)

typedef struct {
    laser_frame_s buf[FRAMES_IN_QUEUE];
    uint8_t head;
    uint8_t tail;
} laser_frame_queue_s;

static laser_frame_queue_s laser_frame_queue = {0};

laser_frame_s* laser_frame_queue_get_empty(void)
{
    if(((laser_frame_queue.head + 1u) & QUEUE_MODULO_MASK) == laser_frame_queue.tail)
    {
        return NULL;
    }

    return &laser_frame_queue.buf[laser_frame_queue.head];
}

err_t laser_frame_queue_enqueue(void)
{    
    if(((laser_frame_queue.head + 1u) & QUEUE_MODULO_MASK) == laser_frame_queue.tail)
    {
        return ERR_GENERAL;
    }
    uint8_t head = (laser_frame_queue.head + 1u) & QUEUE_MODULO_MASK;
    laser_frame_queue.head = head;
    return ERR_OK;
}

laser_frame_s* laser_frame_queue_get_frame(void)
{
    if(laser_frame_queue.head == laser_frame_queue.tail)
    {
        return NULL;
    }

    return &laser_frame_queue.buf[laser_frame_queue.tail];
}

err_t laser_frame_queue_dequeue(void)
{
    if(laser_frame_queue.head == laser_frame_queue.tail)
    {
        return ERR_GENERAL;
    }

    uint8_t tail = (laser_frame_queue.tail + 1u) & QUEUE_MODULO_MASK;
    laser_frame_queue.tail = tail;
    return ERR_OK;
}

err_t laser_frame_queue_is_next_frame(void)
{
    if(laser_frame_queue.tail == laser_frame_queue.head)
    {
        return ERR_GENERAL;
    }
    
    uint8_t tail = (laser_frame_queue.tail + 1) & QUEUE_MODULO_MASK;
    if(tail == laser_frame_queue.head)
    {
        return ERR_GENERAL;
    }

    return ERR_OK;
}