#ifndef _ACQUISITION_H_
#define _ACQUISITION_H_

#include "config.h"

typedef enum
{
    OFF,
    ON
 
}State;

typedef struct
{
    ushort_value_t data_tx_rate;
    State loop_state;
 
}ACQ_set;
  


#endif
