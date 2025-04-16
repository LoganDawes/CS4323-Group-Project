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

#include "testserver.hpp"

// Generate a 1-10 intersections
int numIntersections;

// Generate a 1-10 trains
int numTrains;

void generateBaseConfig()
{
    // Write static intersections
    std::ofstream intersectionsFile("intersections.txt");
    intersectionsFile << "IntersectionA:1\n";
    intersectionsFile << "IntersectionB:2\n";
    intersectionsFile << "IntersectionC:1\n";
    intersectionsFile << "IntersectionD:3\n";
    intersectionsFile << "IntersectionE:1\n";
    intersectionsFile.close();

    // Write static trains
    std::ofstream trainsFile("trains.txt");
    trainsFile << "Train1:IntersectionA,IntersectionB,IntersectionC\n";
    trainsFile << "Train2:IntersectionB,IntersectionD,IntersectionE\n";
    trainsFile << "Train3:IntersectionC,IntersectionD,IntersectionA\n";
    trainsFile << "Train4:IntersectionE,IntersectionB,IntersectionD\n";
    trainsFile.close();

    numIntersections = 5;
    numTrains = 4;
}

// Generates a random initialization of intersections.txt and trains.txt for automated testing.
void generateConfig()
{
    // File: intersections.txt
    std::ofstream intersectionsFile("intersections.txt");

    // Random Seed
    std::srand(std::time(0));

    numIntersections = (std::rand() % 10) + 1;

    printf("testing.cpp: Number of intersections: %d\n", numIntersections);

    // Vector for intersections
    std::vector<std::string> intersections;

    for (int i = 0; i < numIntersections; ++i)
    {
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

    for (int i = 0; i < numTrains; ++i)
    {
        // Generate random intersection number for route
        int numRouteIntersections = (std::rand() % numIntersections) + 1;

        // Set for route intersections
        std::set<std::string> route;

        // Write to file
        trainsFile << "Train" << i + 1 << ':';

        // Generate Route
        for (int j = 0; j < numRouteIntersections; ++j)
        {
            std::string selectedIntersection;
            do
            {
                // Aquire random intersection index to select intersection
                int intersectionIndex = std::rand() % numIntersections;
                selectedIntersection = intersections[intersectionIndex];
            }
            // Go through route to ensure no duplicates
            while (route.find(selectedIntersection) != route.end());

            // Add to route
            route.insert(selectedIntersection);
            printf("testing.cpp: Train %d route intersection: %s\n", i, selectedIntersection.c_str());

            // Comma seperated entries
            if (j != 0)
            {
                trainsFile << ",";
            }

            // Write route
            trainsFile << selectedIntersection;
        }
        trainsFile << std::endl;
    }
    trainsFile.close();
}

void parsing_test()
{
    // Parse files
    auto intersections = parseIntersections("intersections.txt");
    auto trains = parseTrains("trains.txt", intersections);

    if (intersections.size() == numIntersections)
    {
        std::cout << "testing.cpp: Parsed intersections successfully." << std::endl;
    }
    else
    {
        std::cerr << "testing.cpp: Failed to parse intersections." << std::endl;
    }
    if (trains.size() == numTrains)
    {
        std::cout << "testing.cpp: Parsed trains successfully." << std::endl;
    }
    else
    {
        std::cerr << "testing.cpp: Failed to parse trains." << std::endl;
    }
}

void ipc_test()
{
    // Setup IPC
    int result = ipc_setup();
    if (result == 0)
    {
        std::cout << "testing.cpp: IPC setup successful." << std::endl;
    }
    else
    {
        std::cerr << "testing.cpp: IPC setup failed." << std::endl;
        return;
    }

    // Test message
    msg_request testMsg;
    testMsg.mtype = MSG_TYPE_DEFAULT;
    strcpy(testMsg.command, "TEST");
    strcpy(testMsg.intersection, "IntersectionA");
    for (int i = 0; i < 20; ++i)
    {
        testMsg.train_name[i] = i;
    }

    // Send the message
    int sendResult = send_msg(requestQueueId, testMsg);
    if (sendResult == 0)
    {
        std::cout << "testing.cpp: Message sent successfully." << std::endl;
    }
    else
    {
        std::cerr << "testing.cpp: Failed to send message." << std::endl;
        return;
    }

    // Receive the message
    msg_request receivedMsg;
    int recvResult = receive_msg(requestQueueId, receivedMsg);
    if (recvResult != -1)
    {
        std::cout << "testing.cpp: Message received successfully." << std::endl;
    }
    else
    {
        std::cerr << "testing.cpp: Failed to receive message." << std::endl;
        return;
    }

    // Validate message content
    if (strcmp(receivedMsg.command, "TEST") == 0)
    {
        std::cout << "testing.cpp: Command in response matches." << std::endl;
    }
    else
    {
        std::cerr << "testing.cpp: Command in response does not match." << std::endl;
    }

    if (strcmp(receivedMsg.intersection, "IntersectionA") == 0)
    {
        std::cout << "testing.cpp: Intersection in response matches." << std::endl;
    }
    else
    {
        std::cerr << "testing.cpp: Intersection in response does not match." << std::endl;
    }

    // Validate train names in response
    bool trainNamesMatch = true;
    for (int i = 0; i < 20; ++i)
    {
        if (receivedMsg.train_name[i] != i)
        {
            trainNamesMatch = false;
            break;
        }
    }
    if (trainNamesMatch)
    {
        std::cout << "testing.cpp: Train name array matches." << std::endl;
    }
    else
    {
        std::cerr << "testing.cpp: Train name array does not match." << std::endl;
    }
}

void allocation_table_test()
{
    std::cout << "testing.cpp: Starting allocation table test..." << std::endl;

    // Create ResourceAllocationGraph
    ResourceAllocationGraph resourceGraph;

    // Create test intersections
    Intersection intersectionA("IntersectionA", 1); // Mutex
    Intersection intersectionB("IntersectionB", 2); // Semaphore

    // Add intersections to the graph
    resourceGraph.addIntersection(&intersectionA);
    resourceGraph.addIntersection(&intersectionB);

    // Create test trains
    std::vector<Intersection *> emptyRoute;
    Train train1("Train1", emptyRoute);
    Train train2("Train2", emptyRoute);

    // Acquire intersections
    resourceGraph.acquire("IntersectionA", &train1);
    resourceGraph.acquire("IntersectionB", &train1);
    resourceGraph.acquire("IntersectionB", &train2);

    // Get resource table
    auto resourceTable = resourceGraph.getResourceTable();

    // Validate IntersectionA, mutex with 1 train
    if (resourceTable["IntersectionA"].size() == 1 && resourceTable["IntersectionA"][0] == "Train1")
    {
        std::cout << "testing.cpp: IntersectionA allocation validated." << std::endl;
    }
    else
    {
        std::cerr << "testing.cpp: IntersectionA allocation failed." << std::endl;
    }

    // Validate IntersectionB, semaphore with 2 trains
    if (resourceTable["IntersectionB"].size() == 2 &&
        resourceTable["IntersectionB"][0] == "Train1" &&
        resourceTable["IntersectionB"][1] == "Train2")
    {
        std::cout << "testing.cpp: IntersectionB allocation validated." << std::endl;
    }
    else
    {
        std::cerr << "testing.cpp: IntersectionB allocation failed." << std::endl;
    }

    // Print table
    std::cout << "testing.cpp: Resource table:" << std::endl;
    resourceTable.printGraph();

    // Release intersections
    resourceGraph.release("IntersectionA", &train1);
    resourceGraph.release("IntersectionB", &train1);
    resourceGraph.release("IntersectionB", &train2);

    // Validate resource table after release
    resourceTable = resourceGraph.getResourceTable();
    if (resourceTable["IntersectionA"].empty() && resourceTable["IntersectionB"].empty())
    {
        std::cout << "testing.cpp: Resource table cleared successfully after release." << std::endl;
    }
    else
    {
        std::cerr << "testing.cpp: Resource table not cleared properly after release." << std::endl;
    }
}

void train_test()
{
    // Fork the trains
    std::cout << "testing.cpp: Forking trains now..." << std::endl;
    train_forking();
    // TODO: add check for train behavior
}

void deadlock_recovery_test()
{
    // Create test intersections
    Intersection intersectionA("IntersectionA", 1); // Mutex
    Intersection intersectionB("IntersectionB", 1); // Mutex

    // Create test trains with routes
    std::vector<Intersection *> route1 = {&intersectionA, &intersectionB};
    std::vector<Intersection *> route2 = {&intersectionB, &intersectionA};
    Train train1("Train1", route1);
    Train train2("Train2", route2);

    // Create resource table
    map<string, vector<string>> resourceTable;
    resourceTable["IntersectionA"] = {"Train1"};
    resourceTable["IntersectionB"] = {"Train2"};

    // Simulate a deadlock cycle
    // NYI

    // Create train and intersection maps
    map<string, Train *> trains = {{"Train1", &train1}, {"Train2", &train2}};
    unordered_map<string, Intersection *> intersections = {{"IntersectionA", &intersectionA}, {"IntersectionB", &intersectionB}};

    // Call deadlockRecovery (once implemented)
    deadlockRecovery(trains, intersections, resourceTable);

    // Validate results
    // Check if the deadlock was resolved
    if (resourceTable["IntersectionA"].empty() || resourceTable["IntersectionB"].empty())
    {
        std::cout << "testing.cpp: Deadlock recovery resolved the deadlock." << std::endl;
    }
    else
    {
        std::cerr << "testing.cpp: Deadlock recovery failed to resolve the deadlock." << std::endl;
    }
}

void logging_test()
{
    writeLog logger;

    // Basic message log
    logger.log("TEST", "This is a basic log message.");

    // Log train requesting an intersection
    logger.logTrainRequest("1", "A");

    // Granting access
    logger.logGrant("1", "A", "Remaining slots: 1");

    // Intersection is locked (mutex case)
    logger.logLock("2", "A");

    // Intersection full (semaphore case)
    logger.logIntersectionFull("3", "A");

    // Simulate deadlock
    logger.logDeadlockDetected("Train1 ↔ Train3");

    // Release logs
    logger.logRelease("1", "A");

    // Deadlock preemption
    logger.logPreemption("3", "B");

    // Grant after preemption
    logger.logGrantAfterPreemption("2", "B");

    // Proceeding log
    logger.logProceeding("2", "B");

    // Final completion log
    logger.logSimulationComplete();
}

int main()
{
    generateBaseConfig();

    std::cout << "-------------------------------------\n";
    std::cout << "Starting parsing test...\n";
    std::cout << "-------------------------------------\n";

    // Conduct parsing test
    parsing_test();

    std::cout << "-------------------------------------\n";
    std::cout << "Starting IPC test...\n";
    std::cout << "-------------------------------------\n";

    // Conduct IPC test
    ipc_test();

    std::cout << "-------------------------------------\n";
    std::cout << "Starting allocation table test...\n";
    std::cout << "-------------------------------------\n";

    // Conduct allocation table test
    allocation_table_test();

    std::cout << "-------------------------------------\n";
    std::cout << "Starting train test...\n";
    std::cout << "-------------------------------------\n";

    // Conduct train test
    train_test();

    std::cout << "-------------------------------------\n";
    std::cout << "Starting deadlock recovery test...\n";
    std::cout << "-------------------------------------\n";

    // Conduct deadlock recovery test
    deadlock_recovery_test();

    std::cout << "-------------------------------------\n";
    std::cout << "Starting logging test...\n";
    std::cout << "-------------------------------------\n";

    // Conduct logging test
    logging_test();

    std::cout << "-------------------------------------\n";
    std::cout << "Starting server test...\n";
    std::cout << "-------------------------------------\n";

    // Conduct base system test
    if (server())
    {
        std::cerr << "Server encountered error." << std::endl;
        return 1;
    }

    std::cout << "-------------------------------------\n";
    std::cout << "Starting random server test...\n";
    std::cout << "-------------------------------------\n";

    // Generate a random config system test
    generateConfig();
    if (server())
    {
        std::cerr << "Server encountered error." << std::endl;
        return 1;
    }

    return 0;
}
