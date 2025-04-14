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

//bool detectDeadlock(ResourceAllocationGraph& graph);

extern std::mutex mtx;
extern std::condition_variable cv;

extern ResourceAllocationGraph resourceGraph;

int main();

//bool isCyclicUtil(int node, std::map<int, bool>& visited, std::map<int, bool>& recursionStack, const std::map<int, ResourceAllocationGraph&& graph);

#endif