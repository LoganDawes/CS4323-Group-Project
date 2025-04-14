/*
Group: B
Author: Gavin Zlatar
Email: gavin.zlatar@okstate.edu
Date: 4/12/2025

Description: The 'server.cpp' file creates a multithreaded server that processes requests from child processes while
checking for deadlocks. Threads handle requests and update a resource graph, which tracks resource dependencies. 
The program uses this graph to detect deadlocks by looking for cycles. If a deadlock is found, it alerts the user; 
otherwise, it confirms everything is running smoothly. Mutexes and condition variables ensure threads work safely 
and efficiently.
*/

#include "server.hpp"

std::mutex mtx;
std::condition_variable cv;

// simulated resource graph for detecting deadlocks
ResourceAllocationGraph resourceGraph;

int server() {
    auto intersections = parseIntersections("intersections.txt");
    auto trains = parseTrains("trains.txt", intersections);

    for (auto& [name, inter] : intersections) {
        resourceGraph.addIntersection(inter);
    }

    train_forking();

    std::cout << "server.cpp: Server started...\n";
    
    // TODO: receive a resource allocation graph from ipc setup to use in communication between trains
    ipc_setup();

    while (true) {
        receive_msg(requestQueueId, message);
        
        string trainName = message.train_name;
        string intersection = message.intersection;
        Train* train = trains[trainName];
    
        if (strcmp(message.command, "ACQUIRE") == 0) {
            bool success = resourceGraph.acquire(intersection, train);
            if (success) {
                writeLog::logGrant(trainName, intersection);
                strcpy(message.command, "GRANT");
            } else {
                writeLog::logLock(trainName, intersection);
                strcpy(message.command, "WAIT");
            }
        } else if (strcmp(message.command, "RELEASE") == 0) {
            bool success = resourceGraph.release(intersection, train);
            if (success) {
                writeLog::logRelease(trainName, intersection);
                strcpy(message.command, "RELEASED");
            } else {
                writeLog::log("SERVER", "Invalid release request.");
                strcpy(message.command, "DENY");
            }
        }
    
        send_msg(responseQueueId, message);
    }    

    resourceGraph.printGraph();

    /* TEMPORARY COMMENT OUT: NEEDS TO BE INTEGRATED WITH RESOURCE ALLOCATION GRAPH --------------------------------
    // deadlock detection statement
    if (detectDeadlock(resourceGraph)) {
        std::cout << "Deadlock detected! Handing over to the recovery module...\n";

        // pass necessary data structures to deadlockRecovery

        std::map<std::string, Train*> trains; // PLACEHOLDER for actual data for data provided by testing
        std::unordered_map<std::string, Intersection*> intersections; // PLACEHOLDER for intersections for data provided by testing
    
        std::map<std::string, std::vector<std::string>> resourceTable;
        for (const auto& entry : resourceGraph) {
            resourceTable[std::to_string(entry.first)] = 
                std::vector<std::string>(entry.second.begin(), entry.second.end());
        }
    
        //deadlockRecovery(trains, intersections, resourceTable);
    }
    --------------------------------------------------------------------------------------------------------------*/
}

/* TEMPORARY COMMENT OUT: NEEDS TO BE INTEGRATED WITH RESOURCE ALLOCATION GRAPH --------------------------------
void handleRequest(int processID) {
    std::unique_lock<std::mutex> lock(mtx);

    std::cout << "Handling request from Process " << processID << "\n";
    // simulate resource request (add to resource graph)
    resourceGraph[processID] = {processID + 1}; // xxample dependency
    
    cv.notify_all();
}


bool detectDeadlock(const std::map<int, ResourceAllocationGraph& resourceGraph) {
    std::map<int, bool> visited, recursionStack;

    for (const auto& node : resourceGraph) {
        visited[node.first] = false;
        recursionStack[node.first] = false;
    }

    for (const auto& node : resourceGraph) {
        int processID = node.first;
        if (!visited[processID]) {
            if (isCyclicUtil(processID, visited, recursionStack, resourceGraph)) {
                return true;
            }
        }
    }

    return false;
}

bool isCyclicUtil(int node, std::map<int, bool>& visited, std::map<int, bool>& recursionStack, const std::map<int, ResourceAllocationGraph& graph) {
    visited[node] = true;
    recursionStack[node] = true;

    for (int neighbor : graph.at(node)) {
        if (!visited[neighbor] && isCyclicUtil(neighbor, visited, recursionStack, graph)) {
            return true;
        } else if (recursionStack[neighbor]) {
            return true;
        }
    }

    recursionStack[node] = false;
    return false;
}
--------------------------------------------------------------------------------------------------------------*/