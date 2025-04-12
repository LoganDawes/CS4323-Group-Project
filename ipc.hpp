#ifndef IPC_HPP
#define IPC_HPP

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <string>
#include <string.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/msg.h>
#include <iostream>
#include <unordered_map>
#include "parsing.hpp"

#define shm_key_path "/tmp/ipc_shm"
#define mq_request_key_path "/tmp/ipc_req"
#define mq_response_key_path "/tmp/ipc_res"
#define SHARED_MEMORY_SIZE sizeof(int)
#define MSG_TYPE_DEFAULT 1

using namespace std;

struct msg_request {
    long mtype;
    char command[10];
    int train_name[20];
    char intersection[50];
};

// IPC request + response id's
extern int requestQueueId;
extern int responseQueueId;

extern msg_request message;

int ipc_setup();

class ResourceAllocationGraph {
    private:
        unordered_map<string, Intersection*> intersectionMap;
    
    public:
        void acquire(const string& id, Train* train);

        void release(const string& id, Train* train);

        void printGraph();
};

int send_msg(int msgid,const msg_request& msg);
int receive_msg(int msgid, msg_request& msg, long mtype = MSG_TYPE_DEFAULT);

#endif
