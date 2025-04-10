/*
Group B
Author: Evelyn Wilson
Email: evelyn.wilson@okstate.edu
Date: 4/10/2025

Description: Resolving deadlocks by preempting an intersection in one of the active trains.
*/

ifndef DEADLOCK_RECOVERY_H
DEFINE DEADLOCK_RECOVERY_H

#include <map>
#include <vector>
#include <string>
#include <iostream>
#include <unordered_map>
#include "parsing.hpp"
#include "logging.hpp"

// Resource table is included as an input despite not being defined yet
vector<string> getDeadlockCycle(const map<string, vector<string>> &resourceTable){
    // Cycle detection logic
    return 0;
}

void deadclockRecovery(map<string, Train*>& trains, unordered_map<string, Intersection*>& intersections, map<string, vector<string>>& resourceTable){
    vector<string> cycle = getDeadlockCycle(resourceTable);

    // Is the cycle empty?

    // Log that the deadlock was detected

    // Preempt the first train in the cycle
    
    // Log it

    // release the intersection
    
    // Log it
    
    // Update resource table
}
