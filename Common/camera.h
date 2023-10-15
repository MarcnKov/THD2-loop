#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "config.h"

#define MATRIX_W (728)
#define MATRIX_H (544)

typedef struct
{
    ushort_value_t frame_w;
    ushort_value_t frame_h;
    ushort_value_t offset_x;
    ushort_value_t offset_y;
    //TO DO: Change the ushort to uint
    //Possible overflow from GUI side
    //Be ware to change get_param and 
    //set_param stdout parameters acordingly
    ushort_value_t expos_time;

}CAM_set;


#endif
