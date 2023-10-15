#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "../../Common/msg_queue.h"

int main(int argc, char** argv)
{
    
    int i;
    int j;
    int width;	
    int height; 	

    int msgq_id;
    int msgq_size;
    int msg_received;
    int msgq_ctl_status;

    float pixel;
   
    ACQ_msg Acq_msg;
    
    struct msqid_ds Msgq_stats; 
    
    const char *file_name_acq = "RX_TX_data/get_param.c";
    msgq_id = init_msg_queue(file_name_acq);
    if (msgq_id == MSGQ_ERROR)
    {
	return MSGQ_ERROR;
    }
    msgq_ctl_status = msgctl(msgq_id, IPC_STAT, &Msgq_stats);
    if (msgq_ctl_status == MSGQ_SUCCESS)
    {
	msgq_size = Msgq_stats.msg_qnum;
        if (msgq_size == 0)
        {
	    return MSGQ_ERROR;
        }
    }

    msg_received = rx_acq_msg(&Acq_msg, msgq_id);
    if (msg_received == MSGQ_ERROR)
    {
	return MSGQ_ERROR;
    }

    fprintf(stdout,"%d_%f_%f_%f_%f_%d_%d_",
	    Acq_msg.Acq_data.Acq_set.loop_state,
	    Acq_msg.Acq_data.Cog_tx.cog_x.value,
	    Acq_msg.Acq_data.Cog_tx.cog_y.value,
	    Acq_msg.Acq_data.Dac_tx.vltg_avg_x.value,
	    Acq_msg.Acq_data.Dac_tx.vltg_avg_y.value,
	    Acq_msg.Acq_data.Cam_set.expos_time.value,
	    Acq_msg.Acq_data.Acq_set.data_tx_rate.value
	    );
   
    width	= Acq_msg.Acq_data.Cam_set.frame_w.value;
    height 	= Acq_msg.Acq_data.Cam_set.frame_h.value;

    i = 0;    
    while (i < height)
    {
	j = 0;
	while (j < width)
	{
	    if (j > 0)
	    {
		fprintf(stdout,",");
	    }
	    pixel  = (float) Acq_msg.Acq_data.buffer[i*width + j];
	    pixel /= 255.0;
	    fprintf(stdout,"%lf", pixel);
	    j++;
	}
	fprintf(stdout,"\n");
	i++;
    }

    return 0;
}
