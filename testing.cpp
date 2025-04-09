/*
Group B
Author: Logan Dawes
Email: logan.dawes@okstate.edu
Date: 04/05/2025

Description: Performs various tests on each unit, then a system test.
*/

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <string>
#include <set>
#include <cstring>

#include "parsing.hpp"
#include "logging.hpp"
#include "ipc.hpp"
#include "train.hpp"

// Generates a random initialization of intersections.txt and trains.txt for automated testing.
void generateConfig() {
    // File: intersections.txt
    std::ofstream intersectionsFile("intersections.txt");

    // Random Seed
    std::srand(std::time(0));

    // Generate a 1-10 intersections
    int numIntersections = (std::rand() % 10) + 1;
    printf("Number of intersections: %d\n", numIntersections);

    // Vector for intersections
    std::vector<std::string> intersections;

    for (int i = 0; i < numIntersections; ++i) {
        // Generate 1-3 capacity per intersection
        int capacity = (std::rand() % 3) + 1;
        printf("Intersection %d capacity: %d\n", i, capacity);

        // Set intersection number as alphabetical
        char alpha = 'A' + i;

        // Add intersection to vector
        std::string intersectionName = "Intersection" + std::string(1, alpha);
        intersections.push_back(intersectionName);

        // Write to file
        intersectionsFile << intersectionName << ':' << capacity << std::endl;
    }
    intersectionsFile.close();

    // File: trains.txt
    std::ofstream trainsFile("trains.txt");

    // Random Seed
    std::srand(std::time(0));

    // Generate a 1-10 trains
    int numTrains = (std::rand() % 10) + 1;
    printf("Number of trains: %d\n", numTrains);

    for (int i = 0; i < numTrains; ++i) {
        // Generate random intersection number for route
        int numRouteIntersections = (std::rand() % numIntersections) + 1;

        // Set for route intersections
        std::set<std::string> route;

        // Write to file
        trainsFile << "Train" << i+1 << ':';

        // Generate Route
        for (int j = 0; j < numRouteIntersections; ++j) {
            std::string selectedIntersection;
            do {
                // Aquire random intersection index to select intersection
                int intersectionIndex = std::rand() % numIntersections;
                selectedIntersection = intersections[intersectionIndex];
            }
            // Go through route to ensure no duplicates
            while (route.find(selectedIntersection) != route.end());

            // Add to route
            route.insert(selectedIntersection);

            // Comma seperated entries
            if (j != 0) {
                trainsFile << ",";
            }

            // Write route
            trainsFile << selectedIntersection;
        }
        trainsFile << std::endl;

    }
    trainsFile.close();
}

void conductTest(){
    // Test parsing of intersections.txt and trains.txt
    auto intersections = parseIntersections("intersections.txt");
    auto trains = parseTrains("trains.txt", intersections);

    // Test logging
    // TODO: logging function for initialize intersections in server.cpp

    // Test IPC setup
    ipc_setup();

        // Test request
        msg_request sendMsg;
        sendMsg.mtype = 1;
        strcpy(sendMsg.content, "Test message from train");
    
        if (send_msg(requestQueueId, sendMsg) == -1) {
            perror("send_msg");
        } else {
            std::cout << "Message sent to request queue.\n";
        }
    
        // Test response
        msg_request recvMsg;
        if (receive_msg(requestQueueId, recvMsg, 1) == -1) {
            perror("receive_msg");
        } else {
            std::cout << "Message received: " << recvMsg.content << std::endl;
        }

    // Test train forking
    train_forking();
}

int main(){
    // Conduct base system test
    conductTest();

    // Generate a random config
    generateConfig();
    conductTest();
    
    return 0;
}
