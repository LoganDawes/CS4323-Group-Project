/*
Group B
Author: Myron Peoples
Email: myron.peoples@okstate.edu
Date: 
Description: 
*/

#ifndef TRAIN_FORKING_H
#define TRAIN_FORKING_H
#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include "parsing.cpp"

int train_forking() {
    auto intersections = parseIntersections("intersections.txt");
    auto trains = parseTrains("trains.txt", intersections);

    // Create a vector to store pids for each train's fork
    std::vector<pid_t> train_pids;

    // For every train in trains, create a fork
    for(size_t i = 0; i < trains.size(); ++i){
        Train* train = train_list[i];

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
    
    for(size_t i = 0; i < train_pids.size(); ++i) {
        wait(NULL);
    }
    
    std::cout << "All trains have completed their routes!" << std::endl;
    
    return 0;
}

#endif