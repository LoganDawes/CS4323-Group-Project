/*
Group B
Author: Gavin Zlatar
Email: gavin.zlatar@okstate.edu
Date: 4/5/2025

Description: The 'server.cpp' file creates a multithreaded server that processes requests from child processes while
checking for deadlocks. Threads handle requests and update a resource graph, which tracks resource dependencies. 
The program uses this graph to detect deadlocks by looking for cycles. If a deadlock is found, it alerts the user; 
otherwise, it confirms everything is running smoothly. Mutexes and condition variables ensure threads work safely 
and efficiently.
*/
#include "parsing.hpp"
#include "logging.hpp"
#include "ipc.hpp"
#include "train.hpp"
#include <iostream>
#include <vector>
#include <map>
#include <mutex>
#include <thread>
#include <condition_variable>

void handleRequest(int processID);
bool detectDeadlock(const std::map<int, std::vector<int>>& resourceGraph);

std::mutex mtx;
std::condition_variable cv;

// simulated resource graph for detecting deadlocks
std::map<int, std::vector<int>> resourceGraph;

int main() {
    // TODO: move parsing and and intersection initilization logging here

    train_forking();

    std::cout << "Server started...\n";
    
    // TODO: receive a resource allocation graph from ipc setup to use in communication between trains
    ipc_setup();

    // TODO: put these in a loop to receive messages from trains, handle them, and send responses
    receive_msg(requestQueueId, message);
    send_msg(responseQueueId, message);

    // deadlock detection statement
    if (detectDeadlock(resourceGraph)) {
        std::cout << "Deadlock detected! Resolving...\n";
        // TO DO: implement deadlock resolution logic here



    } else {
        std::cout << "No deadlock detected. System running smoothly.\n";
    }

    std::cout << "Server shutting down...\n";
    return 0;
}

void handleRequest(int processID) {
    std::unique_lock<std::mutex> lock(mtx);

    
    std::cout << "Handling request from Process " << processID << "\n";
    // simulate resource request (add to resource graph)
    resourceGraph[processID] = {processID + 1}; // xxample dependency
    
    cv.notify_all();
}


bool detectDeadlock(const std::map<int, std::vector<int>>& resourceGraph) {
    // resource allocation graph for deadlock
    std::map<int, bool> visited, recursionStack;

    for (const auto& node : resourceGraph) {
        visited[node.first] = false;
        recursionStack[node.first] = false;
    }

    for (const auto& node : resourceGraph) {
        int processID = node.first;
        if (!visited[processID]) {
            // placeholder for cycle detection:


            bool cycleFound = false; // TO DO: replace with actual algorithm
            if (cycleFound) return true;
        }
    }

    return false;
}
