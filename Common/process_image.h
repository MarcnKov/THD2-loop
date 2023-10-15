#ifndef _PROCESS_IMAGE_
#define _PROCESS_IMAGE_

#include "../VimbaC/Include/VimbaC.h"
#include "config.h"

typedef struct
{
    float_value_t cog_x_ref;
    float_value_t cog_y_ref;

    float_value_t gain;

}COG_rx;

typedef struct
{
    float_value_t cog_x;
    float_value_t cog_y;
    
}COG_tx;

unsigned char compute_COG(VmbFrame_t *frame, COG_rx* Cog_rx, COG_tx* Cog_tx);

int get_pixel(VmbFrame_t *frame, int x, int y);

#endif
