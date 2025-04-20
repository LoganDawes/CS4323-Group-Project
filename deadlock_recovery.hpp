#ifndef DEADLOCK_RECOVERY_HPP
#define DEADLOCK_RECOVERY_HPP

#include <map>
#include <vector>
#include <string>
#include <iostream>
#include <unordered_map>
#include "logging.hpp"
#include "train.hpp"
#include "ipc.hpp"
#include "parsing.hpp"

using namespace std;

// Resource table is included as an input despite not being defined yet
vector<string> getDeadlockCycle(const map<string, vector<string>> &resourceTable);

void deadlockRecovery(map<string, Train*>& trains, 
    unordered_map<string, Intersection*>& intersections, 
    map<string, vector<string>>& resourceGraph,
    const vector<string>& cycle);

#endif