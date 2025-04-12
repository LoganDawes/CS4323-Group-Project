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
    
    std::cout << "train.cpp: All trains have completed their routes!" << std::endl;
    
    }

void train_behavior(Train* train) {
    writeLog logger;
    
    for (Intersection* intersection : train->route) {
        logger.logTrainRequest(train->name, intersection->name);
        if (intersection->acquire(train)) {
            logger.logGrant(train->name, intersection->name, "");
            logger.logProceeding(train->name, intersection->name);

            std::this_thread::sleep_for(std::chrono::seconds(1));  // Simulate travel time

            intersection->release(train);
            logger.logRelease(train->name, intersection->name);
        } else {
            if (intersection->is_mutex) {
                logger.logLock(train->name, intersection->name);
            } else {
                logger.logIntersectionFull(train->name, intersection->name);
            }
        }
    }
}