#ifndef _MSG_QUEUE_
#define _MSG_QUEUE_

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stddef.h>
#include <stdbool.h>

#include "dac.h"
#include "config.h"
#include "camera.h"
#include "acquisition.h"
#include "process_image.h"

#define MSGQ_ERROR	(-1)
#define MSGQ_SUCCESS	(0)

#define BUFFER_SIZE	(1600)

typedef struct 
{
    long mtype;
    struct Acq_data 
    {
	unsigned char buffer[BUFFER_SIZE];
	//unsigned char *buffer;
	CAM_set Cam_set;
	ACQ_set Acq_set;
	COG_tx	Cog_tx;
	DAC_tx	Dac_tx;	

    }Acq_data;

}ACQ_msg;

typedef struct
{
    long mtype;
    struct Gui_data
    {
	COG_rx 	Cog_rx;
	CAM_set Cam_set;
	ACQ_set Acq_set;

    }Gui_data;

}GUI_msg;

int tx_acq_msg(ACQ_msg *Message, int msgq_id);
int rx_acq_msg(ACQ_msg *Message, int msgq_id);

int tx_gui_msg(GUI_msg *Message, int msgq_id);
int rx_gui_msg(GUI_msg *Message, int msgq_id);

int init_msg_queue(const char *filename);

#endif
