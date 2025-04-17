/*
Group B
Author: Myron Peoples
Email: myron.peoples@okstate.edu
Date: 4/12/2025
Description: 
*/

#include "train.hpp"
#include "logging.hpp"
#include <thread>
#include <chrono>
#include "ipc.hpp"
#include "logging.hpp"
#include <cstring>


using namespace std;

void train_forking() {
    auto intersections = parseIntersections("intersections.txt");
    auto trains = parseTrains("trains.txt", intersections);

    // Create a vector to store pids for each train's fork
    std::vector<pid_t> train_pids;

    std::vector<Train*> train_ptrs;

    // For every train in trains, create a fork
    for(const auto& train_pair : trains){
        Train* train = train_pair.second;  // Access the Train* from the map
        train_ptrs.push_back(train);

        pid_t pid = fork();
    
        if (pid == 0) {
            std::cout << "train.cpp: " << train->name << " starting its journey!" << std::endl;
            train_behavior(train);
            exit(0);
        } else if (pid > 0){
            train_pids.push_back(pid); // To match trains' index
        } else {
            cout << "ERROR";
            exit(1);
        }
    }

    for(size_t i = 0; i < train_pids.size(); ++i) {
        wait(NULL);
    }
    
    for (Train* train : train_ptrs) {
        delete train;
    }
    train_ptrs.clear();
    
    std::cout << "train.cpp: All trains have completed their routes!" << std::endl;
    
    }

void train_behavior(Train* train) {
    writeLog logger;
    
    for (Intersection* intersection : train->route) {
        msg_request msg;
        msg.mtype = MSG_TYPE_DEFAULT;
        strcpy(msg.command, "ACQUIRE");
        strcpy(msg.train_name, train->name.c_str());
        strcpy(msg.intersection, intersection->name.c_str());

        logger.logTrainRequest(train->name, intersection->name);
        send_msg(requestQueueId, msg);
        receive_msg(responseQueueId, msg);

        if(strcmp(msg.command, "GRANT") == 0) { // If the server sends back a GRANT signal
            logger.logGrant(train->name, intersection->name, "");
            logger.logProceeding(train->name, intersection->name);

            std::this_thread::sleep_for(std::chrono::seconds(1));  // Simulate travel time

            // Release (only done if the intersection is granted and after delays)
            strcpy(msg.command, "RELEASE");
            send_msg(requestQueueId, msg);
            
            receive_msg(responseQueueId, msg);
            if(strcmp(msg.command, "RELEASED") == 0) {
                logger.logRelease(train->name, intersection->name);
            }
        } else if (strcmp(msg.command, "WAIT") == 0){ // If the server sends back a WAIT signal
            logger.logIntersectionFull(train->name, intersection->name);
            std::this_thread::sleep_for(std::chrono::milliseconds(500)); // Maybe adjust this?
        } else if (strcmp(msg.command, "DENY") == 0){
            logger.logLock(train->name, intersection->name);
        }
        // This is to create a delay before the train tries again
        // Depending on testing, we may remove this I just thought it would be good in the place of deadlock prevention - Evelyn
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}
