#include "msg_queue.h"

int get_msg_queue(key_t key)
{
    int msgq_id;
    
    msgq_id = msgget(key, 0666 | IPC_CREAT);
    if (msgq_id ==  MSGQ_ERROR)
    {
	return MSGQ_ERROR;
    }
    //printf("Get msg queue\n");
    return msgq_id; 
}

int init_msg_queue(const char *filename)
{
    key_t key;
    int msgq_id;
    key = ftok(filename,'b');
    if (key == MSGQ_ERROR)
    {
	return MSGQ_ERROR;
    }
    //printf("Get file key\n");

    msgq_id = get_msg_queue(key);
    if (msgq_id == MSGQ_ERROR)
    {
	return MSGQ_ERROR;
    }

    return msgq_id;
}

int tx_acq_msg(ACQ_msg *Message, int msgq_id)
{
    int msg_send;
    size_t size;

    size = sizeof Message->Acq_data;
    msg_send = msgsnd(msgq_id, Message, size, IPC_NOWAIT);
    if (msg_send == MSGQ_ERROR)
    {
	return MSGQ_ERROR;
    }
    
    //printf("Message is sent.\n");

    return MSGQ_SUCCESS;
}

int rx_acq_msg(ACQ_msg *Message, int msgq_id)
{
    
    int msg_receive;
    size_t size;
    
    size = sizeof Message->Acq_data;

    msg_receive = msgrcv(msgq_id, Message, size, 0, IPC_NOWAIT);
    if (msg_receive == MSGQ_ERROR)
    {
	return MSGQ_ERROR;
    }
    
    //printf("Message is received.\n");

    return MSGQ_SUCCESS;
}

int tx_gui_msg(GUI_msg *Message, int msgq_id)
{
    int msg_send;
    size_t size;

    size = sizeof Message->Gui_data;
    msg_send = msgsnd(msgq_id, Message, size, IPC_NOWAIT);
    if (msg_send == MSGQ_ERROR)
    {
	return MSGQ_ERROR;
    }
    
    //printf("Message is sent.\n");

    return MSGQ_SUCCESS;
}

int rx_gui_msg(GUI_msg *Message, int msgq_id)
{
    
    int msg_receive;
    size_t size;
    
    size = sizeof Message->Gui_data;

    msg_receive = msgrcv(msgq_id, Message, size, 0, IPC_NOWAIT);
    if (msg_receive == MSGQ_ERROR)
    {
	return MSGQ_ERROR;
    }
    
    //printf("Message is received.\n");

    return MSGQ_SUCCESS;
}

