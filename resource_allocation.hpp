#ifndef RESOURCE_ALLOCATION_HPP
#define RESOURCE_ALLOCATION_HPP

#include <iostream>
#include <sstream>
#include <vector>
#include <thread>
#include <string>
#include <mutex>
#include <fstream>
#include <unordered_map>
#include <semaphore.h>
#include <pthread.h>
#include <condition_variable>
#include <algorithm>

class ResourceAllocationGraph{
    void addIntersection(Intersection* inter);
    void acquire(const string& intersectionName, Train* train);
    void release(const string& intersectionName, Train* train);
    void printGraph();
    unordered_map<string, vector<string>> getResourceTable();
}

#endif