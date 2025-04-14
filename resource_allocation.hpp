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
#include "parsing.hpp"

using namespace std;

class ResourceAllocationGraph{
    private:
    std::unordered_map<std::string, Intersection *> intersectionMap;

    public:
    void addIntersection(Intersection* inter);
    bool acquire(const string& intersectionName, Train* train);
    bool release(const string& intersectionName, Train* train);
    void printGraph();
    unordered_map<string, vector<string>> getResourceTable();
};

#endif