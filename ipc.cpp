/*
Group B
Author: Richard Powers
Email: richard.w.powers@okstate.edu
Date: 4/12/2025

Description: 
*/

// Create shared memory

// Create message queues

// Store the mutexes and semaphores, set the values to match specified capacities

#include "ipc.hpp"

int requestQueueId = -1;
int responseQueueId = -1;
msg_request message;

// TODO: initialize resource allocation graph and functions

int ipc_setup() {
    // Create paths for shared memory and message queues
    std::ofstream(shm_key_path).close();
    std::ofstream(mq_request_key_path).close();
    std::ofstream(mq_response_key_path).close();

    // Keys for shared memory and queues
    key_t key_mem = ftok(shm_key_path, 'M');
    key_t key_req = ftok(mq_request_key_path, 'R');
    key_t key_res = ftok(mq_response_key_path, 'S');

    if (key_req == -1 || key_res == -1 || key_mem == -1) {
        perror("ftok");
        return -1;
    }

    // Creates shared memory
    int shmid = shmget(key_mem, SHARED_MEMORY_SIZE, 0666 | IPC_CREAT);
    if (shmid == -1){
        perror("shmget (Create)");
        return -1;
    }

    // Creates request and response message queues
    requestQueueId = msgget(key_req, 0666 | IPC_CREAT);
    responseQueueId = msgget(key_res, 0666 | IPC_CREAT);

    if (requestQueueId == -1 || responseQueueId == -1) {
        perror("msgget (Create)");
        return -1;
    }

    return 0;
}

// Sends message to the queue
int send_msg(int msgid, const msg_request& msg) {
    return msgsnd(msgid, &msg, sizeof(msg_request) - sizeof(long), 0);
}

// Receives message from the queue
int receive_msg(int msgid, msg_request& msg, long mtype) {
    return msgrcv(msgid, &msg, sizeof(msg_request) - sizeof(long), mtype, 0);
}

int clear_resources() {
    // Clear shared memory resources
    shmid = shmget(key_mem, SHARED_MEMORY_SIZE, IPC_RMID);

    if (shmid == -1) {
        perror ("shmget (Remove)");
        return -1;
    }

    // Clear message queue resources
    requestQueueId = msgctl(key_req, IPC_RMID, &message);
    responseQueueId = msgctl(key_res, IPC_RMID, &message);

    if (requestQueueId == -1) {
        perror("msgctl (Request removal)");
        return -1;
    }
    
    if (responseQueueId == -1) {
        perror("msgctl (Response removal)");
        return -1;
    }
    
}
