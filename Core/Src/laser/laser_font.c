#include "../global_defines.h"
#include <string.h>

#include "laser_font.h"
#include "laser_font_arial.h"
#include "laser_point.h"
#include "laser_frame_processor.h"

int32_t laser_font_print_text(char *txt, int32_t x, int32_t y, int16_t divider, uint8_t r, uint8_t g, uint8_t b)
{
    int32_t act_x = x;
    int32_t act_y = y;
    int32_t status = ERR_OK;

    for(int k = 0; k < strlen(txt); k++)
    {
        laser_font_s *letter = laser_font_arial[(int)txt[k]];
        laser_font_meta_s *meta = &laser_font_arial_meta[(int)txt[k]];

        for(uint16_t p = 0; p < meta->points; p++)
        {
            laser_point_s point = {
                .x = act_x + (letter[p].x / divider),
                .y = act_y + (letter[p].y / divider),
                .r = r,
                .g = g,
                .b = b,
                .on = (letter[p].blanking==0 ? 1 : 0)
            };
            
            status = laser_frame_processor_add_interp_point(&point);
            if(status < 0)
            {
                return status;
            }
        }
        act_x += ((meta->width + 1000) / divider);
    }
    return status;
}