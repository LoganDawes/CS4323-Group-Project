#ifndef SERVER_HPP
#define SERVER_HPP

#include "parsing.hpp"
#include "logging.hpp"
#include "ipc.hpp"
#include "train.hpp"
#include "deadlock_recovery.hpp"
#include "resource_allocation.hpp"
#include <iostream>
#include <vector>
#include <map>
#include <mutex>
#include <thread>
#include <condition_variable>

void handleRequest(int processID);

extern std::mutex mtx;
extern std::condition_variable cv;

extern ResourceAllocationGraph resourceGraph;

int main();

bool detectDeadlock(const unordered_map<string, vector<string>>& waitingGraph, vector<string>& cycle);

bool isCyclicUtil(const string& node,
    unordered_map<string, bool>& visited,
    unordered_map<string, bool>& recursionStack,
    const unordered_map<string, vector<string>>& graph,
    vector<string>& cycle,
    unordered_map<string, string>& parent);

#endif