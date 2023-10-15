#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "../../Common/msg_queue.h"
#include "../../Common/msg_queue.c"

#define NUMBER_OF_ARGUMENTS (11)

int main(int argc, char** argv)
{
    if (argc != NUMBER_OF_ARGUMENTS)
    {
	return -1;
    }
    
    int msgq_id;
    int tx_status;
    
    GUI_msg Gui_msg;

    const char *file_name_gui = "RX_TX_data/set_param.c";

    sscanf(argv[1], "%f",  &Gui_msg.Gui_data.Cog_rx.cog_x_ref.value);
    sscanf(argv[2], "%f",  &Gui_msg.Gui_data.Cog_rx.cog_y_ref.value);
    sscanf(argv[3], "%f",  &Gui_msg.Gui_data.Cog_rx.gain.value);
    sscanf(argv[4], "%hd", &Gui_msg.Gui_data.Cam_set.frame_w.value);
    sscanf(argv[5], "%hd", &Gui_msg.Gui_data.Cam_set.frame_h.value);
    sscanf(argv[6], "%hd", &Gui_msg.Gui_data.Cam_set.offset_x.value);
    sscanf(argv[7], "%hd", &Gui_msg.Gui_data.Cam_set.offset_y.value);
    sscanf(argv[8], "%hd", &Gui_msg.Gui_data.Cam_set.expos_time.value);
    sscanf(argv[9], "%hd", &Gui_msg.Gui_data.Acq_set.data_tx_rate.value);
    
    Gui_msg.Gui_data.Acq_set.loop_state = ON;
    if(strcmp(argv[10], "0") == 0)
    {
	Gui_msg.Gui_data.Acq_set.loop_state = OFF;
    }
       
    msgq_id = init_msg_queue(file_name_gui);
    if (msgq_id == MSGQ_ERROR)
    {
        return MSGQ_ERROR;
    }
    
    tx_status = tx_gui_msg(&Gui_msg, msgq_id);
    if (tx_status == MSGQ_ERROR)
    {
	return MSGQ_ERROR;
    }

    return 0;
}
