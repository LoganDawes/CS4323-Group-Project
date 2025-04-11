/*
Group B
Author: Richard Powers
Email: richard.w.powers@okstate.edu
Date: 4/1/2025

Description: 
*/

// Create shared memory

// Create message queues

// Store the mutexes and semaphores, set the values to match specified capacities

#include "ipc.hpp"
#include <iostream>

int requestQueueId = -1;
int responseQueueId = -1;
msg_request message;

// TODO: initialize resource allocation graph and functions

int ipc_setup() {
    key_t key_mem = ftok(shm_key_path, "M");
    key_t key_req = ftok(mq_request_key_path, 'R');
    key_t key_res = ftok(mq_response_key_path, 'S');

    if (key_req == -1 || key_res == -1 || key_mem == -1) {
        perror("ftok");
        return -1;
    }

    int shmid = shmget(key_mem, SHARED_MEMORY_SIZE, create ? IPC_CREAT | 0666 : 0666);
    if (shmid == -1){
        perror("shmget");
        return -1
    }

    requestQueueId = msgget(key_req, 0666 | IPC_CREAT);
    responseQueueId = msgget(key_res, 0666 | IPC_CREAT);

    if (requestQueueId == -1 || responseQueueId == -1) {
        perror("msgget");
        return -1;
    }

    return 0;
}

class ResourceAllocationGraph {
    private:
        unordered_map<string, Intersection*> intersectionMap;
    
    public:
        void acquire(const string& id, Train* train) {
            if (intersectionMap.count(id)) {
                intersectionMap[id]->acquire(train);
            }
        }

        void release(const string& id, Train* train) {
            if (intersectionMap.count(id)) {
                intersectionMap[id]->release(train);
            }
        }

        void printGraph() {
            for (const auto& pair : intersectionMap) {
                const string& id = pair.first;
                Intersection* intersection = pair.second;
                cout << id << is_mutex << capacity << lock_state << trains_in_intersection;
            }
        }
}

int send_msg(int msgid, const msg_request& msg) {
    return msgsnd(msgid, &msg, sizeof(msg_request) - sizeof(long), 0);
}

int receive_msg(int msgid, msg_request& msg, long mtype) {
    return msgrcv(msgid, &msg, sizeof(msg_request) - sizeof(long), mtype, 0);
}
