#ifndef IPC_HPP
#define IPC_HPP

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <string.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/msg.h>

#define shm_name "/shared_mem"
#define mq_request_key_path "/tmp/ipc_req"
#define mq_response_key_path "/tmp/ipc_res"
#define SHARED_MEMORY_SIZE sizeof(int)
#define MSG_TYPE_DEFAULT 1

struct msg_request {
    long mtype;
    char command[10];
    int train_name[20];
    char intersection[50];
};

// IPC request + response id's
extern int requestQueueId;
extern int responseQueueId;

// Message struct
extern msg_request message; 

int ipc_setup();

int send_msg(int msgid,const msg_request& msg);
int receive_msg(int msgid, msg_request& msg, long mtype = MSG_TYPE_DEFAULT);

#endif
