/*
Group B
Author: Logan Dawes
Email: logan.dawes@okstate.edu
Date: 4/12/2025

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
#include "deadlock_recovery.hpp"
#include "testserver.hpp"

// Generate a 1-10 intersections
int numIntersections;

// Generate a 1-10 trains
int numTrains;

// Generates a random initialization of intersections.txt and trains.txt for automated testing.
void generateConfig() {
    // File: intersections.txt
    std::ofstream intersectionsFile("intersections.txt");

    // Random Seed
    std::srand(std::time(0));

    numIntersections = (std::rand() % 10) + 1;
    printf("testing.cpp: Number of intersections: %d\n", numIntersections);

    // Vector for intersections
    std::vector<std::string> intersections;

    for (int i = 0; i < numIntersections; ++i) {
        // Generate 1-3 capacity per intersection
        int capacity = (std::rand() % 3) + 1;
        printf("testing.cpp: Intersection %d capacity: %d\n", i, capacity);

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

    numTrains = (std::rand() % 10) + 1;
    printf("testing.cpp: Number of trains: %d\n", numTrains);

    for (int i = 0; i < numTrains; ++i) {
        // Generate random intersection number for route
        int numRouteIntersections = (std::rand() % numIntersections) + 1;

        // Set for route intersections
        std::set<std::string> route;

        // Write to file
        trainsFile << "testing.cpp: Train" << i+1 << ':';

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

void parsing_test(){
    // Parse files
    auto intersections = parseIntersections("test_intersections.txt");
    auto trains = parseTrains("test_trains.txt", intersections);

    if(intersections.size() == numIntersections){
        std::cout << "testing.cpp: Parsed intersections successfully." << std::endl;
    } else {
        std::cerr << "testing.cpp: Failed to parse intersections." << std::endl;
    }
    if(trains.size() == numTrains){
        std::cout << "testing.cpp: Parsed trains successfully." << std::endl;
    } else {
        std::cerr << "testing.cpp: Failed to parse trains." << std::endl;
    }
}

void ipc_test(){
    //NYI
}

void train_test(){
    //NYI
}

void deadlock_recovery_test(){
    //NYI
}

void logging_test(){
    //NYI
}

int main(){
    // Conduct parsing test
    parsing_test();

    // Conduct base system test
    if (server())
    {
        std::cerr << "Server encountered error." << std::endl;
        return 1;
    }

    // Generate a random config system test
    generateConfig();
    if (server())
    {
        std::cerr << "Server encountered error." << std::endl;
        return 1;
    }
    
    return 0;
}
