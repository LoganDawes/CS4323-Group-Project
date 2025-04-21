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

// sim_time variable
int sim_time = 0;

int main() {
    waitingGraph.clear();

    // Initialize the resource graph
    resourceGraph = ResourceAllocationGraph();

    auto intersections = parseIntersections("intersections.txt"); // parse for intersections
    auto trains = parseTrains("trains.txt", intersections); // parse for train configs

    std::ostringstream intersectionLog;
    intersectionLog << "Initialized intersections:\n";

    // Format log like project document
    for (const auto &[name, inter] : intersections)
    {
        intersectionLog << "- " << name << " (";
        intersectionLog << (inter->is_mutex ? "Mutex" : "Semaphore") << ", Capacity=" << inter->capacity << ")\n";
    }

    // Log the initialized intersections
    writeLog::log("SERVER", intersectionLog.str(), sim_time);

    // numTrains and completeTrains track route completion
    int numTrains = trains.size();
    int completeTrains = 0;

    // add intersections to resource graph
    for (auto& [name, inter] : intersections) {
        resourceGraph.addIntersection(inter);
    }

    // IPC set up
    if (ipc_setup()==-1) {
        std::cerr << "server.cpp: IPC setup failed.\n";
        return 1;
    };

    msg_request msg;

    pid_t pid = fork();
    if (pid < 0) {
        std::cerr << "server.cpp: Forking failed.\n";
        return 1;

    // PID 0, child process, goes onto train_forking
    } else if (pid == 0) {
        train_forking(intersections, trains);
        exit(0);
    } 

    std::cout << "server.cpp: Server started...\n";

    // main loop
    while (true) {
        // recieve message from request queue
        int receive_success = receive_msg(requestQueueId, msg);
        if (receive_success == -1) {
            std::cerr << "server.cpp: Failed to receive message.\n";
            continue; // Retry if receiving the message fails
        }
        std::cout << "server.cpp: Received message: " << msg.train_name << " " << msg.command << " " << msg.intersection << " " <<  std::endl;
        
        // extraction for train name and intersection info
        string trainName = msg.train_name;
        string intersection = msg.intersection;
        Train* train = trains[trainName];
    
        if (strcmp(msg.command, "ACQUIRE") == 0) {
            sim_time++;
            writeLog::logTrainRequest(trainName, intersection, sim_time);
            bool success = resourceGraph.acquire(intersection, train);
            if (success) {
                // Get semaphore count for logs
                Intersection* inter = resourceGraph.getIntersection(intersection);
                std::string semaphore_count = "";
                if (!inter->is_mutex) {
                    std::string semaphore_count = std::to_string(inter->capacity - inter->trains_in_intersection.size()); // Semaphore count is capacity - trains in intersection
                }

                // Log success and grant access
                writeLog::logGrant(trainName, intersection, semaphore_count, sim_time);
                strcpy(msg.command, "GRANT");
                // sends response message to train
                msg.mtype = 1;
                std::cout << "server.cpp: Sending message: " << msg.train_name << " " << msg.command << " " << msg.intersection << " " << msg.mtype << std::endl;
                send_msg(responseQueueId, msg);

                waitingGraph.erase(trainName); // Remove the train from the waitingGraph.
            } else {
                // log fail and instruct to wait
                writeLog::logLock(trainName, intersection, sim_time);
                strcpy(msg.command, "WAIT");
                // sends response message to train
                msg.mtype = 1;
                std::cout << "server.cpp: Sending message: " << msg.train_name << " " << msg.command << " " << msg.intersection << " " << msg.mtype << std::endl;
                send_msg(responseQueueId, msg);

                Intersection* intrsctn = resourceGraph.getIntersection(intersection);
                if (intersctn && !intrsctn->trains_in_intersection.empty()) {
                    for (Train* intersectionHolder : intersctn->trains_in_intersection) {
                        if (intersectionHolder->name != trainName) {
                            waitingGraph[trainName].push_back(intersectionHolder->name);
                        }
                    }
                }
            }

        } else if (strcmp(msg.command, "RELEASE") == 0) {
            bool success = resourceGraph.release(intersection, train);
            if (success) {
                // log success, cancel wait, adn confirm release
                writeLog::logRelease(trainName, intersection, sim_time);

                waitingGraph.erase(trainName);
            }
            else
            {
                resourceGraph.printGraph(); // Print the resource graph for debugging
                // log invalid request and deny it
                Intersection *inter = resourceGraph.getIntersection(intersection);
                if (!inter)
                {
                    writeLog::log("SERVER", "Invalid release request: Intersection not found: " + intersection, sim_time);
                    std::cerr << "server.cpp: Invalid release request: Intersection not found: " << intersection << std::endl;
                }
                else if (std::find(inter->trains_in_intersection.begin(), inter->trains_in_intersection.end(), train) == inter->trains_in_intersection.end())
                {
                    writeLog::log("SERVER", "Invalid release request: Train " + trainName + " not found in intersection " + intersection, sim_time);
                    std::cerr << "server.cpp: Invalid release request: Train " << trainName << " not found in intersection " << intersection << std::endl;
                }
                else
                {
                    writeLog::log("SERVER", "Invalid release request: Unknown error for train " + trainName + " at intersection " + intersection, sim_time);
                    std::cerr << "server.cpp: Invalid release request: Unknown error for train " << trainName << " at intersection " << intersection << std::endl;
                }
                strcpy(msg.command, "DENY");
                // sends response message to train
                msg.mtype = 1;
                std::cout << "server.cpp: Sending message: " << msg.train_name << " " << msg.command << " " << msg.intersection << msg.mtype << std::endl;
                send_msg(responseQueueId, msg);
            }
        } else if (strcmp(msg.command, "COMPLETE") == 0){
            // Train has completed its route, increment completeTrains
            completeTrains++;

            // If all trains completed, log simualtion complete then exit
            if (completeTrains == numTrains) {
                writeLog::logSimulationComplete(sim_time);
                std::cout << "All trains have completed their routes.\n";
                break; // exit the main loop if all trains are complete
            }
        }
        // JUST FOR TESTING WAITING GRAPH
        std::cout << "\n[DEBUG] Current waiting graph:\n";
        for (const auto& [train, deps] : waitingGraph) {
            std::cout << "  " << train << " is waiting for → ";
            for (const auto& dep : deps) std::cout << dep << " ";
            std::cout << "\n";
        }


        // Deadlock detection statement
        vector<string> cycle;
        if (detectDeadlock(waitingGraph, cycle))
        {
            std::cout << "Deadlock detected! Handing over to the recovery module...\n";

            auto graph = resourceGraph.getResourceGraph();
            deadlockRecovery(trains, graph, cycle, sim_time);
        }
    }

    return 0;
}


bool detectDeadlock(const unordered_map<string, vector<string>>& waitingGraph, vector<string>& cycle) {
    
    unordered_map<string, bool> visited, recursionStack;
    unordered_map<string, string> parent;

    // initializes nodes as unvisited outside the stack
    for (const auto& [node, _] : waitingGraph) {
        std::cout << "DEBUG: for loop, initializing node";
        visited[node] = false;
        recursionStack[node] = false;
    }

    // checks the graph for cycles
    for (const auto& [node, _] : waitingGraph) {
        std::cout << "DEBUG: for loop, checking graph for cycles";
        if (!visited[node]) {
            if (isCyclicUtil(node, visited, recursionStack, waitingGraph, cycle, parent)) {
                return true;
            }
        }
    }

    return false;
}

bool isCyclicUtil(const string& node, // Current train
    unordered_map<string, bool>& visited, // Has the train been seen before?
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
            parent[neighbor] = node;
            if(isCyclicUtil(neighbor, visited, recursionStack, graph, cycle, parent)){
                // Log that a cycle was detected
                std::cout << "cycle detected!";
                return true;
            }
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