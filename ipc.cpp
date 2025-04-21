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

int shmid = -1; // Shared memory ID
key_t key_mem = -1; // Key: shared memory
key_t key_req = -1; // Key: request queue
key_t key_res = -1; // Key: response queue

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

    // Clear all messages in the request queue
    msg_request temp_msg;
    while (msgrcv(requestQueueId, &temp_msg, sizeof(temp_msg) - sizeof(long), 0, IPC_NOWAIT) != -1) {
        std::cout << "ipc.cpp: Cleared a message from the request queue.\n";
    }
    if (errno != ENOMSG) {
        perror("ipc.cpp: Error while clearing request queue");
        return -1;
    }
    std::cout << "ipc.cpp: Request queue is now empty.\n";

    // Clear all messages in the response queue
    while (msgrcv(responseQueueId, &temp_msg, sizeof(temp_msg) - sizeof(long), 0, IPC_NOWAIT) != -1) {
        std::cout << "ipc.cpp: Cleared a message from the response queue.\n";
    }
    if (errno != ENOMSG) {
        perror("ipc.cpp: Error while clearing response queue");
        return -1;
    }

    std::cout << "ipc.cpp: Request Queue ID: " << requestQueueId << std::endl;
    std::cout << "ipc.cpp: Response Queue ID: " << responseQueueId << std::endl;

    return 0;
}

// Sends message to the queue
int send_msg(int msgid, const msg_request& msg) {
    int ret = msgsnd(msgid, &msg, sizeof(msg_request) - sizeof(long), 0);
    // Check if message was sent successfully
    if (ret == -1) {
        perror("ipc.cpp: msgsnd failed");
    }
    return ret;
}

// Receives message from the queue
int receive_msg(int msgid, msg_request& msg, long mtype) {
    int ret = msgrcv(msgid, &msg, sizeof(msg_request) - sizeof(long), mtype, 0);
    // Check if message was received successfully
    if (ret == -1) {
        perror("ipc.cpp: msgsnd failed");
    }
    return ret;
}

int clear_resources() {
    // Clear shared memory resources
    shmid = shmget(key_mem, SHARED_MEMORY_SIZE, IPC_RMID);

    if (shmid == -1) {
        perror ("shmget (Remove)");
        return -1;
    }

    // Clear message queue resources
    requestQueueId = msgctl(key_req, IPC_RMID, nullptr);
    responseQueueId = msgctl(key_res, IPC_RMID, nullptr);

    if (requestQueueId == -1) {
        perror("msgctl (Request removal)");
        return -1;
    }
    
    if (responseQueueId == -1) {
        perror("msgctl (Response removal)");
        return -1;
    }
    
    return 0;
}
