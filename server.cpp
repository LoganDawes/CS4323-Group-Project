/*
Group: B
Author: Gavin Zlatar
Email: gavin.zlatar@okstate.edu
Date: 4/12/2025

Description: The 'server.cpp' file creates a multithreaded server that processes requests from child processes whilst
checking for deadlocks. Threads handle requests and update a resource graph, which tracks resource dependencies. 
The program uses this graph to detect deadlocks by looking for cycles. If a deadlock is found, it alerts the user; 
otherwise, it confirms everything is running smoothly. Mutexes and condition variables ensure threads work safely 
and efficiently.
*/

#include "server.hpp"

std::mutex mtx;
std::condition_variable cv;

ResourceAllocationGraph resourceGraph;

int main() {
    auto intersections = parseIntersections("intersections.txt"); // parse for intersections
    auto trains = parseTrains("trains.txt", intersections); // parse for train configs

    // add intersections to resource graph
    for (auto& [name, inter] : intersections) {
        resourceGraph.addIntersection(inter);
    }

    // fork child for handling trains
    train_forking();


    std::cout << "server.cpp: Server started...\n";
    
    // IPC set up
    ipc_setup();

    // main loop
    while (true) {
        // recieve message from request queue
        receive_msg(requestQueueId, msg);
        
        // extraction for train name and intersection info
        string trainName = msg.train_name;
        string intersection = msg.intersection;
        Train* train = trains[trainName];
    
        if (strcmp(msg.command, "ACQUIRE") == 0) {
            bool success = resourceGraph.acquire(intersection, train);
            if (success) {
                // log success adn grant access
                writeLog::logGrant(trainName, intersection);
                strcpy(msg.command, "GRANT");

            } else {
                // logfail and instruct to wait
                writeLog::logLock(trainName, intersection);
                strcpy(msg.command, "WAIT");

            }

        } else if (strcmp(msg.command, "RELEASE") == 0) {
            bool success = resourceGraph.release(intersection, train);
            if (success) {
                // log success, cancel wait, adn confirm release
                writeLog::logRelease(trainName, intersection);
                strcpy(msg.command, "RELEASED");

            } else {
                // log invalid request and deny it
                writeLog::log("SERVER", "Invalid release request.");
                strcpy(msg.command, "DENY");
            }
        }
    
        // sends response message to train
        send_msg(responseQueueId, msg);
    }    

    resourceGraph.printGraph();

    // Deadlock detection statement
    auto waitingGraph = resourceGraph.buildWaitingGraph();
    if (detectDeadlock(waitingGraph)) {
        std::cout << "Deadlock detected! Handing over to the recovery module...\n";

        auto graph = resourceGraph.getResourceGraph;
        deadlockRecovery(trains, intersections, graph);
    }
}

void handleRequest(int processID) {
    // locks shrared resource access
    std::unique_lock<std::mutex> lock(mtx);

    std::cout << "Handling request from process " << processID << "\n";
    // simulate resource request (add to resource graph)
    resourceGraph[processID] = {processID + 1}; // example dependency
    
    // request process notification
    cv.notify_all();
}


bool detectDeadlock(const unordered_map<string, vector<string>>& waitingGraph) {
    
    unordered_map<string, bool> visited, recursionStack;

    // initializes nodes as unvisited outside the stack
    for (const auto& [node, _] : waitingGraph) {
        visited[node] = false;
        recursionStack[node] = false;
    }

    // checks the graph for cycles
    for (const auto& [node, _] : waitingGraph) {
        if (!visited[node]) {
            if (isCyclicUtil(node, visited, recursionStack, waitingGraph)) {
                return true;
            }
        }
    }

    return false;
}

bool isCyclicUtil(const node, std::map<int, bool>& visited, std::map<int, bool>& recursionStack, const std::map<int, ResourceAllocationGraph& graph) {
    visited[node] = true;
    recursionStack[node] = true;

    // checking neighbors of the current node
    for (int neighbor : graph.at(node)) {
        if (!visited[neighbor] && isCyclicUtil(neighbor, visited, recursionStack, graph)) {
            return true;

        } else if (recursionStack[neighbor]) {
            return true;

        }
    }

    // removes node from stack after processing
    recursionStack[node] = false;
    return false;
}