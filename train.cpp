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
#include <cstring>
#include <iostream>
#include <vector>

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
    
    //Clean up memory in Parent
    for (Train* train : train_ptrs) {
        delete train;
    }
    train_ptrs.clear();
    
    std::cout << "train.cpp: All trains have completed their routes!" << std::endl;
    
    }

// Mutex for train queues
pthread_mutex_t responseMutex = PTHREAD_MUTEX_INITIALIZER;

void train_behavior(Train *train)
{
    while (!train->route.empty())
    {
        Intersection *intersection = train->route.front();
        bool acquired = false;
        bool waitingForResponse = false;

        while (!acquired)
        {
            pthread_mutex_lock(&responseMutex); // Lock the mutex
            if (!waitingForResponse && !acquired)
            {
                // Send ACQUIRE request only if not waiting for a response
                msg_request msg;
                msg.mtype = MSG_TYPE_DEFAULT;
                strcpy(msg.command, "ACQUIRE");
                strcpy(msg.train_name, train->name.c_str());
                strcpy(msg.intersection, intersection->name.c_str());

                std::cout << "train.cpp: Sending message: " << msg.command << " for " << msg.intersection << std::endl;
                send_msg(requestQueueId, msg);

                waitingForResponse = true;
            }
            pthread_mutex_unlock(&responseMutex); // Unlock the mutex

            // Wait for the server's response
            msg_request msg;
            if (receive_msg(responseQueueId, msg) == -1){
                std::cerr << "train.cpp: Failed to receive message" << std::endl;
                continue; // Retry if receiving the message fails
            }
            std::cout << "train.cpp: Received message: " << msg.command << " for " << msg.intersection << std::endl;

            pthread_mutex_lock(&responseMutex); // Lock the mutex when gets a message
            
            if (strcmp(msg.command, "GRANT") == 0)
            {
                acquired = true;
                std::this_thread::sleep_for(std::chrono::seconds(1)); // Simulate travel time

                // Release the intersection after traveling
                strcpy(msg.command, "RELEASE");
                strcpy(msg.train_name, train->name.c_str());
                strcpy(msg.intersection, intersection->name.c_str());
                send_msg(requestQueueId, msg);
                std::cout << "train.cpp: Released intersection: " << intersection->name << std::endl;

                // Remove the intersection from the route
                if (!train->route.empty())
                {
                    train->route.erase(train->route.begin());
                }

                waitingForResponse = false;

                // Update the current intersection
                if (!train->route.empty())
                {
                    intersection = train->route.front();
                }
            }
            else if (strcmp(msg.command, "WAIT") == 0)
            {
                // Wait before retrying
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
                waitingForResponse = false;
            }
            else if (strcmp(msg.command, "DENY") == 0)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
                waitingForResponse = false;
            }
            pthread_mutex_unlock(&responseMutex); // Unlock the mutex for next route
        }
    }

    std::cout << "train.cpp: Train " << train->name << " has completed its route!" << std::endl;
    msg_request msg;
    strcpy(msg.command, "COMPLETE");
    strcpy(msg.train_name, train->name.c_str());
    send_msg(requestQueueId, msg);
}
