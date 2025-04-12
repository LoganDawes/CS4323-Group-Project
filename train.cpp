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
            std::cout << train->name << " starting its journey!" << std::endl;
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
    
    std::cout << "All trains have completed their routes!" << std::endl;
    
    }

Train::Train(std::string name, std::vector<Intersection*> route)
    : name(name), route(route), current_location(nullptr) {}

void train_behavior(Train* train) {
    for (Intersection* intersection : train->route) {
        if (intersection->acquire(train)) {
            log_event(train->name + ": Entered intersection " + intersection->name);
            std::this_thread::sleep_for(std::chrono::seconds(1));  // Simulate travel time
            intersection->release(train);
            log_event(train->name + ": Left intersection " + intersection->name);
        } else {
            log_event(train->name + ": Could not enter intersection " + intersection->name);
        }
    }
}