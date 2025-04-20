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

void deadlockRecovery(unordered_map<string, Train*>& trains, 
    unordered_map<string, vector<string>>& resourceGraph,
    const vector<string>& cycle, int sim_time = 0);

#endif