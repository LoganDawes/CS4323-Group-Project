/*
Group B
Author: Evelyn Wilson
Email: evelyn.wilson@okstate.edu
Date: 4/12/2025

Description: This code creates a resource allocation graph for use in deadlock detection and handling requests
*/

#include "resource_allocation.hpp"

using namespace std;

// Populate the graph with pointers to the existing Intersection objects:
void ResourceAllocationGraph::addIntersection(Intersection *intersection)
{
    intersectionMap[intersection->name] = intersection;
}

bool ResourceAllocationGraph::acquire(const string &intersectionName, Train *train)
{
    return intersectionMap[intersectionName]->acquire(train);
}

bool ResourceAllocationGraph::release(const string &intersectionName, Train *train)
{
    return intersectionMap[intersectionName]->release(train);
}

void ResourceAllocationGraph::printGraph()
{
    for (const auto &pair : intersectionMap)
    {
        const Intersection *inter = pair.second;
        cout << inter->name << " | " << (inter->is_mutex ? "Mutex" : "Semaphore")
             << " | " << inter->capacity << " | Held by: ";
        for (const auto &train : inter->trains_in_intersection)
        {
            cout << train->name << " ";
        }
        cout << endl;
    }
}

unordered_map<string, vector<string>> ResourceAllocationGraph::getResourceTable()
{
    unordered_map<string, vector<string>> table;
    for (const auto &pair : intersectionMap)
    {
        vector<string> trains;
        for (Train *train : pair.second->trains_in_intersection)
        {
            trains.push_back(train->name);
        }
        table[pair.first] = trains;
    }
    return table;
}