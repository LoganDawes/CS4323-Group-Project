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
// This will track what trains are waiting on what based on requests and grants
std::unordered_map<std::string, std::vector<std::string>> waitingGraph;
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

                waitingGraph.erase(trainName); // Remove the train from the waitingGraph.
            } else {
                // logfail and instruct to wait
                writeLog::logLock(trainName, intersection);
                strcpy(msg.command, "WAIT");

                // For every train in the intersection, add them to the list of neighbors for the waiting train
                for(Train* intersectionHolder : resourceGraph.getIntersection(intersection)->trains_in_intersection) {
                    waitingGraph[trainName].push_back(holder->name);
                }
            }

        } else if (strcmp(msg.command, "RELEASE") == 0) {
            bool success = resourceGraph.release(intersection, train);
            if (success) {
                // log success, cancel wait, adn confirm release
                writeLog::logRelease(trainName, intersection);
                strcpy(msg.command, "RELEASED");
                waitingGraph.erase(trainName);
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
    vector<string> cycle;
    if (detectDeadlock(waitingGraph, cycle)) {
        std::cout << "Deadlock detected! Handing over to the recovery module...\n";
    
        auto graph = resourceGraph.getResourceGraph();
        deadlockRecovery(trains, intersections, graph, cycle);
    }
}

void handleRequest(int processID) {
    // locks shared resource access
    std::unique_lock<std::mutex> lock(mtx);

    std::cout << "Handling request from process " << processID << "\n";
    // simulate resource request (add to resource graph)
    resourceGraph[processID] = {processID + 1}; // example dependency
    
    // request process notification
    cv.notify_all();
}


bool detectDeadlock(const unordered_map<string, vector<string>>& waitingGraph, vector<string>& cycle) {
    
    unordered_map<string, bool> visited, recursionStack;
    unordered_map<string, string> parent;

    // initializes nodes as unvisited outside the stack
    for (const auto& [node, _] : waitingGraph) {
        visited[node] = false;
        recursionStack[node] = false;
    }

    // checks the graph for cycles
    for (const auto& [node, _] : waitingGraph) {
        if (!visited[node]) {
            if (isCyclicUtil(node, visited, recursionStack, waitingGraph, cycle, parent)) {
                return true;
            }
        }
    }

    return false;
}

bool isCyclicUtil(const string& node, // Current train
    unorderd_map<string, bool>& visited, // Has the train been seen before?
    unordered_map<string, bool>& recursionStack, // Call stack path
    const unordered_map<string, vector<string>>& graph, // waitingGraph - trains and the trains it's waiting on.
    vector<string>& cycle,
    unordered_map<string, string>& parent) {
    /* For context, the 'neighbors' are what we call the trains that the current train is waiting on.
    So to detect a deadlock the neighbor needs to both already have been visited and in the current recursion path.*/

    // Say the node that we're visiting is visited and add to recursion stack
    visited[node] = true;
    recursionStack[node] = true;

    // checking neighbors of the current node
    for (const string& neighbor : graph.at(node)) {

        // If the neighbor hasn't been visited, we run a recursive call on it
        if (!visited[neighbor]) {
            isCyclicUtil(neighbor, visited, recursionStack, graph, cycle, parent);
            return true;
        } else if (recursionStack[neighbor]) {
            // Reconstructt he cycle so it can be sent to the deadlock recovery
            cycle.clear();
            string current = node;
            cycle.push_back(neighbor);
            while(current != neighbor) {
                cycle.push_back(current);
                current = parent[current];
            }
            // Reverse it so the cycle is in the correct order for recovery
            reverse(cycle.begin(), cycle.end());
            return true;
        }
    }

    // removes node from stack after processing
    recursionStack[node] = false;
    return false;
}