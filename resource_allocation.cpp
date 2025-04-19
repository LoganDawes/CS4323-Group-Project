/*
Group B
Author: Evelyn Wilson
Email: evelyn.wilson@okstate.edu
Date: 4/12/2025

Description: This code creates a resource allocation graph for use in deadlock detection and handling requests
*/

#include "resource_allocation.hpp"

using namespace std;

// Populate the graph with pointers to the existing Intersection objects
// Should be called in a for loop in server to initialize the Resource Allocation graph
void ResourceAllocationGraph::addIntersection(Intersection *intersection)
{
    // Adds an interesection to the intersection map in the graph
    intersectionMap[intersection->name] = intersection;
}

// Calls the logic to acquire a train in parsing.cpp
// Is this really necessary and what is it adding to the functionality of our program?
bool ResourceAllocationGraph::acquire(const string &intersectionName, Train *train)
{
    return intersectionMap[intersectionName]->acquire(train);
}

// Calls the logic to release a train in parsing.cpp
bool ResourceAllocationGraph::release(const string &intersectionName, Train *train)
{
    return intersectionMap[intersectionName]->release(train);
}

void ResourceAllocationGraph::printGraph()
{
    // Goes through the key-value pairs in the map, listing out the fields for each intersection, including what trains it holds.
    for (const auto &pair : intersectionMap)
    {
        const Intersection *inter = pair.second; // pair.second is the value in the key-value pair. It's accessing the actual data.
        cout << inter->name << " | " << (inter->is_mutex ? "Mutex" : "Semaphore")
             << " | " << inter->capacity << " | Held by: ";
        for (const auto &train : inter->trains_in_intersection)
        {
            cout << train->name << " ";
        }
        cout << endl;
    }
}

// Adds necessary logic to be able to detect deadlocks, showing what trains are waiting for what intersections.
unordered_map<string, vector<string>> ResourceAllocationGraph::buildWaitingGraph() {
    unordered_map<string, vector<string>> graph;

    for (const auto& [iname, intersection] : intersectionMap) {
        for (Train* train : intersection->waiting_trains) {
            for (Train* holder : intersection->trains_in_intersection) {
                graph[train->name].push_back(holder->name);
            }
        }
    }

    return graph;
}

// Outputs an unordered map of the intersection names and the trains in the intersection.
// I think this is being used incorrectly.
unordered_map<string, vector<string>> ResourceAllocationGraph::getResourceGraph()
{ 
    unordered_map<string, vector<string>> graph;
    for (const auto &pair : intersectionMap)
    {
        vector<string> trains;
        for (Train *train : pair.second->trains_in_intersection)
        {
            trains.push_back(train->name);
        }
        graph[pair.first] = trains;
    }
    return graph;
}